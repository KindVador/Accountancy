slint::include_modules!();

mod format;

use accountancy_core::accounts::{self, Account, NewAccount};
use accountancy_core::budget;
use accountancy_core::cheques::{self, Cheque, ChequeStatus, Chequebook, NewChequebook};
use accountancy_core::dashboard;
use accountancy_core::dates;
use accountancy_core::db;
use accountancy_core::import::{self, csv, ofx};
use accountancy_core::money::Amount;
use accountancy_core::recurring::{self, EndMode, NewRecurringRule, Period, RecurringRule};
use accountancy_core::reference_lists::{self, Category, NamedItem, ReferenceKind};
use accountancy_core::statistics::{self, Axis};
use accountancy_core::transactions::{self, NewTransaction, Status, Transaction};
use slint::{ComponentHandle, ModelRc, SharedString, VecModel};
use std::cell::RefCell;
use std::rc::Rc;

struct AppState {
    db: db::Database,
    accounts_cache: Vec<Account>,
    transactions_cache: Vec<Transaction>,
    categories_cache: Vec<Category>,
    recurring_cache: Vec<RecurringRule>,
    budget_row_ids: Vec<i64>,
    payees_cache: Vec<NamedItem>,
    chequebooks_cache: Vec<Chequebook>,
    cheques_cache: Vec<Cheque>,
    cheque_option_ids: Vec<i64>,
    selected_account_id: Option<i64>,
    selected_chequebook_id: Option<i64>,
    editing_account_id: Option<i64>,
    editing_transaction_id: Option<i64>,
    editing_recurring_id: Option<i64>,
    editing_cheque_id: Option<i64>,
    pending_csv_content: Option<String>,
}

fn main() -> Result<(), slint::PlatformError> {
    let folder = db::databases_root().join("default");
    let database = db::open(&folder).expect("failed to open the database");

    let state = Rc::new(RefCell::new(AppState {
        db: database,
        accounts_cache: Vec::new(),
        transactions_cache: Vec::new(),
        categories_cache: Vec::new(),
        recurring_cache: Vec::new(),
        budget_row_ids: Vec::new(),
        payees_cache: Vec::new(),
        chequebooks_cache: Vec::new(),
        cheques_cache: Vec::new(),
        cheque_option_ids: Vec::new(),
        selected_account_id: None,
        selected_chequebook_id: None,
        editing_account_id: None,
        editing_transaction_id: None,
        editing_recurring_id: None,
        editing_cheque_id: None,
        pending_csv_content: None,
    }));

    let ui = MainWindow::new()?;
    ui.set_budget_year(accountancy_core::today()[..4].into());
    let (month_start, month_end) =
        dates::month_bounds(dates::parse(&accountancy_core::today()).unwrap());
    ui.set_statistics_start_date(month_start.into());
    ui.set_statistics_end_date(month_end.into());

    refresh_categories(&ui, &state);
    refresh_payees(&ui, &state);
    refresh_accounts(&ui, &state);
    refresh_transactions(&ui, &state);
    refresh_recurring(&ui, &state);
    refresh_budget(&ui, &state);
    refresh_chequebooks(&ui, &state);
    refresh_cheques(&ui, &state);
    refresh_dashboard(&ui, &state);
    refresh_statistics(&ui, &state);

    wire_account_callbacks(&ui, &state);
    wire_transaction_callbacks(&ui, &state);
    wire_recurring_callbacks(&ui, &state);
    wire_budget_callbacks(&ui, &state);
    wire_cheque_callbacks(&ui, &state);
    wire_dashboard_callbacks(&ui, &state);
    wire_statistics_callbacks(&ui, &state);
    wire_import_callbacks(&ui, &state);

    ui.run()
}

// --- Slint <-> core translation ---

fn to_account_item(account: &Account) -> AccountItem {
    AccountItem {
        id: account.id as i32,
        name: account.name.clone().into(),
        currency: account.currency.clone().into(),
        balance_text: format::amount(account.balance_projected).into(),
        is_closed: account.is_closed,
    }
}

fn status_to_index(status: Status) -> i32 {
    match status {
        Status::Cleared => 0,
        Status::Committed => 1,
        Status::Planned => 2,
    }
}

fn index_to_status(index: i32) -> Status {
    match index {
        0 => Status::Cleared,
        2 => Status::Planned,
        _ => Status::Committed,
    }
}

fn period_to_index(period: Period) -> i32 {
    period.as_i64() as i32 - 1
}

fn index_to_period(index: i32) -> Period {
    Period::from_i64(index as i64 + 1)
}

fn period_label(period: Period, frequency: i64) -> String {
    let unit = match period {
        Period::Day => "day(s)",
        Period::Week => "week(s)",
        Period::Month => "month(s)",
        Period::Quarter => "quarter(s)",
        Period::Year => "year(s)",
    };
    format!("Every {frequency} {unit}")
}

fn end_mode_to_index(end_mode: EndMode) -> i32 {
    end_mode.as_i64() as i32 - 1
}

fn index_to_end_mode(index: i32) -> EndMode {
    EndMode::from_i64(index as i64 + 1)
}

fn to_recurring_item(rule: &RecurringRule, accounts: &[Account]) -> RecurringRuleItem {
    let account_text = accounts
        .iter()
        .find(|a| a.id == rule.account_id)
        .map(|a| a.name.clone())
        .unwrap_or_default();
    let end_text = match rule.end_mode {
        EndMode::NoLimit => "No limit".to_string(),
        EndMode::UntilDate => format!("Until {}", rule.end_date.clone().unwrap_or_default()),
        EndMode::OccurrenceCount => {
            format!(
                "{} / {}",
                rule.generated_count,
                rule.occurrence_count.unwrap_or(0)
            )
        }
    };
    RecurringRuleItem {
        id: rule.id as i32,
        account_text: account_text.into(),
        label: rule.label.clone().into(),
        amount_text: format::amount(rule.amount).into(),
        period_text: period_label(rule.period, rule.period_frequency).into(),
        next_due_text: rule.next_due_date.clone().into(),
        end_text: end_text.into(),
    }
}

