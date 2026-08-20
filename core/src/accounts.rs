//! Bank accounts. An account carries three cumulative balances - cleared
//! (reconciled with the bank), committed (cleared + validated transactions),
//! projected (committed + planned transactions) - which `recompute_balances`
//! rebuilds whole from scratch. Nothing ever patches a balance in place.

use crate::error::{CoreError, Result};
use crate::money::Amount;
use crate::transactions::Status;
use rusqlite::{Connection, OptionalExtension, Row, params};

#[derive(Debug, Clone, PartialEq, Eq, serde::Serialize, serde::Deserialize)]
pub struct Account {
    pub id: i64,
    pub name: String,
    pub currency: String,
    pub iban: Option<String>,
    pub bic: Option<String>,
    pub holder_name: Option<String>,
    pub branch_address: Option<String>,
    pub bank_id: i64,
    pub opening_date: Option<String>,
    pub closing_date: Option<String>,
    pub is_closed: bool,
    pub next_check_number: Option<i64>,
    pub opening_balance_cleared: Amount,
    pub opening_balance_committed: Amount,
    pub opening_balance_projected: Amount,
    pub balance_cleared: Amount,
    pub balance_committed: Amount,
    pub balance_projected: Amount,
}

#[derive(Debug, Clone, Default)]
pub struct NewAccount {
    pub name: String,
    pub currency: String,
    pub iban: Option<String>,
    pub bic: Option<String>,
    pub holder_name: Option<String>,
    pub branch_address: Option<String>,
    pub bank_id: i64,
    pub opening_date: Option<String>,
    pub opening_balance_cleared: Amount,
}

fn from_row(row: &Row) -> rusqlite::Result<Account> {
    Ok(Account {
        id: row.get(0)?,
        name: row.get(1)?,
        currency: row.get(2)?,
        iban: row.get(3)?,
        bic: row.get(4)?,
        holder_name: row.get(5)?,
        branch_address: row.get(6)?,
        bank_id: row.get(7)?,
        opening_date: row.get(8)?,
        closing_date: row.get(9)?,
        is_closed: row.get::<_, i64>(10)? != 0,
        next_check_number: row.get(11)?,
        opening_balance_cleared: Amount::from_micros(row.get(12)?),
        opening_balance_committed: Amount::from_micros(row.get(13)?),
        opening_balance_projected: Amount::from_micros(row.get(14)?),
        balance_cleared: Amount::from_micros(row.get(15)?),
        balance_committed: Amount::from_micros(row.get(16)?),
        balance_projected: Amount::from_micros(row.get(17)?),
    })
}

const SELECT_COLUMNS: &str = "id, name, currency, iban, bic, holder_name, branch_address, bank_id, \
     opening_date, closing_date, is_closed, next_check_number, \
     opening_balance_cleared, opening_balance_committed, opening_balance_projected, \
     balance_cleared, balance_committed, balance_projected";

pub fn create(conn: &Connection, new: &NewAccount) -> Result<i64> {
    let name = new.name.trim();
    if name.is_empty() {
        return Err(CoreError::EmptyField { field: "name" });
    }
    let currency = if new.currency.trim().is_empty() {
        "EUR"
    } else {
        new.currency.trim()
    };
    conn.execute(
        "INSERT INTO accounts (name, currency, iban, bic, holder_name, branch_address, bank_id, \
         opening_date, opening_balance_cleared, opening_balance_committed, opening_balance_projected, \
         balance_cleared, balance_committed, balance_projected) \
         VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?9, ?9, ?9, ?9, ?9)",
        params![
            name,
            currency,
            new.iban,
            new.bic,
            new.holder_name,
            new.branch_address,
            new.bank_id,
            new.opening_date,
            new.opening_balance_cleared.micros(),
        ],
    )?;
    Ok(conn.last_insert_rowid())
}

pub fn get(conn: &Connection, id: i64) -> Result<Account> {
    let sql = format!("SELECT {SELECT_COLUMNS} FROM accounts WHERE id = ?1");
    conn.query_row(&sql, params![id], from_row)
        .optional()?
        .ok_or(CoreError::NotFound("account", id))
}

/// Lists accounts, ordered by name. A closed account is excluded unless
/// `include_closed` is set - it stays out of the balances view and its
/// totals, since it no longer represents money the household can spend.
pub fn list(conn: &Connection, include_closed: bool) -> Result<Vec<Account>> {
    let sql = format!(
        "SELECT {SELECT_COLUMNS} FROM accounts {} ORDER BY name COLLATE NOCASE",
        if include_closed {
            ""
        } else {
            "WHERE is_closed = 0"
        }
    );
    let mut stmt = conn.prepare(&sql)?;
    let rows = stmt
        .query_map([], from_row)?
        .collect::<std::result::Result<Vec<_>, _>>()?;
    Ok(rows)
}

