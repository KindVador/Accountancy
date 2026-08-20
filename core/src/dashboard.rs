//! F01 - what the home screen shows: the three totals of the open accounts,
//! and this month's income and expense. No calculation of its own: it reads
//! the balances `accounts::recompute_balances` already keeps up to date,
//! and sums the current month's top-level transactions once.

use crate::dates;
use crate::error::Result;
use crate::money::Amount;
use rusqlite::{Connection, params};

#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub struct Dashboard {
    pub total_cleared: Amount,
    pub total_committed: Amount,
    pub total_projected: Amount,
    pub current_month_income: Amount,
    pub current_month_expense: Amount,
}

impl Dashboard {
    pub fn current_month_net(&self) -> Amount {
        self.current_month_income + self.current_month_expense
    }
}

/// Computes the dashboard as of today. Closed accounts are excluded from
/// the totals, same as `accounts::list`'s default and the balances window -
/// a closed account no longer represents money the household can spend.
pub fn compute(conn: &Connection) -> Result<Dashboard> {
    let today = dates::parse(&dates::today())?;
    let (month_start, month_end) = dates::month_bounds(today);

    let (cleared, committed, projected): (i64, i64, i64) = conn.query_row(
        "SELECT COALESCE(SUM(balance_cleared), 0), COALESCE(SUM(balance_committed), 0), \
         COALESCE(SUM(balance_projected), 0) FROM accounts WHERE is_closed = 0",
        [],
        |row| Ok((row.get(0)?, row.get(1)?, row.get(2)?)),
    )?;

    let income: i64 = conn.query_row(
        "SELECT COALESCE(SUM(t.amount), 0) FROM transactions t JOIN accounts a ON a.id = t.account_id \
         WHERE t.parent_id = 0 AND a.is_closed = 0 AND t.amount > 0 AND t.date BETWEEN ?1 AND ?2",
        params![month_start, month_end],
        |row| row.get(0),
    )?;
    let expense: i64 = conn.query_row(
        "SELECT COALESCE(SUM(t.amount), 0) FROM transactions t JOIN accounts a ON a.id = t.account_id \
         WHERE t.parent_id = 0 AND a.is_closed = 0 AND t.amount < 0 AND t.date BETWEEN ?1 AND ?2",
        params![month_start, month_end],
        |row| row.get(0),
    )?;

    Ok(Dashboard {
        total_cleared: Amount::from_micros(cleared),
        total_committed: Amount::from_micros(committed),
        total_projected: Amount::from_micros(projected),
        current_month_income: Amount::from_micros(income),
        current_month_expense: Amount::from_micros(expense),
    })
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::{accounts, dates, db, transactions};

    #[test]
    fn sums_open_accounts_and_excludes_closed_ones() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();

        accounts::create(
            conn,
            &accounts::NewAccount {
                name: "Checking".into(),
                currency: "EUR".into(),
                opening_balance_cleared: Amount::parse("100").unwrap(),
                ..Default::default()
            },
        )
        .unwrap();
        let closed_id = accounts::create(
            conn,
            &accounts::NewAccount {
                name: "Old".into(),
                currency: "EUR".into(),
                opening_balance_cleared: Amount::parse("500").unwrap(),
                ..Default::default()
            },
        )
        .unwrap();
        accounts::close(conn, closed_id, &dates::today()).unwrap();

        let dashboard = compute(conn).unwrap();
        assert_eq!(dashboard.total_cleared, Amount::parse("100").unwrap());
    }

    #[test]
    fn splits_this_months_transactions_into_income_and_expense() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = accounts::create(
            conn,
            &accounts::NewAccount {
                name: "Checking".into(),
                currency: "EUR".into(),
                ..Default::default()
            },
        )
        .unwrap();

        let (month_start, _) = dates::month_bounds(dates::parse(&dates::today()).unwrap());
        transactions::create(
            conn,
            &transactions::NewTransaction {
                account_id,
                label: "Salary".into(),
                date: Some(month_start.clone()),
                amount: Amount::parse("2000").unwrap(),
                ..Default::default()
            },
        )
        .unwrap();
        transactions::create(
            conn,
            &transactions::NewTransaction {
                account_id,
                label: "Rent".into(),
                date: Some(month_start),
                amount: Amount::parse("-800").unwrap(),
                ..Default::default()
            },
        )
        .unwrap();

        let dashboard = compute(conn).unwrap();
        assert_eq!(
            dashboard.current_month_income,
            Amount::parse("2000").unwrap()
        );
        assert_eq!(
            dashboard.current_month_expense,
            Amount::parse("-800").unwrap()
        );
        assert_eq!(
            dashboard.current_month_net(),
            Amount::parse("1200").unwrap()
        );
    }
}
