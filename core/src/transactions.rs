//! Transactions form a two-level tree: a top-level transaction (`parent_id
//! == 0`) may have children that split its amount across several
//! categories. **Only top-level transactions ever count** in a balance, a
//! total or a statistic - counting a child too would count the same money
//! twice. Every aggregation in this crate filters on `parent_id = 0`; see
//! `accounts::recompute_balances`.
//!
//! A split's last child is the *remainder*: whatever the explicit children
//! didn't allocate, maintained automatically by `sync_remainder` and
//! recognised by `is_remainder`, never by its label.

use crate::accounts;
use crate::error::{CoreError, Result};
use crate::money::Amount;
use rusqlite::{Connection, OptionalExtension, Row, params};

#[derive(Debug, Clone, Copy, PartialEq, Eq, serde::Serialize, serde::Deserialize)]
pub enum Status {
    Cleared = 1,
    Committed = 2,
    Planned = 3,
}

impl Status {
    pub fn from_i64(value: i64) -> Status {
        match value {
            1 => Status::Cleared,
            3 => Status::Planned,
            _ => Status::Committed,
        }
    }

    pub fn as_i64(self) -> i64 {
        self as i64
    }
}

#[derive(Debug, Clone, PartialEq, Eq, serde::Serialize, serde::Deserialize)]
pub struct Transaction {
    pub id: i64,
    pub account_id: i64,
    pub parent_id: i64,
    pub label: String,
    pub date: Option<String>,
    pub clearing_date: Option<String>,
    pub amount: Amount,
    pub status: Status,
    pub category_id: i64,
    pub payee_id: i64,
    pub payment_mode_id: i64,
    pub tag_id: i64,
    pub project_id: i64,
    pub check_id: i64,
    pub comment: Option<String>,
    pub is_remainder: bool,
    pub running_balance: Amount,
    pub external_id: Option<String>,
}

impl Transaction {
    pub fn is_top_level(&self) -> bool {
        self.parent_id == 0
    }
}

#[derive(Debug, Clone, Default)]
pub struct NewTransaction {
    pub account_id: i64,
    pub label: String,
    pub date: Option<String>,
    pub clearing_date: Option<String>,
    pub amount: Amount,
    pub status: Option<Status>,
    pub category_id: i64,
    pub payee_id: i64,
    pub payment_mode_id: i64,
    pub tag_id: i64,
    pub project_id: i64,
    pub check_id: i64,
    pub comment: Option<String>,
    pub external_id: Option<String>,
}

fn from_row(row: &Row) -> rusqlite::Result<Transaction> {
    Ok(Transaction {
        id: row.get(0)?,
        account_id: row.get(1)?,
        parent_id: row.get(2)?,
        label: row.get(3)?,
        date: row.get(4)?,
        clearing_date: row.get(5)?,
        amount: Amount::from_micros(row.get(6)?),
        status: Status::from_i64(row.get(7)?),
        category_id: row.get(8)?,
        payee_id: row.get(9)?,
        payment_mode_id: row.get(10)?,
        tag_id: row.get(11)?,
        project_id: row.get(12)?,
        check_id: row.get(13)?,
        comment: row.get(14)?,
        is_remainder: row.get::<_, i64>(15)? != 0,
        running_balance: Amount::from_micros(row.get(16)?),
        external_id: row.get(17)?,
    })
}

const SELECT_COLUMNS: &str = "id, account_id, parent_id, label, date, clearing_date, amount, status, \
     category_id, payee_id, payment_mode_id, tag_id, project_id, check_id, comment, \
     is_remainder, running_balance, external_id";

fn validate_label(label: &str) -> Result<&str> {
    let trimmed = label.trim();
    if trimmed.is_empty() {
        return Err(CoreError::EmptyField { field: "label" });
    }
    Ok(trimmed)
}

/// Creates a top-level transaction.
pub fn create(conn: &Connection, new: &NewTransaction) -> Result<i64> {
    let label = validate_label(&new.label)?;
    let status = new.status.unwrap_or(Status::Committed);
    conn.execute(
        "INSERT INTO transactions (account_id, parent_id, label, date, clearing_date, amount, status, \
         category_id, payee_id, payment_mode_id, tag_id, project_id, check_id, comment, external_id) \
         VALUES (?1, 0, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14)",
        params![
            new.account_id,
            label,
            new.date,
            new.clearing_date,
            new.amount.micros(),
            status.as_i64(),
            new.category_id,
            new.payee_id,
            new.payment_mode_id,
            new.tag_id,
            new.project_id,
            new.check_id,
            new.comment,
            new.external_id,
        ],
    )?;
    let id = conn.last_insert_rowid();
    accounts::recompute_balances(conn, new.account_id)?;
    Ok(id)
}

pub fn get(conn: &Connection, id: i64) -> Result<Transaction> {
    let sql = format!("SELECT {SELECT_COLUMNS} FROM transactions WHERE id = ?1");
    conn.query_row(&sql, params![id], from_row)
        .optional()?
        .ok_or(CoreError::NotFound("transaction", id))
}

