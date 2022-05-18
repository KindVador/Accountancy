#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "../../src/core/account.hpp"
#include "../../src/core/financialinstitution.hpp"

TEST_CASE( "Account defaultConstructor", "[core]" )
{
    Account a{};
    CHECK(!a.getUid().isNull());
    CHECK(a.getInstitution() == nullptr);
    CHECK(a.getCurrency() == nullptr);
    CHECK(a.getOwners().count() == 0);
    CHECK(a.getInitialBalance() == 0.0);
    CHECK(a.getWarningBalance() == 0.0);
}

TEST_CASE( "Account writeJson", "[core]" )
{
    auto *fi = new FinancialInstitution("Institution1");
    auto *currency = new Currency("Euro", "€");
    auto *owner = new Owner("Owner1", 150.0, "Comment1", false);
    Account a{fi, AccountType::Cash, currency, {owner}, 147.85, 100.0, "123456789AX", "CommentAccount1", true, false};
    QJsonObject jsonData;
    a.write(jsonData);
    CHECK((jsonData.contains("uid") && !jsonData["uid"].isNull()));
    CHECK((jsonData.contains("institution") && !jsonData["institution"].isNull()));
    CHECK((jsonData.contains("currency") && !jsonData["currency"].isNull()));
    CHECK((jsonData.contains("owners") && !jsonData["owners"].isNull()));
    CHECK(jsonData.contains("initialBalance"));
    CHECK(abs(jsonData["initialBalance"].toDouble()- 147.85) < 0.0001);
    CHECK(jsonData.contains("warningBalance"));
    CHECK(jsonData["warningBalance"].toDouble() == 100.00);
    CHECK(jsonData.contains("accountNumber"));
    CHECK(jsonData["accountNumber"].toString() == "123456789AX");
    CHECK(jsonData.contains("comment"));
    CHECK(jsonData["comment"].toString() == "CommentAccount1");
    CHECK(jsonData.contains("isIncludedInTotal"));
    CHECK(jsonData["isIncludedInTotal"].toBool() == true);
    CHECK(jsonData.contains("isHidden"));
    CHECK(jsonData["isHidden"].toBool() == false);
    CHECK(jsonData.contains("type"));
    CHECK(jsonData["type"].toString() == ACCOUNT_TYPE_2_STRING[AccountType::Cash]);

    delete fi;
    delete currency;
    delete owner;
}

TEST_CASE( "Account readJson", "[core]" )
{
    // write data to Json object
    auto *fi = new FinancialInstitution("Institution1");
    auto *currency = new Currency("Euro", "€");
    auto *owner = new Owner("Owner1", 150.0, "Comment1", false);
    Account a0{fi, AccountType::Cash, currency, {owner}, 147.85, 100.0, "123456789AX", "CommentAccount1", true, false};
    a0.setUid(QUuid("67C8770B-44F1-410A-AB9A-F9B5446F13EE"));
    QJsonObject jsonData;
    a0.write(jsonData);

    // read data from Json object
    Account a1;
    a1.read(jsonData);

    // check that values are correctly read
    CHECK(a1.getUid().toString().toUpper() == "{67C8770B-44F1-410A-AB9A-F9B5446F13EE}");
    CHECK(a1.getInstitution() != nullptr);
    CHECK(a1.getCurrency() != nullptr);
    CHECK(a1.getOwners().count() > 0);
    CHECK(abs(a1.getInitialBalance() - 147.85) < 0.0001);
    CHECK(a1.getWarningBalance() == 100.00);
    CHECK(a1.getAccountNumber() == "123456789AX");
    CHECK(a1.getComment() == "CommentAccount1");
    CHECK(a1.isIncludedInTotal() == true);
    CHECK(a1.isHidden() == false);
    CHECK(a1.getType() == AccountType::Cash);

    delete fi;
    delete currency;
    delete owner;
}
