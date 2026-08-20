//! Recurring rules: a schedule that produces planned transactions over
//! time. `forecast` previews the due dates without touching the database;
//! `insert_due` is the only function that actually creates transactions
//! from a rule and advances its progress. Both are built on the same
//! period/end-condition logic, so a preview and what actually gets
//! inserted can never disagree.

use crate::accounts;
use crate::dates::{self, Date};
use crate::error::{CoreError, Result};
use crate::money::Amount;
use crate::transactions::{self, NewTransaction, Status};
use rusqlite::{Connection, OptionalExtension, Row, params};

#[derive(Debug, Clone, Copy, PartialEq, Eq, Default, serde::Serialize, serde::Deserialize)]
pub enum Period {
    Day = 1,
    Week = 2,
    #[default]
    Month = 3,
    Quarter = 4,
    Year = 5,
}

impl Period {
    pub fn from_i64(value: i64) -> Period {
        match value {
            1 => Period::Day,
            2 => Period::Week,
            4 => Period::Quarter,
            5 => Period::Year,
            _ => Period::Month,
        }
    }

    pub fn as_i64(self) -> i64 {
        self as i64
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Default, serde::Serialize, serde::Deserialize)]
pub enum EndMode {
    #[default]
    NoLimit = 1,
    UntilDate = 2,
    OccurrenceCount = 3,
}

impl EndMode {
    pub fn from_i64(value: i64) -> EndMode {
        match value {
            2 => EndMode::UntilDate,
            3 => EndMode::OccurrenceCount,
            _ => EndMode::NoLimit,
        }
    }

    pub fn as_i64(self) -> i64 {
        self as i64
    }
}

/// Advances `date` by one occurrence of `period`, `frequency` times.
pub fn advance(date: Date, period: Period, frequency: i64) -> Date {
    match period {
        Period::Day => dates::add_days(date, frequency),
        Period::Week => dates::add_days(date, frequency * 7),
        Period::Month => dates::add_months(date, frequency),
        Period::Quarter => dates::add_months(date, frequency * 3),
        Period::Year => dates::add_months(date, frequency * 12),
    }
}

#[derive(Debug, Clone, PartialEq, Eq, serde::Serialize, serde::Deserialize)]
pub struct RecurringRule {
    pub id: i64,
    pub account_id: i64,
    pub label: String,
    pub amount: Amount,
    pub category_id: i64,
    pub payee_id: i64,
    pub payment_mode_id: i64,
    pub tag_id: i64,
    pub project_id: i64,
    pub comment: Option<String>,
    pub period: Period,
    pub period_frequency: i64,
    pub end_mode: EndMode,
    pub end_date: Option<String>,
    /// The total number of transactions this rule should ever generate,
    /// when `end_mode` is `OccurrenceCount`.
    pub occurrence_count: Option<i64>,
    pub next_due_date: String,
    pub last_generated_date: Option<String>,
    pub generated_count: i64,
}

#[derive(Debug, Clone, Default)]
pub struct NewRecurringRule {
    pub account_id: i64,
    pub label: String,
    pub amount: Amount,
    pub category_id: i64,
    pub payee_id: i64,
    pub payment_mode_id: i64,
    pub tag_id: i64,
    pub project_id: i64,
    pub comment: Option<String>,
    pub period: Period,
    pub period_frequency: i64,
    pub end_mode: EndMode,
    pub end_date: Option<String>,
    pub occurrence_count: Option<i64>,
    /// Where the schedule starts. Only used by `create` - `update` leaves
    /// `next_due_date` alone, since editing a rule shouldn't silently
    /// rewind or fast-forward its progress.
    pub start_date: String,
}

fn from_row(row: &Row) -> rusqlite::Result<RecurringRule> {
    Ok(RecurringRule {
        id: row.get(0)?,
        account_id: row.get(1)?,
        label: row.get(2)?,
        amount: Amount::from_micros(row.get(3)?),
        category_id: row.get(4)?,
        payee_id: row.get(5)?,
        payment_mode_id: row.get(6)?,
        tag_id: row.get(7)?,
        project_id: row.get(8)?,
        comment: row.get(9)?,
        period: Period::from_i64(row.get(10)?),
        period_frequency: row.get(11)?,
        end_mode: EndMode::from_i64(row.get(12)?),
        end_date: row.get(13)?,
        occurrence_count: row.get(14)?,
        next_due_date: row.get(15)?,
        last_generated_date: row.get(16)?,
        generated_count: row.get(17)?,
    })
}

const SELECT_COLUMNS: &str = "id, account_id, label, amount, category_id, payee_id, payment_mode_id, \
     tag_id, project_id, comment, period, period_frequency, end_mode, end_date, occurrence_count, \
     next_due_date, last_generated_date, generated_count";

