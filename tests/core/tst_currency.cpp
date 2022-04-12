#include <QTest>

#include "../../src/core/currency.hpp"

class CurrencyTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {
        qDebug("Called before everything else.");
    }

    void defaultConstructorTestCase() {
        Currency c{};
        QVERIFY(!c.getUid().isNull());
        QCOMPARE(c.getName(), QString());
        QCOMPARE(c.getSymbol(), QString());
    }

    void initConstructorTestCase() {
        Currency c{"Euro", "€"};
        QVERIFY(!c.getUid().isNull());
        QCOMPARE(c.getName(), QString("Euro"));
        QCOMPARE(c.getSymbol(), QString("€"));
    }

    void writeJsonTestCase() {
        Currency c{"Euro", "€"};
        QJsonObject jsonData;
        c.write(jsonData);
        QVERIFY(jsonData.contains("uid") && !jsonData["uid"].isNull());
        QVERIFY(jsonData.contains("name") && jsonData["name"] == "Euro");
        QVERIFY(jsonData.contains("symbol") && jsonData["symbol"] == "€");
    }

    void readJsonTestCase() {
        Currency c0{"Euro", "€"};
        QJsonObject jsonData;
        c0.write(jsonData);
        Currency c1;
        c1.read(jsonData);
        QVERIFY(!c1.getUid().isNull());
        QCOMPARE(c1.getName(), "Euro");
        QCOMPARE(c1.getSymbol(), "€");
    }
};

QTEST_APPLESS_MAIN(CurrencyTest)
#include "tst_currency.moc"