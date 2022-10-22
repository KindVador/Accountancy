#include <catch2/catch.hpp>

#include "../../src/core/financialinstitution.hpp"
#include "importconfig.hpp"

TEST_CASE("DataImport CaisseEpargne old format", "[core]")
{
    FinancialInstitution fi("CaisseEpargne");

    auto dataFile = QFile("data/caisse_epargne/old_format.csv");
    ImportConfig config;
    config.setName("CaisseEpargne old format");
    config.setSeparatorChar(';');
    config.setNbLinesToSkipStart(5);
    config.setNbLinesToSkipEnd(1);
    config.setDecimalChar('.');
    config.setDateFormat("dd/MM/yy");
    config.setHasTime(true);
    config.addColumn("Date", 0);
    config.addColumn("Name", 2);
    config.addColumn("DebitAmount", 3);
    config.addColumn("CreditAmount", 4);
    config.addColumn("Comment", 5);
    config.addColumn("Time", 1);

    REQUIRE(dataFile.exists());

    QList<Transaction*> transactions = fi.readTransactionsFromFile(dataFile, config);
    // check that all transactions have been read
    CHECK(transactions.count() == 51);

    // check attributes of first transaction
    const Transaction* firstTransaction = transactions.first();
    CHECK(firstTransaction->getName() == "CB E.LECLERC WEB   FACT 080520");
    CHECK(firstTransaction->getAmount() == -37.13);
    CHECK(firstTransaction->getComment() == "CB E.LECLERC WEB   FACT 080520 ");
    qDebug() << firstTransaction->getDateTime();
    CHECK(firstTransaction->getDateTime() == QDateTime(QDate(2020, 05, 11), QTime(8, 50, 06, 800)));

    // check attributes of last transaction
    const Transaction* lastTransaction = transactions.last();
    CHECK(lastTransaction->getName() == "*REMISE OPT INTERNATIONALE F1");
    CHECK(lastTransaction->getAmount() == 0.4);
    CHECK(lastTransaction->getComment() == "*REMISE OPT INTERNATIONALE F1 ");
    qDebug() << lastTransaction->getDateTime();
    CHECK(lastTransaction->getDateTime() == QDateTime(QDate(2020, 03, 13), QTime(18, 52, 31, 234)));
}

TEST_CASE("DataImport CaisseEpargne new format", "[core]")
{
    FinancialInstitution fi("CaisseEpargne");

    auto dataFile = QFile("data/caisse_epargne/new_format.csv");
    ImportConfig config;
    config.setName("CaisseEpargne new format");
    config.setSeparatorChar(';');
    config.setNbLinesToSkipStart(1);
    config.setDecimalChar(',');
    config.setDateFormat("yyyy-MM-dd");
    config.setHasTime(false);
    config.addColumn("Date", 0);
    config.addColumn("Name", 4);
    config.addColumn("DebitAmount", 5);
    config.addColumn("CreditAmount", 5);
    config.addColumn("Comment", 3);

    REQUIRE(dataFile.exists());

    QList<Transaction*> transactions = fi.readTransactionsFromFile(dataFile, config);
    // check that all transactions have been read
    CHECK(transactions.count() == 19);

    // check attributes of first transaction
    const Transaction* firstTransaction = transactions.first();
    CHECK(firstTransaction->getName() == "ELECLERC");   // Libelle simplifie
    CHECK(firstTransaction->getAmount() == -40.32);     // Montant operation
    CHECK(firstTransaction->getComment() == "NOBLADIS");// Libelle operation
    CHECK(firstTransaction->getDateTime() == QDateTime(QDate(2022, 05, 28), QTime(0, 0, 0, 0)));

    // check attributes of last transaction
    const Transaction* lastTransaction = transactions.last();
    CHECK(lastTransaction->getName() == "COTISATIONS BANCAIRES");
    CHECK(lastTransaction->getAmount() == -15.90);
    CHECK(lastTransaction->getComment() == "COTISATIONS BANCAIRES");
    CHECK(lastTransaction->getDateTime() == QDateTime(QDate(2022, 05, 13), QTime(0, 0, 0, 0)));
}

