# Architecture

Two crates, one direction: `app` depends on `core`, never the reverse.

```
core/   accountancy-core — entities, SQLite persistence, business rules.
        No graphical dependency. `cargo test -p accountancy-core` runs it
        standalone.
app/    accountancy — the Slint desktop application. `app/ui/*.slint`
        declares the presentation; `app/src/main.rs` wires Slint callbacks
        to `core` calls and pushes results back into Slint models.
```

## Why the split

The business rules that matter here - balance recomputation, the
parent/child transaction tree, soft-linked reference lists - have nothing to
do with how they're displayed. Keeping them in `core` means:

- they're covered by fast, UI-free `cargo test`;
- the UI layer (`app`) stays thin: it formats data for display and turns
  button clicks into `core` function calls, nothing more;
- a future UI (a different toolkit, a CLI, a web frontend) could reuse
  `core` unchanged.

If you're about to write a calculation, a validation rule, or anything that
touches the database inside `app/src/main.rs` or a `.slint` file, it belongs
in `core` instead.

## The Slint layer

- `app/build.rs` compiles `app/ui/app-window.slint` via `slint-build`.
- `app/src/main.rs` calls `slint::include_modules!()`, which generates a
  `MainWindow` Rust type together with every `export struct` and `export
  component MainWindow`'s properties and callbacks declared in the `.slint`
  file.
- The main window is a single top-level window with panel-based navigation
  (accounts on the left, the selected account's transactions on the right)
  and in-window modal overlays for forms (`FormOverlay` component), rather
  than the many independent OS windows a legacy multi-window desktop app
  might use. See `PROMPT.md` §4 for the reasoning.
- `AppState` (in `main.rs`) holds the open `core::db::Database` plus small
  caches (`accounts_cache`, `transactions_cache`, `categories_cache`) used to
  translate a Slint row index back into a database id when a callback fires.
  It is wrapped in `Rc<RefCell<..>>` and shared across callback closures via
  `ui.as_weak()` + `Rc::clone`, the standard Slint pattern for avoiding
  reference cycles between the window and its own callbacks.

## The portable folder

`core::db::databases_root()` resolves to a `databases/` folder next to the
running executable. Each database is its own subfolder holding
`accountancy.sqlite`, and an `attachments/` folder for files attached to
transactions (once that feature lands). Nothing the application writes goes
anywhere else, so the whole install can be moved or run from removable
media.

## What's implemented so far

- Accounts: create/edit/close/reopen/delete, three cumulative balances
  (cleared/committed/projected) recomputed from scratch on every mutation.
- Transactions: create/edit/delete, the parent/child split tree with an
  auto-maintained remainder line, per-line running balance.
- Reference lists: categories (with a soft link to a budget line), payees,
  payment modes, tags, projects, budget lines, banks - shared CRUD with
  "no empty name", "no duplicate name", and reassign-to-unspecified-on-delete
  rules.

Everything else in `PROMPT.md` (cheques, recurring transactions, budget
control, statistics, OFX/CSV/Excel import, export, printing, backup/restore,
attachments, multi-database, translations) is not built yet - see that file
for the full feature list this project is working toward.
