/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "network_client.h"
namespace redc
{
  //! \brief Context information for methods called by a given client.
  template <class T>
  struct User_Type
  {
    Network_Client* client;
    T* game;

    void* userdata;
  };
}
