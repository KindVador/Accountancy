#include <QTest>

#include "../src/core/financialinstitution.hpp"
#include "../src/core/transaction.hpp"

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
            qWarning() << "PRINT ALL TRANSACTIONS:";
            for (Transaction *transaction : qAsConst(transactions)) {
                transaction->printToConsole();
            }
        } else {
            qWarning("File:telechargement.csv doesn't exist");
        }
    }
};

QTEST_APPLESS_MAIN(DataImportTest)
#include "tst_dataimport.moc"