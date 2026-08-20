# Prompt — Generate a personal bank account management application in Rust + Slint

This document is a ready-to-use prompt for a code-generation agent (Claude
Code or equivalent). It describes how to rebuild, **from scratch**, an
application equivalent to *Radinus* — a personal finance manager written in
Rust — but with a native **Slint** graphical interface (instead of a Tauri
webview). Copy this prompt as-is as the initial request when starting the new
project.

---

## Prompt to copy

You are going to build a cross-platform (macOS, Windows, Linux) desktop
application for managing personal bank accounts, in **Rust**, with a
**Slint** graphical interface (no webview, no Tauri, no Electron). The
application must reproduce the full set of features described below. The
application's name, the naming of types, and the details of the data schema
are left to your discretion — this is not about porting an existing
database, but about building new software with equivalent functionality.

### 1. Architectural principles

Structure the project as a **Cargo workspace** with three members, with a
one-way dependency `view → controller → business core`:

- **`core`** (library) — all business rules, SQLite persistence,
  import/export, backup/restore, database repair. **Depends on no graphical
  framework.** It must compile and be fully testable on its own
  (`cargo test -p core`), without Slint. This guarantees the business logic
  stays verifiable independently of the UI.
- **`app`** (binary) — the entry point: builds the Slint windows, wires
  their callbacks to the business-core functions, manages the application's
  lifecycle (event loop, windows, menu, shortcuts). This layer must stay
  **thin**: it translates a UI event into a call to the core and pushes the
  result back into Slint properties/models. No business rule lives here — if
  you're tempted to write a business rule inside the Slint wiring code, it
  belongs in `core` instead.
- **`.slint` files** — pure presentation: layout, styling, visual states,
  animations. No business logic, only declared `callback`s and bound
  `property`s.

Since the business core is UI-independent, it must remain reusable as-is if
the UI ever changes again (CLI, another toolkit, etc.).

### 2. Persistence and data model

- Local **SQLite** database (consider **SQLCipher** — via the `rusqlite`
  crate with a `bundled-sqlcipher-*` feature — for optional password-based
  database encryption). One database = one folder containing
  `accountancy.sqlite` (or equivalent), a JSON metadata file (label, comment,
  origin), and an `attachments/` subfolder for attached files.
- **Portable folder**: the whole application (executable, databases,
  settings, logs, attachments) must be able to live in a single, movable
  folder (USB drive), with nothing written outside that folder.
- **Integer amounts**, never floating point. Use a micro-unit type (`i64`,
  value × 1,000,000, or a dedicated type such as `rust_decimal`) to avoid any
  rounding drift on money. A negative amount is a debit, a positive amount a
  credit — a single column, a single sign.
- **Explicitly generated keys** (`max(id)+1` or UUID depending on the table),
  no `AUTOINCREMENT` if you want identifiers to stay stable when restoring a
  backup.
- **Soft links to reference lists**: a transaction can reference a category,
  a payee, etc. with an "unspecified" value; deleting a reference-list entry
  must never cascade-delete the transactions that reference it (reassign to
  "unspecified" instead).
- **Two-level tree for transactions**: a "parent" transaction can have
  "child" transactions (sub-transactions, to split a payment across several
  categories). **Only parent transactions count** toward a balance, a total,
  or a statistic — this is the rule easiest to break by accident, so
  centralize this filter in a single business-core function instead of
  repeating it everywhere.
- **Three running balances per account**: cleared (reconciled with the
  bank), committed (cleared + validated transactions), projected (committed
  + planned transactions). Fully recomputed on every mutation, never patched
  incrementally.
- **ISO-8601 dates**, nullable for the transaction date (a planned
  transaction may not yet have a fixed date).
- Document your schema in a `docs/data_model.md` file with a diagram
  (Mermaid `erDiagram` or equivalent) and the rules above spelled out.

### 3. Features to cover

Reproduce the following functional domains in full (screen names below are
indicative — group them as you see fit on the Slint side):

**Accounts**
- Account list with balances; create/edit/close/delete an account; bank
  details (IBAN/BIC/holder/branch); transfer of an amount between two
  accounts (two linked entries, created and deleted together); an "account
  balances" overview (total of open accounts — a closed account drops out of
  the totals).

**Transactions**
- Account transaction list: parent/child tree, running balance line by
  line, per-column filter and sort, multi-selection, export.
- Transaction form: label, date, clearing date, amount, status
  (cleared/committed/planned), category, payee, payment mode, tag, project,
  linked check, comment, attachments.
- Sub-transactions (splitting an amount into several child lines) with an
  automatically recalculated "remainder" line absorbing whatever wasn't
  explicitly allocated.
- Transaction templates (a pre-filled entry applied to a form, writing
  nothing until the form is validated).
- Recurring transactions: a periodicity rule (day/week/month/quarter/year,
  every *n*), an end condition by date or by occurrence count, generation of
  a forecast calendar, insertion of due transactions.
- Quick classification: bulk re-categorization of an account's uncategorized
  transactions.

**Checks**
- Checkbooks (first/last check number, reception date) and checks (number,
  payee, amount, status), with a two-way check ↔ transaction association.

**Reference lists** (user-editable lists, same template for all): categories,
tags, payment modes, payees, projects, budget lines, banks. Shared rules:
label required and unique; deletion automatically reassigns existing
references to "unspecified".