TEST_CASE("DataImport CaisseEpargne new format with French Date format", "[core]")
{
    FinancialInstitution fi("CaisseEpargne");

    auto dataFile = QFile("data/caisse_epargne/new_format_date2.csv");
    ImportConfig config;
    config.setName("CaisseEpargne new format french date format");
    config.setSeparatorChar(';');
    config.setNbLinesToSkipStart(1);
    config.setDecimalChar(',');
    config.setDateFormat("dd/MM/yyyy");
    config.setHasTime(false);
    config.addColumn("Date", 0);
    config.addColumn("Name", 4);
    config.addColumn("DebitAmount", 7);
    config.addColumn("CreditAmount", 7);
    config.addColumn("Comment", 3);

    REQUIRE(dataFile.exists());

    QList<Transaction*> transactions = fi.readTransactionsFromFile(dataFile, config);
    // check that all transactions have been read
    CHECK(transactions.count() == 2);

    // check attributes of first transaction
    const Transaction* firstTransaction = transactions.first();
    CHECK(firstTransaction->getName() == "VIR SEPA M CONTIVAL FLORIAN");// Libelle simplifie
    CHECK(firstTransaction->getAmount() == 900);                        // Montant operation
    CHECK(firstTransaction->getComment() == "Virement reçu");           // Libelle operation
    CHECK(firstTransaction->getDateTime() == QDateTime(QDate(2022, 07, 04), QTime(0, 0, 0, 0)));

    // check attributes of last transaction
    const Transaction* lastTransaction = transactions.last();
    CHECK(lastTransaction->getName() == "VIR SEPA M CONTIVAL FLORIAN");// Libelle simplifie
    CHECK(lastTransaction->getAmount() == 400);                        // Montant operation
    CHECK(lastTransaction->getComment() == "Virement reçu");           // Libelle operation
    CHECK(lastTransaction->getDateTime() == QDateTime(QDate(2022, 06, 06), QTime(0, 0, 0, 0)));
}

TEST_CASE("DataImport HelloBank", "[core]")
{
    FinancialInstitution fi("Hello Bank!");

    auto dataFile = QFile("data/hello_bank/HB_03_10_2022.csv");
    ImportConfig config;
    config.setName("HelloBank CSV");
    config.setSeparatorChar(';');
    config.setNbLinesToSkipStart(1);
    config.setDecimalChar(',');
    config.setDateFormat("dd/MM/yyyy");
    config.setHasTime(false);
    config.addColumn("Date", 0);
    config.addColumn("Name", 3);
    config.addColumn("DebitAmount", 4);
    config.addColumn("CreditAmount", 4);
    config.addColumn("Comment", 1);

    REQUIRE(dataFile.exists());

    QList<Transaction*> transactions = fi.readTransactionsFromFile(dataFile, config);
    // check that all transactions have been read
    CHECK(transactions.count() == 15);

    // check attributes of first transaction
    const Transaction* firstTransaction = transactions.first();
    CHECK(firstTransaction->getName() == "VIR SEPA INST RECU");
    CHECK(firstTransaction->getAmount() == 150.0);
    CHECK(firstTransaction->getComment() == "VIREMENT INSTANTANE RECU");
    CHECK(firstTransaction->getDateTime() == QDateTime(QDate(2022, 8, 24), QTime(0, 0, 0, 0)));

    // check attributes of last transaction
    const Transaction* lastTransaction = transactions.last();
    CHECK(lastTransaction->getName() == "FACTURE CARTE DU 300922 CARREFOUR CARTE");
    CHECK(lastTransaction->getAmount() == -50.09);
    CHECK(lastTransaction->getComment() == "PAIEMENT CB");
    CHECK(lastTransaction->getDateTime() == QDateTime(QDate(2022, 10, 3), QTime(0, 0, 0, 0)));
}