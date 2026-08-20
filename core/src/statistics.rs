//! Cross-tabulated statistics: top-level transactions in a period, summed
//! by one of six axes. Same rule as everywhere else in the core - only
//! `parent_id = 0` transactions are counted, so a split line never doubles
//! its parent's amount into the total.

use crate::error::Result;
use crate::money::Amount;
use rusqlite::{Connection, params};

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Axis {
    Category,
    Tag,
    PaymentMode,
    Payee,
    Project,
    BudgetLine,
}

impl Axis {
    /// The reference table and the column on `transactions` that links to
    /// it. `BudgetLine` has no direct column - it's handled separately in
    /// `compute`, since it's reached through `categories.budget_line_id`.
    fn simple_join(self) -> Option<(&'static str, &'static str)> {
        match self {
            Axis::Category => Some(("categories", "category_id")),
            Axis::Tag => Some(("tags", "tag_id")),
            Axis::PaymentMode => Some(("payment_modes", "payment_mode_id")),
            Axis::Payee => Some(("payees", "payee_id")),
            Axis::Project => Some(("projects", "project_id")),
            Axis::BudgetLine => None,
        }
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct StatisticLine {
    pub label: String,
    pub total: Amount,
}

/// Sums top-level transactions between `start_date` and `end_date`
/// (inclusive), grouped by `axis`, optionally restricted to one account.
/// A transaction whose axis value is unset groups under "Unspecified".
pub fn compute(
    conn: &Connection,
    axis: Axis,
    account_id: Option<i64>,
    start_date: &str,
    end_date: &str,
) -> Result<Vec<StatisticLine>> {
    let account_clause = if account_id.is_some() {
        "AND t.account_id = ?3"
    } else {
        ""
    };

    let sql = match axis.simple_join() {
        Some((table, column)) => format!(
            "SELECT COALESCE(r.name, 'Unspecified') AS label, COALESCE(SUM(t.amount), 0) AS total \
             FROM transactions t \
             LEFT JOIN {table} r ON r.id = t.{column} \
             WHERE t.parent_id = 0 AND t.date IS NOT NULL AND t.date BETWEEN ?1 AND ?2 {account_clause} \
             GROUP BY t.{column} \
             ORDER BY label COLLATE NOCASE"
        ),
        None => format!(
            "SELECT COALESCE(bl.name, 'Unspecified') AS label, COALESCE(SUM(t.amount), 0) AS total \
             FROM transactions t \
             LEFT JOIN categories cat ON cat.id = t.category_id \
             LEFT JOIN budget_lines bl ON bl.id = cat.budget_line_id \
             WHERE t.parent_id = 0 AND t.date IS NOT NULL AND t.date BETWEEN ?1 AND ?2 {account_clause} \
             GROUP BY bl.id \
             ORDER BY label COLLATE NOCASE"
        ),
    };

    let mut stmt = conn.prepare(&sql)?;
    let rows = |row: &rusqlite::Row| {
        Ok(StatisticLine {
            label: row.get(0)?,
            total: Amount::from_micros(row.get(1)?),
        })
    };
    let lines = match account_id {
        Some(id) => stmt
            .query_map(params![start_date, end_date, id], rows)?
            .collect::<std::result::Result<Vec<_>, _>>()?,
        None => stmt
            .query_map(params![start_date, end_date], rows)?
            .collect::<std::result::Result<Vec<_>, _>>()?,
    };
    Ok(lines)
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::{accounts, db, reference_lists, transactions};

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

    fn new_tx(
        conn: &Connection,
        account_id: i64,
        label: &str,
        date: &str,
        amount: &str,
        category_id: i64,
    ) -> i64 {
        transactions::create(
            conn,
            &transactions::NewTransaction {
                account_id,
                label: label.into(),
                date: Some(date.into()),
                amount: Amount::parse(amount).unwrap(),
                category_id,
                ..Default::default()
            },
        )
        .unwrap()
    }

    #[test]
    fn groups_by_category_and_unspecified() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn);
        let groceries = reference_lists::create_category(conn, "Groceries", 0).unwrap();

        new_tx(conn, account_id, "Shop 1", "2026-03-05", "-40", groceries);
        new_tx(conn, account_id, "Shop 2", "2026-03-12", "-25", groceries);
        new_tx(conn, account_id, "Cash", "2026-03-20", "-10", 0);

        let lines = compute(conn, Axis::Category, None, "2026-03-01", "2026-03-31").unwrap();
        assert_eq!(lines.len(), 2);
        let groceries_line = lines.iter().find(|l| l.label == "Groceries").unwrap();
        assert_eq!(groceries_line.total, Amount::parse("-65").unwrap());
        let unspecified = lines.iter().find(|l| l.label == "Unspecified").unwrap();
        assert_eq!(unspecified.total, Amount::parse("-10").unwrap());
    }

    #[test]
    fn excludes_split_lines_and_out_of_period_transactions() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn);
        let groceries = reference_lists::create_category(conn, "Groceries", 0).unwrap();

        let parent = new_tx(
            conn,
            account_id,
            "Big shop",
            "2026-03-05",
            "-100",
            groceries,
        );
        transactions::add_split_line(
            conn,
            parent,
            &transactions::NewSplitLine {
                label: "Wine".into(),
                amount: Amount::parse("-30").unwrap(),
                category_id: groceries,
                ..Default::default()
            },
        )
        .unwrap();
        new_tx(
            conn,
            account_id,
            "April shop",
            "2026-04-01",
            "-999",
            groceries,
        );

        let lines = compute(conn, Axis::Category, None, "2026-03-01", "2026-03-31").unwrap();
        assert_eq!(lines.len(), 1);
        assert_eq!(lines[0].total, Amount::parse("-100").unwrap());
    }

    #[test]
    fn budget_line_axis_follows_category_to_budget_line() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn);
        let food_line =
            reference_lists::create(conn, reference_lists::ReferenceKind::BudgetLine, "Food")
                .unwrap();
        let groceries = reference_lists::create_category(conn, "Groceries", food_line).unwrap();
        let restaurants = reference_lists::create_category(conn, "Restaurants", food_line).unwrap();

        new_tx(conn, account_id, "Shop", "2026-03-05", "-40", groceries);
        new_tx(conn, account_id, "Dinner", "2026-03-06", "-60", restaurants);

        let lines = compute(conn, Axis::BudgetLine, None, "2026-03-01", "2026-03-31").unwrap();
        assert_eq!(lines.len(), 1);
        assert_eq!(lines[0].label, "Food");
        assert_eq!(lines[0].total, Amount::parse("-100").unwrap());
    }

    #[test]
    fn account_filter_restricts_the_totals() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_a = new_account(conn);
        let account_b = new_account(conn);
        let groceries = reference_lists::create_category(conn, "Groceries", 0).unwrap();

        new_tx(conn, account_a, "Shop A", "2026-03-05", "-40", groceries);
        new_tx(conn, account_b, "Shop B", "2026-03-06", "-999", groceries);

        let lines = compute(
            conn,
            Axis::Category,
            Some(account_a),
            "2026-03-01",
            "2026-03-31",
        )
        .unwrap();
        assert_eq!(lines.len(), 1);
        assert_eq!(lines[0].total, Amount::parse("-40").unwrap());
    }
}