/// The transactions of an account, as a flat list in the order the list
/// window shows them: top-level transactions in chronological order, each
/// immediately followed by its children (if any).
pub fn list_for_account(conn: &Connection, account_id: i64) -> Result<Vec<Transaction>> {
    let sql = format!(
        "SELECT {SELECT_COLUMNS} FROM transactions \
         WHERE account_id = ?1 AND parent_id = 0 \
         ORDER BY (date IS NULL), date, id"
    );
    let mut stmt = conn.prepare(&sql)?;
    let parents = stmt
        .query_map(params![account_id], from_row)?
        .collect::<std::result::Result<Vec<_>, _>>()?;

    let child_sql = format!(
        "SELECT {SELECT_COLUMNS} FROM transactions WHERE parent_id = ?1 ORDER BY is_remainder, id"
    );
    let mut child_stmt = conn.prepare(&child_sql)?;

    let mut result = Vec::with_capacity(parents.len());
    for parent in parents {
        let children = child_stmt
            .query_map(params![parent.id], from_row)?
            .collect::<std::result::Result<Vec<_>, _>>()?;
        result.push(parent);
        result.extend(children);
    }
    Ok(result)
}

pub fn update(conn: &Connection, id: i64, new: &NewTransaction) -> Result<()> {
    let label = validate_label(&new.label)?;
    let existing = get(conn, id)?;
    let status = new.status.unwrap_or(existing.status);
    let changed = conn.execute(
        "UPDATE transactions SET label = ?1, date = ?2, clearing_date = ?3, amount = ?4, status = ?5, \
         category_id = ?6, payee_id = ?7, payment_mode_id = ?8, tag_id = ?9, project_id = ?10, \
         check_id = ?11, comment = ?12, external_id = ?13 WHERE id = ?14",
        params![
            label,
            new.date,
            new.clearing_date,
            new.amount.micros(),
            status.as_i64(),
            new.category_id,
            new.payee_id,
            new.payment_mode_id,
            new.tag_id,
            new.project_id,
            new.check_id,
            new.comment,
            new.external_id,
            id,
        ],
    )?;
    if changed == 0 {
        return Err(CoreError::NotFound("transaction", id));
    }
    if existing.is_top_level() {
        accounts::recompute_balances(conn, existing.account_id)?;
    } else if existing.parent_id != 0 {
        sync_remainder(conn, existing.parent_id)?;
    }
    Ok(())
}

/// Deletes a transaction. Deleting a top-level transaction takes its
/// children with it (there is no `ON DELETE CASCADE` on the self-referencing
/// `parent_id`, so this removes them explicitly first).
pub fn delete(conn: &Connection, id: i64) -> Result<()> {
    let existing = get(conn, id)?;
    conn.execute("DELETE FROM transactions WHERE parent_id = ?1", params![id])?;
    let changed = conn.execute("DELETE FROM transactions WHERE id = ?1", params![id])?;
    if changed == 0 {
        return Err(CoreError::NotFound("transaction", id));
    }
    if existing.is_top_level() {
        accounts::recompute_balances(conn, existing.account_id)?;
    } else if !existing.is_remainder {
        sync_remainder(conn, existing.parent_id)?;
    }
    Ok(())
}

#[derive(Debug, Clone, Default)]
pub struct NewSplitLine {
    pub label: String,
    pub amount: Amount,
    pub category_id: i64,
    pub comment: Option<String>,
}

/// Adds an explicit split line under `parent_id`, then rebuilds the
/// remainder line so the children's total still matches the parent's
/// amount. `parent_id` must name a top-level transaction.
pub fn add_split_line(conn: &Connection, parent_id: i64, line: &NewSplitLine) -> Result<i64> {
    let parent = get(conn, parent_id)?;
    if !parent.is_top_level() {
        return Err(CoreError::InvalidAmount(
            "a split line can only be added to a top-level transaction".into(),
        ));
    }
    let label = validate_label(&line.label)?;
    conn.execute(
        "INSERT INTO transactions (account_id, parent_id, label, date, amount, status, category_id, comment) \
         VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8)",
        params![
            parent.account_id,
            parent_id,
            label,
            parent.date,
            line.amount.micros(),
            parent.status.as_i64(),
            line.category_id,
            line.comment,
        ],
    )?;
    let id = conn.last_insert_rowid();
    sync_remainder(conn, parent_id)?;
    Ok(id)
}

