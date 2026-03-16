#include <catch2/catch_test_macros.hpp>
#include <splice/splice.hpp>

TEST_CASE("Priority constants are strictly ordered", "[priority]")
{
  REQUIRE(splice::Priority::Highest < splice::Priority::High);
  REQUIRE(splice::Priority::High < splice::Priority::Normal);
  REQUIRE(splice::Priority::Normal < splice::Priority::Low);
  REQUIRE(splice::Priority::Low < splice::Priority::Lowest);
}

TEST_CASE("Priority arithmetic works as expected", "[priority]")
{
  REQUIRE(splice::Priority::Normal + 1 > splice::Priority::Normal);
  REQUIRE(splice::Priority::Normal - 1 < splice::Priority::Normal);
}

TEST_CASE("Priority::Normal is the midpoint of Highest and Lowest", "[priority]")
{
  REQUIRE(splice::Priority::Normal == (splice::Priority::Highest + splice::Priority::Lowest) / 2);
}
