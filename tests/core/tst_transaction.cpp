#include <catch2/catch.hpp>

#include "../../src/core/transaction.hpp"

TEST_CASE( "Transaction defaultConstructor", "[core]" )
{
    Transaction t{};
    CHECK(t.getName() == QString());
    CHECK(t.getComment() == QString());
    CHECK(t.getAmount() == 0.0);
    CHECK(t.getStatus() == TransactionStatus::Imported);
    CHECK(t.getDateTime() == QDateTime());
}

TEST_CASE( "Transaction initConstructor", "[core]" )
{
    Transaction t(QString("Name1"), QString("Comment1"), TransactionStatus::Created, QDateTime(QDate(2021, 12, 25), QTime(8, 15, 59)), -14.99);
    CHECK(!t.getUid().isNull());
    CHECK(t.getName() == "Name1");
    CHECK(t.getComment() == "Comment1");
    CHECK(t.getStatus() == TransactionStatus::Created);
    CHECK(t.getDateTime() == QDateTime(QDate(2021, 12, 25), QTime(8, 15, 59)));
    CHECK(t.getAmount() == -14.99);
}

TEST_CASE( "Transaction writeJson", "[core]" )
{
    Transaction t(QString("Name1"), QString("Comment1"), TransactionStatus::Created, QDateTime(QDate(2021, 12, 25), QTime(8, 15, 59)), -14.99);
    QJsonObject jsonData;
    t.write(jsonData);
    CHECK((jsonData.contains("uid") && !jsonData["uid"].isNull()));
    CHECK((jsonData.contains("name") && jsonData["name"] == "Name1"));
    CHECK((jsonData.contains("comment") && jsonData["comment"] == "Comment1"));
    CHECK((jsonData.contains("status") && jsonData["status"] == TRANSACTION_STATUS_2_STRING[TransactionStatus::Created]));
    CHECK(jsonData.contains("datetime"));
    CHECK(jsonData["datetime"] == "2021-12-25T08:15:59.000");
    CHECK(jsonData.contains("amount"));
    CHECK(jsonData["amount"].toDouble() == -14.99);
}

TEST_CASE( "Transaction readJson", "[core]" )
{
    Transaction t0(QString("Name1"), QString("Comment1"), TransactionStatus::Created, QDateTime(QDate(2021, 12, 25), QTime(8, 15, 59)), -14.99);
    QJsonObject jsonData;
    t0.write(jsonData);
    Transaction t1;
    t1.read(jsonData);
    CHECK(!t1.getUid().isNull());
    CHECK(t1.getName() == "Name1");
    CHECK(t1.getComment() == "Comment1");
    CHECK(t1.getStatus() == TransactionStatus::Created);
    CHECK(t1.getDateTime() == QDateTime(QDate(2021, 12, 25), QTime(8, 15, 59)));
    CHECK(t1.getAmount() == -14.99);
}
