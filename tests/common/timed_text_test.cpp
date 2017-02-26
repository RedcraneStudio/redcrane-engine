/*
 * Copyright (C) 2017 Luke San Antonio
 * All rights reserved.
 */
#include "catch/catch.hpp"
#include "common/timed_text.h"

using namespace redc;

TEST_CASE("Timed text stream keeps proper track of its text",
          "[Timed_Text_Stream]")
{
  // Half a second timeout
  Timed_Text_Stream text_stream(.5f);

  text_stream.push_line("Hello, world!");
  CHECK("Hello, world!\n" == text_stream.full_text());

  // Time is ticking!
  text_stream.step(.25f);

  text_stream.push_line("Hello, again, world!");
  CHECK("Hello, world!\nHello, again, world!\n" == text_stream.full_text());

  text_stream.step(.30f);

  // That first line should have disappeared.
  CHECK("Hello, again, world!\n" == text_stream.full_text());

  text_stream.step(.5f);
  CHECK(text_stream.full_text().empty());
}

struct Mock_Observer : Timed_Text_Observer
{
  std::string expect_text_;
  Timed_Text_Segment expect_segment_;

  bool on_text_called_ = false;
  bool on_remove_called_ = false;

  void on_text(std::string text)
  {
    CHECK_FALSE(on_text_called_);
    CHECK(expect_text_ == text);
    on_text_called_ = true;
  }
  void on_remove(Timed_Text_Segment segment)
  {
    CHECK_FALSE(on_remove_called_);
    CHECK(expect_segment_.len == segment.len);

    on_remove_called_ = true;
  }
};


TEST_CASE("Timed text stream uses callbacks properly",
          "[Timed_Text_Stream]")
{
  Timed_Text_Stream stream(2.0f);

  auto obs = std::make_shared<Mock_Observer>();
  stream.subscribe(obs);

  obs->expect_text_ = "Hello, world!\n";
  stream.push_line("Hello, world!");

  // Make sure that there was a call to on_text.
  CHECK(obs->on_text_called_);

  // Time is ticking!
  stream.step(1.0f);

  // Get ready for a removal
  obs->expect_segment_.len = 14;

  // My biological clock!
  stream.step(3.0f);

  // Make sure there was a removal.
  CHECK(obs->on_remove_called_);
}