#include <catch2/catch.hpp>

#include "../../src/core/category.hpp"

TEST_CASE("Category defaultConstructor", "[core]")
{
    Category c{};
    CHECK(!c.getUid().isNull());
    CHECK(c.getName() == QString());
    CHECK(c.subCategories().size() == 0);
}