fn cheque_status_to_index(status: ChequeStatus) -> i32 {
    status.as_i64() as i32 - 1
}

fn index_to_cheque_status(index: i32) -> ChequeStatus {
    ChequeStatus::from_i64(index as i64 + 1)
}

fn cheque_status_label(status: ChequeStatus) -> &'static str {
    match status {
        ChequeStatus::Available => "Available",
        ChequeStatus::Issued => "Issued",
        ChequeStatus::Cleared => "Cleared",
        ChequeStatus::Cancelled => "Cancelled",
    }
}

fn payee_name(payees: &[NamedItem], payee_id: i64) -> String {
    if payee_id == 0 {
        "Unspecified".to_string()
    } else {
        payees
            .iter()
            .find(|p| p.id == payee_id)
            .map(|p| p.name.clone())
            .unwrap_or_else(|| "Unspecified".to_string())
    }
}

fn to_chequebook_item(book: &Chequebook) -> ChequebookItem {
    ChequebookItem {
        id: book.id as i32,
        range_text: format!("#{} - #{}", book.first_number, book.last_number).into(),
        count_text: book.count().to_string().into(),
        reception_text: book.reception_date.clone().unwrap_or_default().into(),
    }
}

fn to_cheque_item(cheque: &Cheque, payees: &[NamedItem]) -> ChequeItem {
    ChequeItem {
        id: cheque.id as i32,
        number_text: cheque.number.to_string().into(),
        date_text: cheque.date.clone().unwrap_or_default().into(),
        payee_text: payee_name(payees, cheque.payee_id).into(),
        amount_text: format::amount(cheque.amount).into(),
        status_text: cheque_status_label(cheque.status).into(),
    }
}

fn cheque_label(cheque: &Cheque, payees: &[NamedItem]) -> String {
    format!(
        "#{} - {} ({})",
        cheque.number,
        payee_name(payees, cheque.payee_id),
        cheque_status_label(cheque.status)
    )
}

fn axis_from_index(index: i32) -> Axis {
    match index {
        1 => Axis::Tag,
        2 => Axis::PaymentMode,
        3 => Axis::Payee,
        4 => Axis::Project,
        5 => Axis::BudgetLine,
        _ => Axis::Category,
    }
}

fn csv_date_order_from_index(index: i32) -> csv::DateOrder {
    match index {
        1 => csv::DateOrder::DayMonthYear,
        2 => csv::DateOrder::MonthDayYear,
        _ => csv::DateOrder::YearMonthDay,
    }
}

fn parse_column_index(text: &str) -> Option<usize> {
    let trimmed = text.trim();
    if trimmed.is_empty() {
        None
    } else {
        trimmed.parse().ok()
    }
}

fn import_summary_message(summary: import::ImportSummary) -> String {
    format!(
        "{} transaction(s) imported, {} already on file.",
        summary.imported, summary.skipped_duplicates
    )
}

// --- Refreshing the UI from the database ---

fn refresh_accounts(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    let list = {
        let st = state.borrow();
        accounts::list(st.db.connection(), true).unwrap_or_default()
    };

    let items: Vec<AccountItem> = list.iter().map(to_account_item).collect();
    ui.set_accounts(ModelRc::new(VecModel::from(items)));

    let names: Vec<SharedString> = list
        .iter()
        .map(|a| SharedString::from(a.name.clone()))
        .collect();
    ui.set_account_names(ModelRc::new(VecModel::from(names.clone())));

    let mut names_with_all: Vec<SharedString> = vec!["All accounts".into()];
    names_with_all.extend(names);
    ui.set_statistics_account_names(ModelRc::new(VecModel::from(names_with_all)));

    let selected_id = state.borrow().selected_account_id;
    let index = selected_id.and_then(|id| list.iter().position(|a| a.id == id));
    ui.set_selected_account_index(index.map(|i| i as i32).unwrap_or(-1));

    state.borrow_mut().accounts_cache = list;
}

fn refresh_transactions(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    let selected_account_id = state.borrow().selected_account_id;

    let (list, account) = match selected_account_id {
        Some(id) => {
            let st = state.borrow();
            let list = transactions::list_for_account(st.db.connection(), id).unwrap_or_default();
            let account = accounts::get(st.db.connection(), id).ok();
            (list, account)
        }
        None => (Vec::new(), None),
    };

    let categories = state.borrow().categories_cache.clone();
    let category_name = |id: i64| -> String {
        if id == 0 {
            "Unspecified".to_string()
        } else {
            categories
                .iter()
                .find(|c| c.id == id)
                .map(|c| c.name.clone())
                .unwrap_or_else(|| "Unspecified".to_string())
        }
    };

    let items: Vec<TransactionItem> = list
        .iter()
        .map(|t| TransactionItem {
            id: t.id as i32,
            date_text: format::optional_date(&t.date).into(),
            label: t.label.clone().into(),
            category_text: category_name(t.category_id).into(),
            amount_text: format::amount(t.amount).into(),
            balance_text: format::amount(t.running_balance).into(),
            is_debit: t.amount.is_debit(),
            is_child: t.parent_id != 0,
            is_remainder: t.is_remainder,
        })
        .collect();
    ui.set_transactions(ModelRc::new(VecModel::from(items)));
    ui.set_selected_transaction_index(-1);

    match &account {
        Some(account) => {
            ui.set_selected_account_name(account.name.clone().into());
            ui.set_selected_account_balance_text(
                format::amount_with_currency(account.balance_projected, &account.currency).into(),
            );
        }
        None => {
            ui.set_selected_account_name("No account selected".into());
            ui.set_selected_account_balance_text("".into());
        }
    }

    state.borrow_mut().transactions_cache = list;
    refresh_transaction_cheque_options(ui, state);
}

