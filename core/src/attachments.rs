//! Files attached to a transaction. A file is never kept under its own
//! name on disk: it's copied into the database's `attachments/` folder
//! under a GUID, with its real name, size, MIME type and checksum kept in
//! the row - so two attachments with the same filename never collide, and
//! a renamed or moved source file doesn't orphan the copy.

use crate::dates;
use crate::db::Database;
use crate::error::{CoreError, Result};
use crate::transactions;
use rusqlite::{Connection, OptionalExtension, Row, params};
use sha2::{Digest, Sha256};
use std::path::{Path, PathBuf};
use uuid::Uuid;

#[derive(Debug, Clone, PartialEq, Eq, serde::Serialize, serde::Deserialize)]
pub struct Attachment {
    pub id: i64,
    pub transaction_id: i64,
    pub stored_name: String,
    pub original_name: String,
    pub size: i64,
    pub mime_type: String,
    pub checksum: String,
    pub added_at: String,
}

fn from_row(row: &Row) -> rusqlite::Result<Attachment> {
    Ok(Attachment {
        id: row.get(0)?,
        transaction_id: row.get(1)?,
        stored_name: row.get(2)?,
        original_name: row.get(3)?,
        size: row.get(4)?,
        mime_type: row.get(5)?,
        checksum: row.get(6)?,
        added_at: row.get(7)?,
    })
}

const SELECT_COLUMNS: &str =
    "id, transaction_id, stored_name, original_name, size, mime_type, checksum, added_at";

/// Copies `source_path` into `database`'s `attachments/` folder and records
/// it against `transaction_id`.
pub fn attach(database: &Database, transaction_id: i64, source_path: &Path) -> Result<Attachment> {
    transactions::get(database.connection(), transaction_id)?;

    let io_err = |source: std::io::Error| CoreError::Io {
        path: source_path.to_path_buf(),
        source,
    };
    let bytes = std::fs::read(source_path).map_err(io_err)?;
    let checksum = format!("{:x}", Sha256::digest(&bytes));
    let original_name = source_path
        .file_name()
        .map(|n| n.to_string_lossy().to_string())
        .unwrap_or_else(|| "attachment".to_string());
    let extension = source_path
        .extension()
        .and_then(|e| e.to_str())
        .unwrap_or("");
    let stored_name = if extension.is_empty() {
        Uuid::new_v4().to_string()
    } else {
        format!("{}.{extension}", Uuid::new_v4())
    };
    let mime_type = guess_mime_type(extension);

    let destination = database.attachments_dir().join(&stored_name);
    std::fs::write(&destination, &bytes).map_err(|source| CoreError::Io {
        path: destination.clone(),
        source,
    })?;

    database.connection().execute(
        "INSERT INTO attachments (transaction_id, stored_name, original_name, size, mime_type, checksum, added_at) \
         VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7)",
        params![transaction_id, stored_name, original_name, bytes.len() as i64, mime_type, checksum, dates::today()],
    )?;
    let id = database.connection().last_insert_rowid();

    Ok(Attachment {
        id,
        transaction_id,
        stored_name,
        original_name,
        size: bytes.len() as i64,
        mime_type: mime_type.to_string(),
        checksum,
        added_at: dates::today(),
    })
}

pub fn get(conn: &Connection, id: i64) -> Result<Attachment> {
    let sql = format!("SELECT {SELECT_COLUMNS} FROM attachments WHERE id = ?1");
    conn.query_row(&sql, params![id], from_row)
        .optional()?
        .ok_or(CoreError::NotFound("attachment", id))
}

pub fn list_for_transaction(conn: &Connection, transaction_id: i64) -> Result<Vec<Attachment>> {
    let sql = format!(
        "SELECT {SELECT_COLUMNS} FROM attachments WHERE transaction_id = ?1 ORDER BY added_at, id"
    );
    let mut stmt = conn.prepare(&sql)?;
    let rows = stmt
        .query_map(params![transaction_id], from_row)?
        .collect::<std::result::Result<Vec<_>, _>>()?;
    Ok(rows)
}

pub fn path_on_disk(database: &Database, attachment: &Attachment) -> PathBuf {
    database.attachments_dir().join(&attachment.stored_name)
}

/// Removes the row and its file. Missing on disk is not an error - the
/// database record is what the rest of the app cares about; a file that's
/// already gone just means there's nothing left to delete.
pub fn detach(database: &Database, id: i64) -> Result<()> {
    let attachment = get(database.connection(), id)?;
    let changed = database
        .connection()
        .execute("DELETE FROM attachments WHERE id = ?1", params![id])?;
    if changed == 0 {
        return Err(CoreError::NotFound("attachment", id));
    }
    std::fs::remove_file(path_on_disk(database, &attachment)).ok();
    Ok(())
}

/// Files sitting in `attachments/` that no row points at - left behind by
/// an interrupted operation, or a restore from a backup with a stale
/// database. Nothing removes them automatically; see `delete_orphan_file`.
pub fn orphan_files(database: &Database) -> Result<Vec<String>> {
    let dir = database.attachments_dir();
    if !dir.is_dir() {
        return Ok(Vec::new());
    }

    let mut known: std::collections::HashSet<String> = std::collections::HashSet::new();
    let mut stmt = database
        .connection()
        .prepare("SELECT stored_name FROM attachments")?;
    for row in stmt.query_map([], |r| r.get::<_, String>(0))? {
        known.insert(row?);
    }

    let mut orphans = Vec::new();
    for entry in std::fs::read_dir(&dir).map_err(|source| CoreError::Io {
        path: dir.clone(),
        source,
    })? {
        let entry = entry.map_err(|source| CoreError::Io {
            path: dir.clone(),
            source,
        })?;
        if entry.file_type().is_ok_and(|t| t.is_file()) {
            let name = entry.file_name().to_string_lossy().to_string();
            if !known.contains(&name) {
                orphans.push(name);
            }
        }
    }
    orphans.sort();
    Ok(orphans)
}

