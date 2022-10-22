#include <catch2/catch.hpp>

#include "../../src/core/currency.hpp"

TEST_CASE("Currency default Constructor", "[core]")
{
    Currency c{};
    CHECK(!c.getUid().isNull());
    CHECK(c.getName() == QString());
    CHECK(c.getSymbol() == QString());
}

TEST_CASE("Currency init Constructor", "[core]")
{
    Currency c{"Euro", "€"};
    CHECK(!c.getUid().isNull());
    CHECK(c.getName() == QString("Euro"));
    CHECK(c.getSymbol() == QString("€"));
}

TEST_CASE("Currency writeJson", "[core]")
{
    Currency c0{"Euro", "€"};
    QJsonObject jsonData;
    c0.write(jsonData);
    Currency c1;
    c1.read(jsonData);
    CHECK(!c1.getUid().isNull());
    CHECK(c1.getName() == "Euro");
    CHECK(c1.getSymbol() == "€");
}

TEST_CASE("Currency readJson", "[core]")
{
    Currency c{"Euro", "€"};
    QJsonObject jsonData;
    c.write(jsonData);
    CHECK((jsonData.contains("uid") && !jsonData["uid"].isNull()));
    CHECK((jsonData.contains("name") && jsonData["name"] == "Euro"));
    CHECK((jsonData.contains("symbol") && jsonData["symbol"] == "€"));
}
