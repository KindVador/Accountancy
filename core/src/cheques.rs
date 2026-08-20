//! Chequebooks and cheques. A chequebook is born whole: `create_chequebook`
//! creates its numbered cheques in the same call, and its numbering is
//! fixed from then on. The link between a cheque and the transaction it
//! paid for goes the other way - `transactions.check_id` - and is soft: a
//! cheque may be written and never matched to anything.

use crate::error::{CoreError, Result};
use crate::money::Amount;
use rusqlite::{Connection, OptionalExtension, Row, params};

#[derive(Debug, Clone, PartialEq, Eq, serde::Serialize, serde::Deserialize)]
pub struct Chequebook {
    pub id: i64,
    pub account_id: i64,
    pub first_number: i64,
    pub last_number: i64,
    pub reception_date: Option<String>,
}

impl Chequebook {
    pub fn count(&self) -> i64 {
        self.last_number - self.first_number + 1
    }
}

#[derive(Debug, Clone, Default)]
pub struct NewChequebook {
    pub account_id: i64,
    pub first_number: i64,
    pub last_number: i64,
    pub reception_date: Option<String>,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Default, serde::Serialize, serde::Deserialize)]
pub enum ChequeStatus {
    #[default]
    Available = 1,
    Issued = 2,
    Cleared = 3,
    Cancelled = 4,
}

impl ChequeStatus {
    pub fn from_i64(value: i64) -> ChequeStatus {
        match value {
            2 => ChequeStatus::Issued,
            3 => ChequeStatus::Cleared,
            4 => ChequeStatus::Cancelled,
            _ => ChequeStatus::Available,
        }
    }

    pub fn as_i64(self) -> i64 {
        self as i64
    }
}

#[derive(Debug, Clone, PartialEq, Eq, serde::Serialize, serde::Deserialize)]
pub struct Cheque {
    pub id: i64,
    pub chequebook_id: i64,
    pub number: i64,
    pub date: Option<String>,
    pub payee_id: i64,
    pub amount: Amount,
    pub status: ChequeStatus,
}

fn chequebook_from_row(row: &Row) -> rusqlite::Result<Chequebook> {
    Ok(Chequebook {
        id: row.get(0)?,
        account_id: row.get(1)?,
        first_number: row.get(2)?,
        last_number: row.get(3)?,
        reception_date: row.get(4)?,
    })
}

const CHEQUEBOOK_COLUMNS: &str = "id, account_id, first_number, last_number, reception_date";

/// Creates a chequebook and, in the same call, every cheque it contains -
/// `Available`, unassigned, from `first_number` to `last_number`.
pub fn create_chequebook(conn: &Connection, new: &NewChequebook) -> Result<i64> {
    if new.first_number < 0 || new.last_number < new.first_number {
        return Err(CoreError::Invalid(
            "a chequebook's last number must be at least its first number".into(),
        ));
    }

    conn.execute(
        "INSERT INTO chequebooks (account_id, first_number, last_number, reception_date) VALUES (?1, ?2, ?3, ?4)",
        params![new.account_id, new.first_number, new.last_number, new.reception_date],
    )?;
    let chequebook_id = conn.last_insert_rowid();

    for number in new.first_number..=new.last_number {
        conn.execute(
            "INSERT INTO cheques (chequebook_id, number, status) VALUES (?1, ?2, ?3)",
            params![chequebook_id, number, ChequeStatus::Available.as_i64()],
        )?;
    }

    Ok(chequebook_id)
}

pub fn get_chequebook(conn: &Connection, id: i64) -> Result<Chequebook> {
    let sql = format!("SELECT {CHEQUEBOOK_COLUMNS} FROM chequebooks WHERE id = ?1");
    conn.query_row(&sql, params![id], chequebook_from_row)
        .optional()?
        .ok_or(CoreError::NotFound("chequebook", id))
}

/// Lists chequebooks, optionally restricted to one account.
pub fn list_chequebooks(conn: &Connection, account_id: Option<i64>) -> Result<Vec<Chequebook>> {
    let sql = format!(
        "SELECT {CHEQUEBOOK_COLUMNS} FROM chequebooks {} ORDER BY first_number",
        if account_id.is_some() {
            "WHERE account_id = ?1"
        } else {
            ""
        }
    );
    let mut stmt = conn.prepare(&sql)?;
    let rows = match account_id {
        Some(id) => stmt
            .query_map(params![id], chequebook_from_row)?
            .collect::<std::result::Result<Vec<_>, _>>()?,
        None => stmt
            .query_map([], chequebook_from_row)?
            .collect::<std::result::Result<Vec<_>, _>>()?,
    };
    Ok(rows)
}

