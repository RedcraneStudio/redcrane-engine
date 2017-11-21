/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include "asset_render.h"
#include "common.h"
#include "../common/debugging.h"
#include <glm/gtc/matrix_transform.hpp>
namespace redc { namespace gfx
{
  glm::mat4 local_transformation(Node const& node)
  {
      // Find the model of the current node and then go up a level
      glm::mat4 this_model(1.0f);

      // Replace the identity matrix above with the optional matrix in the node
      if(node.matrix)
      {
        std::memcpy(&this_model[0], &node.matrix.value()[0], 16 * sizeof(float));
      }

      // Scale, rotate and then translate! I don't believe having both a matrix
      // and scale/rotation/translation is allowed in the standard so we
      // shouldn't worry about the order of these two values, just as long as
      // these three are properly ordered.
      if(node.scale)
      {
        auto& arr = node.scale.value();
        this_model = glm::scale(this_model, glm::vec3(arr[0], arr[1], arr[2]));
      }
      if(node.rotation)
      {
        auto& arr = node.rotation.value();
        glm::quat rot(arr[0], arr[1], arr[2], arr[3]);
        this_model = mat4_cast(rot) * this_model;
      }
      if(node.translation)
      {
        auto& arr = node.translation.value();
        this_model = glm::translate(this_model, glm::vec3(arr[0], arr[1], arr[2]));
      }

      return this_model;
  }
  glm::mat4 model_transformation(std::vector<Node> const& nodes, Node_Ref child)
  {
    boost::optional<Node_Ref> cur_node = child;
    glm::mat4 cur_model(1.0f);

    while(cur_node)
    {
      Node const& node = nodes[cur_node.value()];

      cur_model = local_transformation(node) * cur_model;
      cur_node = node.parent;
    }

    return cur_model;
  }

  // TODO: This is a bad name
  struct Render_Params
  {
    IMesh* mesh;
    Primitive const* primitive;
    glm::mat4 local;
    glm::mat4 model;
  };

  glm::mat4 get_local(Param_Decl param, Asset const& asset,
                      Render_Params const& cur_render)
  {
    // If the semantic mentions a particular node, use its model, otherwise use
    // the model from the render params
    if(param.node)
      return local_transformation(asset.nodes[param.node.value()]);

    return cur_render.local;
  }
  glm::mat4 get_model(Param_Decl param, Asset const& asset,
                      Render_Params const& cur_render)
  {
    // If the semantic mentions a particular node, use its model, otherwise use
    // the model from the render params
    if(param.node)
      return model_transformation(asset.nodes, param.node.value());

    return cur_render.model;
  }
  glm::mat4 get_view(gfx::Camera const& cam)
  {
    return camera_view_matrix(cam);
  }
  glm::mat4 get_proj(gfx::Camera const& cam)
  {
    return camera_proj_matrix(cam);
  }