/// Recomputes the remainder line of a split so its amount is always
/// `parent.amount - sum(explicit children)`. Creates the remainder line if
/// none exists yet and at least one explicit child does; removes it if no
/// explicit child is left.
pub fn sync_remainder(conn: &Connection, parent_id: i64) -> Result<()> {
    let parent = get(conn, parent_id)?;

    let explicit_sum: i64 = conn.query_row(
        "SELECT COALESCE(SUM(amount), 0) FROM transactions WHERE parent_id = ?1 AND is_remainder = 0",
        params![parent_id],
        |r| r.get(0),
    )?;
    let explicit_count: i64 = conn.query_row(
        "SELECT COUNT(*) FROM transactions WHERE parent_id = ?1 AND is_remainder = 0",
        params![parent_id],
        |r| r.get(0),
    )?;

    let remainder_id: Option<i64> = conn
        .query_row(
            "SELECT id FROM transactions WHERE parent_id = ?1 AND is_remainder = 1",
            params![parent_id],
            |r| r.get(0),
        )
        .optional()?;

    if explicit_count == 0 {
        if let Some(id) = remainder_id {
            conn.execute("DELETE FROM transactions WHERE id = ?1", params![id])?;
        }
        return Ok(());
    }

    let remainder_amount = parent.amount.micros() - explicit_sum;
    match remainder_id {
        Some(id) => {
            conn.execute(
                "UPDATE transactions SET amount = ?1 WHERE id = ?2",
                params![remainder_amount, id],
            )?;
        }
        None => {
            // The label is left empty on purpose: a remainder line is
            // recognised by `is_remainder`, and the UI substitutes its own
            // translated placeholder rather than storing one language's
            // sentence in the database.
            conn.execute(
                "INSERT INTO transactions (account_id, parent_id, label, date, amount, status, is_remainder) \
                 VALUES (?1, ?2, '', ?3, ?4, ?5, 1)",
                params![parent.account_id, parent_id, parent.date, remainder_amount, parent.status.as_i64()],
            )?;
        }
    }
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::db;

    fn new_account(conn: &Connection) -> i64 {
        accounts::create(
            conn,
            &accounts::NewAccount {
                name: "Checking".into(),
                currency: "EUR".into(),
                ..Default::default()
            },
        )
        .unwrap()
    }

    #[test]
    fn create_and_recompute_balance() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn);

        create(
            conn,
            &NewTransaction {
                account_id,
                label: "Salary".into(),
                date: Some("2026-01-05".into()),
                amount: Amount::parse("2000").unwrap(),
                status: Some(Status::Cleared),
                ..Default::default()
            },
        )
        .unwrap();
        create(
            conn,
            &NewTransaction {
                account_id,
                label: "Rent".into(),
                date: Some("2026-01-01".into()),
                amount: Amount::parse("-800").unwrap(),
                status: Some(Status::Cleared),
                ..Default::default()
            },
        )
        .unwrap();

        let account = accounts::get(conn, account_id).unwrap();
        assert_eq!(account.balance_cleared, Amount::parse("1200").unwrap());

        let list = list_for_account(conn, account_id).unwrap();
        assert_eq!(list.len(), 2);
        // Rent (2026-01-01) comes first chronologically.
        assert_eq!(list[0].label, "Rent");
        assert_eq!(list[0].running_balance, Amount::parse("-800").unwrap());
        assert_eq!(list[1].running_balance, Amount::parse("1200").unwrap());
    }

    #[test]
    fn only_top_level_transactions_count_toward_the_balance() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn);

        let parent_id = create(
            conn,
            &NewTransaction {
                account_id,
                label: "Supermarket".into(),
                amount: Amount::parse("-100").unwrap(),
                status: Some(Status::Cleared),
                ..Default::default()
            },
        )
        .unwrap();

        add_split_line(
            conn,
            parent_id,
            &NewSplitLine {
                label: "Groceries".into(),
                amount: Amount::parse("-60").unwrap(),
                ..Default::default()
            },
        )
        .unwrap();

        let account = accounts::get(conn, account_id).unwrap();
        // Still -100, not -160: the split line is a child, not counted.
        assert_eq!(account.balance_cleared, Amount::parse("-100").unwrap());

        let children = list_for_account(conn, account_id).unwrap();
        assert_eq!(children.len(), 3); // parent + explicit split + remainder
        let remainder = children.iter().find(|t| t.is_remainder).unwrap();
        assert_eq!(remainder.amount, Amount::parse("-40").unwrap());
    }

    #[test]
    fn remainder_disappears_once_the_last_split_line_is_removed() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn);

        let parent_id = create(
            conn,
            &NewTransaction {
                account_id,
                label: "Supermarket".into(),
                amount: Amount::parse("-100").unwrap(),
                ..Default::default()
            },
        )
        .unwrap();
        let split_id = add_split_line(
            conn,
            parent_id,
            &NewSplitLine {
                label: "Groceries".into(),
                amount: Amount::parse("-60").unwrap(),
                ..Default::default()
            },
        )
        .unwrap();

        delete(conn, split_id).unwrap();

        let children = list_for_account(conn, account_id).unwrap();
        assert_eq!(children.len(), 1); // only the parent is left
    }

    #[test]
    fn deleting_a_parent_deletes_its_children() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn);

        let parent_id = create(
            conn,
            &NewTransaction {
                account_id,
                label: "Supermarket".into(),
                amount: Amount::parse("-100").unwrap(),
                ..Default::default()
            },
        )
        .unwrap();
        add_split_line(
            conn,
            parent_id,
            &NewSplitLine {
                label: "Groceries".into(),
                amount: Amount::parse("-60").unwrap(),
                ..Default::default()
            },
        )
        .unwrap();

        delete(conn, parent_id).unwrap();

        assert!(list_for_account(conn, account_id).unwrap().is_empty());
    }
}