fn refresh_transaction_cheque_options(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    let account_id = state.borrow().selected_account_id;
    let payees = state.borrow().payees_cache.clone();
    let cheques = match account_id {
        Some(id) => {
            let st = state.borrow();
            cheques::list_cheques_for_account(st.db.connection(), id).unwrap_or_default()
        }
        None => Vec::new(),
    };

    let mut names: Vec<SharedString> = vec!["None".into()];
    names.extend(
        cheques
            .iter()
            .map(|c| SharedString::from(cheque_label(c, &payees))),
    );
    ui.set_cheque_names(ModelRc::new(VecModel::from(names)));

    state.borrow_mut().cheque_option_ids = cheques.iter().map(|c| c.id).collect();
}

fn refresh_categories(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    let list = {
        let st = state.borrow();
        reference_lists::list_categories(st.db.connection()).unwrap_or_default()
    };

    let mut names: Vec<SharedString> = vec!["Unspecified".into()];
    names.extend(list.iter().map(|c| SharedString::from(c.name.clone())));
    ui.set_category_names(ModelRc::new(VecModel::from(names)));

    state.borrow_mut().categories_cache = list;
}

fn refresh_payees(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    let list = {
        let st = state.borrow();
        reference_lists::list(st.db.connection(), ReferenceKind::Payee).unwrap_or_default()
    };

    let mut names: Vec<SharedString> = vec!["Unspecified".into()];
    names.extend(list.iter().map(|p| SharedString::from(p.name.clone())));
    ui.set_payee_names(ModelRc::new(VecModel::from(names)));

    state.borrow_mut().payees_cache = list;
}

fn refresh_chequebooks(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    let account_id = state.borrow().selected_account_id;
    let list = match account_id {
        Some(id) => {
            let st = state.borrow();
            cheques::list_chequebooks(st.db.connection(), Some(id)).unwrap_or_default()
        }
        None => Vec::new(),
    };

    let items: Vec<ChequebookItem> = list.iter().map(to_chequebook_item).collect();
    ui.set_chequebooks(ModelRc::new(VecModel::from(items)));
    ui.set_selected_chequebook_index(-1);

    let mut st = state.borrow_mut();
    st.selected_chequebook_id = None;
    st.chequebooks_cache = list;
}

fn refresh_cheques(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    let chequebook_id = state.borrow().selected_chequebook_id;
    let payees = state.borrow().payees_cache.clone();
    let list = match chequebook_id {
        Some(id) => {
            let st = state.borrow();
            cheques::list_cheques(st.db.connection(), id).unwrap_or_default()
        }
        None => Vec::new(),
    };

    let items: Vec<ChequeItem> = list.iter().map(|c| to_cheque_item(c, &payees)).collect();
    ui.set_cheques(ModelRc::new(VecModel::from(items)));
    ui.set_selected_cheque_index(-1);

    state.borrow_mut().cheques_cache = list;
}

fn refresh_dashboard(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    let data = {
        let st = state.borrow();
        dashboard::compute(st.db.connection()).unwrap_or_default()
    };

    ui.set_dashboard_data(DashboardData {
        cleared_text: format::amount(data.total_cleared).into(),
        committed_text: format::amount(data.total_committed).into(),
        projected_text: format::amount(data.total_projected).into(),
        income_text: format::amount(data.current_month_income).into(),
        expense_text: format::amount(data.current_month_expense).into(),
        net_text: format::amount(data.current_month_net()).into(),
    });
}

fn refresh_statistics(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    let axis = axis_from_index(ui.get_statistics_axis_index());
    let account_index = ui.get_statistics_account_index();
    let account_id = if account_index <= 0 {
        None
    } else {
        state
            .borrow()
            .accounts_cache
            .get((account_index - 1) as usize)
            .map(|a| a.id)
    };
    let start = ui.get_statistics_start_date().to_string();
    let end = ui.get_statistics_end_date().to_string();

    let lines = {
        let st = state.borrow();
        statistics::compute(st.db.connection(), axis, account_id, &start, &end).unwrap_or_default()
    };

    let total: Amount = lines.iter().map(|l| l.total).sum();
    let items: Vec<StatisticLineItem> = lines
        .iter()
        .map(|l| StatisticLineItem {
            label: l.label.clone().into(),
            amount_text: format::amount(l.total).into(),
            is_debit: l.total.is_debit(),
        })
        .collect();
    ui.set_statistics_rows(ModelRc::new(VecModel::from(items)));
    ui.set_statistics_total_text(format::amount(total).into());
}

fn refresh_recurring(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    let list = {
        let st = state.borrow();
        recurring::list(st.db.connection(), None).unwrap_or_default()
    };

    let accounts = state.borrow().accounts_cache.clone();
    let items: Vec<RecurringRuleItem> = list
        .iter()
        .map(|r| to_recurring_item(r, &accounts))
        .collect();
    ui.set_recurring_rules(ModelRc::new(VecModel::from(items)));
    ui.set_selected_recurring_index(-1);

    state.borrow_mut().recurring_cache = list;
}

fn budget_period(ui: &MainWindow) -> (i32, i32) {
    let year = ui.get_budget_year().parse::<i32>().unwrap_or(2026);
    let month = ui.get_budget_month_index() + 1;
    (year, month)
}

