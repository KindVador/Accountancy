//! Bank statement import. OFX, CSV and Excel parsing each produce a plain
//! list of [`ImportedTransaction`]; [`import`] is the single pipeline that
//! writes them into an account, deduplicating on the bank's own transaction
//! id when one is available. Reconciliation only ever needs that one rule:
//! never re-import the same statement line twice.

use crate::accounts;
use crate::error::Result;
use crate::money::Amount;
use crate::transactions::Status;
use rusqlite::{Connection, params};

/// How the three date components are ordered in a source column - a bank's
/// own choice, not something CSV or Excel encode. Shared by `csv` and
/// `excel`, since a spreadsheet exported "as text" has exactly the same
/// ambiguity a CSV file does.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum DateOrder {
    YearMonthDay,
    DayMonthYear,
    MonthDayYear,
}

/// Parses a `raw` date built only from digits and separators (`/`, `-`,
/// `.`, whitespace...) according to `order`. A two-digit year is read as
/// 20xx.
fn parse_date_field(raw: &str, order: DateOrder) -> Option<String> {
    let parts: Vec<&str> = raw
        .split(|c: char| !c.is_ascii_digit())
        .filter(|s| !s.is_empty())
        .collect();
    if parts.len() != 3 {
        return None;
    }
    let (year_part, month_part, day_part) = match order {
        DateOrder::YearMonthDay => (parts[0], parts[1], parts[2]),
        DateOrder::DayMonthYear => (parts[2], parts[1], parts[0]),
        DateOrder::MonthDayYear => (parts[2], parts[0], parts[1]),
    };
    let mut year: i32 = year_part.parse().ok()?;
    if year_part.len() <= 2 {
        year += 2000;
    }
    let month: u32 = month_part.parse().ok()?;
    let day: u32 = day_part.parse().ok()?;
    if !(1..=12).contains(&month) || !(1..=31).contains(&day) {
        return None;
    }
    Some(format!("{year:04}-{month:02}-{day:02}"))
}

