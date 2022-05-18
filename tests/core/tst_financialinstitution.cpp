#include <catch2/catch.hpp>

#include "../../src/core/financialinstitution.hpp"

TEST_CASE( "FinancialInstitution defaultConstructor", "[core]" )
{
    FinancialInstitution fi{};
    CHECK(!fi.getUid().isNull());
    CHECK(fi.getName() == QString());
}

TEST_CASE( "FinancialInstitution initConstructor", "[core]" )
{
    auto fi = FinancialInstitution("Institution1");
    CHECK(!fi.getUid().isNull());
    CHECK(fi.getName() == "Institution1");
}

TEST_CASE( "FinancialInstitution writeJson", "[core]" )
{
    auto fi = FinancialInstitution("Institution1");
    QJsonObject jsonData;
    fi.write(jsonData);
    CHECK((jsonData.contains("uid") && !jsonData["uid"].isNull()));
    CHECK((jsonData.contains("name") && jsonData["name"] == "Institution1"));
}

TEST_CASE( "FinancialInstitution readJson", "[core]" )
{
    auto fi0 = FinancialInstitution("Institution1");
    QJsonObject jsonData;
    fi0.write(jsonData);
    FinancialInstitution fi1;
    fi1.read(jsonData);
    CHECK(fi1.getName() == "Institution1");
    CHECK(!fi1.getUid().isNull());
}
