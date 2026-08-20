//! Money is always an integer number of micro-units (value * 1_000_000).
//! No floating point ever touches an amount - see `docs/data_model.md`.

use crate::error::{CoreError, Result};
use std::fmt;
use std::iter::Sum;
use std::ops::{Add, AddAssign, Neg, Sub, SubAssign};

const MICROS_PER_UNIT: i64 = 1_000_000;

#[derive(
    Debug,
    Clone,
    Copy,
    PartialEq,
    Eq,
    PartialOrd,
    Ord,
    Default,
    serde::Serialize,
    serde::Deserialize,
)]
pub struct Amount(i64);

impl Amount {
    pub const ZERO: Amount = Amount(0);

    pub fn from_micros(micros: i64) -> Self {
        Amount(micros)
    }

    pub fn micros(self) -> i64 {
        self.0
    }

    /// Parses a decimal amount such as "12.34", "-12,34" or "12" into
    /// micro-units. Accepts both '.' and ',' as the decimal separator since
    /// the UI is expected to localize input before display, not before
    /// storage.
    pub fn parse(input: &str) -> Result<Amount> {
        let trimmed = input.trim();
        if trimmed.is_empty() {
            return Err(CoreError::InvalidAmount(input.to_string()));
        }
        let normalized = trimmed.replace(',', ".");
        let negative = normalized.starts_with('-');
        let unsigned = normalized.trim_start_matches(['-', '+']);

        let mut parts = unsigned.splitn(2, '.');
        let integer_part = parts.next().unwrap_or("0");
        let fraction_part = parts.next().unwrap_or("");
        if fraction_part.len() > 6
            || !integer_part.chars().all(|c| c.is_ascii_digit())
            || !fraction_part.chars().all(|c| c.is_ascii_digit())
        {
            return Err(CoreError::InvalidAmount(input.to_string()));
        }

        let integer: i64 = integer_part.parse().unwrap_or(0);
        let mut fraction = fraction_part.to_string();
        while fraction.len() < 6 {
            fraction.push('0');
        }
        let fraction: i64 = fraction.parse().unwrap_or(0);

        let micros = integer * MICROS_PER_UNIT + fraction;
        Ok(Amount(if negative { -micros } else { micros }))
    }

    pub fn is_debit(self) -> bool {
        self.0 < 0
    }

    pub fn is_credit(self) -> bool {
        self.0 > 0
    }

    pub fn abs(self) -> Amount {
        Amount(self.0.abs())
    }
}

impl fmt::Display for Amount {
    /// Raw, locale-independent formatting (always 2 decimals, '.' separator).
    /// The UI layer is responsible for locale-specific presentation.
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let sign = if self.0 < 0 { "-" } else { "" };
        let whole = self.0.unsigned_abs() / MICROS_PER_UNIT as u64;
        let cents = (self.0.unsigned_abs() % MICROS_PER_UNIT as u64) / 10_000;
        write!(f, "{sign}{whole}.{cents:02}")
    }
}

impl Add for Amount {
    type Output = Amount;
    fn add(self, rhs: Self) -> Self::Output {
        Amount(self.0 + rhs.0)
    }
}

impl Sub for Amount {
    type Output = Amount;
    fn sub(self, rhs: Self) -> Self::Output {
        Amount(self.0 - rhs.0)
    }
}

impl Neg for Amount {
    type Output = Amount;
    fn neg(self) -> Self::Output {
        Amount(-self.0)
    }
}

impl AddAssign for Amount {
    fn add_assign(&mut self, rhs: Self) {
        self.0 += rhs.0;
    }
}

impl SubAssign for Amount {
    fn sub_assign(&mut self, rhs: Self) {
        self.0 -= rhs.0;
    }
}

impl Sum for Amount {
    fn sum<I: Iterator<Item = Amount>>(iter: I) -> Self {
        iter.fold(Amount::ZERO, Add::add)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn parses_plain_decimal() {
        assert_eq!(
            Amount::parse("12.34").unwrap(),
            Amount::from_micros(12_340_000)
        );
    }

    #[test]
    fn parses_comma_decimal() {
        assert_eq!(
            Amount::parse("12,34").unwrap(),
            Amount::from_micros(12_340_000)
        );
    }

    #[test]
    fn parses_negative() {
        assert_eq!(
            Amount::parse("-5.5").unwrap(),
            Amount::from_micros(-5_500_000)
        );
    }

    #[test]
    fn parses_integer() {
        assert_eq!(
            Amount::parse("42").unwrap(),
            Amount::from_micros(42_000_000)
        );
    }

    #[test]
    fn rejects_garbage() {
        assert!(Amount::parse("abc").is_err());
        assert!(Amount::parse("").is_err());
    }

    #[test]
    fn displays_two_decimals() {
        assert_eq!(Amount::from_micros(1_005_000).to_string(), "1.00");
        assert_eq!(Amount::from_micros(-1_255_000).to_string(), "-1.25");
    }

    #[test]
    fn sums_amounts() {
        let amounts = vec![
            Amount::parse("10").unwrap(),
            Amount::parse("-3.50").unwrap(),
        ];
        let total: Amount = amounts.into_iter().sum();
        assert_eq!(total, Amount::parse("6.50").unwrap());
    }
}
