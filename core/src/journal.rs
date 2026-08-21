//! A record of the application's own technical operations - backup,
//! restore, repair - kept inside the database itself so a restored backup
//! carries its own history forward with it, rather than losing it to
//! whatever database it replaces.

use crate::dates;
use crate::error::Result;
use rusqlite::{Connection, Row, params};

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum OperationKind {
    Backup,
    Restore,
    Repair,
}

impl OperationKind {
    fn as_str(self) -> &'static str {
        match self {
            OperationKind::Backup => "backup",
            OperationKind::Restore => "restore",
            OperationKind::Repair => "repair",
        }
    }

    fn from_str(value: &str) -> OperationKind {
        match value {
            "restore" => OperationKind::Restore,
            "repair" => OperationKind::Repair,
            _ => OperationKind::Backup,
        }
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct LogEntry {
    pub id: i64,
    pub happened_at: String,
    pub kind: OperationKind,
    pub succeeded: bool,
    pub detail: Option<String>,
}

pub fn record(
    conn: &Connection,
    kind: OperationKind,
    succeeded: bool,
    detail: Option<&str>,
) -> Result<()> {
    conn.execute(
        "INSERT INTO operations_log (happened_at, kind, succeeded, detail) VALUES (?1, ?2, ?3, ?4)",
        params![dates::today(), kind.as_str(), succeeded as i64, detail],
    )?;
    Ok(())
}

/// The most recent `limit` entries, newest first - the technical log only
/// ever needs to show what recently happened, not a full history.
pub fn list(conn: &Connection, limit: i64) -> Result<Vec<LogEntry>> {
    let mut stmt =
        conn.prepare("SELECT id, happened_at, kind, succeeded, detail FROM operations_log ORDER BY id DESC LIMIT ?1")?;
    let rows = stmt
        .query_map(params![limit], |row: &Row| {
            Ok(LogEntry {
                id: row.get(0)?,
                happened_at: row.get(1)?,
                kind: OperationKind::from_str(&row.get::<_, String>(2)?),
                succeeded: row.get::<_, i64>(3)? != 0,
                detail: row.get(4)?,
            })
        })?
        .collect::<std::result::Result<Vec<_>, _>>()?;
    Ok(rows)
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::db;

    #[test]
    fn records_and_lists_entries_most_recent_first() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();

        record(
            conn,
            OperationKind::Backup,
            true,
            Some("saved to /tmp/a.zip"),
        )
        .unwrap();
        record(conn, OperationKind::Restore, false, Some("bad archive")).unwrap();

        let entries = list(conn, 10).unwrap();
        assert_eq!(entries.len(), 2);
        assert_eq!(entries[0].kind, OperationKind::Restore);
        assert!(!entries[0].succeeded);
        assert_eq!(entries[1].kind, OperationKind::Backup);
        assert!(entries[1].succeeded);
    }

    #[test]
    fn list_respects_the_limit() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        for _ in 0..5 {
            record(conn, OperationKind::Backup, true, None).unwrap();
        }
        assert_eq!(list(conn, 3).unwrap().len(), 3);
    }
}
