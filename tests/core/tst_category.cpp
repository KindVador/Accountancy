#include <catch2/catch.hpp>

#include "../../src/core/category.hpp"
#include <QJsonObject>

TEST_CASE("Category defaultConstructor", "[core]")
{
    Category c{};
    CHECK(!c.getUid().isNull());
    CHECK(c.getName() == QString());
    CHECK(c.subCategories().isEmpty());
}

TEST_CASE("Category init Constructor", "[core]")
{
    Category c{"Category1"};
    CHECK(!c.getUid().isNull());
    CHECK(c.getName() == QString("Category1"));
}

TEST_CASE("Category addSubCategory", "[core]")
{
    Category c{"Category1"};
    Category sc{"SubCategory1"};
    c.addSubCategory(&sc);
    CHECK(!c.subCategories().isEmpty());
    CHECK(c.subCategories().at(0)->getName() == "SubCategory1");
}

TEST_CASE("Category removeSubCategory", "[core]")
{
    Category c{"Category1"};
    Category sc{"SubCategory1"};
    c.addSubCategory(&sc);
    CHECK(!c.subCategories().isEmpty());
    c.removeSubCategory(&sc);
    CHECK(c.subCategories().isEmpty());
}

TEST_CASE("Category removeSubCategoryByName", "[core]")
{
    Category c{"Category1"};
    Category sc{"SubCategory1"};
    c.addSubCategory(&sc);
    CHECK(!c.subCategories().isEmpty());
    c.removeSubCategoryByName("SubCategory1");
    CHECK(c.subCategories().isEmpty());
}

TEST_CASE("Category writeJson", "[core]")
{
    Category c0{"Category1"};
    Category sc{"SubCategory1"};
    c0.addSubCategory(&sc);
    QJsonObject jsonData;
    c0.write(jsonData);
    Category c1;
    c1.read(jsonData);
    CHECK(!c1.getUid().isNull());
    CHECK(c1.getName() == "Category1");
    CHECK(!c1.subCategories().isEmpty());
    const Category* sc1 = c1.subCategories().at(0);
    CHECK(sc1->getName() == "SubCategory1");
}

TEST_CASE("Category readJson", "[core]")
{
    Category c{"Category1"};
    Category sc{"SubCategory1"};
    c.addSubCategory(&sc);
    QJsonObject jsonData;
    c.write(jsonData);
    CHECK((jsonData.contains("uid") && !jsonData["uid"].isNull()));
    CHECK((jsonData.contains("name") && jsonData["name"] == "Category1"));
    CHECK(!c.subCategories().isEmpty());
    const Category* sc1 = c.subCategories().at(0);
    CHECK(sc1->getName() == "SubCategory1");
}