/// Deletes a chequebook and, through `ON DELETE CASCADE`, every cheque in
/// it. Transactions that had been matched to one of those cheques are
/// reassigned to "unspecified" first, never left pointing at a row that no
/// longer exists.
pub fn delete_chequebook(conn: &Connection, id: i64) -> Result<()> {
    conn.execute(
        "UPDATE transactions SET check_id = 0 WHERE check_id IN (SELECT id FROM cheques WHERE chequebook_id = ?1)",
        params![id],
    )?;
    let changed = conn.execute("DELETE FROM chequebooks WHERE id = ?1", params![id])?;
    if changed == 0 {
        return Err(CoreError::NotFound("chequebook", id));
    }
    Ok(())
}

fn cheque_from_row(row: &Row) -> rusqlite::Result<Cheque> {
    Ok(Cheque {
        id: row.get(0)?,
        chequebook_id: row.get(1)?,
        number: row.get(2)?,
        date: row.get(3)?,
        payee_id: row.get(4)?,
        amount: Amount::from_micros(row.get(5)?),
        status: ChequeStatus::from_i64(row.get(6)?),
    })
}

const CHEQUE_COLUMNS: &str = "id, chequebook_id, number, date, payee_id, amount, status";

pub fn get_cheque(conn: &Connection, id: i64) -> Result<Cheque> {
    let sql = format!("SELECT {CHEQUE_COLUMNS} FROM cheques WHERE id = ?1");
    conn.query_row(&sql, params![id], cheque_from_row)
        .optional()?
        .ok_or(CoreError::NotFound("cheque", id))
}

/// The cheques of one chequebook, in number order.
pub fn list_cheques(conn: &Connection, chequebook_id: i64) -> Result<Vec<Cheque>> {
    let sql =
        format!("SELECT {CHEQUE_COLUMNS} FROM cheques WHERE chequebook_id = ?1 ORDER BY number");
    let mut stmt = conn.prepare(&sql)?;
    let rows = stmt
        .query_map(params![chequebook_id], cheque_from_row)?
        .collect::<std::result::Result<Vec<_>, _>>()?;
    Ok(rows)
}

/// Every cheque belonging to an account, across all its chequebooks, in
/// number order - what a transaction form offers to associate with.
pub fn list_cheques_for_account(conn: &Connection, account_id: i64) -> Result<Vec<Cheque>> {
    let sql = format!(
        "SELECT {CHEQUE_COLUMNS} FROM cheques c \
         JOIN chequebooks b ON b.id = c.chequebook_id \
         WHERE b.account_id = ?1 ORDER BY c.number"
    );
    let mut stmt = conn.prepare(&sql)?;
    let rows = stmt
        .query_map(params![account_id], cheque_from_row)?
        .collect::<std::result::Result<Vec<_>, _>>()?;
    Ok(rows)
}

/// Writes a cheque: sets who it's paid to, for how much, on what date, and
/// its status directly (a cheque can also reach `Issued` implicitly, via
/// `associate`).
pub fn write_cheque(
    conn: &Connection,
    id: i64,
    payee_id: i64,
    amount: Amount,
    date: Option<String>,
    status: ChequeStatus,
) -> Result<()> {
    let changed = conn.execute(
        "UPDATE cheques SET payee_id = ?1, amount = ?2, date = ?3, status = ?4 WHERE id = ?5",
        params![payee_id, amount.micros(), date, status.as_i64(), id],
    )?;
    if changed == 0 {
        return Err(CoreError::NotFound("cheque", id));
    }
    Ok(())
}

/// Links `transaction_id` to `cheque_id`. A cheque still `Available` moves
/// to `Issued` - matching it to what paid for it is exactly what "writing"
/// it means.
pub fn associate(conn: &Connection, transaction_id: i64, cheque_id: i64) -> Result<()> {
    get_cheque(conn, cheque_id)?;
    let changed = conn.execute(
        "UPDATE transactions SET check_id = ?1 WHERE id = ?2",
        params![cheque_id, transaction_id],
    )?;
    if changed == 0 {
        return Err(CoreError::NotFound("transaction", transaction_id));
    }
    conn.execute(
        "UPDATE cheques SET status = ?1 WHERE id = ?2 AND status = ?3",
        params![
            ChequeStatus::Issued.as_i64(),
            cheque_id,
            ChequeStatus::Available.as_i64()
        ],
    )?;
    Ok(())
}

