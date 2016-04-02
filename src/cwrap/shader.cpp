/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file provides the implementation for the engine's C interface,
 * specifically tailored for LuaJIT's FFI facilities.
 *
 * Shader stuff
 */
#include "redcrane.hpp"

using namespace redc;

extern "C"
{
  void *redc_make_shader(void *eng, const char* dir)
  {
    auto rce = (redc::Engine*) eng;

    auto shade = rce->driver->make_shader_repr();

    using namespace std::literals;
    auto base_path = rce->share_path / "shader"s / std::string{dir};

    shade->load_vertex_part((base_path / "vs.glsl").native());
    shade->load_fragment_part((base_path / "fs.glsl").native());

    shade->set_model_name("model");
    shade->set_view_name("view");
    shade->set_projection_name("proj");

    // Make a peer for luajit
    auto peer = new Peer_Ptr<gfx::Shader>(std::move(shade));

    // Give the engine a peer
    rce->shaders.push_back(peer->peer());

    return peer;
  }
  void redc_unmake_shader(void *shader)
  {
    auto ptr = (Peer_Ptr<gfx::Shader>*) shader;
    delete ptr;
  }
}