fn validate(new: &NewRecurringRule) -> Result<&str> {
    let label = new.label.trim();
    if label.is_empty() {
        return Err(CoreError::EmptyField { field: "label" });
    }
    if new.period_frequency < 1 {
        return Err(CoreError::Invalid(
            "period frequency must be at least 1".into(),
        ));
    }
    Ok(label)
}

pub fn create(conn: &Connection, new: &NewRecurringRule) -> Result<i64> {
    let label = validate(new)?;
    dates::parse(&new.start_date)?;
    conn.execute(
        "INSERT INTO recurring_rules (account_id, label, amount, category_id, payee_id, payment_mode_id, \
         tag_id, project_id, comment, period, period_frequency, end_mode, end_date, occurrence_count, \
         next_due_date) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14, ?15)",
        params![
            new.account_id,
            label,
            new.amount.micros(),
            new.category_id,
            new.payee_id,
            new.payment_mode_id,
            new.tag_id,
            new.project_id,
            new.comment,
            new.period.as_i64(),
            new.period_frequency,
            new.end_mode.as_i64(),
            new.end_date,
            new.occurrence_count,
            new.start_date,
        ],
    )?;
    Ok(conn.last_insert_rowid())
}

pub fn get(conn: &Connection, id: i64) -> Result<RecurringRule> {
    let sql = format!("SELECT {SELECT_COLUMNS} FROM recurring_rules WHERE id = ?1");
    conn.query_row(&sql, params![id], from_row)
        .optional()?
        .ok_or(CoreError::NotFound("recurring rule", id))
}

/// Lists recurring rules, optionally restricted to one account.
pub fn list(conn: &Connection, account_id: Option<i64>) -> Result<Vec<RecurringRule>> {
    let sql = format!(
        "SELECT {SELECT_COLUMNS} FROM recurring_rules {} ORDER BY next_due_date, id",
        if account_id.is_some() {
            "WHERE account_id = ?1"
        } else {
            ""
        }
    );
    let mut stmt = conn.prepare(&sql)?;
    let rows = match account_id {
        Some(id) => stmt
            .query_map(params![id], from_row)?
            .collect::<std::result::Result<Vec<_>, _>>()?,
        None => stmt
            .query_map([], from_row)?
            .collect::<std::result::Result<Vec<_>, _>>()?,
    };
    Ok(rows)
}

/// Updates a rule's definition. Progress fields (`next_due_date`,
/// `last_generated_date`, `generated_count`) are untouched - only
/// `insert_due` advances them.
pub fn update(conn: &Connection, id: i64, new: &NewRecurringRule) -> Result<()> {
    let label = validate(new)?;
    let changed = conn.execute(
        "UPDATE recurring_rules SET account_id = ?1, label = ?2, amount = ?3, category_id = ?4, \
         payee_id = ?5, payment_mode_id = ?6, tag_id = ?7, project_id = ?8, comment = ?9, period = ?10, \
         period_frequency = ?11, end_mode = ?12, end_date = ?13, occurrence_count = ?14 WHERE id = ?15",
        params![
            new.account_id,
            label,
            new.amount.micros(),
            new.category_id,
            new.payee_id,
            new.payment_mode_id,
            new.tag_id,
            new.project_id,
            new.comment,
            new.period.as_i64(),
            new.period_frequency,
            new.end_mode.as_i64(),
            new.end_date,
            new.occurrence_count,
            id,
        ],
    )?;
    if changed == 0 {
        return Err(CoreError::NotFound("recurring rule", id));
    }
    Ok(())
}

pub fn delete(conn: &Connection, id: i64) -> Result<()> {
    let changed = conn.execute("DELETE FROM recurring_rules WHERE id = ?1", params![id])?;
    if changed == 0 {
        return Err(CoreError::NotFound("recurring rule", id));
    }
    Ok(())
}

/// The due dates a rule would produce from its current `next_due_date` up
/// to and including `horizon`, respecting its end condition. Read-only:
/// looking at a forecast never changes the rule.
pub fn forecast(rule: &RecurringRule, horizon: &str) -> Result<Vec<String>> {
    let horizon = dates::parse(horizon)?;
    let end_date = rule.end_date.as_deref().map(dates::parse).transpose()?;

    let mut due = dates::parse(&rule.next_due_date)?;
    let mut generated = rule.generated_count;
    let mut result = Vec::new();

    while due <= horizon {
        if let (EndMode::UntilDate, Some(end)) = (rule.end_mode, end_date)
            && due > end
        {
            break;
        }
        if let (EndMode::OccurrenceCount, Some(limit)) = (rule.end_mode, rule.occurrence_count)
            && generated >= limit
        {
            break;
        }
        result.push(dates::format(due));
        due = advance(due, rule.period, rule.period_frequency);
        generated += 1;
    }
    Ok(result)
}