/// Clears whatever cheque `transaction_id` was matched to. Leaves the
/// cheque's own status untouched - it may already have cleared at the bank.
pub fn dissociate(conn: &Connection, transaction_id: i64) -> Result<()> {
    let changed = conn.execute(
        "UPDATE transactions SET check_id = 0 WHERE id = ?1",
        params![transaction_id],
    )?;
    if changed == 0 {
        return Err(CoreError::NotFound("transaction", transaction_id));
    }
    Ok(())
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

    #[test]
    fn creating_a_chequebook_creates_its_cheques() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn);

        let book_id = create_chequebook(
            conn,
            &NewChequebook {
                account_id,
                first_number: 100,
                last_number: 103,
                reception_date: Some("2026-01-01".into()),
            },
        )
        .unwrap();

        let cheques = list_cheques(conn, book_id).unwrap();
        assert_eq!(cheques.len(), 4);
        assert_eq!(cheques[0].number, 100);
        assert_eq!(cheques[3].number, 103);
        assert!(cheques.iter().all(|c| c.status == ChequeStatus::Available));
    }

    #[test]
    fn rejects_a_backwards_range() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn);

        assert!(matches!(
            create_chequebook(
                conn,
                &NewChequebook {
                    account_id,
                    first_number: 10,
                    last_number: 5,
                    ..Default::default()
                }
            ),
            Err(CoreError::Invalid(_))
        ));
    }

    #[test]
    fn associating_a_cheque_issues_it_and_links_the_transaction() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn);
        let book_id = create_chequebook(
            conn,
            &NewChequebook {
                account_id,
                first_number: 1,
                last_number: 1,
                ..Default::default()
            },
        )
        .unwrap();
        let cheque_id = list_cheques(conn, book_id).unwrap()[0].id;

        let tx_id = transactions::create(
            conn,
            &transactions::NewTransaction {
                account_id,
                label: "Rent".into(),
                amount: Amount::parse("-500").unwrap(),
                ..Default::default()
            },
        )
        .unwrap();

        associate(conn, tx_id, cheque_id).unwrap();

        assert_eq!(transactions::get(conn, tx_id).unwrap().check_id, cheque_id);
        assert_eq!(
            get_cheque(conn, cheque_id).unwrap().status,
            ChequeStatus::Issued
        );

        dissociate(conn, tx_id).unwrap();
        assert_eq!(transactions::get(conn, tx_id).unwrap().check_id, 0);
        // Dissociating doesn't un-issue the cheque.
        assert_eq!(
            get_cheque(conn, cheque_id).unwrap().status,
            ChequeStatus::Issued
        );
    }

    #[test]
    fn deleting_a_chequebook_clears_associations_and_removes_its_cheques() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn);
        let book_id = create_chequebook(
            conn,
            &NewChequebook {
                account_id,
                first_number: 1,
                last_number: 1,
                ..Default::default()
            },
        )
        .unwrap();
        let cheque_id = list_cheques(conn, book_id).unwrap()[0].id;
        let tx_id = transactions::create(
            conn,
            &transactions::NewTransaction {
                account_id,
                label: "Rent".into(),
                amount: Amount::parse("-500").unwrap(),
                ..Default::default()
            },
        )
        .unwrap();
        associate(conn, tx_id, cheque_id).unwrap();

        delete_chequebook(conn, book_id).unwrap();

        assert!(list_cheques(conn, book_id).unwrap().is_empty());
        assert_eq!(transactions::get(conn, tx_id).unwrap().check_id, 0);
    }

    #[test]
    fn cheque_numbers_are_unique_within_a_book() {
        let db = db::open_in_memory().unwrap();
        let conn = db.connection();
        let account_id = new_account(conn);
        let book_id = create_chequebook(
            conn,
            &NewChequebook {
                account_id,
                first_number: 1,
                last_number: 1,
                ..Default::default()
            },
        )
        .unwrap();

        let duplicate = conn.execute(
            "INSERT INTO cheques (chequebook_id, number) VALUES (?1, 1)",
            params![book_id],
        );
        assert!(duplicate.is_err());
    }
}
