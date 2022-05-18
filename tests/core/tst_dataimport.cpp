#include <catch2/catch.hpp>

#include "../../src/core/financialinstitution.hpp"

TEST_CASE( "DataImport CaisseEpargne", "[core]" )
{
    FinancialInstitution fi("CaisseEpargne");

    auto dataFile = QFile("../../data/caisse_epargne/telechargement.csv");
    if (dataFile.exists()) {
        QList<Transaction *> transactions = fi.readTransactionsFromFile(dataFile);
        // check that all transactions have been read
        CHECK(transactions.count() == 51);

        // check attributes of first transaction
        const Transaction *firstTransaction = transactions.first();
        CHECK(firstTransaction->getName() == "CB E.LECLERC WEB   FACT 080520");
        CHECK(firstTransaction->getAmount() == -37.13);
        CHECK(firstTransaction->getComment() == "CB E.LECLERC WEB   FACT 080520 ");
        CHECK(firstTransaction->getDateTime() == QDateTime(QDate(2020, 05, 11), QTime(8, 50, 06, 800)));

        // check attributes of last transaction
        const Transaction *lastTransaction = transactions.last();
        CHECK(lastTransaction->getName() == "*REMISE OPT INTERNATIONALE F1");
        CHECK(lastTransaction->getAmount() == 0.4);
        CHECK(lastTransaction->getComment() == "*REMISE OPT INTERNATIONALE F1 ");
        CHECK(lastTransaction->getDateTime() == QDateTime(QDate(2020, 03, 13), QTime(18, 52, 31, 234)));
    } else {
        qWarning("File:telechargement.csv doesn't exist");
    }
}