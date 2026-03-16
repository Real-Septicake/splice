#include <catch2/catch_test_macros.hpp>
#include <splice/splice.hpp>

struct DummyEntity
{
  int m_id = 42;
  float m_health = 100.0f;
  std::string m_name = "entity";
};

TEST_CASE("shadow_get reads an int member", "[shadow][get]")
{
  DummyEntity entity;
  int id = splice::shadow_get<^^DummyEntity::m_id>(&entity);
  REQUIRE(id == 42);
}

TEST_CASE("shadow_get reads a float member", "[shadow][get]")
{
  DummyEntity entity;
  float health = splice::shadow_get<^^DummyEntity::m_health>(&entity);
  REQUIRE(health == 100.0f);
}

TEST_CASE("shadow_get reads a string member", "[shadow][get]")
{
  DummyEntity entity;
  const std::string &name = splice::shadow_get<^^DummyEntity::m_name>(&entity);
  REQUIRE(name == "entity");
}

TEST_CASE("shadow_get returns a reference to the member", "[shadow][get]")
{
  DummyEntity entity;
  decltype(auto) ref = splice::shadow_get<^^DummyEntity::m_id>(&entity);
  ref = 99;
  REQUIRE(splice::shadow_get<^^DummyEntity::m_id>(&entity) == 99);
}

TEST_CASE("shadow_set writes an int member", "[shadow][set]")
{
  DummyEntity entity;
  splice::shadow_set<^^DummyEntity::m_id>(&entity, 99);
  REQUIRE(splice::shadow_get<^^DummyEntity::m_id>(&entity) == 99);
}

TEST_CASE("shadow_set writes a float member", "[shadow][set]")
{
  DummyEntity entity;
  splice::shadow_set<^^DummyEntity::m_health>(&entity, 50.0f);
  REQUIRE(splice::shadow_get<^^DummyEntity::m_health>(&entity) == 50.0f);
}

TEST_CASE("shadow_set move-overwrites a string member", "[shadow][set]")
{
  DummyEntity entity;
  splice::shadow_set<^^DummyEntity::m_name>(&entity, std::string("moved"));
  REQUIRE(splice::shadow_get<^^DummyEntity::m_name>(&entity) == "moved");
}

TEST_CASE("shadow_set round-trips correctly", "[shadow][set]")
{
  DummyEntity entity;
  splice::shadow_set<^^DummyEntity::m_id>(&entity, 7);
  REQUIRE(splice::shadow_get<^^DummyEntity::m_id>(&entity) == 7);
}
