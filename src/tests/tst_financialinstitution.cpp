#include <QTest>

#include "src/core/financialinstitution.hpp"

class FinancialInstitutionTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {
        qDebug("Called before everything else.");
    }

    void defaultConstructorTestCase() {
        FinancialInstitution fi{};
        QVERIFY(fi.getId());
        QCOMPARE(fi.getName(), QString());
    }

    void initConstructorTestCase() {
        auto fi = FinancialInstitution("Institution1");
        QVERIFY(fi.getId());
        QCOMPARE(fi.getName(), "Institution1");
    }

    void writeJsonTestCase() {
        auto fi = FinancialInstitution("Institution1");
        QJsonObject jsonData;
        fi.write(jsonData);
        QVERIFY(jsonData.contains("id") && !jsonData["id"].isNull());
        QVERIFY(jsonData.contains("name") && jsonData["name"] == "Institution1");
    }

    void readJsonTestCase() {
        auto fi0 = FinancialInstitution("Institution1");
        QJsonObject jsonData;
        fi0.write(jsonData);
        FinancialInstitution fi1;
        fi1.read(jsonData);
        QCOMPARE(fi1.getName(), "Institution1");
        QVERIFY(fi1.getId());
    }

};

QTEST_APPLESS_MAIN(FinancialInstitutionTest)
#include "tst_financialinstitution.moc"