pub fn update(conn: &Connection, id: i64, new: &NewAccount) -> Result<()> {
    let name = new.name.trim();
    if name.is_empty() {
        return Err(CoreError::EmptyField { field: "name" });
    }
    let changed = conn.execute(
        "UPDATE accounts SET name = ?1, currency = ?2, iban = ?3, bic = ?4, holder_name = ?5, \
         branch_address = ?6, bank_id = ?7, opening_date = ?8, opening_balance_cleared = ?9 \
         WHERE id = ?10",
        params![
            name,
            new.currency,
            new.iban,
            new.bic,
            new.holder_name,
            new.branch_address,
            new.bank_id,
            new.opening_date,
            new.opening_balance_cleared.micros(),
            id,
        ],
    )?;
    if changed == 0 {
        return Err(CoreError::NotFound("account", id));
    }
    recompute_balances(conn, id)
}

pub fn close(conn: &Connection, id: i64, closing_date: &str) -> Result<()> {
    let changed = conn.execute(
        "UPDATE accounts SET is_closed = 1, closing_date = ?1 WHERE id = ?2",
        params![closing_date, id],
    )?;
    if changed == 0 {
        return Err(CoreError::NotFound("account", id));
    }
    Ok(())
}

pub fn reopen(conn: &Connection, id: i64) -> Result<()> {
    let changed = conn.execute(
        "UPDATE accounts SET is_closed = 0, closing_date = NULL WHERE id = ?1",
        params![id],
    )?;
    if changed == 0 {
        return Err(CoreError::NotFound("account", id));
    }
    Ok(())
}

/// Deletes the account and, through `ON DELETE CASCADE`, every transaction
/// that belongs to it.
pub fn delete(conn: &Connection, id: i64) -> Result<()> {
    let changed = conn.execute("DELETE FROM accounts WHERE id = ?1", params![id])?;
    if changed == 0 {
        return Err(CoreError::NotFound("account", id));
    }
    Ok(())
}

/// Recomputes the account's three balances, and every transaction's
/// `running_balance`, from scratch. Called after anything that adds,
/// removes, or edits a top-level transaction on this account.
///
/// `running_balance` tracks the *cleared* balance only, line by line in
/// chronological order - it is the reconciliation reference a bank
/// statement is checked against, so it only moves on cleared lines. The
/// account's `balance_committed` and `balance_projected` each layer the
/// next status on top of cleared, regardless of transaction order.
pub fn recompute_balances(conn: &Connection, account_id: i64) -> Result<()> {
    let account = get(conn, account_id)?;

    let mut stmt = conn.prepare(
        "SELECT id, amount, status FROM transactions \
         WHERE account_id = ?1 AND parent_id = 0 \
         ORDER BY (date IS NULL), date, id",
    )?;
    let rows = stmt
        .query_map(params![account_id], |row| {
            Ok((
                row.get::<_, i64>(0)?,
                row.get::<_, i64>(1)?,
                row.get::<_, i64>(2)?,
            ))
        })?
        .collect::<std::result::Result<Vec<_>, _>>()?;

    let mut running_cleared = account.opening_balance_cleared;
    let mut cleared_sum = Amount::ZERO;
    let mut committed_sum = Amount::ZERO;
    let mut planned_sum = Amount::ZERO;

    let mut update_running =
        conn.prepare("UPDATE transactions SET running_balance = ?1 WHERE id = ?2")?;
    for (id, amount, status) in rows {
        let amount = Amount::from_micros(amount);
        match Status::from_i64(status) {
            Status::Cleared => {
                cleared_sum += amount;
                running_cleared += amount;
            }
            Status::Committed => committed_sum += amount,
            Status::Planned => planned_sum += amount,
        }
        update_running.execute(params![running_cleared.micros(), id])?;
    }
    drop(update_running);

    let cleared = account.opening_balance_cleared + cleared_sum;
    let committed = account.opening_balance_committed + cleared_sum + committed_sum;
    let projected = account.opening_balance_projected + cleared_sum + committed_sum + planned_sum;

    conn.execute(
        "UPDATE accounts SET balance_cleared = ?1, balance_committed = ?2, balance_projected = ?3 WHERE id = ?4",
        params![cleared.micros(), committed.micros(), projected.micros(), account_id],
    )?;

    Ok(())
}

/// Sum of the `balance_projected` of every open account - what the
/// balances window and the dashboard show as "everything you have".
pub fn total_projected_balance(conn: &Connection) -> Result<Amount> {
    let total: i64 = conn.query_row(
        "SELECT COALESCE(SUM(balance_projected), 0) FROM accounts WHERE is_closed = 0",
        [],
        |r| r.get(0),
    )?;
    Ok(Amount::from_micros(total))
}