/// Creates a planned transaction for every date `forecast` returns up to
/// `up_to`, tags each with `recurring_rule_id`, and advances the rule's
/// `next_due_date` / `last_generated_date` / `generated_count` past the
/// last one inserted. Returns the ids of the transactions created.
pub fn insert_due(conn: &Connection, rule_id: i64, up_to: &str) -> Result<Vec<i64>> {
    let rule = get(conn, rule_id)?;
    let due_dates = forecast(&rule, up_to)?;
    let mut inserted = Vec::with_capacity(due_dates.len());

    for date in &due_dates {
        let tx_id = transactions::create(
            conn,
            &NewTransaction {
                account_id: rule.account_id,
                label: rule.label.clone(),
                date: Some(date.clone()),
                amount: rule.amount,
                status: Some(Status::Planned),
                category_id: rule.category_id,
                payee_id: rule.payee_id,
                payment_mode_id: rule.payment_mode_id,
                tag_id: rule.tag_id,
                project_id: rule.project_id,
                comment: rule.comment.clone(),
                ..Default::default()
            },
        )?;
        conn.execute(
            "UPDATE transactions SET recurring_rule_id = ?1 WHERE id = ?2",
            params![rule_id, tx_id],
        )?;
        inserted.push(tx_id);
    }

    if let Some(last) = due_dates.last() {
        let next = advance(dates::parse(last)?, rule.period, rule.period_frequency);
        conn.execute(
            "UPDATE recurring_rules SET next_due_date = ?1, last_generated_date = ?2, generated_count = ?3 \
             WHERE id = ?4",
            params![dates::format(next), last, rule.generated_count + due_dates.len() as i64, rule_id],
        )?;
        accounts::recompute_balances(conn, rule.account_id)?;
    }

    Ok(inserted)
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

    fn monthly_rule(account_id: i64, start: &str) -> NewRecurringRule {
        NewRecurringRule {
            account_id,
            label: "Rent".into(),
            amount: Amount::parse("-800").unwrap(),
            period: Period::Month,
            period_frequency: 1,
            start_date: start.into(),
            ..Default::default()
        }
    }

    #[test]
    fn forecast_lists_due_dates_without_touching_the_database() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn);

        let rule_id = create(conn, &monthly_rule(account_id, "2026-01-01")).unwrap();
        let rule = get(conn, rule_id).unwrap();

        let due = forecast(&rule, "2026-04-01").unwrap();
        assert_eq!(
            due,
            vec!["2026-01-01", "2026-02-01", "2026-03-01", "2026-04-01"]
        );
        // Forecasting doesn't mutate the rule.
        assert_eq!(get(conn, rule_id).unwrap().next_due_date, "2026-01-01");
    }

    #[test]
    fn insert_due_creates_transactions_and_advances_the_rule() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn);

        let rule_id = create(conn, &monthly_rule(account_id, "2026-01-01")).unwrap();
        let ids = insert_due(conn, rule_id, "2026-03-15").unwrap();
        assert_eq!(ids.len(), 3);

        let rule = get(conn, rule_id).unwrap();
        assert_eq!(rule.next_due_date, "2026-04-01");
        assert_eq!(rule.last_generated_date.as_deref(), Some("2026-03-01"));
        assert_eq!(rule.generated_count, 3);

        let transactions = transactions::list_for_account(conn, account_id).unwrap();
        assert_eq!(transactions.len(), 3);
        assert!(transactions.iter().all(|t| t.status == Status::Planned));

        let account = accounts::get(conn, account_id).unwrap();
        assert_eq!(account.balance_projected, Amount::parse("-2400").unwrap());

        // Running it again with the same horizon inserts nothing more.
        assert!(insert_due(conn, rule_id, "2026-03-15").unwrap().is_empty());
    }

    #[test]
    fn occurrence_count_stops_the_schedule() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn);

        let mut new_rule = monthly_rule(account_id, "2026-01-01");
        new_rule.end_mode = EndMode::OccurrenceCount;
        new_rule.occurrence_count = Some(2);
        let rule_id = create(conn, &new_rule).unwrap();

        let ids = insert_due(conn, rule_id, "2026-12-31").unwrap();
        assert_eq!(ids.len(), 2);
        assert!(insert_due(conn, rule_id, "2027-12-31").unwrap().is_empty());
    }

    #[test]
    fn until_date_stops_the_schedule() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn);

        let mut new_rule = monthly_rule(account_id, "2026-01-01");
        new_rule.end_mode = EndMode::UntilDate;
        new_rule.end_date = Some("2026-02-15".into());
        let rule_id = create(conn, &new_rule).unwrap();
        let rule = get(conn, rule_id).unwrap();

        let due = forecast(&rule, "2026-12-31").unwrap();
        assert_eq!(due, vec!["2026-01-01", "2026-02-01"]);
    }
}
