use super::schema;
use crate::error::{CoreError, Result};
use rusqlite::Connection;
use std::path::{Path, PathBuf};

/// A single database: one folder holding `accountancy.sqlite` and an
/// `attachments/` subfolder. Everything the application writes for this
/// database stays inside that folder, so the whole thing is portable.
pub struct Database {
    conn: Connection,
    folder: PathBuf,
}

impl Database {
    pub fn connection(&self) -> &Connection {
        &self.conn
    }

    pub fn folder(&self) -> &Path {
        &self.folder
    }

    pub fn attachments_dir(&self) -> PathBuf {
        self.folder.join("attachments")
    }
}

/// The folder every database lives under: `databases/` next to the running
/// executable, so the whole application (binary, databases, attachments)
/// can be carried on a USB drive with nothing written outside it.
pub fn databases_root() -> PathBuf {
    let exe_dir = std::env::current_exe()
        .ok()
        .and_then(|p| p.parent().map(Path::to_path_buf))
        .unwrap_or_else(|| PathBuf::from("."));
    exe_dir.join("databases")
}

/// Opens (creating if needed) the database that lives in `folder`, and
/// brings its schema up to date.
pub fn open(folder: &Path) -> Result<Database> {
    std::fs::create_dir_all(folder).map_err(|source| CoreError::Io {
        path: folder.to_path_buf(),
        source,
    })?;
    let attachments = folder.join("attachments");
    std::fs::create_dir_all(&attachments).map_err(|source| CoreError::Io {
        path: attachments,
        source,
    })?;

    let sqlite_path = folder.join("accountancy.sqlite");
    let conn = Connection::open(&sqlite_path)?;
    conn.pragma_update(None, "foreign_keys", "ON")?;
    schema::migrate(&conn)?;

    Ok(Database {
        conn,
        folder: folder.to_path_buf(),
    })
}

/// Opens an in-memory database with the schema applied. Used by tests.
pub fn open_in_memory() -> Result<Database> {
    let conn = Connection::open_in_memory()?;
    conn.pragma_update(None, "foreign_keys", "ON")?;
    schema::migrate(&conn)?;
    Ok(Database {
        conn,
        folder: PathBuf::from(":memory:"),
    })
}