  // This should really be two functions one to retrieve / calculate the value
  // of the semantic and one to set it but it's hard to do this efficiently
  // because of the copying between matrices / arrays.
  void set_semantic_value(IShader& shader, Param_Decl param,
                          Asset const& asset,
                          Render_Params const& cur_render,
                          gfx::Camera const& cam)
  {
    // We must be dealing with semantic *parameters*
    REDC_ASSERT(static_cast<bool>(param.semantic) == true);

    Param_Semantic semantic =
      boost::get<Param_Semantic>(param.semantic.value());

    Param_Bind bind = boost::get<Param_Bind>(param.bind);

    switch(semantic)
    {
    case Param_Semantic::Local:
    {
      REDC_ASSERT(param.type == Value_Type::Mat4);
      glm::mat4 model = get_local(param, asset, cur_render);
      shader.set_mat4(bind, model);
      break;
    }
    case Param_Semantic::Model:
    {
      REDC_ASSERT(param.type == Value_Type::Mat4);
      glm::mat4 model = get_model(param, asset, cur_render);
      shader.set_mat4(bind, model);
      break;
    }
    case Param_Semantic::View:
    {
      REDC_ASSERT(param.type == Value_Type::Mat4);
      glm::mat4 view = get_view(cam);
      shader.set_mat4(bind, view);
      break;
    }
    case Param_Semantic::Projection:
    {
      REDC_ASSERT(param.type == Value_Type::Mat4);
      glm::mat4 proj = camera_proj_matrix(cam);
      shader.set_mat4(bind, proj);
      break;
    }
    case Param_Semantic::Model_View:
    {
      REDC_ASSERT(param.type == Value_Type::Mat4);
      glm::mat4 model = get_model(param, asset, cur_render);
      glm::mat4 view = get_view(cam);
      glm::mat4 view_model = view * model;
      shader.set_mat4(bind, view_model);
      break;
    }
    case Param_Semantic::Model_View_Projection:
    {
      REDC_ASSERT(param.type == Value_Type::Mat4);
      glm::mat4 model = get_model(param, asset, cur_render);
      glm::mat4 view = get_view(cam);
      glm::mat4 proj = get_proj(cam);
      glm::mat4 mat = proj * view * model;
      shader.set_mat4(bind, mat);
      break;
    }
    case Param_Semantic::Model_Inverse:
    {
      glm::mat4 model = get_model(param, asset, cur_render);
      glm::mat4 mat = glm::inverse(model);
      shader.set_mat4(bind, mat);
      break;
    }
    case Param_Semantic::View_Inverse:
    {
      glm::mat4 view = get_view(cam);
      glm::mat4 mat = glm::inverse(view);
      shader.set_mat4(bind, mat);
      break;
    }
    case Param_Semantic::Projection_Inverse:
    {
      glm::mat4 proj = get_proj(cam);
      glm::mat4 mat = glm::inverse(proj);
      shader.set_mat4(bind, mat);
      break;
    }
    case Param_Semantic::Model_View_Inverse:
    {
      REDC_ASSERT(param.type == Value_Type::Mat4);
      glm::mat4 model = get_model(param, asset, cur_render);
      glm::mat4 view = get_view(cam);
      glm::mat4 view_model = glm::inverse(view * model);
      shader.set_mat4(bind, view_model);
      break;
    }
    case Param_Semantic::Model_View_Projection_Inverse:
    {
      REDC_ASSERT(param.type == Value_Type::Mat4);
      glm::mat4 model = get_model(param, asset, cur_render);
      glm::mat4 view = get_view(cam);
      glm::mat4 proj = get_proj(cam);
      glm::mat4 mat = glm::inverse(proj * view * model);
      shader.set_mat4(bind, mat);
      break;
    }
    case Param_Semantic::Model_Inverse_Transpose:
    {
      glm::mat4 model = get_model(param, asset, cur_render);
      glm::mat4 mat = glm::transpose(glm::inverse(model));
      shader.set_mat4(bind, mat);
      break;
    }
    case Param_Semantic::Model_View_Inverse_Transpose:
    {
      REDC_ASSERT(param.type == Value_Type::Mat3);
      glm::mat4 model = get_model(param, asset, cur_render);
      glm::mat4 view = get_view(cam);
      glm::mat3 mat = glm::mat3(glm::transpose(glm::inverse(view * model)));
      shader.set_mat3(bind, mat);
      break;
    }
    default:
      REDC_UNREACHABLE_MSG("Rendering code doesn't support this param "
                           "semantic (%)", static_cast<unsigned int>(semantic));
      break;
    }
  }

  struct Rendering_State
  {
    Technique_Ref cur_technique_i = -1;
    Material_Ref cur_material_i = -1;

    std::vector<Param_Override> overrides;
  };

