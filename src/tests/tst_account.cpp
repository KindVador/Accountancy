#include <QTest>

#include "src/core/account.hpp"
#include "src/core/financialinstitution.hpp"

class AccountTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {
        qDebug("Called before everything else.");
    }

    void defaultConstructorTestCase() {
        Account a{};
        QVERIFY(a.getId());
        QCOMPARE(a.getInstitution(), nullptr);
        QCOMPARE(a.getCurrency(), nullptr);
        QCOMPARE(a.getOwners().count(), 0);
        QCOMPARE(a.getInitialBalance(), 0.0);
        QCOMPARE(a.getWarningBalance(), 0.0);
    }

    void writeJsonTestCase() {
        auto *fi = new FinancialInstitution("Institution1");
        auto *currency = new Currency("Euro", "€");
        auto *owner = new Owner("Owner1", 150.0, "Comment1", false);
        Account a{fi, AccountType::Cash, currency, owner, 147.85, 100.0, "123456789AX", "CommentAccount1", true, false};
        QJsonObject jsonData;
        a.write(jsonData);
        QVERIFY(jsonData.contains("id") && !jsonData["id"].isNull());
        QVERIFY(jsonData.contains("institution") && !jsonData["institution"].isNull());
        QVERIFY(jsonData.contains("currency") && !jsonData["currency"].isNull());
        QVERIFY(jsonData.contains("owners") && !jsonData["owners"].isNull());
        QVERIFY(jsonData.contains("initialBalance"));
        QVERIFY(abs(jsonData["initialBalance"].toDouble()- 147.85) < 0.0001);
        QVERIFY(jsonData.contains("warningBalance"));
        QCOMPARE(jsonData["warningBalance"].toDouble(), 100.00);
        QVERIFY(jsonData.contains("accountNumber"));
        QCOMPARE(jsonData["accountNumber"].toString(), "123456789AX");
        QVERIFY(jsonData.contains("comment"));
        QCOMPARE(jsonData["comment"].toString(), "CommentAccount1");
        QVERIFY(jsonData.contains("isIncludedInTotal"));
        QCOMPARE(jsonData["isIncludedInTotal"].toBool(), true);
        QVERIFY(jsonData.contains("isHidden"));
        QCOMPARE(jsonData["isHidden"].toBool(), false);
        QVERIFY(jsonData.contains("type"));
        QCOMPARE(jsonData["type"].toString(), ACCOUNT_TYPE_2_STRING[AccountType::Cash]);

        delete fi;
        delete currency;
        delete owner;
    }

    void readJsonTestCase() {
        // write data to Json object
        auto *fi = new FinancialInstitution("Institution1");
        auto *currency = new Currency("Euro", "€");
        auto *owner = new Owner("Owner1", 150.0, "Comment1", false);
        Account a0{fi, AccountType::Cash, currency, owner, 147.85, 100.0, "123456789AX", "CommentAccount1", true, false};
        a0.setId(99);
        QJsonObject jsonData;
        a0.write(jsonData);

        // read data from Json object
        Account a1;
        a1.read(jsonData);

        // check that values are correctly read
        QCOMPARE(a1.getId(), 99);
        QVERIFY(a1.getInstitution() != nullptr);
        QVERIFY(a1.getCurrency() != nullptr);
        QVERIFY(a1.getOwners().count() > 0);
        QVERIFY(abs(a1.getInitialBalance() - 147.85) < 0.0001);
        QCOMPARE(a1.getWarningBalance(), 100.00);
        QCOMPARE(a1.getAccountNumber(), "123456789AX");
        QCOMPARE(a1.getComment(), "CommentAccount1");
        QCOMPARE(a1.isIncludedInTotal() , true);
        QCOMPARE(a1.isHidden(), false);
        QCOMPARE(a1.getType(), AccountType::Cash);

        delete fi;
        delete currency;
        delete owner;
    }

};

QTEST_APPLESS_MAIN(AccountTest)
#include "tst_account.moc"
