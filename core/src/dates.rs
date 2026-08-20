//! Small ISO-8601 (`YYYY-MM-DD`) date helpers shared by the recurring-rule
//! engine, the budget module, and form defaults. Deliberately minimal: a
//! transaction date is a calendar day, not an instant, so there is no
//! timezone or locale handling here.

use crate::error::{CoreError, Result};
use time::{Duration, Month, OffsetDateTime};

pub type Date = time::Date;

pub fn today() -> String {
    format(OffsetDateTime::now_utc().date())
}

pub fn parse(text: &str) -> Result<Date> {
    let mut parts = text.splitn(3, '-');
    let (Some(y), Some(m), Some(d)) = (parts.next(), parts.next(), parts.next()) else {
        return Err(CoreError::InvalidDate(text.to_string()));
    };
    let invalid = || CoreError::InvalidDate(text.to_string());
    let year: i32 = y.parse().map_err(|_| invalid())?;
    let month: u8 = m.parse().map_err(|_| invalid())?;
    let day: u8 = d.parse().map_err(|_| invalid())?;
    let month = Month::try_from(month).map_err(|_| invalid())?;
    Date::from_calendar_date(year, month, day).map_err(|_| invalid())
}

pub fn format(date: Date) -> String {
    format!(
        "{:04}-{:02}-{:02}",
        date.year(),
        u8::from(date.month()),
        date.day()
    )
}

/// Adds `count` months to `date`, clamping the day to the target month's
/// length - Jan 31 plus one month is Feb 28 (or 29 on a leap year), not an
/// error and not a roll-over into March.
pub fn add_months(date: Date, count: i64) -> Date {
    let total = date.year() as i64 * 12 + (u8::from(date.month()) as i64 - 1) + count;
    let year = total.div_euclid(12) as i32;
    let month =
        Month::try_from((total.rem_euclid(12) + 1) as u8).expect("0..=11 + 1 is a valid month");
    let day = date.day().min(month.length(year));
    Date::from_calendar_date(year, month, day).expect("clamped day is always valid for its month")
}

pub fn add_days(date: Date, count: i64) -> Date {
    date + Duration::days(count)
}

/// The first and last day of `date`'s month, as `YYYY-MM-DD` text - the
/// bounds `dashboard::compute` sums the current month against.
pub fn month_bounds(date: Date) -> (String, String) {
    let first =
        Date::from_calendar_date(date.year(), date.month(), 1).expect("day 1 always exists");
    let last_day = date.month().length(date.year());
    let last = Date::from_calendar_date(date.year(), date.month(), last_day)
        .expect("a month's own length is a valid day");
    (format(first), format(last))
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn round_trips_through_text() {
        let date = parse("2026-03-05").unwrap();
        assert_eq!(format(date), "2026-03-05");
    }

    #[test]
    fn rejects_garbage() {
        assert!(parse("not-a-date").is_err());
        assert!(parse("2026-13-01").is_err());
    }

    #[test]
    fn clamps_month_end_when_adding_months() {
        let jan_31 = parse("2026-01-31").unwrap();
        assert_eq!(format(add_months(jan_31, 1)), "2026-02-28");
    }

    #[test]
    fn keeps_leap_day_on_a_leap_year() {
        let jan_31 = parse("2028-01-31").unwrap();
        assert_eq!(format(add_months(jan_31, 1)), "2028-02-29");
    }

    #[test]
    fn wraps_the_year_when_adding_past_december() {
        let nov_15 = parse("2026-11-15").unwrap();
        assert_eq!(format(add_months(nov_15, 3)), "2027-02-15");
    }

    #[test]
    fn month_bounds_covers_the_whole_month() {
        let feb_15 = parse("2028-02-15").unwrap();
        let (first, last) = month_bounds(feb_15);
        assert_eq!(first, "2028-02-01");
        assert_eq!(last, "2028-02-29"); // 2028 is a leap year
    }
}