fn refresh_budget(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    let (year, month) = budget_period(ui);
    let control_mode = ui.get_budget_mode() == "control";

    let (items, ids): (Vec<BudgetRowItem>, Vec<i64>) = {
        let st = state.borrow();
        let conn = st.db.connection();
        if control_mode {
            let rows = budget::control_for_month(conn, year, month).unwrap_or_default();
            let ids = rows.iter().map(|r| r.budget_line_id).collect();
            let items = rows
                .iter()
                .map(|r| BudgetRowItem {
                    name: r.budget_line_name.clone().into(),
                    budgeted_text: format::amount(r.budgeted).into(),
                    actual_text: format::amount(r.actual).into(),
                    difference_text: format::amount(r.difference()).into(),
                })
                .collect();
            (items, ids)
        } else {
            let rows = budget::entries_for_month(conn, year, month).unwrap_or_default();
            let ids = rows.iter().map(|r| r.budget_line_id).collect();
            let items = rows
                .iter()
                .map(|r| BudgetRowItem {
                    name: r.budget_line_name.clone().into(),
                    budgeted_text: format::amount(r.budgeted).into(),
                    actual_text: "".into(),
                    difference_text: "".into(),
                })
                .collect();
            (items, ids)
        }
    };

    ui.set_budget_rows(ModelRc::new(VecModel::from(items)));
    state.borrow_mut().budget_row_ids = ids;
}

fn category_index_for(state: &Rc<RefCell<AppState>>, category_id: i64) -> i32 {
    if category_id == 0 {
        return 0;
    }
    state
        .borrow()
        .categories_cache
        .iter()
        .position(|c| c.id == category_id)
        .map(|i| i as i32 + 1)
        .unwrap_or(0)
}

fn category_id_for(state: &Rc<RefCell<AppState>>, index: i32) -> i64 {
    if index <= 0 {
        return 0;
    }
    state
        .borrow()
        .categories_cache
        .get((index - 1) as usize)
        .map(|c| c.id)
        .unwrap_or(0)
}

fn payee_index_for(state: &Rc<RefCell<AppState>>, payee_id: i64) -> i32 {
    if payee_id == 0 {
        return 0;
    }
    state
        .borrow()
        .payees_cache
        .iter()
        .position(|p| p.id == payee_id)
        .map(|i| i as i32 + 1)
        .unwrap_or(0)
}

fn payee_id_for(state: &Rc<RefCell<AppState>>, index: i32) -> i64 {
    if index <= 0 {
        return 0;
    }
    state
        .borrow()
        .payees_cache
        .get((index - 1) as usize)
        .map(|p| p.id)
        .unwrap_or(0)
}

fn cheque_option_id_for(state: &Rc<RefCell<AppState>>, index: i32) -> i64 {
    if index <= 0 {
        return 0;
    }
    state
        .borrow()
        .cheque_option_ids
        .get((index - 1) as usize)
        .copied()
        .unwrap_or(0)
}

fn cheque_option_index_for(state: &Rc<RefCell<AppState>>, cheque_id: i64) -> i32 {
    if cheque_id == 0 {
        return 0;
    }
    state
        .borrow()
        .cheque_option_ids
        .iter()
        .position(|&id| id == cheque_id)
        .map(|i| i as i32 + 1)
        .unwrap_or(0)
}

fn report_error(ui: &MainWindow, error: impl std::fmt::Display) {
    ui.set_error_message(error.to_string().into());
}

fn report_success(ui: &MainWindow, message: &str) {
    ui.set_error_message("".into());
    ui.set_status_message(message.into());
}

// --- Callback wiring ---

fn wire_account_callbacks(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_account_selected(move |index| {
            let ui = ui_weak.upgrade().unwrap();
            let id = state
                .borrow()
                .accounts_cache
                .get(index as usize)
                .map(|a| a.id);
            state.borrow_mut().selected_account_id = id;
            refresh_accounts(&ui, &state);
            refresh_transactions(&ui, &state);
            refresh_chequebooks(&ui, &state);
            refresh_cheques(&ui, &state);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_add_account_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            state.borrow_mut().editing_account_id = None;
            ui.set_account_form_is_new(true);
            ui.set_account_form_name("".into());
            ui.set_account_form_currency("EUR".into());
            ui.set_account_form_iban("".into());
            ui.set_account_form_opening_balance("0".into());
            ui.set_account_form_visible(true);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_edit_account_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            let Some(id) = state.borrow().selected_account_id else {
                return;
            };
            let Some(account) = state
                .borrow()
                .accounts_cache
                .iter()
                .find(|a| a.id == id)
                .cloned()
            else {
                return;
            };

            state.borrow_mut().editing_account_id = Some(id);
            ui.set_account_form_is_new(false);
            ui.set_account_form_name(account.name.into());
            ui.set_account_form_currency(account.currency.into());
            ui.set_account_form_iban(account.iban.unwrap_or_default().into());
            ui.set_account_form_opening_balance(
                format::amount(account.opening_balance_cleared).into(),
            );
            ui.set_account_form_visible(true);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_close_account_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            let Some(id) = state.borrow().selected_account_id else {
                return;
            };
            let today = accountancy_core::today();
            let result = {
                let st = state.borrow();
                accounts::close(st.db.connection(), id, &today)
            };
            match result {
                Ok(()) => {
                    report_success(&ui, "Account closed.");
                    refresh_accounts(&ui, &state);
                    refresh_transactions(&ui, &state);
                }
                Err(e) => report_error(&ui, e),
            }
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_delete_account_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            let Some(id) = state.borrow().selected_account_id else {
                return;
            };
            let result = {
                let st = state.borrow();
                accounts::delete(st.db.connection(), id)
            };
            match result {
                Ok(()) => {
                    state.borrow_mut().selected_account_id = None;
                    report_success(&ui, "Account deleted.");
                    refresh_accounts(&ui, &state);
                    refresh_transactions(&ui, &state);
                    refresh_chequebooks(&ui, &state);
                    refresh_cheques(&ui, &state);
                }
                Err(e) => report_error(&ui, e),
            }
        });
    }

    {
        let ui_weak = ui.as_weak();
        ui.on_cancel_account_form(move || {
            ui_weak.upgrade().unwrap().set_account_form_visible(false);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_save_account_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();

            let opening_balance = match Amount::parse(&ui.get_account_form_opening_balance()) {
                Ok(a) => a,
                Err(e) => {
                    report_error(&ui, e);
                    return;
                }
            };
            let iban = ui.get_account_form_iban().to_string();

            let new_account = NewAccount {
                name: ui.get_account_form_name().to_string(),
                currency: ui.get_account_form_currency().to_string(),
                iban: if iban.trim().is_empty() {
                    None
                } else {
                    Some(iban)
                },
                opening_balance_cleared: opening_balance,
                ..Default::default()
            };

            let editing_id = state.borrow().editing_account_id;
            let result = {
                let st = state.borrow();
                match editing_id {
                    Some(id) => accounts::update(st.db.connection(), id, &new_account),
                    None => accounts::create(st.db.connection(), &new_account).map(|_| ()),
                }
            };

            match result {
                Ok(()) => {
                    report_success(&ui, "Account saved.");
                    ui.set_account_form_visible(false);
                    refresh_accounts(&ui, &state);
                }
                Err(e) => report_error(&ui, e),
            }
        });
    }
}