  void render_asset(Asset const& asset, Camera const& camera, IDriver& driver,
                    std::unique_ptr<Deferred_Shading>& deferred)
  {
    Rendering_State cur_rendering_state;

    // Figure out all the models of every mesh.
    // TODO: This can be cached!
    std::vector<Render_Params> render_params;

    std::size_t node_i = 0;
    for(auto node : asset.nodes)
    {
      glm::mat4 model = model_transformation(asset.nodes, node_i);
      glm::mat4 local_mat = local_transformation(asset.nodes[node_i]);

      // If there is a mesh associated with that node we need to render it
      for(std::size_t mesh_ref : node.meshes)
      {
        Mesh const& mesh = asset.meshes[mesh_ref];
        for(Primitive const& primitive : asset.meshes[mesh_ref].primitives)
        {
          Render_Params render;
          render.mesh = mesh.repr.get();
          render.primitive = &primitive;
          render.model = model;
          render.local = local_mat;
          render_params.push_back(render);
        }
      }

      ++node_i;
    }

    // Sort by technique first and material second
    std::sort(render_params.begin(), render_params.end(),
    [&](auto const& lhs, auto const& rhs)
    {
      Primitive const& lhprim = *lhs.primitive;
      Primitive const& rhprim = *rhs.primitive;

      Material const& lhmat = asset.materials[lhprim.mat_i];
      Material const& rhmat = asset.materials[rhprim.mat_i];

      // If the techniques are the same:
      if(lhmat.technique_i == rhmat.technique_i)
      {
        // Sort by material.
        return lhprim.mat_i < rhprim.mat_i;
      }
      // Otherwise sort by technique.

      // Forward rendering goes first
      Technique const& lhtec = asset.techniques[lhmat.technique_i];
      Technique const& rhtec = asset.techniques[rhmat.technique_i];

      // If the left hand side uses deferred rendering and the right hand side
      // uses forward rendering, the left side should go first.
      if(lhtec.is_deferred && !rhtec.is_deferred)
      {
        return true;
      }
      else if(!lhtec.is_deferred && rhtec.is_deferred)
      {
        // And vice versa
        return false;
      }
      else
      {
        // Just sort by technique, since both either use forward or deferred.
        return lhmat.technique_i < rhmat.technique_i;
      }
    });

    // Render each set of parameters!
    bool ran_deferred = false;
    for(auto render : render_params)
    {
      Primitive const& primitive = *render.primitive;

      Material const& mat = asset.materials[primitive.mat_i];
      Technique const& technique = asset.techniques[mat.technique_i];

      // This should be very efficient because of the above sort.
      if(cur_rendering_state.cur_material_i != primitive.mat_i)
      {
        // Load the material of the primitive.

        // Set this state for later.
        cur_rendering_state.cur_material_i = primitive.mat_i;

        if(cur_rendering_state.cur_technique_i != mat.technique_i)
        {
          // Load the technique of the material.

          if(technique.is_deferred)
          {
            if(!deferred)
            {
              deferred = std::make_unique<gfx::Deferred_Shading>(driver);

              gfx::Output_Interface oi;

              Attachment pos;
              pos.type = Attachment_Type::Color;
              pos.i = 0;

              Attachment normal;
              normal.type = Attachment_Type::Color;
              normal.i = 1;

              Attachment color;
              color.type = Attachment_Type::Color;
              color.i = 2;

              Attachment depth;
              depth.type = Attachment_Type::Depth_Stencil;
              depth.i = 0;

              oi.attachments.push_back(pos);
              oi.attachments.push_back(normal);
              oi.attachments.push_back(color);
              oi.attachments.push_back(depth);

              deferred->init(driver.window_extents(), oi);
            }

            if(!deferred->is_active())
            {
              deferred->use();
              ran_deferred = true;
            }
          }
          else
          {
            // We need to do forward rendering
            if(deferred && deferred->is_active())
            {
              // Disable and render
              deferred->finish();
            }

            // Continue doing forward rendering.
          }

          // Set this state for later.
          cur_rendering_state.cur_technique_i = mat.technique_i;

          // Use the shader program.
          driver.use_shader(*asset.programs[technique.program_i].repr);
        }

        // We have to set parameters each time because the technique may have
        // default values that are overrided on a per-material basis.
        Texture_Slot texture_slot = 0;
        for(auto param_pair : technique.parameters)
        {
          // We don't know how to do count
          REDC_ASSERT_MSG(param_pair.second.count == 1,
                          "technique parameter '%' must have count == 1",
                          param_pair.first);

          Param_Decl const& param = param_pair.second;

          Param_Bind bind = boost::get<Param_Bind>(param.bind);

          // If there isn't a semantic but it has a default value it will be
          // static so it won't need be set again as long as another technique
          // hasn't been used.
          if(!param.semantic && param.default_value)
          {
            // Set the value if it has one

            // WHO THE HELL WROTE THIS CODE? You have a driver and you are
            // passing it in, because you need the driver, yet we have to do it
            // this way due to how the driver handles texture binding. The
            // shader itself can handle most of this use case itself.
            set_parameter(driver, *driver.active_shader(), bind, param.type,
                          param.default_value.value(), texture_slot);
          }
        }

        // Override technique parameters with values in the material
        for(auto parameter_pair : mat.parameters)
        {
          set_parameter(driver, *driver.active_shader(), parameter_pair.first,
                        parameter_pair.second, texture_slot);
        }

        // Override material parameters with ones in the render state overrides.
        for(Param_Override param_override : cur_rendering_state.overrides)
        {
          // Find the declaration
          auto decl_find = technique.parameters.find(param_override.name);

          // If it's bad don't worry about it.
          if(decl_find == technique.parameters.end()) continue;

          // Use the declaration to get the type and bind point
          Param_Decl const& decl = decl_find->second;
          // Now set the parameter
          set_parameter(driver, *driver.active_shader(), decl.bind, decl.type,
                        param_override.value, texture_slot);
        }
      }


      // For each semantic technique parameter
      // Is it possible for these to override material specific values? And is
      // that a bug?
      for(auto param_pair : technique.parameters)
       {
        Param_Decl const& param = param_pair.second;

        // We can only do this for parameters that have a semantic
        if(!param.semantic) continue;

        // Retrieve / calculate and set semantic value.
        set_semantic_value(*driver.active_shader(), param, asset, render,
                           camera);
      }

      // Format the mesh vao for this set of primitives.
      // TODO: Cache buffer formatting somehow

      std::size_t min_elements = 0;
      for(auto attribute : primitive.attributes)
      {
        Attrib_Semantic semantic = attribute.first;
        Accessor const& accessor = asset.accessors[attribute.second];

        // May return a bad attribute
        Attrib_Bind bind = get_attrib_semantic_bind(technique, semantic);

        // If the bind was not found, forget about it.
        if(!is_good_attrib_bind(bind))
        {
          // We can't log because it will totally spam the console or filesystem
          // if the user has debug logging on. Leave this commented until we
          // find a way to only log it once.

          //log_d("Could not find bind for semantic '%'", to_string(semantic));
          continue;
        }

        REDC_ASSERT_MSG(accessor.buffer != nullptr,
                        "Buffer expected to be uploaded to the GPU");
        render.mesh->format_buffer(*accessor.buffer, bind, accessor.attrib_type,
                                   accessor.data_type, accessor.stride,
                                   accessor.offset);

        min_elements = std::min(min_elements, accessor.count);
      }
      if(primitive.indices)
      {
        Accessor const& indices = asset.accessors[primitive.indices.value()];

        REDC_ASSERT_MSG(indices.buffer != nullptr,
                        "Buffer expected to be uploaded to the GPU");

        render.mesh->use_element_buffer(*indices.buffer, indices.data_type);
        render.mesh->set_primitive_type(primitive.mode);

        std::size_t data_size = data_type_size(indices.data_type);

        unsigned int start = indices.offset / data_size;
        render.mesh->draw_elements(start, indices.count);
      }
      else
      {
        render.mesh->set_primitive_type(primitive.mode);
        render.mesh->draw_arrays(0, min_elements);
      }
    }

    if(ran_deferred)
    {
      std::vector<Transformed_Light> lights;
      for(Node_Ref node_i = 0; node_i < asset.nodes.size(); ++node_i)
      {
        Node const& node = asset.nodes[node_i];

        // Find light nodes and see if they are active.
        if(!node.lights.size()) continue;

        // Find light
        Light const& light = asset.lights[node.lights[0]];

        if(light.is_active)
        {
          Transformed_Light light_with_pos;
          light_with_pos.light = light;
          light_with_pos.model = model_transformation(asset.nodes, node_i);

          lights.push_back(light_with_pos);
        }
      }

      deferred->render(camera, lights.size(), &lights[0]);
    }
  }
} }
