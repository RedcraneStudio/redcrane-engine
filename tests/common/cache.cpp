/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
//#include <boost/optional/optional_io.hpp>
#include "catch/catch.hpp"
#include "common/cache.h"

using redc::Cache;

TEST_CASE("Simple tests", "[cache]")
{
  Cache<int, std::function<std::unique_ptr<int> (std::unique_ptr<int>)> > cache;
  using ptr_type = decltype(cache)::ptr_type;
  int generates = 0;

  cache.gen_func(
  [&](ptr_type&& p)
  {
    ++generates;
    p.reset(new int());
    return std::move(p);
  });

  SECTION("Invalidate")
  {
    // We haven't run that lambda have we, yet?
    CHECK(0 == generates);

    // Our cache should have been successful.
    CHECK(cache.generate());

    // Forget that value.
    cache.invalidate();
    CHECK(cache.generate());

    // We should have generated that string twice now.
    CHECK(2 == generates);
  }
  SECTION("Generate")
  {
    cache.generate();
    cache.generate();
    cache.generate();
    CHECK(3 == generates);
  }
  SECTION("Cache")
  {
    cache.invalidate();

    // ccache shouldn't regenerate!
    cache.ccache();
    CHECK(0 == generates);

    // cache *should* generate, especially since we already invalidated the
    // current cache.
    cache.cache();
    CHECK(1 == generates);

    cache.invalidate();

    // What's our current Cache? What's our current Cache? What's our current
    // Cache?
    cache.ccache();
    cache.ccache();
    cache.ccache();

    // This. This. This! (No generations should have occurred, the "This" should
    // be non conditional.)
    CHECK(1 == generates);
  }
}
TEST_CASE("Cache with dependency tests", "[cache]")
{
  Cache<int,
      std::function<std::unique_ptr<int> (std::unique_ptr<int>, int&)>, int> cache;
  using ptr_type = decltype(cache)::ptr_type;

  SECTION("Dependencies are set")
  {
    // Set the int dependency.
    int dependency_expected = 5;
    cache.set_dependency<0>(dependency_expected);

    // Make sure get_dependency works.
    CHECK(dependency_expected == cache.get_dependency<0>());

    // Now check whether it's passed to the gen func correctly.
    cache.gen_func(
    [](ptr_type p, int x)
    {
      p.reset(new int(x));
      return p;
    });

    // The cache value should be equal to the int dependency.
    CHECK(dependency_expected == *cache.cache());
  }
  SECTION("Dependencies transfer")
  {
    // Set a dependency.
    int dependency_expected = 5;
    cache.set_dependency<0>(dependency_expected);

    // Passthroredch gen func.
    cache.gen_func(
    [](ptr_type p, int x)
    {
      p.reset(new int(x));
      return p;
    });

    // Copy.
    decltype(cache) other(cache);

    // The dependency should have been copied.
    CHECK(dependency_expected == *other.cache());

    // Now move.
    decltype(cache) moved(cache);

    // The dependency should have been moved.
    CHECK(dependency_expected == *moved.cache());
  }
  SECTION("Set dependency uses equality")
  {
    int generates = 0;
    cache.gen_func(
    [&](ptr_type p, int x)
    {
      ++generates;
      p.reset(new int(x));
      return p;
    });

    CHECK(0 == generates);

    int dep_val = 1;
    cache.set_dependency<0>(dep_val);

    cache.generate();
    CHECK(1 == generates);

    cache.set_dependency<0>(dep_val);
    cache.cache();
    CHECK(1 == generates);
  }

  // This test makes sure that the dependencies can be changed within the
  // generation function.
  SECTION("Dependency arguments change persists")
  {
    int expected = 1;

    cache.gen_func(
    [=](ptr_type p, int& x)
    {
      x = expected;
      return p;
    });
    cache.generate();

    CHECK(expected == cache.get_dependency<0>());
  }
}