/// Reads an amount from either a single signed column or a pair of
/// debit/credit columns - the two shapes real bank exports come in, for
/// CSV and Excel alike.
fn amount_from_columns(
    field: impl Fn(usize) -> String,
    amount_column: Option<usize>,
    debit_column: Option<usize>,
    credit_column: Option<usize>,
) -> Option<Amount> {
    if let Some(col) = amount_column {
        return Amount::parse(&field(col)).ok();
    }
    let debit = debit_column
        .and_then(|c| Amount::parse(&field(c)).ok())
        .unwrap_or(Amount::ZERO);
    let credit = credit_column
        .and_then(|c| Amount::parse(&field(c)).ok())
        .unwrap_or(Amount::ZERO);
    if debit == Amount::ZERO && credit == Amount::ZERO {
        return None;
    }
    Some(credit - debit.abs())
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct ImportedTransaction {
    pub date: String,
    pub amount: Amount,
    /// The bank's unique id for this line (OFX `FITID`, or a CSV reference
    /// column) - what `import` matches against to skip a line already on
    /// file. `None` when the source has no such id (most CSV exports).
    pub fit_id: Option<String>,
    pub label: String,
    pub memo: Option<String>,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub struct ImportSummary {
    pub imported: usize,
    pub skipped_duplicates: usize,
}

/// Writes `transactions` into `account_id` as cleared transactions,
/// skipping any whose `fit_id` already exists on that account. Recomputes
/// the account's balances once at the end, not once per row - an import is
/// a batch, not a sequence of independent edits.
pub fn import(
    conn: &Connection,
    account_id: i64,
    transactions: &[ImportedTransaction],
) -> Result<ImportSummary> {
    let mut summary = ImportSummary::default();

    for tx in transactions {
        if let Some(fit_id) = tx.fit_id.as_deref() {
            let already_present = conn
                .prepare("SELECT 1 FROM transactions WHERE account_id = ?1 AND external_id = ?2")?
                .exists(params![account_id, fit_id])?;
            if already_present {
                summary.skipped_duplicates += 1;
                continue;
            }
        }

        let label = if tx.label.trim().is_empty() {
            "Imported transaction"
        } else {
            tx.label.trim()
        };
        conn.execute(
            "INSERT INTO transactions (account_id, parent_id, label, date, amount, status, comment, external_id) \
             VALUES (?1, 0, ?2, ?3, ?4, ?5, ?6, ?7)",
            params![account_id, label, tx.date, tx.amount.micros(), Status::Cleared.as_i64(), tx.memo, tx.fit_id],
        )?;
        summary.imported += 1;
    }

    if summary.imported > 0 {
        accounts::recompute_balances(conn, account_id)?;
    }

    Ok(summary)
}

/// OFX 1.x is SGML, not XML: tags are often left unclosed. Rather than
/// parse it as a document, this isolates each `<STMTTRN>` container and
/// reads it as a flat run of `<TAG>value` fragments, exactly like the
/// legacy import this project is inspired by - see `PROMPT.md`.
pub mod ofx {
    use super::ImportedTransaction;
    use crate::money::Amount;

    /// Parses every `<STMTTRN>` block found in `content`. A block missing a
    /// usable date or amount is silently skipped, same as any tag this
    /// parser doesn't recognise.
    pub fn parse(content: &str) -> Vec<ImportedTransaction> {
        let lower = content.to_ascii_lowercase();
        let mut transactions = Vec::new();
        let mut cursor = 0;

        while let Some(open_rel) = lower[cursor..].find("<stmttrn>") {
            let body_start = cursor + open_rel + "<stmttrn>".len();
            let Some(close_rel) = lower[body_start..].find("</stmttrn>") else {
                break;
            };
            let body_end = body_start + close_rel;

            if let Some(tx) = parse_block(&content[body_start..body_end]) {
                transactions.push(tx);
            }
            cursor = body_end + "</stmttrn>".len();
        }

        transactions
    }

    fn parse_block(block: &str) -> Option<ImportedTransaction> {
        let mut date = None;
        let mut amount = None;
        let mut fit_id = None;
        let mut name = None;
        let mut memo = None;

        for fragment in block.split('<').skip(1) {
            let Some(end_of_tag) = fragment.find('>') else {
                continue;
            };
            let tag = fragment[..end_of_tag].trim().to_ascii_uppercase();
            let value = fragment[end_of_tag + 1..]
                .lines()
                .next()
                .unwrap_or("")
                .trim();
            if value.is_empty() && !matches!(tag.as_str(), "MEMO") {
                continue;
            }
            match tag.as_str() {
                "DTPOSTED" => date = ofx_date_to_iso(value),
                "TRNAMT" => amount = Amount::parse(value).ok(),
                "FITID" => fit_id = Some(value.to_string()),
                "NAME" | "PAYEE" => name = Some(value.to_string()),
                "MEMO" => memo = Some(value.to_string()),
                _ => {}
            }
        }

        Some(ImportedTransaction {
            date: date?,
            amount: amount?,
            fit_id,
            label: name
                .or_else(|| memo.clone())
                .unwrap_or_else(|| "Imported transaction".to_string()),
            memo,
        })
    }

    /// `DTPOSTED` is `YYYYMMDD`, optionally followed by a time and a
    /// timezone (`YYYYMMDDHHMMSS.mmm[:TZ]`) - only the date part matters
    /// for a transaction's calendar day.
    fn ofx_date_to_iso(raw: &str) -> Option<String> {
        let digits: String = raw.chars().take_while(char::is_ascii_digit).collect();
        if digits.len() < 8 {
            return None;
        }
        Some(format!(
            "{}-{}-{}",
            &digits[0..4],
            &digits[4..6],
            &digits[6..8]
        ))
    }
}

/// CSV import: a column-mapping the caller builds from the file's header
/// (or a preview of its first rows), fed into the same [`import`] pipeline
/// as OFX. Handles the two shapes real bank exports come in - one signed
/// amount column, or separate debit/credit columns - and a handful of date
/// orderings, since which one a given bank uses isn't standard.
pub mod csv {
    use super::{ImportedTransaction, amount_from_columns, parse_date_field};

    pub use super::DateOrder;

    #[derive(Debug, Clone)]
    pub struct ColumnMapping {
        pub delimiter: char,
        pub has_header: bool,
        pub date_column: usize,
        pub date_order: DateOrder,
        pub label_column: Option<usize>,
        /// A single signed amount column. Mutually exclusive with
        /// `debit_column`/`credit_column` in practice - if both are given,
        /// this one wins.
        pub amount_column: Option<usize>,
        pub debit_column: Option<usize>,
        pub credit_column: Option<usize>,
        /// A column holding a value unique to the line (a bank reference
        /// number), used for de-duplication exactly like OFX's `FITID`.
        pub reference_column: Option<usize>,
    }

    pub fn parse(content: &str, mapping: &ColumnMapping) -> Vec<ImportedTransaction> {
        let mut lines = content.lines();
        if mapping.has_header {
            lines.next();
        }

        lines
            .filter(|line| !line.trim().is_empty())
            .filter_map(|line| parse_line(line, mapping))
            .collect()
    }

    fn parse_line(line: &str, mapping: &ColumnMapping) -> Option<ImportedTransaction> {
        let fields = split_line(line, mapping.delimiter);
        let field = |index: usize| fields.get(index).cloned().unwrap_or_default();

        let date = parse_date_field(&field(mapping.date_column), mapping.date_order)?;
        let amount = amount_from_columns(
            field,
            mapping.amount_column,
            mapping.debit_column,
            mapping.credit_column,
        )?;
        let label = mapping
            .label_column
            .map(field)
            .filter(|s| !s.is_empty())
            .unwrap_or_else(|| "Imported transaction".to_string());
        let fit_id = mapping
            .reference_column
            .map(field)
            .filter(|s| !s.is_empty());

        Some(ImportedTransaction {
            date,
            amount,
            fit_id,
            label,
            memo: None,
        })
    }

    /// A small quoted-field splitter: handles `"a, b"` and a doubled `""`
    /// as an escaped quote, which covers what real bank exports use.
    fn split_line(line: &str, delimiter: char) -> Vec<String> {
        let mut fields = Vec::new();
        let mut current = String::new();
        let mut in_quotes = false;
        let mut chars = line.chars().peekable();

        while let Some(c) = chars.next() {
            if in_quotes {
                if c == '"' {
                    if chars.peek() == Some(&'"') {
                        current.push('"');
                        chars.next();
                    } else {
                        in_quotes = false;
                    }
                } else {
                    current.push(c);
                }
            } else if c == '"' {
                in_quotes = true;
            } else if c == delimiter {
                fields.push(current.trim().to_string());
                current.clear();
            } else {
                current.push(c);
            }
        }
        fields.push(current.trim().to_string());
        fields
    }
}

/// Excel/OpenDocument import (`.xls`, `.xlsx`, `.xlsb`, `.ods`): the same
/// column-mapping idea as `csv`, applied to the first worksheet's cells
/// instead of delimited text. Cell values that already carry a real type
/// (a date cell, a numeric amount cell) are read as that type rather than
/// through their displayed text, since a spreadsheet's own formatting can't
/// be relied on to round-trip through a string cleanly.
pub mod excel {
    use super::{ImportedTransaction, amount_from_columns, parse_date_field};
    use crate::error::{CoreError, Result};
    use crate::money::Amount;
    use calamine::{Data, ExcelDateTime, ExcelDateTimeType, Reader, open_workbook_auto};
    use std::path::Path;

    pub use super::DateOrder;

    #[derive(Debug, Clone)]
    pub struct ColumnMapping {
        pub has_header: bool,
        pub date_column: usize,
        pub date_order: DateOrder,
        pub label_column: Option<usize>,
        pub amount_column: Option<usize>,
        pub debit_column: Option<usize>,
        pub credit_column: Option<usize>,
        pub reference_column: Option<usize>,
    }

    fn open_first_sheet(path: &Path) -> Result<Vec<Vec<Data>>> {
        let mut workbook =
            open_workbook_auto(path).map_err(|e| CoreError::Invalid(e.to_string()))?;
        let range = match workbook.worksheet_range_at(0) {
            Some(Ok(range)) => range,
            Some(Err(e)) => return Err(CoreError::Invalid(e.to_string())),
            None => return Err(CoreError::Invalid("the workbook has no worksheets".into())),
        };
        Ok(range.rows().map(|row| row.to_vec()).collect())
    }

    /// The first `limit` rows of the first worksheet, as plain text - for
    /// showing the user what's in the file before they pick column numbers.
    pub fn preview_rows(path: &Path, limit: usize) -> Result<Vec<Vec<String>>> {
        let rows = open_first_sheet(path)?;
        Ok(rows
            .into_iter()
            .take(limit)
            .map(|row| row.iter().map(cell_to_string).collect())
            .collect())
    }

    pub fn parse(path: &Path, mapping: &ColumnMapping) -> Result<Vec<ImportedTransaction>> {
        let rows = open_first_sheet(path)?;
        let rows = if mapping.has_header {
            rows.into_iter().skip(1).collect()
        } else {
            rows
        };

        Ok(rows
            .iter()
            .filter_map(|row| parse_row(row, mapping))
            .collect())
    }

    fn parse_row(row: &[Data], mapping: &ColumnMapping) -> Option<ImportedTransaction> {
        let cell = |index: usize| row.get(index);
        let field = |index: usize| cell(index).map(cell_to_string).unwrap_or_default();

        let date = cell(mapping.date_column).and_then(|c| cell_to_date(c, mapping.date_order))?;
        let amount =
            cell_to_amount(cell(mapping.amount_column.unwrap_or(usize::MAX))).or_else(|| {
                amount_from_columns(
                    field,
                    mapping.amount_column,
                    mapping.debit_column,
                    mapping.credit_column,
                )
            })?;
        let label = mapping
            .label_column
            .map(field)
            .filter(|s| !s.is_empty())
            .unwrap_or_else(|| "Imported transaction".to_string());
        let fit_id = mapping
            .reference_column
            .map(field)
            .filter(|s| !s.is_empty());

        Some(ImportedTransaction {
            date,
            amount,
            fit_id,
            label,
            memo: None,
        })
    }

    fn cell_to_string(cell: &Data) -> String {
        match cell {
            Data::Empty => String::new(),
            Data::String(s) => s.clone(),
            Data::Int(i) => i.to_string(),
            Data::Float(f) => {
                if f.fract() == 0.0 {
                    format!("{}", *f as i64)
                } else {
                    f.to_string()
                }
            }
            Data::Bool(b) => b.to_string(),
            Data::DateTimeIso(s) | Data::DurationIso(s) => s.clone(),
            Data::DateTime(dt) => dt.to_string(),
            Data::Error(e) => format!("{e:?}"),
        }
    }

    /// A date column's cell is read as a date whenever it plausibly is one:
    /// a genuine date-typed cell, or - since Excel doesn't require a cell
    /// to carry a date *format* to hold a date *serial* - a bare number in
    /// the range real Excel dates fall in (1900 through ~2100). Anything
    /// else falls back to parsing its displayed text with `date_order`.
    fn cell_to_date(cell: &Data, order: DateOrder) -> Option<String> {
        match cell {
            Data::DateTime(dt) => Some(ymd(dt.to_ymd_hms_milli())),
            Data::Float(serial) if (1.0..80_000.0).contains(serial) => Some(ymd(
                ExcelDateTime::new(*serial, ExcelDateTimeType::DateTime, false).to_ymd_hms_milli(),
            )),
            _ => parse_date_field(&cell_to_string(cell), order),
        }
    }

    fn ymd((year, month, day, ..): (u16, u8, u8, u8, u8, u8, u16)) -> String {
        format!("{year:04}-{month:02}-{day:02}")
    }

    /// Reads a genuinely numeric cell directly, without round-tripping
    /// through its displayed text - avoids float-formatting surprises on
    /// large amounts. Returns `None` for anything that isn't a plain
    /// number, so callers fall back to `amount_from_columns`.
    fn cell_to_amount(cell: Option<&Data>) -> Option<Amount> {
        match cell? {
            Data::Int(i) => Some(Amount::from_micros(i * 1_000_000)),
            Data::Float(f) => Some(Amount::from_micros((f * 1_000_000.0).round() as i64)),
            _ => None,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::{accounts, db, transactions};

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

    const SAMPLE_OFX: &str = r#"
        <OFX>
        <BANKMSGSRSV1>
        <STMTTRNRS>
        <STMTRS>
        <BANKTRANLIST>
        <STMTTRN>
        <TRNTYPE>DEBIT
        <DTPOSTED>20260305120000
        <TRNAMT>-45.67
        <FITID>2026030500001
        <NAME>SUPERMARKET
        <MEMO>Grocery shopping
        </STMTTRN>
        <STMTTRN>
        <TRNTYPE>CREDIT
        <DTPOSTED>20260301
        <TRNAMT>2000.00
        <FITID>2026030100001
        <NAME>SALARY
        </STMTTRN>
        </BANKTRANLIST>
        </STMTRS>
        </STMTTRNRS>
        </BANKMSGSRSV1>
        </OFX>
    "#;

    #[test]
    fn ofx_parses_the_transaction_list() {
        let transactions = ofx::parse(SAMPLE_OFX);
        assert_eq!(transactions.len(), 2);
        assert_eq!(transactions[0].date, "2026-03-05");
        assert_eq!(transactions[0].amount, Amount::parse("-45.67").unwrap());
        assert_eq!(transactions[0].fit_id.as_deref(), Some("2026030500001"));
        assert_eq!(transactions[0].label, "SUPERMARKET");
        assert_eq!(transactions[1].amount, Amount::parse("2000.00").unwrap());
    }

    #[test]
    fn ofx_skips_a_block_missing_an_amount() {
        let broken = "<STMTTRN><DTPOSTED>20260305<FITID>1</STMTTRN>";
        assert!(ofx::parse(broken).is_empty());
    }

    #[test]
    fn csv_reads_a_single_signed_amount_column() {
        let content =
            "Date;Label;Amount\n05/03/2026;Supermarket;-45.67\n01/03/2026;Salary;2000.00\n";
        let mapping = csv::ColumnMapping {
            delimiter: ';',
            has_header: true,
            date_column: 0,
            date_order: csv::DateOrder::DayMonthYear,
            label_column: Some(1),
            amount_column: Some(2),
            debit_column: None,
            credit_column: None,
            reference_column: None,
        };
        let transactions = csv::parse(content, &mapping);
        assert_eq!(transactions.len(), 2);
        assert_eq!(transactions[0].date, "2026-03-05");
        assert_eq!(transactions[0].label, "Supermarket");
        assert_eq!(transactions[0].amount, Amount::parse("-45.67").unwrap());
    }

    #[test]
    fn csv_reads_separate_debit_and_credit_columns() {
        let content = "01/03/2026,Salary,,2000.00\n05/03/2026,Supermarket,45.67,\n";
        let mapping = csv::ColumnMapping {
            delimiter: ',',
            has_header: false,
            date_column: 0,
            date_order: csv::DateOrder::DayMonthYear,
            label_column: Some(1),
            amount_column: None,
            debit_column: Some(2),
            credit_column: Some(3),
            reference_column: None,
        };
        let transactions = csv::parse(content, &mapping);
        assert_eq!(transactions.len(), 2);
        assert_eq!(transactions[0].amount, Amount::parse("2000.00").unwrap());
        assert_eq!(transactions[1].amount, Amount::parse("-45.67").unwrap());
    }

    #[test]
    fn import_skips_rows_already_seen_by_fit_id() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn);

        let batch = vec![
            ImportedTransaction {
                date: "2026-03-05".into(),
                amount: Amount::parse("-45.67").unwrap(),
                fit_id: Some("abc".into()),
                label: "Supermarket".into(),
                memo: None,
            },
            ImportedTransaction {
                date: "2026-03-01".into(),
                amount: Amount::parse("2000").unwrap(),
                fit_id: Some("def".into()),
                label: "Salary".into(),
                memo: None,
            },
        ];

        let first = import(conn, account_id, &batch).unwrap();
        assert_eq!(first.imported, 2);
        assert_eq!(first.skipped_duplicates, 0);

        let second = import(conn, account_id, &batch).unwrap();
        assert_eq!(second.imported, 0);
        assert_eq!(second.skipped_duplicates, 2);

        assert_eq!(
            transactions::list_for_account(conn, account_id)
                .unwrap()
                .len(),
            2
        );
        assert_eq!(
            accounts::get(conn, account_id).unwrap().balance_cleared,
            Amount::parse("1954.33").unwrap()
        );
    }

    #[test]
    fn import_without_a_fit_id_never_deduplicates() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn);

        let batch = vec![ImportedTransaction {
            date: "2026-03-05".into(),
            amount: Amount::parse("-10").unwrap(),
            fit_id: None,
            label: "Coffee".into(),
            memo: None,
        }];

        import(conn, account_id, &batch).unwrap();
        import(conn, account_id, &batch).unwrap();

        assert_eq!(
            transactions::list_for_account(conn, account_id)
                .unwrap()
                .len(),
            2
        );
    }

    fn write_sample_xlsx(path: &std::path::Path) {
        use rust_xlsxwriter::{ExcelDateTime, Workbook};

        let mut workbook = Workbook::new();
        let sheet = workbook.add_worksheet();
        sheet.write(0, 0, "Date").unwrap();
        sheet.write(0, 1, "Label").unwrap();
        sheet.write(0, 2, "Amount").unwrap();
        sheet.write(0, 3, "Reference").unwrap();

        sheet
            .write(1, 0, ExcelDateTime::from_ymd(2026, 3, 5).unwrap())
            .unwrap();
        sheet.write(1, 1, "Supermarket").unwrap();
        sheet.write(1, 2, -45.67).unwrap();
        sheet.write(1, 3, "ref-001").unwrap();

        sheet.write(2, 0, "01/03/2026").unwrap();
        sheet.write(2, 1, "Salary").unwrap();
        sheet.write(2, 2, 2000).unwrap();
        sheet.write(2, 3, "ref-002").unwrap();

        workbook.save(path).unwrap();
    }

    #[test]
    fn excel_reads_a_real_date_cell_and_a_numeric_amount_cell() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("statement.xlsx");
        write_sample_xlsx(&path);

        let mapping = excel::ColumnMapping {
            has_header: true,
            date_column: 0,
            date_order: excel::DateOrder::DayMonthYear,
            label_column: Some(1),
            amount_column: Some(2),
            debit_column: None,
            credit_column: None,
            reference_column: Some(3),
        };

        let transactions = excel::parse(&path, &mapping).unwrap();
        assert_eq!(transactions.len(), 2);
        assert_eq!(transactions[0].date, "2026-03-05");
        assert_eq!(transactions[0].label, "Supermarket");
        assert_eq!(transactions[0].amount, Amount::parse("-45.67").unwrap());
        assert_eq!(transactions[0].fit_id.as_deref(), Some("ref-001"));

        // Second row's date is a text cell in a different format - still
        // parsed via the same date-order rule as a real date cell.
        assert_eq!(transactions[1].date, "2026-03-01");
        assert_eq!(transactions[1].amount, Amount::parse("2000").unwrap());
    }

    #[test]
    fn excel_preview_rows_shows_cell_text() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("statement.xlsx");
        write_sample_xlsx(&path);

        let preview = excel::preview_rows(&path, 2).unwrap();
        assert_eq!(preview.len(), 2);
        assert_eq!(preview[0], vec!["Date", "Label", "Amount", "Reference"]);
        assert_eq!(preview[1][1], "Supermarket");
    }
}
