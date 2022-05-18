#include <catch2/catch.hpp>

#include "../../src/core/owner.hpp"

TEST_CASE( "Owner defaultConstructor", "[core]" )
{
    Owner o{};
    CHECK(o.getName() == QString());
    CHECK(o.getWarningBalance() == 0.0);
    CHECK(o.getComment() == QString());
    CHECK(!o.getIsHidden());
    CHECK(!o.getUid().isNull());
}

TEST_CASE( "Owner initConstructor", "[core]" )
{
    Owner o{"Name1", 150.0, "Comment1", false};
    CHECK(o.getName() == "Name1");
    CHECK(o.getWarningBalance() == 150.0);
    CHECK(o.getComment() == "Comment1");
    CHECK(!o.getIsHidden());
    CHECK(!o.getUid().isNull());
}

TEST_CASE( "Owner writeJson", "[core]" )
{
    Owner o{"Name1", 150.0, "Comment1", false};
    QJsonObject jsonData;
    o.write(jsonData);
    CHECK((jsonData.contains("uid") && !jsonData["uid"].isNull()));
    CHECK((jsonData.contains("name") && jsonData["name"] == "Name1"));
    CHECK((jsonData.contains("comment") && jsonData["comment"] == "Comment1"));
    CHECK(jsonData.contains("warning_balance"));
    CHECK(jsonData["warning_balance"].toDouble() == 150.0);
    CHECK((jsonData.contains("is_hidden") && !jsonData["is_hidden"].toBool()));
}

TEST_CASE( "Owner readJson", "[core]" )
{
    Owner o0{"Name1", 150.0, "Comment1", false};
    QJsonObject jsonData;
    o0.write(jsonData);
    Owner o1;
    o1.read(jsonData);
    CHECK(o1.getName() == "Name1");
    CHECK(o1.getWarningBalance() == 150.0);
    CHECK(o1.getComment() == "Comment1");
    CHECK(!o1.getIsHidden());
    CHECK(!o1.getUid().isNull());
}
