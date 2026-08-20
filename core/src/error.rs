use std::path::PathBuf;

#[derive(Debug, thiserror::Error)]
pub enum CoreError {
    #[error("database error: {0}")]
    Database(#[from] rusqlite::Error),
    #[error("io error at {path}: {source}")]
    Io {
        path: PathBuf,
        #[source]
        source: std::io::Error,
    },
    #[error("{0} not found (id {1})")]
    NotFound(&'static str, i64),
    #[error("{field} cannot be empty")]
    EmptyField { field: &'static str },
    #[error("a {kind} named '{name}' already exists")]
    DuplicateName { kind: &'static str, name: String },
    #[error("invalid amount: {0}")]
    InvalidAmount(String),
    #[error("invalid date: {0}")]
    InvalidDate(String),
    #[error("invalid month: {0} (must be 1-12)")]
    InvalidMonth(i32),
    #[error("{0}")]
    Invalid(String),
}

pub type Result<T> = std::result::Result<T, CoreError>;
