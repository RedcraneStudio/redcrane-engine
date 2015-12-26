/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "load_scene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "../gfx/mesh_data.h"

#include "../gfx/support/allocate.h"
#include "../gfx/support/format.h"
#include "../gfx/support/write_data_to_mesh.h"

#include "../gfx/support/json.h"
#include "../gfx/support/mesh_conversion.h"
#include "../gfx/support/load_wavefront.h"

#include "../collisionlib/triangle_conversion.h"
namespace game { namespace fps
{
  Scene load_scene(std::string fn, std::unique_ptr<game::Mesh> msh) noexcept
  {
    using namespace game;

    auto json = load_json(fn);

    auto scene = Scene{};
    scene.player_pos = vec3_from_js(json["player_pos"]);

    // TODO: Find a way to abstract all this mesh-data caching + allocating mesh
    // for all of them.

    std::vector<std::tuple<Indexed_Mesh_Data, glm::mat4> > metas;

    auto iter = json["objects"].Begin();
    auto enditer = json["objects"].End();
    for(; iter != enditer; ++iter)
    {
      auto const& obj = (*iter);

      auto obj_fn = std::string{obj["obj"].GetString()};
      auto msh_data = gfx::to_indexed_mesh_data(gfx::load_wavefront(obj_fn));

      // Generate the matrix using the list of transformations.
      glm::mat4 model;

      auto const& trans = obj["transformations"];
      auto trans_iter = trans.Begin();
      for(; trans_iter != trans.End(); ++trans_iter)
      {
        auto str = std::string{(*trans_iter)[0].GetString()};
        if(str == "scale")
        {
          model = glm::scale(model, glm::vec3((*trans_iter)[1].GetDouble(),
                                              (*trans_iter)[2].GetDouble(),
                                              (*trans_iter)[3].GetDouble()));
        }
        else if(str == "translation")
        {
          model = glm::translate(model,
                                 glm::vec3((*trans_iter)[1].GetDouble(),
                                           (*trans_iter)[2].GetDouble(),
                                           (*trans_iter)[3].GetDouble()));
        }
        else if(str == "rotation")
        {
          model = glm::rotate(model,
                            glm::radians((float)(*trans_iter)[1].GetDouble()),
                              glm::vec3((float) (*trans_iter)[2].GetDouble(),
                                        (float) (*trans_iter)[3].GetDouble(),
                                        (float) (*trans_iter)[4].GetDouble()));
        }
      }

      using std::move;
      metas.emplace_back(move(msh_data), move(model));
    }

    // We have all of our needed mesh data and each model;
    int vertices = 0;
    int elements = 0;
    for(auto const& mesh_data : metas)
    {
      vertices += std::get<0>(mesh_data).vertices.size();
      elements += std::get<0>(mesh_data).elements.size();
    }

    gfx::allocate_standard_mesh_buffers(vertices, elements, *msh,
                                        Usage_Hint::Draw, Upload_Hint::Static);
    gfx::format_standard_mesh_buffers(*msh);

    msh->use_elements(msh->get_buffer(3));

    int accum_vertex_off = 0, accum_element_off = 0;

    Maybe_Owned<Mesh> mesh = std::move(msh);
    for(std::size_t i = 0; i < metas.size(); ++i)
    {
      auto mesh_for_chunk = Maybe_Owned<Mesh>{};
      if(i == 0)
      {
        mesh_for_chunk = std::move(mesh);
      }
      else
      {
        mesh_for_chunk.set_pointer(mesh);
      }

      // Write the object's data to the mesh and get the mesh chunk.
      // We still want to own our maybe owned to a mesh.
      gfx::Mesh_Chunk chunk = gfx::write_data_to_mesh(std::get<0>(metas[i]),
                                                      std::move(mesh_for_chunk),
                                                      accum_vertex_off,
                                                      accum_element_off);

      // Record how far off we went into the mesh.
      accum_vertex_off += std::get<0>(metas[i]).vertices.size();
      accum_element_off += std::get<0>(metas[i]).elements.size();

      // Construct the structure.
      scene.objects.push_back(Object{std::move(chunk), std::get<1>(metas[i])});
    }

    // Before discarding the indexed mesh data, lets copy it into scene.
    // Unfortunately it's a different representation so it must be copied.
    for(auto iter = metas.begin(); iter != metas.end(); ++iter)
    {
      collis::append_triangles(scene.collision_triangles,
                               std::get<0>(*iter), true);
    }

    return scene;
  }
} }
