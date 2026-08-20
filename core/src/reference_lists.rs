//! The application's editable reference lists: payees, payment modes, tags,
//! projects, budget lines, banks, and categories. All seven share the same
//! two rules - a name cannot be empty, a name cannot be a duplicate - and
//! deleting an entry reassigns whatever referenced it to "unspecified" (id
//! `0`) rather than cascading the deletion into transactions or accounts.

use crate::error::{CoreError, Result};
use rusqlite::{Connection, params};

#[derive(Debug, Clone, PartialEq, Eq, serde::Serialize, serde::Deserialize)]
pub struct NamedItem {
    pub id: i64,
    pub name: String,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ReferenceKind {
    Payee,
    PaymentMode,
    Tag,
    Project,
    BudgetLine,
    Bank,
}

impl ReferenceKind {
    fn table(self) -> &'static str {
        match self {
            ReferenceKind::Payee => "payees",
            ReferenceKind::PaymentMode => "payment_modes",
            ReferenceKind::Tag => "tags",
            ReferenceKind::Project => "projects",
            ReferenceKind::BudgetLine => "budget_lines",
            ReferenceKind::Bank => "banks",
        }
    }

    fn label(self) -> &'static str {
        match self {
            ReferenceKind::Payee => "payee",
            ReferenceKind::PaymentMode => "payment mode",
            ReferenceKind::Tag => "tag",
            ReferenceKind::Project => "project",
            ReferenceKind::BudgetLine => "budget line",
            ReferenceKind::Bank => "bank",
        }
    }

    /// Where this kind is referenced from, so deletion can reassign to
    /// "unspecified" instead of leaving a dangling id or cascading.
    fn referrers(self) -> &'static [(&'static str, &'static str)] {
        match self {
            ReferenceKind::Payee => &[("transactions", "payee_id")],
            ReferenceKind::PaymentMode => &[("transactions", "payment_mode_id")],
            ReferenceKind::Tag => &[("transactions", "tag_id")],
            ReferenceKind::Project => &[("transactions", "project_id")],
            ReferenceKind::BudgetLine => &[("categories", "budget_line_id")],
            ReferenceKind::Bank => &[("accounts", "bank_id")],
        }
    }
}

fn validate_name(name: &str) -> Result<&str> {
    let trimmed = name.trim();
    if trimmed.is_empty() {
        return Err(CoreError::EmptyField { field: "name" });
    }
    Ok(trimmed)
}

pub fn list(conn: &Connection, kind: ReferenceKind) -> Result<Vec<NamedItem>> {
    let sql = format!(
        "SELECT id, name FROM {} ORDER BY name COLLATE NOCASE",
        kind.table()
    );
    let mut stmt = conn.prepare(&sql)?;
    let rows = stmt
        .query_map([], |row| {
            Ok(NamedItem {
                id: row.get(0)?,
                name: row.get(1)?,
            })
        })?
        .collect::<std::result::Result<Vec<_>, _>>()?;
    Ok(rows)
}

pub fn create(conn: &Connection, kind: ReferenceKind, name: &str) -> Result<i64> {
    let name = validate_name(name)?;
    let sql = format!("INSERT INTO {} (name) VALUES (?1)", kind.table());
    conn.execute(&sql, params![name])
        .map_err(|e| duplicate_or(e, kind.label(), name))?;
    Ok(conn.last_insert_rowid())
}

pub fn rename(conn: &Connection, kind: ReferenceKind, id: i64, name: &str) -> Result<()> {
    let name = validate_name(name)?;
    let sql = format!("UPDATE {} SET name = ?1 WHERE id = ?2", kind.table());
    let changed = conn
        .execute(&sql, params![name, id])
        .map_err(|e| duplicate_or(e, kind.label(), name))?;
    if changed == 0 {
        return Err(CoreError::NotFound(kind.label(), id));
    }
    Ok(())
}

pub fn delete(conn: &Connection, kind: ReferenceKind, id: i64) -> Result<()> {
    for (table, column) in kind.referrers() {
        let sql = format!("UPDATE {table} SET {column} = 0 WHERE {column} = ?1");
        conn.execute(&sql, params![id])?;
    }
    let sql = format!("DELETE FROM {} WHERE id = ?1", kind.table());
    let changed = conn.execute(&sql, params![id])?;
    if changed == 0 {
        return Err(CoreError::NotFound(kind.label(), id));
    }
    Ok(())
}

