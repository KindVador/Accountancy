//! Budget: an amount entered per budget line, per calendar month
//! (`set_amount` / `entries_for_month`), checked against what actually
//! happened (`control_for_month`). The link from a transaction to a budget
//! line is indirect - transaction -> category -> budget line - so moving a
//! category to a different budget line moves its past spending with it.

use crate::error::{CoreError, Result};
use crate::money::Amount;
use rusqlite::{Connection, params};

#[derive(Debug, Clone, PartialEq, Eq, serde::Serialize, serde::Deserialize)]
pub struct BudgetLineAmount {
    pub budget_line_id: i64,
    pub budget_line_name: String,
    pub budgeted: Amount,
}

#[derive(Debug, Clone, PartialEq, Eq, serde::Serialize, serde::Deserialize)]
pub struct BudgetControlLine {
    pub budget_line_id: i64,
    pub budget_line_name: String,
    pub budgeted: Amount,
    /// Sum of the top-level transactions whose category points at this
    /// budget line, for the month. Carries the same debit/credit sign as
    /// the transactions themselves, so it's directly comparable to
    /// `budgeted` - see `docs/data_model.md`.
    pub actual: Amount,
}

impl BudgetControlLine {
    pub fn difference(&self) -> Amount {
        self.actual - self.budgeted
    }
}

fn validate_month(month: i32) -> Result<()> {
    if !(1..=12).contains(&month) {
        return Err(CoreError::InvalidMonth(month));
    }
    Ok(())
}

/// Sets the budgeted amount for one budget line in one month (upsert).
pub fn set_amount(
    conn: &Connection,
    year: i32,
    month: i32,
    budget_line_id: i64,
    amount: Amount,
) -> Result<()> {
    validate_month(month)?;
    conn.execute(
        "INSERT INTO budget_entries (year, month, budget_line_id, amount) VALUES (?1, ?2, ?3, ?4) \
         ON CONFLICT (year, month, budget_line_id) DO UPDATE SET amount = excluded.amount",
        params![year, month, budget_line_id, amount.micros()],
    )?;
    Ok(())
}

/// Every budget line with its budgeted amount for the month (`0` for a
/// line that has no entry yet) - what the budget entry screen shows.
pub fn entries_for_month(
    conn: &Connection,
    year: i32,
    month: i32,
) -> Result<Vec<BudgetLineAmount>> {
    validate_month(month)?;
    let mut stmt = conn.prepare(
        "SELECT bl.id, bl.name, COALESCE(be.amount, 0) \
         FROM budget_lines bl \
         LEFT JOIN budget_entries be ON be.budget_line_id = bl.id AND be.year = ?1 AND be.month = ?2 \
         ORDER BY bl.name COLLATE NOCASE",
    )?;
    let rows = stmt
        .query_map(params![year, month], |row| {
            Ok(BudgetLineAmount {
                budget_line_id: row.get(0)?,
                budget_line_name: row.get(1)?,
                budgeted: Amount::from_micros(row.get(2)?),
            })
        })?
        .collect::<std::result::Result<Vec<_>, _>>()?;
    Ok(rows)
}

