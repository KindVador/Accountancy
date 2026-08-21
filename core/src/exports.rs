//! Exporting an account's transactions to a handful of plain-text formats.
//! Every format is built from the same [`ExportLine`] list, resolved once
//! from the database (soft-linked ids turned into names) so CSV, TSV, JSON
//! and QIF can never disagree with each other about what a line says.

use crate::error::Result;
use crate::money::Amount;
use rusqlite::{Connection, params};

#[derive(Debug, Clone, PartialEq, Eq, serde::Serialize)]
pub struct ExportLine {
    pub date: Option<String>,
    pub label: String,
    pub category: String,
    pub payee: String,
    pub amount: Amount,
    pub status: String,
    pub comment: Option<String>,
}

fn status_label(status: i64) -> &'static str {
    match status {
        1 => "Cleared",
        3 => "Planned",
        _ => "Committed",
    }
}

/// The top-level transactions of an account, in the order the transaction
/// list shows them, with every soft-linked id resolved to a name. Split
/// lines are never included - same rule as everywhere else: exporting a
/// child too would export the same money twice.
pub fn export_lines(conn: &Connection, account_id: i64) -> Result<Vec<ExportLine>> {
    let mut stmt = conn.prepare(
        "SELECT t.date, t.label, COALESCE(c.name, 'Unspecified'), COALESCE(p.name, 'Unspecified'), \
         t.amount, t.status, t.comment \
         FROM transactions t \
         LEFT JOIN categories c ON c.id = t.category_id \
         LEFT JOIN payees p ON p.id = t.payee_id \
         WHERE t.account_id = ?1 AND t.parent_id = 0 \
         ORDER BY (t.date IS NULL), t.date, t.id",
    )?;
    let rows = stmt
        .query_map(params![account_id], |row| {
            Ok(ExportLine {
                date: row.get(0)?,
                label: row.get(1)?,
                category: row.get(2)?,
                payee: row.get(3)?,
                amount: Amount::from_micros(row.get(4)?),
                status: status_label(row.get(5)?).to_string(),
                comment: row.get(6)?,
            })
        })?
        .collect::<std::result::Result<Vec<_>, _>>()?;
    Ok(rows)
}

fn escape_delimited_field(field: &str, delimiter: char) -> String {
    if field.contains(delimiter) || field.contains('"') || field.contains('\n') {
        format!("\"{}\"", field.replace('"', "\"\""))
    } else {
        field.to_string()
    }
}

fn to_delimited(lines: &[ExportLine], delimiter: char) -> String {
    let mut out = String::new();
    out.push_str(
        &[
            "Date", "Label", "Category", "Payee", "Amount", "Status", "Comment",
        ]
        .join(&delimiter.to_string()),
    );
    out.push('\n');
    for line in lines {
        let fields = [
            line.date.clone().unwrap_or_default(),
            line.label.clone(),
            line.category.clone(),
            line.payee.clone(),
            line.amount.to_string(),
            line.status.clone(),
            line.comment.clone().unwrap_or_default(),
        ];
        out.push_str(
            &fields
                .iter()
                .map(|f| escape_delimited_field(f, delimiter))
                .collect::<Vec<_>>()
                .join(&delimiter.to_string()),
        );
        out.push('\n');
    }
    out
}

pub fn to_csv(lines: &[ExportLine]) -> String {
    to_delimited(lines, ',')
}

pub fn to_tsv(lines: &[ExportLine]) -> String {
    to_delimited(lines, '\t')
}

pub fn to_json(lines: &[ExportLine]) -> Result<String> {
    Ok(serde_json::to_string_pretty(lines)?)
}

/// `YYYY-MM-DD` to QIF's traditional `MM/DD/YYYY` - the format the widest
/// range of importers (Quicken-derived or not) reads correctly.
fn qif_date(date: &str) -> String {
    let parts: Vec<&str> = date.split('-').collect();
    match parts.as_slice() {
        [year, month, day] => format!("{month}/{day}/{year}"),
        _ => date.to_string(),
    }
}

