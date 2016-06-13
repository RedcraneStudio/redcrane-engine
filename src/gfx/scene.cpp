/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "scene.h"
#include "../common/debugging.h"
#include <boost/variant/get.hpp>

#ifdef REDC_USE_OPENGL
#include "gl/shader_technique.hpp"
#endif

namespace redc
{

#ifdef REDC_USE_OPENGL
  std::vector<Buf> make_buffers(std::size_t num)
  {
    std::vector<Buf> bufs;
    bufs.resize(num);

    // We reference the buf member of the first element, assuming they are
    // tightly packed, etc.
    glGenBuffers(num, &bufs[0].buf);

    return bufs;
  }

  void upload_data(Buf buf, std::vector<uint8_t> const& data, std::size_t off,
                   std::size_t length, Buffer_Target target)
  {
    glBindBuffer((GLenum) target, buf.buf);
    glBufferData((GLenum) target, length, &data[0] + off, GL_STATIC_DRAW);
  }

  std::vector<Texture_Repr> make_textures(std::size_t num)
  {
    std::vector<Texture_Repr> texs;
    texs.resize(num);
    glGenTextures(num, &texs[0].tex);
    return texs;
  }

  void upload_image(Texture_Repr tex, std::vector<uint8_t> const& data,
                    Texture_Target target, Texture_Format format,
                    std::size_t width, std::size_t height, Data_Type type)
  {
    glBindTexture((GLenum) target, tex.tex);
    glTexImage2D((GLenum) target, 0, (GLenum) format, width, height, 0,
                 (GLenum) format, (GLenum) type, &data[0]);
  }

