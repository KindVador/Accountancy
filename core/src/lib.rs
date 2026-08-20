//! `accountancy-core` - the business-rule core of the application: entities,
//! SQLite persistence, and the invariants that protect them (balance
//! recomputation, the parent/child transaction tree, soft-linked reference
//! lists). Depends on no graphical toolkit; compiles and is tested entirely
//! on its own with `cargo test -p accountancy-core`.

pub mod accounts;
pub mod budget;
pub mod cheques;
pub mod dashboard;
pub mod dates;
pub mod db;
pub mod error;
pub mod money;
pub mod recurring;
pub mod reference_lists;
pub mod statistics;
pub mod transactions;

pub use dates::today;
pub use error::{CoreError, Result};
