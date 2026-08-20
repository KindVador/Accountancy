# Accountancy

[![Windows Release](https://github.com/KindVador/Accountancy/actions/workflows/windows_release.yml/badge.svg)](https://github.com/KindVador/Accountancy/actions/workflows/windows_release.yml)
[![macOS Release](https://github.com/KindVador/Accountancy/actions/workflows/macOS_release.yml/badge.svg)](https://github.com/KindVador/Accountancy/actions/workflows/macOS_release.yml)
[![linux Release](https://github.com/KindVador/Accountancy/actions/workflows/linux_release.yml/badge.svg)](https://github.com/KindVador/Accountancy/actions/workflows/linux_release.yml)
[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit)](https://github.com/pre-commit/pre-commit)

Personal application to manage bank accounts. Being rewritten in **Rust**,
with a **Slint** desktop UI (previously Qt/C++) - see [`PROMPT.md`](PROMPT.md)
for the full feature scope and rationale, and [`docs/architecture.md`](docs/architecture.md) /
[`docs/data_model.md`](docs/data_model.md) for how the rewrite is structured.

## Status

Early rewrite. Working today: accounts (create/edit/close/delete, three
cumulative balances), transactions with split lines and an
automatically-maintained remainder, and the shared reference lists
(categories, payees, payment modes, tags, projects, budget lines, banks).
Everything else listed in `PROMPT.md` (cheques, recurring transactions,
budget control, statistics, import/export, printing, backup/restore,
attachments, multi-database, translations) is not built yet.

## Building and running

```bash
cargo test --workspace
cargo clippy --workspace --all-targets
cargo run -p accountancy
```

The application stores its databases in a `databases/` folder created next
to the built executable (portable by design - see `docs/architecture.md`).