fn duplicate_or(err: rusqlite::Error, kind_label: &'static str, name: &str) -> CoreError {
    if let rusqlite::Error::SqliteFailure(ref e, _) = err
        && e.code == rusqlite::ErrorCode::ConstraintViolation
    {
        return CoreError::DuplicateName {
            kind: kind_label,
            name: name.to_string(),
        };
    }
    CoreError::Database(err)
}

// --- Categories: the one reference list with an extra soft link. ---

#[derive(Debug, Clone, PartialEq, Eq, serde::Serialize, serde::Deserialize)]
pub struct Category {
    pub id: i64,
    pub name: String,
    /// `0` means "unspecified".
    pub budget_line_id: i64,
}

pub fn list_categories(conn: &Connection) -> Result<Vec<Category>> {
    let mut stmt = conn
        .prepare("SELECT id, name, budget_line_id FROM categories ORDER BY name COLLATE NOCASE")?;
    let rows = stmt
        .query_map([], |row| {
            Ok(Category {
                id: row.get(0)?,
                name: row.get(1)?,
                budget_line_id: row.get(2)?,
            })
        })?
        .collect::<std::result::Result<Vec<_>, _>>()?;
    Ok(rows)
}

pub fn create_category(conn: &Connection, name: &str, budget_line_id: i64) -> Result<i64> {
    let name = validate_name(name)?;
    conn.execute(
        "INSERT INTO categories (name, budget_line_id) VALUES (?1, ?2)",
        params![name, budget_line_id],
    )
    .map_err(|e| duplicate_or(e, "category", name))?;
    Ok(conn.last_insert_rowid())
}

pub fn update_category(conn: &Connection, id: i64, name: &str, budget_line_id: i64) -> Result<()> {
    let name = validate_name(name)?;
    let changed = conn
        .execute(
            "UPDATE categories SET name = ?1, budget_line_id = ?2 WHERE id = ?3",
            params![name, budget_line_id, id],
        )
        .map_err(|e| duplicate_or(e, "category", name))?;
    if changed == 0 {
        return Err(CoreError::NotFound("category", id));
    }
    Ok(())
}

pub fn delete_category(conn: &Connection, id: i64) -> Result<()> {
    conn.execute(
        "UPDATE transactions SET category_id = 0 WHERE category_id = ?1",
        params![id],
    )?;
    let changed = conn.execute("DELETE FROM categories WHERE id = ?1", params![id])?;
    if changed == 0 {
        return Err(CoreError::NotFound("category", id));
    }
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::db;

    #[test]
    fn create_list_rename_delete_round_trip() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();

        let id = create(conn, ReferenceKind::Payee, "Landlord").unwrap();
        assert_eq!(
            list(conn, ReferenceKind::Payee).unwrap(),
            vec![NamedItem {
                id,
                name: "Landlord".into()
            }]
        );

        rename(conn, ReferenceKind::Payee, id, "  New Landlord  ").unwrap();
        assert_eq!(
            list(conn, ReferenceKind::Payee).unwrap()[0].name,
            "New Landlord"
        );

        delete(conn, ReferenceKind::Payee, id).unwrap();
        assert!(list(conn, ReferenceKind::Payee).unwrap().is_empty());
    }

    #[test]
    fn rejects_empty_and_duplicate_names() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();

        assert!(matches!(
            create(conn, ReferenceKind::Tag, "  "),
            Err(CoreError::EmptyField { .. })
        ));

        create(conn, ReferenceKind::Tag, "Recurring").unwrap();
        assert!(matches!(
            create(conn, ReferenceKind::Tag, "Recurring"),
            Err(CoreError::DuplicateName { .. })
        ));
    }

    #[test]
    fn deleting_a_payee_reassigns_transactions_to_unspecified() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        conn.execute("INSERT INTO accounts (id, name) VALUES (1, 'Checking')", [])
            .unwrap();

        let payee_id = create(conn, ReferenceKind::Payee, "Landlord").unwrap();
        conn.execute(
            "INSERT INTO transactions (id, account_id, label, amount, payee_id) VALUES (1, 1, 'Rent', -1000, ?1)",
            params![payee_id],
        )
        .unwrap();

        delete(conn, ReferenceKind::Payee, payee_id).unwrap();

        let payee_id_after: i64 = conn
            .query_row("SELECT payee_id FROM transactions WHERE id = 1", [], |r| {
                r.get(0)
            })
            .unwrap();
        assert_eq!(payee_id_after, 0);
    }
}