  void set_sampler(Texture_Repr tex, Texture_Target target)
  {
    // Can we avoid rebinding?
    glBindTexture((GLenum) target, tex.tex);
    glTextureParameterf((GLenum) target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameterf((GLenum) target, GL_TEXTURE_MIN_FILTER,
                        GL_NEAREST_MIPMAP_LINEAR);

    glTextureParameterf((GLenum) target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameterf((GLenum) target, GL_TEXTURE_WRAP_T, GL_REPEAT);

  }

  std::vector<Mesh_Repr> make_mesh_reprs(std::size_t num)
  {
    std::vector<Mesh_Repr> meshs;
    meshs.resize(num);
    glGenVertexArrays(num, &meshs[0].vao);
    return meshs;
  }

#endif

  Data_Type to_data_type(int val)
  {
    switch(val)
    {
    case TINYGLTF_COMPONENT_TYPE_BYTE:
      return Data_Type::Byte;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
      return Data_Type::UByte;
    case TINYGLTF_COMPONENT_TYPE_SHORT:
      return Data_Type::Short;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
      return Data_Type::UShort;
    case TINYGLTF_COMPONENT_TYPE_INT:
      return Data_Type::Int;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
      return Data_Type::UInt;
    case TINYGLTF_COMPONENT_TYPE_FLOAT:
      return Data_Type::Float;
    case TINYGLTF_COMPONENT_TYPE_DOUBLE:
      return Data_Type::Double;
    default:
      REDC_ASSERT_MSG(false, "Unknown component type");
      // This should never be reached.
      return Data_Type::Float;
    }
  }
  Buffer_Target to_buffer_target(int val)
  {
    switch(val)
    {
    case TINYGLTF_TARGET_ARRAY_BUFFER:
      return Buffer_Target::Array;
    case TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER:
      return Buffer_Target::Element_Array;
    default:
      REDC_ASSERT_MSG(false, "Unknown buffer target");
      // This should never be reached.
      return Buffer_Target::Array;
    }
  }

  void load_buffers(tinygltf::Scene const& scene, std::vector<Buf>& bufs,
                    std::vector<std::string>& buf_view_names)
  {

    // Upload all buffer views as GPU buffers
    bufs = make_buffers(scene.bufferViews.size());
    buf_view_names.resize(bufs.size());

    std::size_t i = 0;
    for(auto pair : scene.bufferViews)
    {
      auto& buf_view = pair.second;

      // Push the name for later cross-referencing, the index of the name in
      // this vector is the same index of the buffer in the scene.
      buf_view_names.push_back(pair.first);

      // Look up the buffer source
      auto buf = scene.buffers.find(buf_view.buffer);
      REDC_ASSERT(buf != scene.buffers.end());

      // Find buffer target
      Buffer_Target buf_target = to_buffer_target(buf_view.target);

      // Upload data to a gpu buffer.
      upload_data(bufs[i], buf->second.data, buf_view.byteOffset,
                  buf_view.byteLength, buf_target);

      ++i;
    }
  }

  Attrib_Type to_attrib_type(int val)
  {
    switch(val)
    {
    case TINYGLTF_TYPE_SCALAR:
      return Attrib_Type::Scalar;
    case TINYGLTF_TYPE_VEC2:
      return Attrib_Type::Vec2;
    case TINYGLTF_TYPE_VEC3:
      return Attrib_Type::Vec3;
    case TINYGLTF_TYPE_VEC4:
      return Attrib_Type::Vec4;
    case TINYGLTF_TYPE_MAT2:
      return Attrib_Type::Mat2;
    case TINYGLTF_TYPE_MAT3:
      return Attrib_Type::Mat3;
    case TINYGLTF_TYPE_MAT4:
      return Attrib_Type::Mat4;
    default:
      REDC_ASSERT_MSG(false, "Unknown attribute type");
      // This should never be reached
      return Attrib_Type::Vec4;
    }
  }

  template <class... Args>
  std::size_t find_string_index(std::vector<std::string> strs,
                                std::string str,
                                Args&&... msg_args)
  {
    auto find_res = std::find(strs.begin(), strs.end(), str);
    REDC_ASSERT_MSG(find_res != strs.end(), std::forward<Args>(msg_args)...);
    return find_res - strs.begin();
  }


  void load_accessors(tinygltf::Scene const& scene,
                      std::vector<Accessor>& accessors,
                      std::vector<std::string>& accessor_names,
                      std::vector<std::string> const& buf_names)
  {
    accessors.resize(scene.accessors.size());
    accessor_names.resize(scene.accessors.size());

    std::size_t i = 0;
    for(auto pair : scene.accessors)
    {
      auto& in_accessor = pair.second;

      // Push the string name for cross-referencing
      accessor_names.push_back(pair.first);

      // Make our accessor
      Accessor acc;

      // Just copy these fields.
      acc.count = in_accessor.count;
      acc.offset = in_accessor.byteOffset;
      acc.stride = in_accessor.byteStride;

      // Find the buffer by name
      acc.buf_i = find_string_index(buf_names, in_accessor.bufferView,
                                    "Accessor references invalid bufferView");

      // Now do data and attribute type
      acc.data_type = to_data_type(in_accessor.componentType);
      acc.attrib_type = to_attrib_type(in_accessor.type);

      accessors.push_back(acc);

      ++i;
    }
  }

  Texture_Target to_texture_target(int val)
  {
    switch(val)
    {
    case TINYGLTF_TEXTURE_TARGET_TEXTURE2D:
      return Texture_Target::Tex_2D;
    default:
      REDC_ASSERT_MSG(false, "Invalid texture target");
      // This should never be reached.
      return Texture_Target::Tex_2D;
    }
  }

  Texture_Format to_texture_format(int val)
  {
    switch(val)
    {
    case TINYGLTF_TEXTURE_FORMAT_RGBA:
      return Texture_Format::Rgba;
    default:
      REDC_ASSERT_MSG(false, "Invalid texture format");
      // This should never be reached.
      return Texture_Format::Rgba;
    };
  }

  void load_textures(tinygltf::Scene const& scene,
                     std::vector<Texture_Repr>& textures,
                     std::vector<std::string>& texture_names)
  {
    textures = make_textures(textures.size());
    texture_names.resize(textures.size());

    std::size_t i = 0;
    for(auto tex_pair : scene.textures)
    {
      auto& in_tex = tex_pair.second;
      texture_names.push_back(tex_pair.first);

      // Find image by name
      auto image_find = scene.images.find(in_tex.source);
      REDC_ASSERT(image_find != scene.images.end());

      // Load as enums
      Texture_Target target = to_texture_target(in_tex.target);
      Texture_Format format = to_texture_format(in_tex.format);
      Data_Type   data_type = to_data_type(in_tex.type);

      // Upload the image data
      upload_image(textures[i], image_find->second.image, target, format,
                   image_find->second.width, image_find->second.height,
                   data_type);

      // tinygltfloader doesn't load in samplers so we'll just guess and
      // generate mipmaps anyway.
      set_sampler(textures[i], target);

      ++i;
    }
  }

  void load_materials(tinygltf::Scene const& scene,
                      std::vector<Material>& materials,
                      std::vector<std::string>& material_names,
                      std::vector<std::unique_ptr<Technique> > const& techs,
                      std::vector<Texture_Repr> const& textures,
                      std::vector<std::string> const& texture_names)
  {
    materials.resize(scene.materials.size());
    material_names.resize(scene.materials.size());

    for(auto mat_pair : scene.materials)
    {
      auto& in_mat = mat_pair.second;

      material_names.push_back(mat_pair.first);

      Material mat;

      // Look up / make techniques based on the name.
      // For now use technique 0.
      mat.technique_i = 0;

      auto& technique = *techs[mat.technique_i];

      // Now use the technique to find parameter bind points
      for(auto param_pair : in_mat.values)
      {
        // This is like the name of the uniform, or tag in our case.
        auto& name = param_pair.first;

        Parameter param;

        // A parameter either references a texture or its a vector of values
        if(!param_pair.second.string_value.empty())
        {
          // Reference the texture
          auto texture_i =
            find_string_index(texture_names, param_pair.second.string_value,
                              "Material references invalid texture");
          param.value = textures[texture_i];
        }
        else
        {
          // Load in the values
          std::vector<float> values;
          values.resize(param_pair.second.number_array.size());
          for(double d : param_pair.second.number_array)
          {
            // Do an explicit cast.
            values.push_back((float) d);
          }
          param.value = values;
        }

        // Find the bind point for this parameter.
        auto bind = technique.get_param_bind(name);
        mat.params.emplace_back(param, bind);
      }

      materials.push_back(mat);
    }
  }

  Render_Mode to_render_mode(int val)
  {
    switch(val)
    {
    case TINYGLTF_MODE_POINTS:
      return Render_Mode::Points;
    case TINYGLTF_MODE_LINE:
      return Render_Mode::Lines;
    case TINYGLTF_MODE_LINE_LOOP:
      return Render_Mode::Line_Loop;
    case TINYGLTF_MODE_TRIANGLES:
      return Render_Mode::Triangles;
    case TINYGLTF_MODE_TRIANGLE_STRIP:
      return Render_Mode::Triangle_Strip;
    case TINYGLTF_MODE_TRIANGLE_FAN:
      return Render_Mode::Triangle_Fan;
    default:
      REDC_ASSERT_MSG(false, "Invalid primitive type");
      // This should never be reached.
      return Render_Mode::Triangles;
    }
  }

  void load_meshes(tinygltf::Scene const& scene, std::vector<Mesh>& meshes,
                   std::vector<std::string> mesh_names,
                   std::vector<Mesh_Repr> const& mesh_reprs,
                   std::vector<std::string> const& accessor_names,
                   std::vector<Material> const& materials,
                   std::vector<std::string> const& mat_names,
                   std::vector<std::unique_ptr<Technique> > const& techs)
  {
    for(auto mesh_pair : scene.meshes)
    {
      mesh_names.push_back(mesh_pair.first);

      Mesh our_mesh;

      our_mesh.repr_i = 0;

      for(auto& in_prim : mesh_pair.second.primitives)
      {
        Primitive prim;

        prim.mode = to_render_mode(in_prim.mode);

        // Find the referenced material
        prim.mat_i =
          find_string_index(mat_names, in_prim.material,
                            "Primitive references invalid material name");

        // Figure out where to bind accessors
        for(auto accessor_pair : in_prim.attributes)
        {
          auto name = accessor_pair.first;
          auto accessor = accessor_pair.second;

          auto& tech = techs[materials[prim.mat_i].technique_i];
          auto attrib_bind = tech->get_attrib_bind(accessor);

          // Find the index / ref of the accessor
          auto access_ref =
            find_string_index(accessor_names, accessor,
                              "Primitive references invalid accessor");

          // We know have an accessor bound to some attribute point.
          prim.accessors.emplace_back(access_ref, attrib_bind);
        }

        // Indices attribute
        if(!in_prim.indices.empty())
        {
          // Use this accessor for the element arrayxy
          prim.indices =
            find_string_index(accessor_names, in_prim.indices,
                              "Primitive references invalid accessor");
        }

        our_mesh.primitives.push_back(prim);
      }
      meshes.push_back(std::move(our_mesh));
    }
  }

  Scene load_scene(tinygltf::Scene const& scene)
  {
    Scene ret;

    std::vector<std::string> buf_view_names;
    load_buffers(scene, ret.buffers, buf_view_names);

    std::vector<std::string> accessor_names;
    load_accessors(scene, ret.accessors, accessor_names, buf_view_names);

    std::vector<std::string> texture_names;
    load_textures(scene, ret.textures, texture_names);

    // Add one global diffuse technique
    ret.techniques.push_back(make_diffuse_technique());

    std::vector<std::string> material_names;
    load_materials(scene, ret.materials, material_names, ret.techniques,
                   ret.textures, texture_names);

    // Add a single "mesh representation" for everything. In the OpenGL case
    // this is just a VAO.
    ret.mesh_reprs = make_mesh_reprs(1);

    std::vector<std::string> mesh_names;
    load_meshes(scene, ret.meshes, mesh_names, ret.mesh_reprs, accessor_names,
                ret.materials, material_names, ret.techniques);

    return ret;
  }
}
