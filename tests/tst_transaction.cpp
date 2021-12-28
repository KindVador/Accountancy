#include <QTest>

#include "../src/core/transaction.hpp"

class TransactionTest : public QObject {
Q_OBJECT

private slots:
    void initTestCase() {
        qDebug("Called before everything else.");
    }

    void defaultConstructorTestCase() {
        Transaction t{};
        QCOMPARE(t.getName(), QString());
        QCOMPARE(t.getComment(), QString());
        QCOMPARE(t.getAmount(), 0.0);
        QCOMPARE(t.getStatus(), TransactionStatus::Imported);
        QCOMPARE(t.getDate(), QDate());
    }

    void writeJsonTestCase() {
        Transaction t{};
        t.setAmount(-14.99);
    }

    void readJsonTestCase() {

}
};

QTEST_APPLESS_MAIN(TransactionTest)
#include "tst_transaction.moc"