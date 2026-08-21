//! Backup and restore. A backup is a zip archive of a consistent snapshot
//! of `accountancy.sqlite` (taken with `VACUUM INTO`, never a raw file
//! copy) plus every file under `attachments/`. Restoring validates the
//! archive - the sqlite file must open and pass `PRAGMA integrity_check` -
//! in a staging folder before it ever touches the live database folder, so
//! a corrupt or foreign zip can't damage data that was working.
//!
//! This is this project's own format, not a port of any legacy
//! application's backup layout - see `PROMPT.md` §6.

use crate::db::Database;
use crate::error::{CoreError, Result};
use crate::journal::{self, OperationKind};
use rusqlite::{Connection, params};
use std::fs::File;
use std::io::Write;
use std::path::{Path, PathBuf};
use zip::write::SimpleFileOptions;
use zip::{CompressionMethod, ZipArchive, ZipWriter};

fn io_err(path: &Path) -> impl Fn(std::io::Error) -> CoreError + '_ {
    move |source| CoreError::Io {
        path: path.to_path_buf(),
        source,
    }
}

/// Writes a backup of `database` to `destination` (a `.zip` file). Records
/// the attempt in the database's own journal either way.
pub fn create(database: &Database, destination: &Path) -> Result<()> {
    let result = create_inner(database, destination);
    let detail = match &result {
        Ok(()) => format!("saved to {}", destination.display()),
        Err(e) => e.to_string(),
    };
    let _ = journal::record(
        database.connection(),
        OperationKind::Backup,
        result.is_ok(),
        Some(&detail),
    );
    result
}

fn create_inner(database: &Database, destination: &Path) -> Result<()> {
    let snapshot_path = database.folder().join(".backup-snapshot.sqlite");
    if snapshot_path.exists() {
        std::fs::remove_file(&snapshot_path).map_err(io_err(&snapshot_path))?;
    }

    // `VACUUM INTO` gives a consistent, defragmented copy taken through
    // SQLite itself - safe to do while the live connection is open, unlike
    // copying the raw file.
    database.connection().execute(
        "VACUUM INTO ?1",
        params![snapshot_path.to_string_lossy().to_string()],
    )?;

    let write_result = write_archive(database, &snapshot_path, destination);
    std::fs::remove_file(&snapshot_path).ok();
    write_result
}

fn write_archive(database: &Database, snapshot_path: &Path, destination: &Path) -> Result<()> {
    let file = File::create(destination).map_err(io_err(destination))?;
    let mut zip = ZipWriter::new(file);
    let options = SimpleFileOptions::default().compression_method(CompressionMethod::Deflated);

    zip.start_file("accountancy.sqlite", options)?;
    zip.write_all(&std::fs::read(snapshot_path).map_err(io_err(snapshot_path))?)
        .map_err(io_err(destination))?;

    let attachments_dir = database.attachments_dir();
    if attachments_dir.is_dir() {
        for entry in std::fs::read_dir(&attachments_dir).map_err(io_err(&attachments_dir))? {
            let entry = entry.map_err(io_err(&attachments_dir))?;
            if entry
                .file_type()
                .map_err(io_err(&attachments_dir))?
                .is_file()
            {
                let path = entry.path();
                zip.start_file(
                    format!("attachments/{}", entry.file_name().to_string_lossy()),
                    options,
                )?;
                zip.write_all(&std::fs::read(&path).map_err(io_err(&path))?)
                    .map_err(io_err(destination))?;
            }
        }
    }

    zip.finish()?;
    Ok(())
}

/// Restores `source_zip` into `destination_folder`, replacing whatever is
/// there. Extracts into a sibling staging folder first and validates the
/// archive's `accountancy.sqlite` (it must open and pass an integrity
/// check) before swapping it in - a bad archive never touches live data.
///
/// The caller is responsible for having released any open connection to
/// `destination_folder` before calling this, and for reopening it after -
/// this function only ever touches the filesystem.
pub fn restore(source_zip: &Path, destination_folder: &Path) -> Result<()> {
    let staging = sibling_path(destination_folder, "-restoring");
    if staging.exists() {
        std::fs::remove_dir_all(&staging).map_err(io_err(&staging))?;
    }
    std::fs::create_dir_all(&staging).map_err(io_err(&staging))?;

    if let Err(e) = extract_archive(source_zip, &staging) {
        std::fs::remove_dir_all(&staging).ok();
        return Err(e);
    }

    if let Err(e) = validate_sqlite_file(&staging.join("accountancy.sqlite")) {
        std::fs::remove_dir_all(&staging).ok();
        return Err(e);
    }

    if destination_folder.exists() {
        std::fs::remove_dir_all(destination_folder).map_err(io_err(destination_folder))?;
    }
    std::fs::rename(&staging, destination_folder).map_err(io_err(destination_folder))?;
    Ok(())
}