pub fn delete_orphan_file(database: &Database, file_name: &str) -> Result<()> {
    let path = database.attachments_dir().join(file_name);
    std::fs::remove_file(&path).map_err(|source| CoreError::Io { path, source })
}

fn guess_mime_type(extension: &str) -> &'static str {
    match extension.to_ascii_lowercase().as_str() {
        "pdf" => "application/pdf",
        "png" => "image/png",
        "jpg" | "jpeg" => "image/jpeg",
        "gif" => "image/gif",
        "txt" => "text/plain",
        "csv" => "text/csv",
        "doc" => "application/msword",
        "docx" => "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
        "xls" => "application/vnd.ms-excel",
        "xlsx" => "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
        "zip" => "application/zip",
        "html" | "htm" => "text/html",
        "json" => "application/json",
        "xml" => "application/xml",
        _ => "application/octet-stream",
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::money::Amount;
    use crate::{accounts, db};

    fn new_transaction(database: &Database) -> i64 {
        let account_id = accounts::create(
            database.connection(),
            &accounts::NewAccount {
                name: "Checking".into(),
                currency: "EUR".into(),
                ..Default::default()
            },
        )
        .unwrap();
        transactions::create(
            database.connection(),
            &transactions::NewTransaction {
                account_id,
                label: "Rent".into(),
                amount: Amount::parse("-500").unwrap(),
                ..Default::default()
            },
        )
        .unwrap()
    }

    #[test]
    fn attach_copies_the_file_and_records_its_checksum() {
        let dir = tempfile::tempdir().unwrap();
        let database = db::open(dir.path()).unwrap();
        let transaction_id = new_transaction(&database);

        let source_dir = tempfile::tempdir().unwrap();
        let source_path = source_dir.path().join("receipt.pdf");
        std::fs::write(&source_path, b"pdf bytes here").unwrap();

        let attachment = attach(&database, transaction_id, &source_path).unwrap();
        assert_eq!(attachment.original_name, "receipt.pdf");
        assert_eq!(attachment.mime_type, "application/pdf");
        assert_eq!(attachment.size, 14);
        assert!(attachment.stored_name.ends_with(".pdf"));
        assert_ne!(attachment.stored_name, "receipt.pdf");

        let on_disk = path_on_disk(&database, &attachment);
        assert!(on_disk.exists());
        assert_eq!(std::fs::read(&on_disk).unwrap(), b"pdf bytes here");

        let listed = list_for_transaction(database.connection(), transaction_id).unwrap();
        assert_eq!(listed, vec![attachment]);
    }

    #[test]
    fn detach_removes_the_row_and_the_file() {
        let dir = tempfile::tempdir().unwrap();
        let database = db::open(dir.path()).unwrap();
        let transaction_id = new_transaction(&database);
        let source_dir = tempfile::tempdir().unwrap();
        let source_path = source_dir.path().join("note.txt");
        std::fs::write(&source_path, b"hello").unwrap();
        let attachment = attach(&database, transaction_id, &source_path).unwrap();
        let on_disk = path_on_disk(&database, &attachment);

        detach(&database, attachment.id).unwrap();

        assert!(!on_disk.exists());
        assert!(
            list_for_transaction(database.connection(), transaction_id)
                .unwrap()
                .is_empty()
        );
    }

    #[test]
    fn deleting_a_transaction_cascades_to_its_attachments_rows() {
        let dir = tempfile::tempdir().unwrap();
        let database = db::open(dir.path()).unwrap();
        let transaction_id = new_transaction(&database);
        let source_dir = tempfile::tempdir().unwrap();
        let source_path = source_dir.path().join("note.txt");
        std::fs::write(&source_path, b"hello").unwrap();
        attach(&database, transaction_id, &source_path).unwrap();

        transactions::delete(database.connection(), transaction_id).unwrap();

        let count: i64 = database
            .connection()
            .query_row("SELECT COUNT(*) FROM attachments", [], |r| r.get(0))
            .unwrap();
        assert_eq!(count, 0);
    }

    #[test]
    fn orphan_files_finds_files_with_no_matching_row() {
        let dir = tempfile::tempdir().unwrap();
        let database = db::open(dir.path()).unwrap();
        let transaction_id = new_transaction(&database);
        let source_dir = tempfile::tempdir().unwrap();
        let source_path = source_dir.path().join("known.txt");
        std::fs::write(&source_path, b"hello").unwrap();
        attach(&database, transaction_id, &source_path).unwrap();

        // A file that just appeared in attachments/ with no database row.
        std::fs::write(database.attachments_dir().join("stray.bin"), b"???").unwrap();

        let orphans = orphan_files(&database).unwrap();
        assert_eq!(orphans, vec!["stray.bin".to_string()]);

        delete_orphan_file(&database, "stray.bin").unwrap();
        assert!(orphan_files(&database).unwrap().is_empty());
    }
}