/// Every budget line with its budgeted amount and what actually happened
/// that month - what the budget control screen shows.
pub fn control_for_month(
    conn: &Connection,
    year: i32,
    month: i32,
) -> Result<Vec<BudgetControlLine>> {
    validate_month(month)?;
    let year_text = format!("{year:04}");
    let month_text = format!("{month:02}");
    let mut stmt = conn.prepare(
        "SELECT bl.id, bl.name, COALESCE(be.amount, 0), COALESCE((\
             SELECT SUM(t.amount) FROM transactions t \
             JOIN categories c ON c.id = t.category_id \
             WHERE c.budget_line_id = bl.id \
               AND t.parent_id = 0 \
               AND substr(t.date, 1, 4) = ?3 \
               AND substr(t.date, 6, 2) = ?4\
         ), 0) \
         FROM budget_lines bl \
         LEFT JOIN budget_entries be ON be.budget_line_id = bl.id AND be.year = ?1 AND be.month = ?2 \
         ORDER BY bl.name COLLATE NOCASE",
    )?;
    let rows = stmt
        .query_map(params![year, month, year_text, month_text], |row| {
            Ok(BudgetControlLine {
                budget_line_id: row.get(0)?,
                budget_line_name: row.get(1)?,
                budgeted: Amount::from_micros(row.get(2)?),
                actual: Amount::from_micros(row.get(3)?),
            })
        })?
        .collect::<std::result::Result<Vec<_>, _>>()?;
    Ok(rows)
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::{accounts, db, reference_lists, transactions};

    #[test]
    fn set_amount_is_visible_in_entries_for_month() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let line_id = reference_lists::create(
            conn,
            reference_lists::ReferenceKind::BudgetLine,
            "Groceries",
        )
        .unwrap();

        set_amount(conn, 2026, 3, line_id, Amount::parse("-400").unwrap()).unwrap();

        let entries = entries_for_month(conn, 2026, 3).unwrap();
        assert_eq!(entries.len(), 1);
        assert_eq!(entries[0].budgeted, Amount::parse("-400").unwrap());

        // A different month still shows the line, unbudgeted.
        let other_month = entries_for_month(conn, 2026, 4).unwrap();
        assert_eq!(other_month[0].budgeted, Amount::ZERO);
    }

    #[test]
    fn set_amount_upserts_rather_than_duplicating() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let line_id = reference_lists::create(
            conn,
            reference_lists::ReferenceKind::BudgetLine,
            "Groceries",
        )
        .unwrap();

        set_amount(conn, 2026, 3, line_id, Amount::parse("-400").unwrap()).unwrap();
        set_amount(conn, 2026, 3, line_id, Amount::parse("-450").unwrap()).unwrap();

        let entries = entries_for_month(conn, 2026, 3).unwrap();
        assert_eq!(entries.len(), 1);
        assert_eq!(entries[0].budgeted, Amount::parse("-450").unwrap());
    }

    #[test]
    fn control_sums_only_top_level_transactions_in_the_month() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let line_id = reference_lists::create(
            conn,
            reference_lists::ReferenceKind::BudgetLine,
            "Groceries",
        )
        .unwrap();
        let category_id = reference_lists::create_category(conn, "Supermarket", line_id).unwrap();
        let account_id = accounts::create(
            conn,
            &accounts::NewAccount {
                name: "Checking".into(),
                currency: "EUR".into(),
                ..Default::default()
            },
        )
        .unwrap();

        set_amount(conn, 2026, 3, line_id, Amount::parse("-400").unwrap()).unwrap();

        let march_parent = transactions::create(
            conn,
            &transactions::NewTransaction {
                account_id,
                label: "Big shop".into(),
                date: Some("2026-03-10".into()),
                amount: Amount::parse("-120").unwrap(),
                category_id,
                ..Default::default()
            },
        )
        .unwrap();
        // A split line under the March transaction must not be double-counted.
        transactions::add_split_line(
            conn,
            march_parent,
            &transactions::NewSplitLine {
                label: "Wine".into(),
                amount: Amount::parse("-20").unwrap(),
                category_id,
                ..Default::default()
            },
        )
        .unwrap();
        // A transaction in a different month must not leak into March.
        transactions::create(
            conn,
            &transactions::NewTransaction {
                account_id,
                label: "April shop".into(),
                date: Some("2026-04-01".into()),
                amount: Amount::parse("-999").unwrap(),
                category_id,
                ..Default::default()
            },
        )
        .unwrap();

        let control = control_for_month(conn, 2026, 3).unwrap();
        assert_eq!(control.len(), 1);
        assert_eq!(control[0].budgeted, Amount::parse("-400").unwrap());
        assert_eq!(control[0].actual, Amount::parse("-120").unwrap());
        assert_eq!(control[0].difference(), Amount::parse("280").unwrap());
    }

    #[test]
    fn rejects_an_out_of_range_month() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        assert!(matches!(
            entries_for_month(conn, 2026, 13),
            Err(CoreError::InvalidMonth(13))
        ));
    }
}
