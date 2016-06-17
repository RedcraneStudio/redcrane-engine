/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include <chrono>
#include <string>
#include "sdl_helper.h"
#include "gfx/scene.h"
#include "assets/minigltf.h"
#include "common/log.h"
int main(int argc, char** argv)
{
  redc::Scoped_Log_Init log_raii{};
  if(argc < 2)
  {
    redc::log_e("usage: % <filename.gltf>", argv[0]);
    return EXIT_FAILURE;
  }

  auto sdl_init = init_sdl("glTF viewer", redc::Vec<int>{1000, 1000}, false,
                           false);

  auto before = std::chrono::high_resolution_clock::now();
  auto desc = redc::load_gltf_file(argv[1]);
  auto asset = redc::load_asset(desc.value());
  auto after = std::chrono::high_resolution_clock::now();

  auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
  redc::log_i("Took %s to load", dt.count() / 1000.0);

  // List materials, shaders, programs and techniques
  for(auto mat_pair : desc->materials)
  {
    redc::log_i("Material: %", mat_pair.first);
    redc::log_i("\tname: %", mat_pair.second.name);
    redc::log_i("\ttechnique: %", mat_pair.second.technique);
    for(auto param_pair : mat_pair.second.values)
    {
      redc::log_i("\tParameter: %", param_pair.first);
      redc::log_i("\t\tstring_value: %", param_pair.second.string_value);
      std::string number_array = "[";
      for(auto num : param_pair.second.number_array)
      {
        number_array += std::to_string(num) + ", ";
      }
      number_array += "]";
      redc::log_i("\t\tnumber_array: %", number_array);
    }
  }
  for(auto mesh_pair : desc->meshes)
  {
    redc::log_i("Mesh: %", mesh_pair.first);
    std::size_t i = 0;
    for(auto prim : mesh_pair.second.primitives)
    {
      redc::log_i("\t% material: %", i, prim.material);
      redc::log_i("\t% indices: %", i, prim.indices);
      redc::log_i("\t% mode: %", i, prim.mode);
      for(auto attrib_pair : prim.attributes)
      {
        redc::log_i("\t%\tattribute %:%", i, attrib_pair.first,
                    attrib_pair.second);
      }
      ++i;
    }
  }
  for(auto shader_pair : desc->shaders)
  {
    redc::log_i("Shader: %", shader_pair.first);
    redc::log_i("\tname: %", shader_pair.second.name);
    redc::log_i("\ttype: %", shader_pair.second.type);

    std::string source(shader_pair.second.source.begin(),
                       shader_pair.second.source.end());
    redc::log_i("\tsource:\n%", source);
  }
  for(auto program_pair : desc->programs)
  {
    redc::log_i("Program: %", program_pair.first);
    redc::log_i("\tvertexShader: %", program_pair.second.vertexShader);
    redc::log_i("\tfragmentShader: %", program_pair.second.fragmentShader);
    for(auto attrib : program_pair.second.attributes)
    {
      redc::log_i("\tattribute %", attrib);
    }
  }
  for(auto technique_pair : desc->techniques)
  {
    redc::log_i("Technique: %", technique_pair.first);
    redc::log_i("\tname: %", technique_pair.second.name);
    redc::log_i("\tprogram: %", technique_pair.second.program);

    for(auto attrib_pair : technique_pair.second.attributes)
    {
      redc::log_i("\tattribute %:%", attrib_pair.first, attrib_pair.second);
    }
    for(auto uniform_pair : technique_pair.second.uniforms)
    {
      redc::log_i("\tuniform %:%", uniform_pair.first, uniform_pair.second);
    }
    for(auto parameter_pair : technique_pair.second.parameters)
    {
      redc::log_i("\tParameter: %", parameter_pair.first);
      redc::log_i("\t\tcount: %", parameter_pair.second.count);
      redc::log_i("\t\tnode: %", parameter_pair.second.node);
      redc::log_i("\t\ttype: %", parameter_pair.second.type);
      redc::log_i("\t\tsemantic: %", parameter_pair.second.semantic);
      redc::log_i("\t\tValue:");
      redc::log_i("\t\t\tstring_value: %",
                  parameter_pair.second.value.string_value);
      std::string number_array = "[";
      for(auto num : parameter_pair.second.value.number_array)
      {
        number_array += std::to_string(num) + ", ";
      }
      number_array += "]";
      redc::log_i("\t\t\tnumber_array: %", number_array);

    }
  }

  return EXIT_SUCCESS;
}
