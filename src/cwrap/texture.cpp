/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 */

#include "redcrane.hpp"

#include "../use/texture.h"

using namespace redc;

extern "C"
{
  void* redc_load_texture(void* eng, const char* str)
  {
    auto rce = (Engine*) eng;

    // This only makes sense for clients to do
    REDC_ASSERT_HAS_CLIENT(rce);

    // Load the texture
    auto texture = load_texture(*rce->client->driver, std::string{str});

    // Lua needs one peer
    auto peer = new Peer_Ptr<Texture>(std::move(texture));

    // The engine needs the other
    rce->client->peers.push_back(peer->peer());

    return peer;
  }
  void redc_unload_texture(void* tex)
  {
    auto peer = (Peer_Ptr<Texture>*) tex;
    delete peer;
  }
}
