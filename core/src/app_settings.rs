//! Where the app remembers which database to reopen next launch, and which
//! ones were opened recently - the one piece of state that lives outside
//! any single database, since it has to be readable before any database is
//! chosen. Lives at `settings/app_config.json`, next to the `databases/`
//! folder - both under the portable root.

use crate::error::{CoreError, Result};
use serde::{Deserialize, Serialize};
use std::path::{Path, PathBuf};

#[derive(Debug, Clone, Serialize, Deserialize, Default, PartialEq, Eq)]
pub struct AppSettings {
    pub last_database: Option<String>,
    #[serde(default)]
    pub recent_databases: Vec<String>,
    /// The bundled-translation locale to select at startup (`"fr"`, or
    /// `None`/`"en"` for the source language). See `PROMPT.md`'s
    /// internationalization requirement.
    #[serde(default)]
    pub language: Option<String>,
}

const MAX_RECENT: usize = 5;

fn default_path() -> PathBuf {
    let exe_dir = std::env::current_exe()
        .ok()
        .and_then(|p| p.parent().map(Path::to_path_buf))
        .unwrap_or_else(|| PathBuf::from("."));
    exe_dir.join("settings").join("app_config.json")
}

/// Reads the settings file, or a default (no last database, empty recents)
/// if it's missing or unreadable - a bad or absent settings file must never
/// stop the application from starting.
pub fn load() -> AppSettings {
    load_from(&default_path())
}

fn load_from(path: &Path) -> AppSettings {
    std::fs::read_to_string(path)
        .ok()
        .and_then(|text| serde_json::from_str(&text).ok())
        .unwrap_or_default()
}

pub fn save(settings: &AppSettings) -> Result<()> {
    save_to(&default_path(), settings)
}

fn save_to(path: &Path, settings: &AppSettings) -> Result<()> {
    if let Some(parent) = path.parent() {
        std::fs::create_dir_all(parent).map_err(|source| CoreError::Io {
            path: parent.to_path_buf(),
            source,
        })?;
    }
    let text = serde_json::to_string_pretty(settings).unwrap_or_default();
    std::fs::write(path, text).map_err(|source| CoreError::Io {
        path: path.to_path_buf(),
        source,
    })
}

/// Records `name` as the last-opened database and moves it to the front of
/// the recent list, capped at five. Best-effort: a settings write that
/// fails is not worth interrupting the user over.
pub fn record_opened(name: &str) {
    let mut settings = load();
    settings.last_database = Some(name.to_string());
    settings.recent_databases.retain(|n| n != name);
    settings.recent_databases.insert(0, name.to_string());
    settings.recent_databases.truncate(MAX_RECENT);
    let _ = save(&settings);
}

/// Records the chosen translation locale for the next launch.
/// Best-effort, same reasoning as `record_opened`.
pub fn record_language(language: &str) {
    let mut settings = load();
    settings.language = Some(language.to_string());
    let _ = save(&settings);
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn round_trips_through_the_file() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("app_config.json");

        let settings = AppSettings {
            last_database: Some("Household".into()),
            recent_databases: vec!["Household".into()],
            language: Some("fr".into()),
        };
        save_to(&path, &settings).unwrap();

        assert_eq!(load_from(&path), settings);
    }

    #[test]
    fn missing_file_loads_as_default() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("does-not-exist.json");
        assert_eq!(load_from(&path), AppSettings::default());
    }
}
