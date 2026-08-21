use crate::error::Result;
use rusqlite::Connection;

/// Creates whatever tables are missing. Idempotent: safe to call on every
/// open. Statements use `IF NOT EXISTS` rather than a version-tracked
/// migration chain for now - a real migration system (à la
/// `PRAGMA user_version`) belongs here once the schema needs to evolve
/// under existing user data.
///
/// `IF NOT EXISTS` only covers whole tables, not columns added to a table
/// that already exists on disk - `ensure_column` below handles that case
/// for databases created before a column existed.
pub fn migrate(conn: &Connection) -> Result<()> {
    conn.execute_batch(
        r#"
        -- Reference lists: same shape, all soft-linked from `transactions`.
        CREATE TABLE IF NOT EXISTS payees (
            id   INTEGER PRIMARY KEY,
            name TEXT NOT NULL UNIQUE
        );
        CREATE TABLE IF NOT EXISTS payment_modes (
            id   INTEGER PRIMARY KEY,
            name TEXT NOT NULL UNIQUE
        );
        CREATE TABLE IF NOT EXISTS tags (
            id   INTEGER PRIMARY KEY,
            name TEXT NOT NULL UNIQUE
        );
        CREATE TABLE IF NOT EXISTS projects (
            id   INTEGER PRIMARY KEY,
            name TEXT NOT NULL UNIQUE
        );
        CREATE TABLE IF NOT EXISTS budget_lines (
            id   INTEGER PRIMARY KEY,
            name TEXT NOT NULL UNIQUE
        );
        CREATE TABLE IF NOT EXISTS banks (
            id   INTEGER PRIMARY KEY,
            name TEXT NOT NULL UNIQUE
        );
        -- Categories are a reference list too, but each one can point at a
        -- budget line (soft link: 0 means "unspecified", deleting the
        -- budget line must not delete the category).
        CREATE TABLE IF NOT EXISTS categories (
            id             INTEGER PRIMARY KEY,
            name           TEXT NOT NULL UNIQUE,
            budget_line_id INTEGER NOT NULL DEFAULT 0
        );

        -- One row per bank account. Balances are cumulative and recomputed
        -- whole by `accounts::recompute_balances`, never patched in place.
        CREATE TABLE IF NOT EXISTS accounts (
            id                        INTEGER PRIMARY KEY,
            name                      TEXT NOT NULL,
            currency                  TEXT NOT NULL DEFAULT 'EUR',
            iban                      TEXT,
            bic                       TEXT,
            holder_name               TEXT,
            branch_address            TEXT,
            bank_id                   INTEGER NOT NULL DEFAULT 0,
            opening_date              TEXT,
            closing_date              TEXT,
            is_closed                 INTEGER NOT NULL DEFAULT 0,
            next_check_number         INTEGER,
            -- what the account held before the first transaction it knows about
            opening_balance_cleared   INTEGER NOT NULL DEFAULT 0,
            opening_balance_committed INTEGER NOT NULL DEFAULT 0,
            opening_balance_projected INTEGER NOT NULL DEFAULT 0,
            -- current cumulative balances, recomputed whole on every mutation
            balance_cleared           INTEGER NOT NULL DEFAULT 0,
            balance_committed         INTEGER NOT NULL DEFAULT 0,
            balance_projected         INTEGER NOT NULL DEFAULT 0
        );

        -- Transactions form a two-level tree: a top-level (parent_id = 0)
        -- transaction may have children splitting its amount. Only
        -- top-level transactions ever count in a balance, a total or a
        -- statistic - see `transactions::is_top_level`.
        CREATE TABLE IF NOT EXISTS transactions (
            id               INTEGER PRIMARY KEY,
            account_id       INTEGER NOT NULL REFERENCES accounts (id) ON DELETE CASCADE,
            parent_id        INTEGER NOT NULL DEFAULT 0,
            label            TEXT NOT NULL,
            date             TEXT,
            clearing_date    TEXT,
            amount           INTEGER NOT NULL,
            -- 1 = cleared, 2 = committed, 3 = planned
            status           INTEGER NOT NULL DEFAULT 2,
            category_id      INTEGER NOT NULL DEFAULT 0,
            payee_id         INTEGER NOT NULL DEFAULT 0,
            payment_mode_id  INTEGER NOT NULL DEFAULT 0,
            tag_id           INTEGER NOT NULL DEFAULT 0,
            project_id       INTEGER NOT NULL DEFAULT 0,
            check_id         INTEGER NOT NULL DEFAULT 0,
            comment          TEXT,
            -- 1 marks the auto-maintained remainder line of a split - see
            -- `transactions::sync_remainder`. Never inferred from the label.
            is_remainder     INTEGER NOT NULL DEFAULT 0,
            running_balance  INTEGER NOT NULL DEFAULT 0,
            -- the bank's unique id for the transaction (OFX FitID), used to
            -- avoid re-importing the same statement line twice
            external_id      TEXT
        );
        CREATE INDEX IF NOT EXISTS idx_transactions_account ON transactions (account_id);
        CREATE INDEX IF NOT EXISTS idx_transactions_parent ON transactions (parent_id);

        -- A rule producing planned transactions over time. `next_due_date`
        -- and `generated_count` advance every time `recurring::insert_due`
        -- runs; nothing else patches them.
        CREATE TABLE IF NOT EXISTS recurring_rules (
            id                  INTEGER PRIMARY KEY,
            account_id          INTEGER NOT NULL REFERENCES accounts (id) ON DELETE CASCADE,
            label               TEXT NOT NULL,
            amount              INTEGER NOT NULL,
            category_id         INTEGER NOT NULL DEFAULT 0,
            payee_id            INTEGER NOT NULL DEFAULT 0,
            payment_mode_id     INTEGER NOT NULL DEFAULT 0,
            tag_id              INTEGER NOT NULL DEFAULT 0,
            project_id          INTEGER NOT NULL DEFAULT 0,
            comment             TEXT,
            -- 1 = day, 2 = week, 3 = month, 4 = quarter, 5 = year
            period              INTEGER NOT NULL,
            period_frequency    INTEGER NOT NULL DEFAULT 1,
            -- 1 = no limit, 2 = until end_date, 3 = a fixed occurrence count
            end_mode            INTEGER NOT NULL DEFAULT 1,
            end_date            TEXT,
            occurrence_count    INTEGER,
            next_due_date       TEXT NOT NULL,
            last_generated_date TEXT,
            generated_count     INTEGER NOT NULL DEFAULT 0
        );

        -- One budgeted amount per budget line, per calendar month. The link
        -- to actual spending is indirect: a transaction carries a category,
        -- a category carries a budget line - see `budget::control_for_month`.
        CREATE TABLE IF NOT EXISTS budget_entries (
            id             INTEGER PRIMARY KEY,
            year           INTEGER NOT NULL,
            month          INTEGER NOT NULL,
            budget_line_id INTEGER NOT NULL REFERENCES budget_lines (id) ON DELETE CASCADE,
            amount         INTEGER NOT NULL DEFAULT 0,
            UNIQUE (year, month, budget_line_id)
        );

        -- A chequebook is born whole: `cheques::create_chequebook` creates
        -- its numbered cheques with it, in the same call.
        CREATE TABLE IF NOT EXISTS chequebooks (
            id             INTEGER PRIMARY KEY,
            account_id     INTEGER NOT NULL REFERENCES accounts (id) ON DELETE CASCADE,
            first_number   INTEGER NOT NULL,
            last_number    INTEGER NOT NULL,
            reception_date TEXT
        );

        -- One row per physical cheque. The link to the transaction it paid
        -- for goes the other way, from `transactions.check_id` - a cheque
        -- may exist and never be matched to anything.
        CREATE TABLE IF NOT EXISTS cheques (
            id            INTEGER PRIMARY KEY,
            chequebook_id INTEGER NOT NULL REFERENCES chequebooks (id) ON DELETE CASCADE,
            number        INTEGER NOT NULL,
            date          TEXT,
            payee_id      INTEGER NOT NULL DEFAULT 0,
            amount        INTEGER NOT NULL DEFAULT 0,
            -- 1 = available, 2 = issued (written to a payee), 3 = cleared, 4 = cancelled
            status        INTEGER NOT NULL DEFAULT 1,
            UNIQUE (chequebook_id, number)
        );
        CREATE INDEX IF NOT EXISTS idx_cheques_chequebook ON cheques (chequebook_id);

        -- What happened, and did it work - backup, restore, repair. Lives
        -- inside the database itself so a restored backup carries its own
        -- history forward with it; see `journal::record`.
        CREATE TABLE IF NOT EXISTS operations_log (
            id          INTEGER PRIMARY KEY,
            happened_at TEXT NOT NULL,
            kind        TEXT NOT NULL,
            succeeded   INTEGER NOT NULL,
            detail      TEXT
        );

        -- A file attached to a transaction. `stored_name` is a GUID-based
        -- name inside the database's own `attachments/` folder - never the
        -- file's original name, which is kept only for display and re-export.
        -- Cascades: an attachment cannot outlive the transaction it documents.
        CREATE TABLE IF NOT EXISTS attachments (
            id             INTEGER PRIMARY KEY,
            transaction_id INTEGER NOT NULL REFERENCES transactions (id) ON DELETE CASCADE,
            stored_name    TEXT NOT NULL UNIQUE,
            original_name  TEXT NOT NULL,
            size           INTEGER NOT NULL,
            mime_type      TEXT NOT NULL,
            checksum       TEXT NOT NULL,
            added_at       TEXT NOT NULL
        );
        CREATE INDEX IF NOT EXISTS idx_attachments_transaction ON attachments (transaction_id);
        "#,
    )?;

    // `recurring_rule_id` was added after the first release of the
    // `transactions` table; `ensure_column` backfills it on databases that
    // predate it, and is a no-op on a freshly created table (already
    // created with the column below).
    ensure_column(
        conn,
        "transactions",
        "recurring_rule_id",
        "INTEGER NOT NULL DEFAULT 0",
    )?;

    Ok(())
}

/// Adds `column` to `table` if it isn't there yet. `ALTER TABLE ... ADD
/// COLUMN` is cheap in SQLite (no table rewrite for a column with a
/// constant default), so this runs unconditionally on every open.
fn ensure_column(conn: &Connection, table: &str, column: &str, definition: &str) -> Result<()> {
    let already_present = conn
        .prepare(&format!(
            "SELECT 1 FROM pragma_table_info('{table}') WHERE name = ?1"
        ))?
        .exists([column])?;
    if !already_present {
        conn.execute(
            &format!("ALTER TABLE {table} ADD COLUMN {column} {definition}"),
            [],
        )?;
    }
    Ok(())
}
