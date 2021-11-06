#ifndef LOANACCOUNT_H
#define LOANACCOUNT_H


class LoanAccount {
public:
    LoanAccount();
    ~LoanAccount() = default;

private:
    int id = -1;
};

Q_DECLARE_METATYPE(LoanAccount*)

#endif // LOANACCOUNT_H
