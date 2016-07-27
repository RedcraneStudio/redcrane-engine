/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "catch/catch.hpp"
#include "common/utility.h"
#include <memory>

TEST_CASE("Vector cast works", "[utility]")
{
  std::vector<float> floats = { 5.135f, 6.123f, 7.23f };

  // Test using implicit conversion.
  std::vector<int> ints = redc::vector_cast<int>(floats);
  std::vector<int> expected = { 5, 6, 7 };
  CHECK(expected == ints);

  // Test using the explicit conversion.
  std::vector<std::string> strings =
     redc::vector_cast<std::string>(floats,
                  [](float f) { return std::to_string(static_cast<int>(f)); });
  std::vector<std::string> expected_strings{ "5", "6", "7" };
  CHECK(expected_strings == strings);
}
TEST_CASE("Get data vector", "[utility]")
{
  using redc::get_data_vector;

  std::vector<std::unique_ptr<int> > ints;
  for(int i = 0; i < 10; ++i)
  {
    ints.push_back(std::make_unique<int>(i));
  }

  std::vector<int*> result = get_data_vector<int*>(ints);
  std::vector<const int*> const_result = get_data_vector<const int*>(ints);

  auto compare = [](const std::unique_ptr<int>& p, const int* pi)
  { return p.get() == pi; };

  using std::begin;
  CHECK(std::equal(begin(ints), end(ints), begin(result), compare));
  CHECK(std::equal(begin(ints), end(ints), begin(const_result), compare));
}
