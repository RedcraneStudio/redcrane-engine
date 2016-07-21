/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "structure.h"

#include "../common/json.h"
#include "../gfx/idriver.h"
#include "../gfx/support/load_wavefront.h"
#include "../gfx/support/mesh_conversion.h"
#include "../gfx/support/allocate.h"
#include "../gfx/support/format.h"
#include "../gfx/support/generate_aabb.h"
#include "../gfx/support/write_data_to_mesh.h"
#include "../gfx/support/texture_load.h"
namespace redc { namespace strat
{
  Structure::Structure(gfx::Mesh_Chunk&& m, AABB aabb,
                       Maybe_Owned<Texture> tex, std::string name,
                       std::string desc) noexcept
    : mesh_chunk_(std::move(m)), aabb_(aabb), texture_(std::move(tex)),
      name_(name), desc_(desc) { }

  gfx::Mesh_Chunk const& Structure::mesh_chunk() const noexcept
  {
    return mesh_chunk_;
  }
  Maybe_Owned<Texture> const& Structure::texture() const noexcept
  {
    return texture_;
  }

  std::vector<Structure>
  load_structures(std::string filename, Maybe_Owned<IMesh> mesh,
                  gfx::IDriver& driver,
                  std::vector<Indexed_Mesh_Data>* imd) noexcept
  {
    GAME_LOG_ATTEMPT_INIT();

    auto doc = load_json(filename);

    std::vector<std::tuple<std::string, std::string> > structure_meta;
    std::vector<Indexed_Mesh_Data> structure_data;
    std::vector<std::string> texture_paths;
    for(auto iter = doc.Begin(); iter != doc.End(); ++iter)
    {
      std::string name = (*iter)["name"].GetString();
      std::string desc = (*iter)["desc"].GetString();

      // If we are given the same texture (or model for that matter) we should
      // only load and reference it once, however, given the behavior of mesh
      // chunk and it's included maybe_owned means we'll have to put it in
      // someplace that will *definitely outlive* the structures that reference
      // it.

      // For now load it as many times as we want.

      std::string model_filename = (*iter)["model"].GetString();
      auto split_data = gfx::load_wavefront(model_filename);
      auto model_data = gfx::to_indexed_mesh_data(split_data);

      structure_data.push_back(std::move(model_data));

      // Push the meta data for later.
      structure_meta.emplace_back(name, desc);

      texture_paths.push_back((*iter)["texture"].GetString());
    }

    int vertices = 0;
    int elements = 0;
    for(auto const& mesh_data : structure_data)
    {
      vertices += mesh_data.vertices.size();
      elements += mesh_data.elements.size();
    }

    gfx::allocate_standard_mesh_buffers(vertices, elements, *mesh,
                                        Usage_Hint::Draw, Upload_Hint::Static);
    gfx::format_standard_mesh_buffers(*mesh);

    // This isn't strictly necessary, since we only have one element buffer.
    mesh->use_elements(mesh->get_buffer(3));

    int accum_vertex_off = 0, accum_element_off = 0;

    std::vector<Structure> structures;
    for(std::size_t i = 0; i < structure_data.size(); ++i)
    {

      // Unpack the name and description.
      std::string name, desc;
      std::tie(name, desc) = structure_meta[i];

      // Generate an AABB from the vertex data.
      auto aabb = gfx::generate_aabb(structure_data[i]);

      auto texture = driver.make_texture_repr();
      load_png(texture_paths[i], *texture);

      // Write the structure's data to the mesh and get the mesh chunk.
      // We still want to own our maybe owned to a mesh.
      gfx::Mesh_Chunk chunk = gfx::write_data_to_mesh(structure_data[i],
                                                      ref_mo(mesh),
                                                      accum_vertex_off,
                                                      accum_element_off);

      // Record how far off we went into the mesh.
      accum_vertex_off += structure_data[i].vertices.size();
      accum_element_off += structure_data[i].elements.size();

      // Add the mesh data to the vector if the client code wants that.
      if(imd) imd->push_back(std::move(structure_data[i]));

      log_d("Loaded structure: '%' - '%'", name, desc);

      // Construct the structure.
      structures.emplace_back(std::move(chunk), aabb, std::move(texture),
                              std::move(name), std::move(desc));
    }

    return structures;
  }

  glm::vec3 ray_to_structure_bottom_center(Structure const& s) noexcept
  {
    return ray_to_aabb_bottom_center(s.aabb());
  }
} }