fn sibling_path(folder: &Path, suffix: &str) -> PathBuf {
    let name = folder
        .file_name()
        .and_then(|n| n.to_str())
        .unwrap_or("database");
    folder.with_file_name(format!("{name}{suffix}"))
}

fn extract_archive(source_zip: &Path, destination_folder: &Path) -> Result<()> {
    let file = File::open(source_zip).map_err(io_err(source_zip))?;
    let mut archive = ZipArchive::new(file)?;

    for i in 0..archive.len() {
        let mut entry = archive.by_index(i)?;
        // `enclosed_name` refuses absolute paths and `..` components - a
        // zip crafted to write outside `destination_folder` is ignored,
        // not followed.
        let Some(enclosed) = entry.enclosed_name() else {
            continue;
        };
        let out_path = destination_folder.join(enclosed);

        if entry.is_dir() {
            std::fs::create_dir_all(&out_path).map_err(io_err(&out_path))?;
            continue;
        }
        if let Some(parent) = out_path.parent() {
            std::fs::create_dir_all(parent).map_err(io_err(parent))?;
        }
        let mut out_file = File::create(&out_path).map_err(io_err(&out_path))?;
        std::io::copy(&mut entry, &mut out_file).map_err(io_err(&out_path))?;
    }
    Ok(())
}

fn validate_sqlite_file(path: &Path) -> Result<()> {
    if !path.exists() {
        return Err(CoreError::Invalid(
            "the backup archive has no accountancy.sqlite file".into(),
        ));
    }
    let conn = Connection::open(path)?;
    let result: String = conn.query_row("PRAGMA integrity_check", [], |row| row.get(0))?;
    if result != "ok" {
        return Err(CoreError::Invalid(format!(
            "the backup archive failed an integrity check: {result}"
        )));
    }
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::money::Amount;
    use crate::{accounts, db, transactions};

    #[test]
    fn backup_and_restore_round_trip_the_data() {
        let source_dir = tempfile::tempdir().unwrap();
        let database = db::open(source_dir.path()).unwrap();
        let account_id = accounts::create(
            database.connection(),
            &accounts::NewAccount {
                name: "Checking".into(),
                currency: "EUR".into(),
                opening_balance_cleared: Amount::parse("100").unwrap(),
                ..Default::default()
            },
        )
        .unwrap();
        transactions::create(
            database.connection(),
            &transactions::NewTransaction {
                account_id,
                label: "Coffee".into(),
                date: Some("2026-03-05".into()),
                amount: Amount::parse("-4.5").unwrap(),
                status: Some(transactions::Status::Cleared),
                ..Default::default()
            },
        )
        .unwrap();

        let zip_dir = tempfile::tempdir().unwrap();
        let zip_path = zip_dir.path().join("backup.zip");
        create(&database, &zip_path).unwrap();
        assert!(zip_path.exists());

        let restore_target = tempfile::tempdir().unwrap();
        let destination_folder = restore_target.path().join("restored");
        restore(&zip_path, &destination_folder).unwrap();

        // The backup's own "it worked" journal entry is written to the live
        // database *after* the snapshot is taken, so it's on the source,
        // not inside the archive it describes.
        let source_entries = journal::list(database.connection(), 5).unwrap();
        assert!(
            source_entries
                .iter()
                .any(|e| e.kind == OperationKind::Backup && e.succeeded)
        );

        let restored = db::open(&destination_folder).unwrap();
        let accounts = accounts::list(restored.connection(), true).unwrap();
        assert_eq!(accounts.len(), 1);
        assert_eq!(accounts[0].balance_cleared, Amount::parse("95.5").unwrap());
    }

    #[test]
    fn restore_rejects_a_file_that_isnt_a_backup_archive() {
        let garbage_dir = tempfile::tempdir().unwrap();
        let garbage_path = garbage_dir.path().join("not-a-backup.zip");
        std::fs::write(&garbage_path, b"not a zip file at all").unwrap();

        let destination_parent = tempfile::tempdir().unwrap();
        let destination = destination_parent.path().join("destination");
        let result = restore(&garbage_path, &destination);
        assert!(result.is_err());
        assert!(!destination.exists());
    }

    #[test]
    fn restore_never_touches_the_destination_when_the_archive_is_invalid() {
        let destination_dir = tempfile::tempdir().unwrap();
        let database = db::open(destination_dir.path()).unwrap();
        accounts::create(
            database.connection(),
            &accounts::NewAccount {
                name: "Existing".into(),
                currency: "EUR".into(),
                ..Default::default()
            },
        )
        .unwrap();
        drop(database);

        let garbage_dir = tempfile::tempdir().unwrap();
        let garbage_path = garbage_dir.path().join("garbage.zip");
        std::fs::write(&garbage_path, b"garbage").unwrap();

        assert!(restore(&garbage_path, destination_dir.path()).is_err());

        let still_there = db::open(destination_dir.path()).unwrap();
        assert_eq!(
            accounts::list(still_there.connection(), true).unwrap()[0].name,
            "Existing"
        );
    }
}
