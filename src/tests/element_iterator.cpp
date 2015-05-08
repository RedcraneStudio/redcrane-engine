/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "../ui/elements/empty.h"
#include "../ui/elements/layouts/linear_layout.h"
#include "../ui/element_iterator.h"
#include "catch/catch.hpp"

using namespace game;

TEST_CASE("Element iterator works.", "[Element_Iterator]")
{
  auto container = std::make_shared<ui::Linear_Layout>();

  container->push_child(std::make_shared<ui::Empty>());

  auto sec_child = std::make_shared<ui::Linear_Layout>();
  sec_child->push_child(std::make_shared<ui::Empty>());
  sec_child->push_child(std::make_shared<ui::Empty>());

  container->push_child(sec_child);

  auto iter = ui::D_F_Elem_Iter{container};

  REQUIRE(&(*iter) == container.get());
  REQUIRE(&(*++iter) == container->child_at(0).get());
  REQUIRE(&(*++iter) == container->child_at(1).get());
  REQUIRE(&(*++iter) == container->child_at(1)->child_at(0).get());
  REQUIRE(&(*++iter) == container->child_at(1)->child_at(1).get());
}
