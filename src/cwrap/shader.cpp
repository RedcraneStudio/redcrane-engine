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

#include <unordered_map>

using namespace redc;

extern "C"
{
  struct Shader_Builder
  {
    gfx::Shader::shader_source_t vertex_shader;
    gfx::Shader::shader_source_t fragment_shader;
    gfx::Shader::shader_source_t geometry_shader;

    // This is a hashmap of tags to variables
    std::unordered_map<std::string, std::string> tags;

    Peer_Ptr<gfx::Shader> shader;

    std::string name;
  };

  void *redc_make_shader_builder(void *eng, const char *name)
  {
    // We need a client
    auto rce = (Engine*) eng;
    REDC_ASSERT_HAS_CLIENT(rce);

    // Make a new shader builder
    auto ret = new Shader_Builder;

    // And a new shader
    ret->shader = rce->client->driver->make_shader_repr();

    // Put a peer into the engine
    rce->client->peers.push_back(ret->shader.peer());

    // Set the name, this is important for debugging when there is a compile
    // error, it's good to know which shader was being compiled, obviously.
    ret->name = name;

    return ret;
  }

  // DON'T call this if you've already compiled the shader. DO call this if you
  // want to cancel the creation of a shader, etc.
  void redc_unmake_shader_builder(void *builder)
  {
    auto ptr = (Shader_Builder*) builder;
    delete ptr;
  }

  void redc_shader_append_vertex_part(void *shade, const char *str)
  {
    if(!str) return;

    auto builder = (Shader_Builder*) shade;

    using std::begin; using std::end;
    auto insert_pos = end(builder->vertex_shader);
    builder->vertex_shader.insert(insert_pos, str, str + strlen(str));
  }
  void redc_shader_append_fragment_part(void *shade, const char *str)
  {
    if(!str) return;

    auto builder = (Shader_Builder*) shade;

    using std::begin; using std::end;
    auto insert_pos = end(builder->fragment_shader);
    builder->fragment_shader.insert(insert_pos, str, str + strlen(str));
  }
  void redc_shader_append_geometry_part(void *shade, const char *str)
  {
    if(!str) return;

    auto builder = (Shader_Builder*) shade;

    using std::begin; using std::end;
    auto insert_pos = end(builder->geometry_shader);
    builder->geometry_shader.insert(insert_pos, str, str + strlen(str));
  }
  void redc_shader_compile_vertex_part(void *shade)
  {
    auto builder = (Shader_Builder*) shade;

    std::string name = builder->name + " <vertex>";
    builder->shader->load_vertex_part(builder->vertex_shader, name);
  }
  void redc_shader_compile_fragment_part(void *shade)
  {
    auto builder = (Shader_Builder*) shade;

    std::string name = builder->name + " <fragment>";
    builder->shader->load_fragment_part(builder->fragment_shader, name);
  }
  void redc_shader_compile_geometry_part(void *shade)
  {
    auto builder = (Shader_Builder*) shade;

    std::string name = builder->name + " <geometry>";
    builder->shader->load_geometry_part(builder->geometry_shader, name);
  }

  void *redc_shader_link(void *shade)
  {
    auto builder = (Shader_Builder*) shade;

    // If linking happens to fail, we expect a log message, most importantly it
    // means we don't have to worry about tagging uniforms.
    if(!builder->shader->link()) return nullptr;

    // Notify the shader of all the tags. It should be caching the location of
    // each one so they can be relatively easily set.
    for(auto tag_var_pair : builder->tags)
    {
      builder->shader->set_var_tag(tag_var_pair.first, tag_var_pair.second);
    }

    // Move the shader out since everything was presumably successful.
    auto shader = std::move(builder->shader);

    // Properly delete the builder
    redc_unmake_shader_builder(builder);

    // Make a new peer on the heap for lua
    auto ret = new Peer_Ptr<gfx::Shader>(std::move(shader));

    return ret;
  }

  void redc_shader_destroy(void *shader)
  {
    auto ptr = (Peer_Ptr<gfx::Shader>*) shader;
    delete ptr;
  }

  void redc_shader_tag_uniform(void *shade, const char *tag, const char *name)
  {
    auto builder = (Shader_Builder*) shade;

    // Set the tag to that variable name.
    if(builder->tags.count(tag)) builder->tags.at(tag) = name;
    else builder->tags.insert({tag, name});

    // We hand these tags to the shader after compiling and linking is all said
    // and done.
  }
}
