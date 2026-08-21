//! Listing, creating, duplicating and deleting whole databases - the
//! folders under the portable `databases/` root, each one independent, all
//! sharing the schema `open` applies. Every function here takes the root
//! folder explicitly rather than reading `databases_root()` itself, so it
//! can be pointed at a temporary directory in tests without touching the
//! real one.

use super::{Database, open};
use crate::error::{CoreError, Result};
use rusqlite::params;
use std::path::{Path, PathBuf};

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct DatabaseInfo {
    pub name: String,
    pub folder: PathBuf,
}

fn io_err(path: &Path) -> impl Fn(std::io::Error) -> CoreError + '_ {
    move |source| CoreError::Io {
        path: path.to_path_buf(),
        source,
    }
}

fn sanitize_name(name: &str) -> Result<String> {
    let trimmed = name.trim();
    if trimmed.is_empty() {
        return Err(CoreError::EmptyField { field: "name" });
    }
    let cleaned: String = trimmed
        .chars()
        .map(|c| {
            if c.is_alphanumeric() || c == ' ' || c == '-' || c == '_' {
                c
            } else {
                '_'
            }
        })
        .collect();
    Ok(cleaned)
}

/// Every database folder under `root` - one whose `accountancy.sqlite`
/// exists - in name order.
pub fn list(root: &Path) -> Result<Vec<DatabaseInfo>> {
    if !root.exists() {
        return Ok(Vec::new());
    }
    let mut result = Vec::new();
    for entry in std::fs::read_dir(root).map_err(io_err(root))? {
        let entry = entry.map_err(io_err(root))?;
        let folder = entry.path();
        if folder.is_dir() && folder.join("accountancy.sqlite").exists() {
            let name = folder
                .file_name()
                .and_then(|n| n.to_str())
                .unwrap_or("database")
                .to_string();
            result.push(DatabaseInfo { name, folder });
        }
    }
    result.sort_by_key(|a| a.name.to_lowercase());
    Ok(result)
}

/// Creates a new, empty database folder named `name` under `root`, with its
/// schema already applied. Fails if a database with that name exists.
pub fn create(root: &Path, name: &str) -> Result<PathBuf> {
    let clean = sanitize_name(name)?;
    let folder = root.join(&clean);
    if folder.exists() {
        return Err(CoreError::DuplicateName {
            kind: "database",
            name: clean,
        });
    }
    open(&folder)?;
    Ok(folder)
}

/// Copies `source_folder` (its data, not its live connection) to a new
/// database named `new_name` under `root`. Takes a consistent snapshot via
/// `VACUUM INTO`, the same mechanism `backup::create` uses, rather than
/// copying the raw `.sqlite` file - safe even if `source_folder` happens to
/// be the database currently open elsewhere in the app.
pub fn duplicate(root: &Path, source_folder: &Path, new_name: &str) -> Result<PathBuf> {
    let clean = sanitize_name(new_name)?;
    let destination = root.join(&clean);
    if destination.exists() {
        return Err(CoreError::DuplicateName {
            kind: "database",
            name: clean,
        });
    }
    std::fs::create_dir_all(&destination).map_err(io_err(&destination))?;

    let source_db: Database = open(source_folder)?;
    let destination_sqlite = destination.join("accountancy.sqlite");
    source_db.connection().execute(
        "VACUUM INTO ?1",
        params![destination_sqlite.to_string_lossy().to_string()],
    )?;
    drop(source_db);

    let source_attachments = source_folder.join("attachments");
    let destination_attachments = destination.join("attachments");
    std::fs::create_dir_all(&destination_attachments).map_err(io_err(&destination_attachments))?;
    if source_attachments.is_dir() {
        for entry in std::fs::read_dir(&source_attachments).map_err(io_err(&source_attachments))? {
            let entry = entry.map_err(io_err(&source_attachments))?;
            if entry
                .file_type()
                .map_err(io_err(&source_attachments))?
                .is_file()
            {
                let to = destination_attachments.join(entry.file_name());
                std::fs::copy(entry.path(), &to).map_err(io_err(&to))?;
            }
        }
    }
    Ok(destination)
}

/// Permanently deletes a database folder. No undo - the caller must have
/// already confirmed with the user.
pub fn delete(folder: &Path) -> Result<()> {
    std::fs::remove_dir_all(folder).map_err(io_err(folder))
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::accounts;
    use crate::money::Amount;

    #[test]
    fn create_makes_a_ready_to_use_database() {
        let root = tempfile::tempdir().unwrap();
        let folder = create(root.path(), "Household").unwrap();

        let listed = list(root.path()).unwrap();
        assert_eq!(listed.len(), 1);
        assert_eq!(listed[0].name, "Household");
        assert_eq!(listed[0].folder, folder);
    }

    #[test]
    fn create_rejects_a_name_already_in_use() {
        let root = tempfile::tempdir().unwrap();
        create(root.path(), "Household").unwrap();
        assert!(matches!(
            create(root.path(), "Household"),
            Err(CoreError::DuplicateName { .. })
        ));
    }

    #[test]
    fn duplicate_copies_the_data_under_a_new_name() {
        let root = tempfile::tempdir().unwrap();
        let source_folder = create(root.path(), "Original").unwrap();
        let source_db = open(&source_folder).unwrap();
        accounts::create(
            source_db.connection(),
            &accounts::NewAccount {
                name: "Checking".into(),
                currency: "EUR".into(),
                opening_balance_cleared: Amount::parse("42").unwrap(),
                ..Default::default()
            },
        )
        .unwrap();
        drop(source_db);

        let copy_folder = duplicate(root.path(), &source_folder, "Copy").unwrap();
        let copy_db = open(&copy_folder).unwrap();
        let accounts = accounts::list(copy_db.connection(), true).unwrap();
        assert_eq!(accounts.len(), 1);
        assert_eq!(accounts[0].balance_cleared, Amount::parse("42").unwrap());

        // The original is untouched and still listed alongside the copy.
        assert_eq!(list(root.path()).unwrap().len(), 2);
    }

    #[test]
    fn delete_removes_the_folder_entirely() {
        let root = tempfile::tempdir().unwrap();
        let folder = create(root.path(), "Temporary").unwrap();
        assert!(list(root.path()).unwrap().len() == 1);

        delete(&folder).unwrap();

        assert!(list(root.path()).unwrap().is_empty());
        assert!(!folder.exists());
    }

    #[test]
    fn rejects_an_empty_name() {
        let root = tempfile::tempdir().unwrap();
        assert!(matches!(
            create(root.path(), "   "),
            Err(CoreError::EmptyField { .. })
        ));
    }
}