/// `!Type:Bank` QIF - one `D`/`T`/`P`/`L`/`M` block per transaction, closed
/// by `^`. The oldest widely-supported personal-finance interchange format,
/// and still the one most accounting software can read.
pub fn to_qif(lines: &[ExportLine]) -> String {
    let mut out = String::from("!Type:Bank\n");
    for line in lines {
        if let Some(date) = &line.date {
            out.push_str(&format!("D{}\n", qif_date(date)));
        }
        out.push_str(&format!("T{}\n", line.amount));
        if line.payee != "Unspecified" {
            out.push_str(&format!("P{}\n", line.payee));
        }
        if line.category != "Unspecified" {
            out.push_str(&format!("L{}\n", line.category));
        }
        if let Some(comment) = &line.comment {
            out.push_str(&format!("M{comment}\n"));
        }
        out.push_str("^\n");
    }
    out
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::{accounts, db, reference_lists, transactions};

    fn sample_lines(conn: &Connection) -> Vec<ExportLine> {
        let account_id = accounts::create(
            conn,
            &accounts::NewAccount {
                name: "Checking".into(),
                currency: "EUR".into(),
                ..Default::default()
            },
        )
        .unwrap();
        let groceries = reference_lists::create_category(conn, "Groceries", 0).unwrap();
        transactions::create(
            conn,
            &transactions::NewTransaction {
                account_id,
                label: "Supermarket, weekly".into(),
                date: Some("2026-03-05".into()),
                amount: Amount::parse("-45.67").unwrap(),
                category_id: groceries,
                comment: Some("has a \"quote\"".into()),
                ..Default::default()
            },
        )
        .unwrap();
        export_lines(conn, account_id).unwrap()
    }

    #[test]
    fn csv_quotes_fields_containing_the_delimiter_or_quotes() {
        let db = db::open_in_memory().unwrap();
        let lines = sample_lines(db.connection());

        let csv = to_csv(&lines);
        assert!(csv.contains("\"Supermarket, weekly\""));
        assert!(csv.contains("\"has a \"\"quote\"\"\""));
    }

    #[test]
    fn tsv_uses_tabs_and_does_not_quote_commas() {
        let db = db::open_in_memory().unwrap();
        let lines = sample_lines(db.connection());

        let tsv = to_tsv(&lines);
        assert!(tsv.contains("Supermarket, weekly\tGroceries"));
    }

    #[test]
    fn json_round_trips_the_amount_as_micros() {
        let db = db::open_in_memory().unwrap();
        let lines = sample_lines(db.connection());

        let json = to_json(&lines).unwrap();
        let parsed: serde_json::Value = serde_json::from_str(&json).unwrap();
        assert_eq!(parsed[0]["label"], "Supermarket, weekly");
    }

    #[test]
    fn qif_formats_the_date_and_skips_unspecified_fields() {
        let db = db::open_in_memory().unwrap();
        let lines = sample_lines(db.connection());

        let qif = to_qif(&lines);
        assert!(qif.starts_with("!Type:Bank\n"));
        assert!(qif.contains("D03/05/2026\n"));
        assert!(qif.contains("T-45.67\n"));
        assert!(qif.contains("LGroceries\n"));
        assert!(!qif.contains("PUnspecified"));
        assert!(qif.contains("^\n"));
    }

    #[test]
    fn export_lines_excludes_split_children() {
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
        let parent = transactions::create(
            conn,
            &transactions::NewTransaction {
                account_id,
                label: "Big shop".into(),
                amount: Amount::parse("-100").unwrap(),
                ..Default::default()
            },
        )
        .unwrap();
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

        let lines = export_lines(conn, account_id).unwrap();
        assert_eq!(lines.len(), 1);
        assert_eq!(lines[0].label, "Big shop");
    }
}
