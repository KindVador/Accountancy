#include <QTest>

#include "src/core/owner.hpp"

class OwnerTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {
        qDebug("Called before everything else.");
    }

    void defaultConstructorTestCase() {
        Owner o{};
        QCOMPARE(o.getName(), QString());
        QCOMPARE(o.getWarningBalance(), 0.0);
        QCOMPARE(o.getComment(), QString());
        QVERIFY(!o.getIsHidden());
        QVERIFY(o.getId());
    }

    void initConstructorTestCase() {
        Owner o{"Name1", 150.0, "Comment1", false};
        QCOMPARE(o.getName(), "Name1");
        QCOMPARE(o.getWarningBalance(), 150.0);
        QCOMPARE(o.getComment(), "Comment1");
        QVERIFY(!o.getIsHidden());
        QVERIFY(o.getId());
    }

    void writeJsonTestCase() {
        Owner o{"Name1", 150.0, "Comment1", false};
        QJsonObject jsonData;
        o.write(jsonData);
        QVERIFY(jsonData.contains("id") && !jsonData["id"].isNull());
        QVERIFY(jsonData.contains("name") && jsonData["name"] == "Name1");
        QVERIFY(jsonData.contains("comment") && jsonData["comment"] == "Comment1");
        QVERIFY(jsonData.contains("warning_balance"));
        QCOMPARE(jsonData["warning_balance"].toDouble(), 150.0);
        QVERIFY(jsonData.contains("is_hidden") && !jsonData["is_hidden"].toBool());
    }

    void readJsonTestCase() {
        Owner o0{"Name1", 150.0, "Comment1", false};
        QJsonObject jsonData;
        o0.write(jsonData);
        Owner o1;
        o1.read(jsonData);
        QCOMPARE(o1.getName(), "Name1");
        QCOMPARE(o1.getWarningBalance(), 150.0);
        QCOMPARE(o1.getComment(), "Comment1");
        QVERIFY(!o1.getIsHidden());
        QVERIFY(o1.getId());
    }
};

QTEST_APPLESS_MAIN(OwnerTest)
#include "tst_owner.moc"