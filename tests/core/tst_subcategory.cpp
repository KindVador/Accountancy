#include <catch2/catch.hpp>

#include "../../src/core/subcategory.hpp"

TEST_CASE("SubCategory defaultConstructor", "[core]")
{
    SubCategory sc{};
    CHECK(!sc.getUid().isNull());
    CHECK(sc.getName() == QString());
    CHECK(sc.subCategories().size() == 0);
    CHECK(sc.getParent() == QUuid());
}