fn wire_transaction_callbacks(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    {
        let ui_weak = ui.as_weak();
        ui.on_transaction_selected(move |index| {
            ui_weak
                .upgrade()
                .unwrap()
                .set_selected_transaction_index(index);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_add_transaction_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            if state.borrow().selected_account_id.is_none() {
                return;
            }
            state.borrow_mut().editing_transaction_id = None;
            ui.set_transaction_form_is_new(true);
            ui.set_transaction_form_label("".into());
            ui.set_transaction_form_date(accountancy_core::today().into());
            ui.set_transaction_form_amount("0".into());
            ui.set_transaction_form_category_index(0);
            ui.set_transaction_form_status_index(1);
            ui.set_transaction_form_cheque_index(0);
            ui.set_transaction_form_visible(true);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_edit_transaction_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            let index = ui.get_selected_transaction_index();
            if index < 0 {
                return;
            }
            let Some(tx) = state.borrow().transactions_cache.get(index as usize).cloned() else { return };
            if tx.parent_id != 0 {
                report_error(&ui, "Split lines are edited from the split dialog of their parent transaction (not implemented yet).");
                return;
            }

            state.borrow_mut().editing_transaction_id = Some(tx.id);
            ui.set_transaction_form_is_new(false);
            ui.set_transaction_form_label(tx.label.into());
            ui.set_transaction_form_date(tx.date.unwrap_or_default().into());
            ui.set_transaction_form_amount(format::amount(tx.amount).into());
            ui.set_transaction_form_category_index(category_index_for(&state, tx.category_id));
            ui.set_transaction_form_status_index(status_to_index(tx.status));
            ui.set_transaction_form_cheque_index(cheque_option_index_for(&state, tx.check_id));
            ui.set_transaction_form_visible(true);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_delete_transaction_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            let index = ui.get_selected_transaction_index();
            if index < 0 {
                return;
            }
            let Some(id) = state
                .borrow()
                .transactions_cache
                .get(index as usize)
                .map(|t| t.id)
            else {
                return;
            };

            let result = {
                let st = state.borrow();
                transactions::delete(st.db.connection(), id)
            };
            match result {
                Ok(()) => {
                    report_success(&ui, "Transaction deleted.");
                    refresh_transactions(&ui, &state);
                    refresh_accounts(&ui, &state);
                }
                Err(e) => report_error(&ui, e),
            }
        });
    }

    {
        let ui_weak = ui.as_weak();
        ui.on_cancel_transaction_form(move || {
            ui_weak
                .upgrade()
                .unwrap()
                .set_transaction_form_visible(false);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_save_transaction_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            let Some(account_id) = state.borrow().selected_account_id else {
                return;
            };

            let amount = match Amount::parse(&ui.get_transaction_form_amount()) {
                Ok(a) => a,
                Err(e) => {
                    report_error(&ui, e);
                    return;
                }
            };
            let date_text = ui.get_transaction_form_date().to_string();
            let category_id = category_id_for(&state, ui.get_transaction_form_category_index());

            let new_transaction = NewTransaction {
                account_id,
                label: ui.get_transaction_form_label().to_string(),
                date: if date_text.trim().is_empty() {
                    None
                } else {
                    Some(date_text)
                },
                amount,
                status: Some(index_to_status(ui.get_transaction_form_status_index())),
                category_id,
                ..Default::default()
            };

            let editing_id = state.borrow().editing_transaction_id;
            let tx_result = {
                let st = state.borrow();
                match editing_id {
                    Some(id) => {
                        transactions::update(st.db.connection(), id, &new_transaction).map(|()| id)
                    }
                    None => transactions::create(st.db.connection(), &new_transaction),
                }
            };

            let tx_id = match tx_result {
                Ok(id) => id,
                Err(e) => {
                    report_error(&ui, e);
                    return;
                }
            };

            let cheque_id = cheque_option_id_for(&state, ui.get_transaction_form_cheque_index());
            let association_result = {
                let st = state.borrow();
                if cheque_id == 0 {
                    cheques::dissociate(st.db.connection(), tx_id)
                } else {
                    cheques::associate(st.db.connection(), tx_id, cheque_id)
                }
            };

            match association_result {
                Ok(()) => {
                    report_success(&ui, "Transaction saved.");
                    ui.set_transaction_form_visible(false);
                    refresh_transactions(&ui, &state);
                    refresh_accounts(&ui, &state);
                    refresh_cheques(&ui, &state);
                }
                Err(e) => report_error(&ui, e),
            }
        });
    }
}

