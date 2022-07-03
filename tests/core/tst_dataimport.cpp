#include <catch2/catch.hpp>

#include "../../src/core/financialinstitution.hpp"
#include "importconfig.hpp"

TEST_CASE( "DataImport CaisseEpargne old format", "[core]" )
{
    FinancialInstitution fi("CaisseEpargne");

    auto dataFile = QFile("../../data/caisse_epargne/old_format.csv");
    ImportConfig config;
    config.setName("CaisseEpargne old format");
    config.setSeparatorChar(';');
    config.setNbLinesToSkipStart(6);
    config.setNbLinesToSkipEnd(1);
    config.setDecimalChar('.');
    config.setDateFormat("dd/MM/yy");
    config.setHasTime(true);
    config.addColumn("Date", 0);
    config.addColumn("Name", 2);
    config.addColumn("Amount", 5);
    config.addColumn("Comment", 5);
    config.addColumn("Time", 1);
    if (dataFile.exists()) {
        QList<Transaction *> transactions = fi.readTransactionsFromFile(dataFile, config);
        // check that all transactions have been read
        CHECK(transactions.count() == 51);

        // check attributes of first transaction
        const Transaction *firstTransaction = transactions.first();
        CHECK(firstTransaction->getName() == "CB E.LECLERC WEB   FACT 080520");
        CHECK(firstTransaction->getAmount() == -37.13);
        CHECK(firstTransaction->getComment() == "CB E.LECLERC WEB   FACT 080520 ");
        qDebug() << firstTransaction->getDateTime();
        CHECK(firstTransaction->getDateTime() == QDateTime(QDate(2020, 05, 11), QTime(8, 50, 06, 800)));

        // check attributes of last transaction
        const Transaction *lastTransaction = transactions.last();
        CHECK(lastTransaction->getName() == "*REMISE OPT INTERNATIONALE F1");
        CHECK(lastTransaction->getAmount() == 0.4);
        CHECK(lastTransaction->getComment() == "*REMISE OPT INTERNATIONALE F1 ");
        qDebug() << lastTransaction->getDateTime();
        CHECK(lastTransaction->getDateTime() == QDateTime(QDate(2020, 03, 13), QTime(18, 52, 31, 234)));
    } else {
        qWarning("File:telechargement.csv doesn't exist");
    }
}

TEST_CASE( "DataImport CaisseEpargne new format", "[core]" )
{
    FinancialInstitution fi("CaisseEpargne");

    auto dataFile = QFile("../../data/caisse_epargne/new_format.csv");
    ImportConfig config;
    config.setName("CaisseEpargne new format");
    config.setSeparatorChar(';');
    config.setNbLinesToSkipStart(1);
    config.setDecimalChar(',');
    config.setDateFormat("dd/MM/yyyy");
    config.setHasTime(false);
    config.addColumn("Date", 0);
    config.addColumn("Name", 4);
    config.addColumn("Amount", 5);
    config.addColumn("Comment", 3);

    if (dataFile.exists()) {
        QList<Transaction *> transactions = fi.readTransactionsFromFile(dataFile, config);
        // check that all transactions have been read
        CHECK(transactions.count() == 19);

        // check attributes of first transaction
        const Transaction *firstTransaction = transactions.first();
        CHECK(firstTransaction->getName() == "ELECLERC");       // Libelle simplifie
        CHECK(firstTransaction->getAmount() == -40.32);         // Montant operation
        CHECK(firstTransaction->getComment() == "NOBLADIS");    // Libelle operation
        CHECK(firstTransaction->getDateTime() == QDateTime(QDate(2022, 05, 28), QTime(0, 0, 0, 0)));

        // check attributes of last transaction
        const Transaction *lastTransaction = transactions.last();
        CHECK(lastTransaction->getName() == "COTISATIONS BANCAIRES");
        CHECK(lastTransaction->getAmount() == -15.90);
        CHECK(lastTransaction->getComment() == "COTISATIONS BANCAIRES");
        CHECK(lastTransaction->getDateTime() == QDateTime(QDate(2022, 05, 13), QTime(0, 0, 0, 0)));
    } else {
        qWarning("File:new_format.csv doesn't exist");
    }
}