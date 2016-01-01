/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
namespace redc
{
  /*!
   * \brief Calls std::abort after displaying the message passed in!
   *
   * \note Does not return, ever!
   */
  [[noreturn]] inline void crash(const std::string& why)
  {
    std::cerr << "Crashing..." << " " << why << std::endl;
    std::abort();
  }
}