fn wire_recurring_callbacks(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    {
        let ui_weak = ui.as_weak();
        ui.on_recurring_selected(move |index| {
            ui_weak
                .upgrade()
                .unwrap()
                .set_selected_recurring_index(index);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_add_recurring_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            if state.borrow().accounts_cache.is_empty() {
                report_error(&ui, "Create an account first.");
                return;
            }
            state.borrow_mut().editing_recurring_id = None;
            ui.set_recurring_form_is_new(true);
            ui.set_recurring_form_account_index(0);
            ui.set_recurring_form_label("".into());
            ui.set_recurring_form_amount("0".into());
            ui.set_recurring_form_category_index(0);
            ui.set_recurring_form_period_index(period_to_index(Period::Month));
            ui.set_recurring_form_frequency("1".into());
            ui.set_recurring_form_end_mode_index(end_mode_to_index(EndMode::NoLimit));
            ui.set_recurring_form_end_date("".into());
            ui.set_recurring_form_occurrence_count("".into());
            ui.set_recurring_form_next_due_date(accountancy_core::today().into());
            ui.set_recurring_form_visible(true);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_edit_recurring_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            let index = ui.get_selected_recurring_index();
            if index < 0 {
                return;
            }
            let Some(rule) = state.borrow().recurring_cache.get(index as usize).cloned() else {
                return;
            };
            let account_index = state
                .borrow()
                .accounts_cache
                .iter()
                .position(|a| a.id == rule.account_id)
                .unwrap_or(0) as i32;

            state.borrow_mut().editing_recurring_id = Some(rule.id);
            ui.set_recurring_form_is_new(false);
            ui.set_recurring_form_account_index(account_index);
            ui.set_recurring_form_label(rule.label.into());
            ui.set_recurring_form_amount(format::amount(rule.amount).into());
            ui.set_recurring_form_category_index(category_index_for(&state, rule.category_id));
            ui.set_recurring_form_period_index(period_to_index(rule.period));
            ui.set_recurring_form_frequency(rule.period_frequency.to_string().into());
            ui.set_recurring_form_end_mode_index(end_mode_to_index(rule.end_mode));
            ui.set_recurring_form_end_date(rule.end_date.unwrap_or_default().into());
            ui.set_recurring_form_occurrence_count(
                rule.occurrence_count
                    .map(|c| c.to_string())
                    .unwrap_or_default()
                    .into(),
            );
            ui.set_recurring_form_next_due_date(rule.next_due_date.into());
            ui.set_recurring_form_visible(true);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_delete_recurring_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            let index = ui.get_selected_recurring_index();
            if index < 0 {
                return;
            }
            let Some(id) = state
                .borrow()
                .recurring_cache
                .get(index as usize)
                .map(|r| r.id)
            else {
                return;
            };

            let result = {
                let st = state.borrow();
                recurring::delete(st.db.connection(), id)
            };
            match result {
                Ok(()) => {
                    report_success(&ui, "Recurring rule deleted.");
                    refresh_recurring(&ui, &state);
                }
                Err(e) => report_error(&ui, e),
            }
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_generate_recurring_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            let index = ui.get_selected_recurring_index();
            if index < 0 {
                return;
            }
            let Some(id) = state
                .borrow()
                .recurring_cache
                .get(index as usize)
                .map(|r| r.id)
            else {
                return;
            };

            let today = accountancy_core::today();
            let result = {
                let st = state.borrow();
                recurring::insert_due(st.db.connection(), id, &today)
            };
            match result {
                Ok(inserted) => {
                    report_success(
                        &ui,
                        &format!("{} transaction(s) generated.", inserted.len()),
                    );
                    refresh_recurring(&ui, &state);
                    refresh_accounts(&ui, &state);
                    refresh_transactions(&ui, &state);
                }
                Err(e) => report_error(&ui, e),
            }
        });
    }

    {
        let ui_weak = ui.as_weak();
        ui.on_cancel_recurring_form(move || {
            ui_weak.upgrade().unwrap().set_recurring_form_visible(false);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_save_recurring_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();

            let amount = match Amount::parse(&ui.get_recurring_form_amount()) {
                Ok(a) => a,
                Err(e) => {
                    report_error(&ui, e);
                    return;
                }
            };
            let frequency: i64 = match ui.get_recurring_form_frequency().parse() {
                Ok(n) if n >= 1 => n,
                _ => {
                    report_error(&ui, "Frequency must be a whole number of at least 1.");
                    return;
                }
            };
            let account_index = ui.get_recurring_form_account_index();
            let Some(account_id) = state
                .borrow()
                .accounts_cache
                .get(account_index as usize)
                .map(|a| a.id)
            else {
                report_error(&ui, "Select an account.");
                return;
            };
            let category_id = category_id_for(&state, ui.get_recurring_form_category_index());
            let end_mode = index_to_end_mode(ui.get_recurring_form_end_mode_index());
            let end_date = ui.get_recurring_form_end_date().to_string();
            let occurrence_count = ui.get_recurring_form_occurrence_count().to_string();

            let new_rule = NewRecurringRule {
                account_id,
                label: ui.get_recurring_form_label().to_string(),
                amount,
                category_id,
                period: index_to_period(ui.get_recurring_form_period_index()),
                period_frequency: frequency,
                end_mode,
                end_date: if end_mode == EndMode::UntilDate {
                    Some(end_date)
                } else {
                    None
                },
                occurrence_count: if end_mode == EndMode::OccurrenceCount {
                    occurrence_count.parse().ok()
                } else {
                    None
                },
                start_date: ui.get_recurring_form_next_due_date().to_string(),
                ..Default::default()
            };

            let editing_id = state.borrow().editing_recurring_id;
            let result = {
                let st = state.borrow();
                match editing_id {
                    Some(id) => recurring::update(st.db.connection(), id, &new_rule),
                    None => recurring::create(st.db.connection(), &new_rule).map(|_| ()),
                }
            };

            match result {
                Ok(()) => {
                    report_success(&ui, "Recurring rule saved.");
                    ui.set_recurring_form_visible(false);
                    refresh_recurring(&ui, &state);
                }
                Err(e) => report_error(&ui, e),
            }
        });
    }
}

