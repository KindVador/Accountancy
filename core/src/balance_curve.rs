//! The balance-over-time chart's data source: the cleared balance sampled
//! at every date it changed within a period, reusing the `running_balance`
//! column that `accounts::recompute_balances` already maintains rather than
//! recomputing anything.

use crate::accounts;
use crate::error::Result;
use crate::money::Amount;
use rusqlite::{Connection, OptionalExtension, params};

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct BalancePoint {
    pub date: String,
    pub balance: Amount,
}

/// The account's cleared balance at each date it changed, between
/// `start_date` and `end_date` (inclusive). The first point is always
/// dated `start_date`, carrying the balance as it stood at the start of
/// the period - the last `running_balance` before it, or the account's
/// opening balance if there is none - so the chart starts at the right
/// level instead of at zero. When several transactions share a date, only
/// the last one's running balance is kept, since that's the balance
/// actually reached by the end of that day.
pub fn balance_curve(
    conn: &Connection,
    account_id: i64,
    start_date: &str,
    end_date: &str,
) -> Result<Vec<BalancePoint>> {
    let account = accounts::get(conn, account_id)?;
    let opening_micros: i64 = conn
        .query_row(
            "SELECT running_balance FROM transactions \
             WHERE account_id = ?1 AND parent_id = 0 AND date IS NOT NULL AND date < ?2 \
             ORDER BY date DESC, id DESC LIMIT 1",
            params![account_id, start_date],
            |row| row.get(0),
        )
        .optional()?
        .unwrap_or_else(|| account.opening_balance_cleared.micros());

    let mut stmt = conn.prepare(
        "SELECT date, running_balance FROM transactions \
         WHERE account_id = ?1 AND parent_id = 0 AND date IS NOT NULL \
         AND date BETWEEN ?2 AND ?3 \
         ORDER BY date, id",
    )?;
    let rows = stmt
        .query_map(params![account_id, start_date, end_date], |row| {
            Ok((row.get::<_, String>(0)?, row.get::<_, i64>(1)?))
        })?
        .collect::<std::result::Result<Vec<_>, _>>()?;

    let mut points = vec![BalancePoint {
        date: start_date.to_string(),
        balance: Amount::from_micros(opening_micros),
    }];
    for (date, running_balance) in rows {
        let balance = Amount::from_micros(running_balance);
        if points.last().is_some_and(|p| p.date == date) {
            points.last_mut().unwrap().balance = balance;
        } else {
            points.push(BalancePoint { date, balance });
        }
    }
    Ok(points)
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::{db, transactions};

    fn new_account(conn: &Connection, opening: &str) -> i64 {
        accounts::create(
            conn,
            &accounts::NewAccount {
                name: "Checking".into(),
                currency: "EUR".into(),
                opening_balance_cleared: Amount::parse(opening).unwrap(),
                ..Default::default()
            },
        )
        .unwrap()
    }

    fn new_tx(conn: &Connection, account_id: i64, date: &str, amount: &str) -> i64 {
        transactions::create(
            conn,
            &transactions::NewTransaction {
                account_id,
                label: "Tx".into(),
                date: Some(date.into()),
                amount: Amount::parse(amount).unwrap(),
                status: Some(transactions::Status::Cleared),
                ..Default::default()
            },
        )
        .unwrap()
    }

    #[test]
    fn builds_a_step_curve_from_the_opening_balance() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn, "100");

        new_tx(conn, account_id, "2026-03-05", "-40");
        new_tx(conn, account_id, "2026-03-12", "20");

        let points = balance_curve(conn, account_id, "2026-03-01", "2026-03-31").unwrap();
        assert_eq!(points.len(), 3);
        assert_eq!(points[0].date, "2026-03-01");
        assert_eq!(points[0].balance, Amount::parse("100").unwrap());
        assert_eq!(points[1].date, "2026-03-05");
        assert_eq!(points[1].balance, Amount::parse("60").unwrap());
        assert_eq!(points[2].date, "2026-03-12");
        assert_eq!(points[2].balance, Amount::parse("80").unwrap());
    }

    #[test]
    fn same_date_transactions_keep_only_the_last_balance() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn, "0");

        new_tx(conn, account_id, "2026-03-05", "-40");
        new_tx(conn, account_id, "2026-03-05", "-10");

        let points = balance_curve(conn, account_id, "2026-03-01", "2026-03-31").unwrap();
        assert_eq!(points.len(), 2);
        assert_eq!(points[1].date, "2026-03-05");
        assert_eq!(points[1].balance, Amount::parse("-50").unwrap());
    }

    #[test]
    fn carries_the_balance_forward_into_the_period() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn, "0");

        new_tx(conn, account_id, "2026-02-15", "500");
        new_tx(conn, account_id, "2026-03-10", "-50");

        let points = balance_curve(conn, account_id, "2026-03-01", "2026-03-31").unwrap();
        assert_eq!(points.len(), 2);
        assert_eq!(points[0].date, "2026-03-01");
        assert_eq!(points[0].balance, Amount::parse("500").unwrap());
        assert_eq!(points[1].date, "2026-03-10");
        assert_eq!(points[1].balance, Amount::parse("450").unwrap());
    }

    #[test]
    fn excludes_split_lines_and_out_of_period_transactions() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn, "0");

        let parent = new_tx(conn, account_id, "2026-03-05", "-100");
        transactions::add_split_line(
            conn,
            parent,
            &transactions::NewSplitLine {
                label: "Wine".into(),
                amount: Amount::parse("-30").unwrap(),
                ..Default::default()
            },
        )
        .unwrap();
        new_tx(conn, account_id, "2026-04-01", "-999");

        let points = balance_curve(conn, account_id, "2026-03-01", "2026-03-31").unwrap();
        assert_eq!(points.len(), 2);
        assert_eq!(points[1].balance, Amount::parse("-100").unwrap());
    }
}
