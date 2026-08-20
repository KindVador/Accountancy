use accountancy_core::money::Amount;

/// Presentation-only formatting: `Amount`'s own `Display` is the raw,
/// locale-independent form the core stores and compares against in tests.
/// Locale-aware grouping/separators belong here, in the UI layer.
pub fn amount(value: Amount) -> String {
    value.to_string()
}

pub fn amount_with_currency(value: Amount, currency: &str) -> String {
    format!("{} {}", amount(value), currency)
}

pub fn optional_date(date: &Option<String>) -> String {
    date.clone().unwrap_or_default()
}