fn wire_budget_callbacks(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_budget_period_changed(move || {
            refresh_budget(&ui_weak.upgrade().unwrap(), &state);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_budget_mode_selected(move |_mode| {
            refresh_budget(&ui_weak.upgrade().unwrap(), &state);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_budget_amount_accepted(move |index, text| {
            let ui = ui_weak.upgrade().unwrap();
            let Some(budget_line_id) = state.borrow().budget_row_ids.get(index as usize).copied()
            else {
                return;
            };
            let amount = match Amount::parse(&text) {
                Ok(a) => a,
                Err(e) => {
                    report_error(&ui, e);
                    return;
                }
            };
            let (year, month) = budget_period(&ui);
            let result = {
                let st = state.borrow();
                budget::set_amount(st.db.connection(), year, month, budget_line_id, amount)
            };
            match result {
                Ok(()) => {
                    report_success(&ui, "Budget updated.");
                    refresh_budget(&ui, &state);
                }
                Err(e) => report_error(&ui, e),
            }
        });
    }
}

fn wire_cheque_callbacks(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_chequebook_selected(move |index| {
            let ui = ui_weak.upgrade().unwrap();
            let id = state
                .borrow()
                .chequebooks_cache
                .get(index as usize)
                .map(|b| b.id);
            state.borrow_mut().selected_chequebook_id = id;
            ui.set_selected_chequebook_index(index);
            refresh_cheques(&ui, &state);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_add_chequebook_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            if state.borrow().selected_account_id.is_none() {
                report_error(&ui, "Select an account first.");
                return;
            }
            ui.set_chequebook_form_first_number("".into());
            ui.set_chequebook_form_last_number("".into());
            ui.set_chequebook_form_reception_date(accountancy_core::today().into());
            ui.set_chequebook_form_visible(true);
        });
    }

    {
        let ui_weak = ui.as_weak();
        ui.on_cancel_chequebook_form(move || {
            ui_weak
                .upgrade()
                .unwrap()
                .set_chequebook_form_visible(false);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_save_chequebook_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            let Some(account_id) = state.borrow().selected_account_id else {
                return;
            };

            let (Ok(first_number), Ok(last_number)) = (
                ui.get_chequebook_form_first_number().parse::<i64>(),
                ui.get_chequebook_form_last_number().parse::<i64>(),
            ) else {
                report_error(&ui, "First and last number must be whole numbers.");
                return;
            };
            let reception_date = ui.get_chequebook_form_reception_date().to_string();

            let result = {
                let st = state.borrow();
                cheques::create_chequebook(
                    st.db.connection(),
                    &NewChequebook {
                        account_id,
                        first_number,
                        last_number,
                        reception_date: if reception_date.trim().is_empty() {
                            None
                        } else {
                            Some(reception_date)
                        },
                    },
                )
            };

            match result {
                Ok(_) => {
                    report_success(&ui, "Chequebook created.");
                    ui.set_chequebook_form_visible(false);
                    refresh_chequebooks(&ui, &state);
                    refresh_transaction_cheque_options(&ui, &state);
                }
                Err(e) => report_error(&ui, e),
            }
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_delete_chequebook_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            let index = ui.get_selected_chequebook_index();
            if index < 0 {
                return;
            }
            let Some(id) = state
                .borrow()
                .chequebooks_cache
                .get(index as usize)
                .map(|b| b.id)
            else {
                return;
            };

            let result = {
                let st = state.borrow();
                cheques::delete_chequebook(st.db.connection(), id)
            };
            match result {
                Ok(()) => {
                    report_success(&ui, "Chequebook deleted.");
                    refresh_chequebooks(&ui, &state);
                    refresh_cheques(&ui, &state);
                    refresh_transaction_cheque_options(&ui, &state);
                }
                Err(e) => report_error(&ui, e),
            }
        });
    }

    {
        let ui_weak = ui.as_weak();
        ui.on_cheque_selected(move |index| {
            ui_weak.upgrade().unwrap().set_selected_cheque_index(index);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_write_cheque_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            let index = ui.get_selected_cheque_index();
            if index < 0 {
                return;
            }
            let Some(cheque) = state.borrow().cheques_cache.get(index as usize).cloned() else {
                return;
            };

            state.borrow_mut().editing_cheque_id = Some(cheque.id);
            ui.set_cheque_form_number_text(cheque.number.to_string().into());
            ui.set_cheque_form_payee_index(payee_index_for(&state, cheque.payee_id));
            ui.set_cheque_form_amount(format::amount(cheque.amount).into());
            ui.set_cheque_form_date(cheque.date.unwrap_or_default().into());
            ui.set_cheque_form_status_index(cheque_status_to_index(cheque.status));
            ui.set_cheque_form_visible(true);
        });
    }

    {
        let ui_weak = ui.as_weak();
        ui.on_cancel_cheque_form(move || {
            ui_weak.upgrade().unwrap().set_cheque_form_visible(false);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_save_cheque_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            let Some(cheque_id) = state.borrow().editing_cheque_id else {
                return;
            };

            let amount = match Amount::parse(&ui.get_cheque_form_amount()) {
                Ok(a) => a,
                Err(e) => {
                    report_error(&ui, e);
                    return;
                }
            };
            let payee_id = payee_id_for(&state, ui.get_cheque_form_payee_index());
            let date_text = ui.get_cheque_form_date().to_string();
            let status = index_to_cheque_status(ui.get_cheque_form_status_index());

            let result = {
                let st = state.borrow();
                cheques::write_cheque(
                    st.db.connection(),
                    cheque_id,
                    payee_id,
                    amount,
                    if date_text.trim().is_empty() {
                        None
                    } else {
                        Some(date_text)
                    },
                    status,
                )
            };

            match result {
                Ok(()) => {
                    report_success(&ui, "Cheque saved.");
                    ui.set_cheque_form_visible(false);
                    refresh_cheques(&ui, &state);
                    refresh_transaction_cheque_options(&ui, &state);
                }
                Err(e) => report_error(&ui, e),
            }
        });
    }
}

fn wire_dashboard_callbacks(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    let ui_weak = ui.as_weak();
    let state = state.clone();
    ui.on_refresh_dashboard_clicked(move || {
        refresh_dashboard(&ui_weak.upgrade().unwrap(), &state);
    });
}

fn wire_statistics_callbacks(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    let ui_weak = ui.as_weak();
    let state = state.clone();
    ui.on_statistics_refresh_clicked(move || {
        refresh_statistics(&ui_weak.upgrade().unwrap(), &state);
    });
}

fn wire_import_callbacks(ui: &MainWindow, state: &Rc<RefCell<AppState>>) {
    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_import_ofx_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            let Some(account_id) = state.borrow().selected_account_id else {
                return;
            };
            let Some(path) = rfd::FileDialog::new()
                .add_filter("OFX", &["ofx", "qfx"])
                .pick_file()
            else {
                return;
            };

            let content = match std::fs::read_to_string(&path) {
                Ok(c) => c,
                Err(e) => {
                    report_error(&ui, e);
                    return;
                }
            };
            let parsed = ofx::parse(&content);
            if parsed.is_empty() {
                report_error(&ui, "No transactions found in that file.");
                return;
            }

            let result = {
                let st = state.borrow();
                import::import(st.db.connection(), account_id, &parsed)
            };
            match result {
                Ok(summary) => {
                    report_success(&ui, &import_summary_message(summary));
                    refresh_transactions(&ui, &state);
                    refresh_accounts(&ui, &state);
                }
                Err(e) => report_error(&ui, e),
            }
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_import_csv_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            if state.borrow().selected_account_id.is_none() {
                return;
            }
            let Some(path) = rfd::FileDialog::new()
                .add_filter("CSV", &["csv", "txt"])
                .pick_file()
            else {
                return;
            };

            let content = match std::fs::read_to_string(&path) {
                Ok(c) => c,
                Err(e) => {
                    report_error(&ui, e);
                    return;
                }
            };

            let preview: String = content.lines().take(3).collect::<Vec<_>>().join("\n");
            ui.set_csv_preview_text(preview.into());
            state.borrow_mut().pending_csv_content = Some(content);
            ui.set_csv_import_form_visible(true);
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_cancel_csv_import_form(move || {
            ui_weak
                .upgrade()
                .unwrap()
                .set_csv_import_form_visible(false);
            state.borrow_mut().pending_csv_content = None;
        });
    }

    {
        let ui_weak = ui.as_weak();
        let state = state.clone();
        ui.on_confirm_csv_import_clicked(move || {
            let ui = ui_weak.upgrade().unwrap();
            let Some(account_id) = state.borrow().selected_account_id else {
                return;
            };
            let Some(content) = state.borrow().pending_csv_content.clone() else {
                return;
            };

            let Some(date_column) = parse_column_index(&ui.get_csv_date_column()) else {
                report_error(&ui, "Date column must be a number.");
                return;
            };
            let mapping = csv::ColumnMapping {
                delimiter: ui.get_csv_delimiter().chars().next().unwrap_or(','),
                has_header: ui.get_csv_has_header(),
                date_column,
                date_order: csv_date_order_from_index(ui.get_csv_date_order_index()),
                label_column: parse_column_index(&ui.get_csv_label_column()),
                amount_column: parse_column_index(&ui.get_csv_amount_column()),
                debit_column: parse_column_index(&ui.get_csv_debit_column()),
                credit_column: parse_column_index(&ui.get_csv_credit_column()),
                reference_column: parse_column_index(&ui.get_csv_reference_column()),
            };

            let parsed = csv::parse(&content, &mapping);
            if parsed.is_empty() {
                report_error(&ui, "No transactions could be read with these settings.");
                return;
            }

            let result = {
                let st = state.borrow();
                import::import(st.db.connection(), account_id, &parsed)
            };
            match result {
                Ok(summary) => {
                    report_success(&ui, &import_summary_message(summary));
                    ui.set_csv_import_form_visible(false);
                    state.borrow_mut().pending_csv_content = None;
                    refresh_transactions(&ui, &state);
                    refresh_accounts(&ui, &state);
                }
                Err(e) => report_error(&ui, e),
            }
        });
    }
}
