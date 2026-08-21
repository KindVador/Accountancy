//! Printing: an account's transaction list, and its bank details (RIB).
//! Composed as HTML and laid out by `printpdf`'s own renderer - a
//! pure-Rust engine compiled into this binary, not the platform webview -
//! so the same statement comes out byte-for-byte identical on macOS,
//! Windows and Linux. See `PROMPT.md` §"Export and printing".

use crate::accounts::Account;
use crate::error::{CoreError, Result};
use crate::exports::ExportLine;
use printpdf::{GeneratePdfOptions, PdfDocument, PdfSaveOptions};
use std::path::Path;

fn html_escape(text: &str) -> String {
    text.replace('&', "&amp;")
        .replace('<', "&lt;")
        .replace('>', "&gt;")
        .replace('"', "&quot;")
}

const CELL: &str = "border:1px solid #999; padding:4px 6px; font-size:10px;";
const HEADER_CELL: &str =
    "border:1px solid #999; padding:4px 6px; font-size:10px; background:#eee; text-align:left;";

fn cell(text: &str, align_right: bool) -> String {
    let align = if align_right {
        " text-align:right;"
    } else {
        ""
    };
    format!("<td style=\"{CELL}{align}\">{}</td>", html_escape(text))
}

/// Renders `destination` as a PDF listing `lines` under `account`'s name -
/// what the print menu of the transaction list produces.
pub fn transaction_list_pdf(
    account: &Account,
    lines: &[ExportLine],
    destination: &Path,
) -> Result<()> {
    let mut rows = String::new();
    for line in lines {
        rows.push_str("<tr>");
        rows.push_str(&cell(line.date.as_deref().unwrap_or(""), false));
        rows.push_str(&cell(&line.label, false));
        rows.push_str(&cell(&line.category, false));
        rows.push_str(&cell(&line.payee, false));
        rows.push_str(&cell(&line.amount.to_string(), true));
        rows.push_str("</tr>");
    }

    let html = format!(
        r#"<html><body style="font-family: sans-serif;">
<h1 style="font-size:16px;">{name}</h1>
<p style="font-size:11px; color:#555;">Transactions</p>
<table style="border-collapse: collapse; width: 100%;">
<tr>
<th style="{HEADER_CELL}">Date</th>
<th style="{HEADER_CELL}">Label</th>
<th style="{HEADER_CELL}">Category</th>
<th style="{HEADER_CELL}">Payee</th>
<th style="{HEADER_CELL} text-align:right;">Amount</th>
</tr>
{rows}
</table>
</body></html>"#,
        name = html_escape(&account.name),
    );

    write_pdf(&html, destination)
}

/// Renders `destination` as a one-page PDF of `account`'s bank details -
/// the RIB.
pub fn bank_details_pdf(account: &Account, destination: &Path) -> Result<()> {
    let field = |label: &str, value: &str| {
        format!(
            "<tr><td style=\"padding:4px 10px 4px 0; font-weight:bold; font-size:11px;\">{}</td>\
             <td style=\"padding:4px 0; font-size:11px;\">{}</td></tr>",
            html_escape(label),
            html_escape(value)
        )
    };

    let html = format!(
        r#"<html><body style="font-family: sans-serif;">
<h1 style="font-size:16px;">Bank details</h1>
<table style="border-collapse: collapse;">
{account_row}
{holder_row}
{iban_row}
{bic_row}
{branch_row}
</table>
</body></html>"#,
        account_row = field("Account", &account.name),
        holder_row = field("Holder", account.holder_name.as_deref().unwrap_or("")),
        iban_row = field("IBAN", account.iban.as_deref().unwrap_or("")),
        bic_row = field("BIC", account.bic.as_deref().unwrap_or("")),
        branch_row = field("Branch", account.branch_address.as_deref().unwrap_or("")),
    );

    write_pdf(&html, destination)
}

fn write_pdf(html: &str, destination: &Path) -> Result<()> {
    let options = GeneratePdfOptions {
        page_width: Some(210.0),
        page_height: Some(297.0),
        margin_top: Some(15.0),
        margin_right: Some(15.0),
        margin_bottom: Some(15.0),
        margin_left: Some(15.0),
        show_page_numbers: Some(true),
        ..Default::default()
    };

    let mut warnings = Vec::new();
    let doc = PdfDocument::from_html(
        html,
        &Default::default(),
        &Default::default(),
        &options,
        &mut warnings,
    )
    .map_err(CoreError::Pdf)?;
    let bytes = doc.save(&PdfSaveOptions::default(), &mut warnings);
    std::fs::write(destination, bytes).map_err(|source| CoreError::Io {
        path: destination.to_path_buf(),
        source,
    })
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::money::Amount;
    use crate::{accounts, db, transactions};

    #[test]
    fn transaction_list_pdf_writes_a_valid_pdf_file() {
        let dir = tempfile::tempdir().unwrap();
        let database = db::open(dir.path()).unwrap();
        let account_id = accounts::create(
            database.connection(),
            &accounts::NewAccount {
                name: "Checking".into(),
                currency: "EUR".into(),
                iban: Some("FR7630006000011234567890189".into()),
                ..Default::default()
            },
        )
        .unwrap();
        transactions::create(
            database.connection(),
            &transactions::NewTransaction {
                account_id,
                label: "Coffee".into(),
                date: Some("2026-03-05".into()),
                amount: Amount::parse("-4.5").unwrap(),
                ..Default::default()
            },
        )
        .unwrap();

        let account = accounts::get(database.connection(), account_id).unwrap();
        let lines = crate::exports::export_lines(database.connection(), account_id).unwrap();

        let out_dir = tempfile::tempdir().unwrap();
        let pdf_path = out_dir.path().join("statement.pdf");
        transaction_list_pdf(&account, &lines, &pdf_path).unwrap();

        let bytes = std::fs::read(&pdf_path).unwrap();
        assert!(bytes.starts_with(b"%PDF"));
        assert!(bytes.len() > 200);
    }

    #[test]
    fn bank_details_pdf_writes_a_valid_pdf_file() {
        let dir = tempfile::tempdir().unwrap();
        let database = db::open(dir.path()).unwrap();
        let account_id = accounts::create(
            database.connection(),
            &accounts::NewAccount {
                name: "Checking".into(),
                currency: "EUR".into(),
                iban: Some("FR76...".into()),
                ..Default::default()
            },
        )
        .unwrap();
        let account = accounts::get(database.connection(), account_id).unwrap();

        let out_dir = tempfile::tempdir().unwrap();
        let pdf_path = out_dir.path().join("rib.pdf");
        bank_details_pdf(&account, &pdf_path).unwrap();

        let bytes = std::fs::read(&pdf_path).unwrap();
        assert!(bytes.starts_with(b"%PDF"));
    }
}
