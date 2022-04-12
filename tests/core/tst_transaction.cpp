#include <QTest>

#include "../../src/core/transaction.hpp"

class TransactionTest : public QObject
{
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

    void initConstructorTestCase() {
        Transaction t(QString("Name1"), QString("Comment1"), TransactionStatus::Created, QDate(2021, 12, 25), -14.99);
        QVERIFY(!t.getUid().isNull());
        QVERIFY(t.getName() == "Name1");
        QVERIFY(t.getComment() == "Comment1");
        QVERIFY(t.getStatus() == TransactionStatus::Created);
        QCOMPARE(t.getDate(), QDate(2021, 12, 25));
        QCOMPARE(t.getAmount(), -14.99);
    }

    void writeJsonTestCase() {
        Transaction t(QString("Name1"), QString("Comment1"), TransactionStatus::Created, QDate(2021, 12, 25), -14.99);
        QJsonObject jsonData;
        t.write(jsonData);
        QVERIFY(jsonData.contains("uid") && !jsonData["uid"].isNull());
        QVERIFY(jsonData.contains("name") && jsonData["name"] == "Name1");
        QVERIFY(jsonData.contains("comment") && jsonData["comment"] == "Comment1");
        QVERIFY(jsonData.contains("status") && jsonData["status"] == TRANSACTION_STATUS_2_STRING[TransactionStatus::Created]);
        QVERIFY(jsonData.contains("date"));
        QCOMPARE(jsonData["date"], "25/12/2021");
        QVERIFY(jsonData.contains("amount"));
        QCOMPARE(jsonData["amount"].toDouble(), -14.99);
    }

    void readJsonTestCase() {
        Transaction t0(QString("Name1"), QString("Comment1"), TransactionStatus::Created, QDate(2021, 12, 25), -14.99);
        QJsonObject jsonData;
        t0.write(jsonData);
        Transaction t1;
        t1.read(jsonData);
        QVERIFY(!t1.getUid().isNull());
        QCOMPARE(t1.getName(), "Name1");
        QCOMPARE(t1.getComment(), "Comment1");
        QCOMPARE(t1.getStatus(), TransactionStatus::Created);
        QCOMPARE(t1.getDate(), QDate(2021, 12, 25));
        QCOMPARE(t1.getAmount(), -14.99);
    }
};

QTEST_APPLESS_MAIN(TransactionTest)
#include "tst_transaction.moc"