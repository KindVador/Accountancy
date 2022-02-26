#include <QTest>

#include "src/core/financialinstitution.hpp"
#include "src/core/transaction.hpp"

class DataImportTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        qDebug("Called before everything else.");
    }

    void importCaisseEpargne() {
        FinancialInstitution fi("CaisseEpargne");

        auto dataFile = QFile("../../data/caisse_epargne/telechargement.csv");
        if (dataFile.exists()) {
            QList<Transaction *> transactions = fi.readTransactionsFromFile(dataFile);
            // check that all transactions have been read
            QVERIFY(transactions.count() == 51);

            // check attributes of first transaction
            const Transaction *firstTransaction = transactions.first();
            QCOMPARE(firstTransaction->getName(), "CB E.LECLERC WEB   FACT 080520");
            QCOMPARE(firstTransaction->getAmount(), -37.13);
            QCOMPARE(firstTransaction->getComment(), "CB E.LECLERC WEB   FACT 080520 ");
            QCOMPARE(firstTransaction->getDate(), QDate(2020, 05, 11));

            // check attributes of last transaction
            const Transaction *lastTransaction = transactions.last();
            QCOMPARE(lastTransaction->getName(), "*REMISE OPT INTERNATIONALE F1");
            QCOMPARE(lastTransaction->getAmount(), 0.4);
            QCOMPARE(lastTransaction->getComment(), "*REMISE OPT INTERNATIONALE F1 ");
            QCOMPARE(lastTransaction->getDate(), QDate(2020, 03, 13));
        } else {
            qWarning("File:telechargement.csv doesn't exist");
        }
    }
};

QTEST_APPLESS_MAIN(DataImportTest)
#include "tst_dataimport.moc"