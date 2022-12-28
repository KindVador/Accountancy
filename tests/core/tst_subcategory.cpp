#include <catch2/catch.hpp>

#include "../../src/core/subcategory.hpp"
#include <QJsonObject>
#include <QUuid>

TEST_CASE("SubCategory defaultConstructor", "[core]")
{
    SubCategory sc{};
    CHECK(!sc.getUid().isNull());
    CHECK(sc.getName() == QString());
    CHECK(sc.subCategories().isEmpty());
    CHECK(sc.getParent() == QUuid());
}

TEST_CASE("SubCategory init Constructor", "[core]")
{
    Category parent{"ParentCategory"};
    SubCategory sc{"Category1", &parent};
    CHECK(!sc.getUid().isNull());
    CHECK(sc.getName() == QString("Category1"));
    CHECK(sc.getParent() == parent.getUid());
}

TEST_CASE("SubCategory writeJson", "[core]")
{
    Category parent{"ParentCategory"};
    SubCategory sc0{"Category1", &parent};
    QJsonObject jsonData;
    sc0.write(jsonData);
    SubCategory sc1;
    sc1.read(jsonData);
    CHECK(!sc1.getUid().isNull());
    CHECK(sc1.getName() == "Category1");
    CHECK(sc1.getParent() == parent.getUid());
}

TEST_CASE("SubCategory readJson", "[core]")
{
    Category parent{"ParentCategory"};
    SubCategory sc{"Category1", &parent};
    QJsonObject jsonData;
    sc.write(jsonData);
    CHECK((jsonData.contains("uid") && !jsonData["uid"].isNull()));
    CHECK((jsonData.contains("name") && jsonData["name"] == "Category1"));
    CHECK((jsonData.contains("parentUid") && QUuid(jsonData["parentUid"].toString()) == parent.getUid()));
}