**Budget**
- Entry of a budgeted amount per budget line, per month, per year.
- A control screen comparing budgeted vs. actual (actual flows through the
  chain transaction → category → budget line).

**Statistics and dashboard**
- Cross-tabulated statistics along six axes (category, tag, budget line,
  payment mode, payee, project), over a chosen period.
- Dashboard: totals of open accounts, current-month indicators.
- Balance-over-time chart.

**Import**
- Bank statement import in **OFX** format (reconciliation via each bank
  transaction's unique identifier — FitID or equivalent — to avoid
  duplicates).
- CSV import with a column-mapping screen that then reuses the same
  integration pipeline as OFX.
- Excel import with area detection, field mapping, and association to a
  destination account.

**Export and printing**
- Transaction export to several formats (CSV/TSV, Excel, QIF, JSON,
  XML/INI/YAML/TOML as you see fit).
- Printing/PDF generation: an account's transaction list, bank details. Use
  a pure-Rust typesetting engine (e.g. `typst`) rather than the system's HTML
  rendering, to get identical output across all platforms.

**Backup and maintenance**
- Database backup/restore (a zip archive containing the data), with a
  technical-operations log (backup, restore, repair) showing at minimum
  date, type, and result.
- Repair tool: reindexing, balance recomputation, sub-transaction
  consistency check.
- Password protection for a database (SQLCipher encryption).
- Attachments: files attached to a transaction, stored under an internal
  name (GUID) while keeping their real name, size, MIME type, and checksum
  in the database; a manager listing orphan attachments.

**Multi-database and internationalization**
- A database picker at startup (create/open/duplicate/delete a database,
  recent databases).
- Multilingual application (at least French + English), with a centralized
  translation mechanism — no sentence hardcoded in a screen's code, to stay
  grammatically correct across languages (watch for gender, plurals, word
  order).

This list can be split into phases if you generate the project
incrementally, but every feature above must eventually be covered — this is
a complete finance manager, not a demo.

### 4. Slint-specific guidance

- **Write the UI in `.slint` files**, compiled by `slint-build` in
  `build.rs`. Don't mix business logic into `.slint` files: they declare
  `property`s, `callback`s, and layout; the Rust binary supplies data and
  reacts to callbacks.
- **Rethink multi-window navigation.** The application this prompt is
  derived from opened a large number of independent OS-level windows (about
  thirty). Slint does support several top-level windows, but that much
  proliferation isn't idiomatic Slint usage. Recommended approach: one main
  window with view/panel-based navigation (account list → transaction list →
  transaction form, etc.), and **Slint modals or popups** (`Dialog`,
  `PopupWindow`) for one-off forms (account form, transaction form,
  confirmations). Only use separate OS windows where it's genuinely useful
  (e.g. keeping a second account's transaction list open side by side with
  the first).
- **Reusable components**: build a generic table component (column sorting,
  filtering, multi-selection) on top of `TableView`/`ListView`/
  `StandardTableView`, and an amount-field component that formats/parses
  according to currency and locale — these are the two building blocks
  almost every screen reuses.
- **Data models**: expose lists to Slint via `ModelRc`/`VecModel`/
  `FilterModel`/`SortModel`, updated from Rust after each business-core call;
  don't duplicate business state on both sides.
- **Translations**: use Slint's native mechanism (the `@tr(...)` macro in
  `.slint` files, extraction via `slint-tr-extractor`, `.po` files) rather
  than reinventing a dictionary system in Rust.
- **Theme**: pick one Slint style (`fluent`, `material`, or `cosmic`) and
  stay consistent: same components, same spacing, a single shared
  stylesheet/set of color constants across all `.slint` files.
- **Testing**: `.slint` files themselves aren't easily unit-testable — push
  as much logic as possible (formatting, validation, calculations) into
  `core`, tested with `cargo test`, and keep the Slint layer as thin as
  possible to limit what escapes automated testing.

### 5. Expected quality bar

- `cargo test --workspace`, `cargo clippy --workspace --all-targets`, and
  `cargo fmt --check` must all pass before any delivery.
- The business core (`core`) must have meaningful test coverage on the
  risky areas: balance computation, sub-transaction splitting, recurring
  transaction generation, OFX/CSV import (deduplication), schema migrations.
- Document the architecture (layers, portable folder, backup format) in
  `docs/`, in the style of an `ARCHITECTURE.md`.
- Code identifiers and comments in English; every user-facing message goes
  through the translation mechanism, never hardcoded in Rust or Slint code.

### 6. Out of scope (unless explicitly requested)

- Binary compatibility with an existing third-party application's backup
  format — build your own coherent backup/restore format, with no constraint
  from a legacy application to port.
- Cloud sync, multi-user support, or a remote server: the application stays
  local and single-user.
- Multi-currency conversion: one currency per account is enough, with no
  exchange rate.

---

## Notes for whoever uses this prompt

- This prompt was generated by analyzing an existing Rust project (Tauri +
  web UI) implementing a very complete personal bank account manager. It
  carries over its features and architectural principles (strict separation
  between business core and UI), adapting the Tauri-specific parts (many
  system windows, JSON IPC bridge, webview) to Slint idioms.
- The new project isn't meant to be data-compatible with the original
  application: the database schema, table/column names, and backup format
  are entirely free choices.
- Suggested usage: paste the "Prompt to copy" section as-is at the very
  start of a conversation with the code-generation agent, before any other
  context instruction.
