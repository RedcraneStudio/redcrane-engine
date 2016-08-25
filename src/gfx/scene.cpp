/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "scene.h"
#include "../common/debugging.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <boost/variant/get.hpp>

namespace redc { namespace gfx
{

  // = Helper functions

  template <class Bind_T, class Decl_T, class Semantic_T>
  Bind_T get_semantic_bind(
    std::unordered_map<std::string, Decl_T> const& parameters,
    Semantic_T looking_for_semantic, Bind_T default_bind)
  {
    auto param_find = std::find_if(parameters.begin(),
                                   parameters.end(),
    [&](auto const& tech_param)
    {
      // This parameter doesn't have a semantic; skip.
      if(tech_param.second.semantic == boost::none) return false;

      Semantic_T this_semantic = tech_param.second.semantic.value();

      // If semantics are equal we found our parameter
      return this_semantic == looking_for_semantic;
    });

    if(param_find == parameters.end())
    {
      // If we didn't find it return the default / bad one.
      return default_bind;
    }

    // Now get the bind
    return param_find->second.bind;
  }

  Attrib_Bind get_attrib_semantic_bind(Technique const& tech,
                                       Attrib_Semantic attrib_semantic)
  {
    return get_semantic_bind<Attrib_Bind, Attrib_Decl>(
      tech.attributes, attrib_semantic, bad_attrib_bind()
    );
  }

  Param_Bind get_param_semantic_bind(Technique const& tech,
                                     Param_Semantic param_semantic)
  {
    return get_semantic_bind<Param_Bind, Param_Decl>(
      tech.parameters, param_semantic, bad_param_bind()
    );
  }

  template <class... Args>
  std::size_t find_string_index(std::vector<std::string> strs,
                                std::string str,
                                Args&&... msg_args)
  {
    // Search in reverse, so that newer things with identical names will be
    // chosen first.
    auto find_res = std::find(strs.rbegin(), strs.rend(), str);
    REDC_ASSERT_MSG(find_res != strs.rend(), std::forward<Args>(msg_args)...);
    return strs.size() - 1 - (find_res - strs.rbegin());
  }

  // = Conversion functions (from tinygltfloader to our enums).

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
      REDC_UNREACHABLE_MSG("Unknown component type");
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
      return Buffer_Target::CPU;
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
      REDC_UNREACHABLE_MSG("Unknown attribute type");
      // This should never be reached
      return Attrib_Type::Vec4;
    }
  }

  Texture_Target to_texture_target(int val)
  {
    switch(val)
    {
    case TINYGLTF_TEXTURE_TARGET_TEXTURE2D:
      return Texture_Target::Tex_2D;
    default:
      REDC_UNREACHABLE_MSG("Invalid texture target");
      // This should never be reached.
      return Texture_Target::Tex_2D;
    }
  }

  Texture_Format to_texture_format(int val)
  {
    switch(val)
    {
    case TINYGLTF_TEXTURE_FORMAT_ALPHA:
      return Texture_Format::Alpha;
    case TINYGLTF_TEXTURE_FORMAT_RGB:
      return Texture_Format::Rgb;
    case TINYGLTF_TEXTURE_FORMAT_RGBA:
      return Texture_Format::Rgba;
    case TINYGLTF_TEXTURE_FORMAT_SRGB:
      return Texture_Format::Srgb;
    case TINYGLTF_TEXTURE_FORMAT_SRGB_ALPHA:
      return Texture_Format::Srgb_Alpha;
    default:
      REDC_UNREACHABLE_MSG("Invalid / unsupported texture format");
      // This should never be reached.
      return Texture_Format::Rgba;
    };
  }

  Value to_param_value(tinygltf::Parameter const& param, Value_Type type,
                             Asset const& asset)
  {
    Value ret;

    switch(type)
    {
    case Value_Type::Byte:
      ret.byte = static_cast<int8_t>(param.number_array[0]);
      break;
    case Value_Type::UByte:
      ret.ubyte = static_cast<uint8_t>(param.number_array[0]);
      break;
    case Value_Type::Short:
      ret.shrt = static_cast<short>(param.number_array[0]);
      break;
    case Value_Type::UShort:
      ret.ushrt = static_cast<unsigned short>(param.number_array[0]);
      break;
    case Value_Type::UInt:
      ret.ints[0] = static_cast<int>(param.number_array[0]);
      break;
    case Value_Type::IVec4:
      ret.ints[3] = static_cast<int>(param.number_array[3]);
    case Value_Type::IVec3:
      ret.ints[2] = static_cast<int>(param.number_array[2]);
    case Value_Type::IVec2:
      ret.ints[1] = static_cast<int>(param.number_array[1]);
    case Value_Type::Int:
      ret.ints[0] = static_cast<int>(param.number_array[0]);
      break;
    case Value_Type::Vec4:
      ret.floats[3] = static_cast<float>(param.number_array[3]);
    case Value_Type::Vec3:
      ret.floats[2] = static_cast<float>(param.number_array[2]);
    case Value_Type::Vec2:
      ret.floats[1] = static_cast<float>(param.number_array[1]);
    case Value_Type::Float:
      ret.floats[0] = static_cast<float>(param.number_array[0]);
      break;
    case Value_Type::BVec4:
      ret.bools[3] = static_cast<bool>(param.number_array[3]);
    case Value_Type::BVec3:
      ret.bools[2] = static_cast<bool>(param.number_array[2]);
    case Value_Type::BVec2:
      ret.bools[1] = static_cast<bool>(param.number_array[1]);
    case Value_Type::Bool:
      ret.bools[0] = static_cast<bool>(param.number_array[0]);
      break;
    case Value_Type::Mat2:
    case Value_Type::Mat3:
    case Value_Type::Mat4:
      // Copy into our floats array
      std::copy(param.number_array.begin(), param.number_array.end(),
                ret.floats.begin());
      break;
    case Value_Type::Sampler2D:
      REDC_ASSERT(param.string_value.size() != 0);
      // Using the string, find a reference to that texture and store it in the
      // uint member.
      std::size_t tex_ref =
        find_string_index(asset.texture_names, param.string_value,
                          "Parameter value references invalid texture");
      ret.texture = asset.textures[tex_ref].get();
      break;
    }

    return ret;
  }

  Primitive_Type to_primitive_type(int val)
  {
    switch(val)
    {
    case TINYGLTF_MODE_POINTS:
      return Primitive_Type::Points;
    case TINYGLTF_MODE_LINE:
      return Primitive_Type::Line;
    case TINYGLTF_MODE_LINE_LOOP:
      return Primitive_Type::Line_Loop;
    case TINYGLTF_MODE_TRIANGLES:
      return Primitive_Type::Triangles;
    case TINYGLTF_MODE_TRIANGLE_STRIP:
      return Primitive_Type::Triangle_Strip;
    case TINYGLTF_MODE_TRIANGLE_FAN:
      return Primitive_Type::Triangle_Fan;
    default:
      REDC_UNREACHABLE_MSG("Invalid primitive type");
      // This should never be reached.
      return Primitive_Type::Triangles;
    }
  }

  Attrib_Semantic to_attrib_semantic(std::string str, bool recurse = false)
  {
    // Check for an underscore to separate semantic from number
    // ie: SAMPLER_2

    Attrib_Semantic ret;

    auto sep_find = std::find(str.begin(), str.end(), '_');
    if(sep_find == str.end() || recurse)
    {
      // No separator, try to load the string as-is.
      if(str == "POSITION") ret.kind = Attrib_Semantic::Position;
      else if(str == "NORMAL") ret.kind = Attrib_Semantic::Normal;
      else if(str == "TEXCOORD") ret.kind = Attrib_Semantic::Texcoord;
      else if(str == "COLOR") ret.kind = Attrib_Semantic::Color;
      else if(str == "JOINT") ret.kind = Attrib_Semantic::Joint;
      else if(str == "WEIGHT") ret.kind = Attrib_Semantic::Weight;
      else REDC_UNREACHABLE_MSG("Invalid attribute semantic");

      // Don't set this if it wasn't specified initially
      //ret.index = 0;

      return ret;
    }
    else
    {
      auto offset = sep_find - str.begin();

      // Recurse into this function just to parse the semantic string. This
      // can't recurse infinitely and I only did it this way because the type of
      // Attrib_Semantic.kind is an anonymous enum so we can't return it's value
      // directly. I suppose we could with some sort of decltype or something
      // but this is neater.
      auto ret = to_attrib_semantic(str.substr(0, offset), true);
      // Add one to the offset to account for the underscore.
      ret.index = std::stoi(str.substr(offset+1, str.size()));
      return ret;
    }
  }

  Param_Semantic to_param_semantic(std::string str)
  {
    if(str == "LOCAL") return Param_Semantic::Local;
    if(str == "MODEL") return Param_Semantic::Model;
    if(str == "VIEW") return Param_Semantic::View;
    if(str == "PROJECTION") return Param_Semantic::Projection;
    if(str == "MODELVIEW") return Param_Semantic::Model_View;
    if(str == "MODELVIEWPROJECTION")
      return Param_Semantic::Model_View_Projection;
    if(str == "MODELINVERSE") return Param_Semantic::Model_Inverse;
    if(str == "VIEWINVERSE") return Param_Semantic::View_Inverse;
    if(str == "PROJECTIONINVERSE") return Param_Semantic::Projection_Inverse;
    if(str == "MODELVIEWINVERSE") return Param_Semantic::Model_View_Inverse;
    if(str == "MODELVIEWPROJECTIONINVERSE")
      return Param_Semantic::Model_View_Projection_Inverse;
    if(str == "MODELINVERSETRANSPOSE")
      return Param_Semantic::Model_Inverse_Transpose;
    if(str == "MODELVIEWINVERSETRANSPOSE")
      return Param_Semantic::Model_View_Inverse_Transpose;
    if(str == "VIEWPORT") return Param_Semantic::Viewport;
    if(str == "JOINTMATRIX") return Param_Semantic::Joint_Matrix;

    REDC_UNREACHABLE_MSG("Invalid parameter semantic '%'", str);
  }

  Value_Type to_param_type(int ty)
  {
    switch(ty)
    {
    case TINYGLTF_PARAMETER_TYPE_BYTE:
      return Value_Type::Byte;
    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
      return Value_Type::UByte;
    case TINYGLTF_PARAMETER_TYPE_SHORT:
      return Value_Type::Short;
    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
      return Value_Type::UShort;
    case TINYGLTF_PARAMETER_TYPE_INT:
      return Value_Type::Int;
    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
      return Value_Type::UInt;
    case TINYGLTF_PARAMETER_TYPE_FLOAT:
      return Value_Type::Float;
    case TINYGLTF_PARAMETER_TYPE_FLOAT_VEC2:
      return Value_Type::Vec2;
    case TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3:
      return Value_Type::Vec3;
    case TINYGLTF_PARAMETER_TYPE_FLOAT_VEC4:
      return Value_Type::Vec4;
    case TINYGLTF_PARAMETER_TYPE_INT_VEC2:
      return Value_Type::IVec2;
    case TINYGLTF_PARAMETER_TYPE_INT_VEC3:
      return Value_Type::IVec3;
    case TINYGLTF_PARAMETER_TYPE_INT_VEC4:
      return Value_Type::IVec4;
    case TINYGLTF_PARAMETER_TYPE_BOOL:
      return Value_Type::Bool;
    case TINYGLTF_PARAMETER_TYPE_BOOL_VEC2:
      return Value_Type::BVec2;
    case TINYGLTF_PARAMETER_TYPE_BOOL_VEC3:
      return Value_Type::BVec3;
    case TINYGLTF_PARAMETER_TYPE_BOOL_VEC4:
      return Value_Type::BVec4;
    case TINYGLTF_PARAMETER_TYPE_FLOAT_MAT2:
      return Value_Type::Mat2;
    case TINYGLTF_PARAMETER_TYPE_FLOAT_MAT3:
      return Value_Type::Mat3;
    case TINYGLTF_PARAMETER_TYPE_FLOAT_MAT4:
      return Value_Type::Mat4;
    case TINYGLTF_PARAMETER_TYPE_SAMPLER_2D:
      return Value_Type::Sampler2D;
    default:
      REDC_UNREACHABLE_MSG("Invalid technique parameter type");
      break;
    }
  }

  Texture_Filter to_texture_filter(int f)
  {
    switch(f)
    {
    case TINYGLTF_TEXTURE_FILTER_NEAREST:
      return Texture_Filter::Nearest;
    case TINYGLTF_TEXTURE_FILTER_LINEAR:
      return Texture_Filter::Linear;
    case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
      return Texture_Filter::Nearest_Mipmap_Nearest;
    case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
      return Texture_Filter::Linear_Mipmap_Nearest;
    case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
      return Texture_Filter::Nearest_Mipmap_Linear;
    case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
      return Texture_Filter::Linear_Mipmap_Linear;
    default:
      REDC_UNREACHABLE_MSG("Invalid texture filter type");
      return Texture_Filter::Nearest;
    }
  }
  Texture_Wrap to_texture_wrap(int w)
  {
    switch(w)
    {
    case TINYGLTF_TEXTURE_WRAP_RPEAT:
      return Texture_Wrap::Repeat;
    case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
      return Texture_Wrap::Clamp_To_Edge;
    case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
      return Texture_Wrap::Mirrored_Repeat;
    default:
      REDC_UNREACHABLE_MSG("Invalid texture wrap type");
      return Texture_Wrap::Clamp_To_Edge;
    }
  }

  // = Enum -> String functions
  std::string to_string(Attrib_Semantic attrib)
  {
    std::string ret;
    switch(attrib.kind)
    {
    case Attrib_Semantic::Position:
      ret = "POSITION";
      break;
    case Attrib_Semantic::Normal:
      ret = "NORMAL";
      break;
    case Attrib_Semantic::Texcoord:
      ret = "TEXCOORD";
      break;
    case Attrib_Semantic::Color:
      ret = "COLOR";
      break;
    case Attrib_Semantic::Joint:
      ret = "JOINT";
      break;
    case Attrib_Semantic::Weight:
      ret = "WEIGHT";
      break;
    }

    if(attrib.index)
    {
      ret += "_";
      ret += std::to_string(attrib.index.value());
    }
    return ret;
  }

  Asset::~Asset() {}

  // = Load functions

  void load_buffers(IDriver& driver, Asset& asset, tinygltf::Scene const& scene)
  {
    // Add room for new buffers
    asset.buffers.reserve(asset.buffers.size() + scene.bufferViews.size());
    asset.buf_names.reserve(asset.buf_names.size() + scene.bufferViews.size());

    // We sort of compress the concept of buffers and buffer views.

    for(auto pair : scene.bufferViews)
    {
      tinygltf::BufferView const& buf_view = pair.second;

      // Push the name for later cross-referencing, the index of the name in
      // this vector is the same index of the buffer in the scene.
      asset.buf_names.push_back(pair.first);

      // Form our own buffer
      Buffer our_buf;
      // Figure our target - this will affect how we store the data later.
      our_buf.target = to_buffer_target(buf_view.target);

      // Look up the buffer this view references
      auto their_buf_iter = scene.buffers.find(buf_view.buffer);
      REDC_ASSERT(their_buf_iter != scene.buffers.end());

      tinygltf::Buffer const& their_buf = their_buf_iter->second;

      // Copy data - we do this no matter the target. If it's a GPU target
      // though we could save memory by forgetting about it.
      our_buf.data.resize(their_buf.data.size());
      std::memcpy(&our_buf.data[0], &their_buf.data[buf_view.byteOffset],
                  buf_view.byteLength);

      // Make a repr if the target requires.
      if(our_buf.target == Buffer_Target::Array ||
         our_buf.target == Buffer_Target::Element_Array)
      {
        std::unique_ptr<IBuffer> repr = driver.make_buffer_repr();
        // Upload our data to our GPU buffer now. No need to use the offset
        // because we already did when copying over to Buffer::data.
        repr->allocate(our_buf.target, our_buf.data.size(), &our_buf.data[0],
                       Usage_Hint::Draw, Upload_Hint::Static);

        our_buf.repr = std::move(repr);
      }
      else
      {
        our_buf.repr = nullptr;
      }

      asset.buffers.push_back(std::move(our_buf));
    }
  }

  void load_nodes_given_names(Asset& asset, std::size_t node_offset,
                              tinygltf::Scene const& scene)
  {
    asset.nodes.reserve(asset.nodes.size() + asset.node_names.size() -
                        node_offset);

    // This maps names to indexes into the nodes array parameter.
    std::unordered_map<std::string, std::size_t> node_indices;

    // Start at a given offset in the node names vector
    auto name_iter = asset.node_names.begin() + node_offset;
    auto name_iter_end = asset.node_names.end();
    for(; name_iter != name_iter_end; ++name_iter)
    {
      std::string const& name = *name_iter;

      auto node_find = scene.nodes.find(name);
      REDC_ASSERT(node_find != scene.nodes.end());
      auto const& in_node = node_find->second;

      Node node;

      // Load in local transformation
      if(in_node.rotation.size())
      {
        node.rotation = std::array<float, 4>{};
        std::copy(in_node.rotation.begin(), in_node.rotation.end(),
                  node.rotation->begin());
      }
      if(in_node.scale.size())
      {
        node.scale = std::array<float, 3>{};
        std::copy(in_node.scale.begin(), in_node.scale.end(),
                  node.scale->begin());
      }
      if(in_node.translation.size())
      {
        node.translation = std::array<float, 3>{};
        std::copy(in_node.translation.begin(),
                  in_node.translation.end(),
                  node.translation->begin());
      }
      if(in_node.matrix.size())
      {
        node.matrix = std::array<float, 16>{};
        std::copy(in_node.matrix.begin(), in_node.matrix.end(),
                  node.matrix->begin());
      }

      // Add any references to meshes
      for(auto req_mesh : in_node.meshes)
      {
        auto mesh_index = find_string_index(asset.mesh_names, req_mesh,
                                            "Node references invalid mesh '%'",
                                            req_mesh);
        node.meshes.push_back(mesh_index);
      }

      // Add this node to the main vector of nodes
      asset.nodes.push_back(node);
      // Add its index for later cross-referencing.
      node_indices.insert({name, asset.nodes.size() - 1});
    }

    // Now traverse the hiearchy.
    for(auto const& node_pair : scene.nodes)
    {
      // Tinygltfloader uses a flat list of nodes as well, well it doesn't use a
      // vector but there is no specific hierarchy. Here we are effectively
      // iterating over every node in the asset.

      // For this second pass, add references for each node to its parents and
      // children.
      std::string const& name = node_pair.first;

      // We recorded at what index we put each node in our own scene
      // representation by name, so look it up!
      auto const& node_index_iter = node_indices.find(name);
      REDC_ASSERT(node_index_iter != node_indices.end());

      // Now take that node
      Node& node = asset.nodes[node_index_iter->second];

      // 1. Add indices of our children to this node
      // 2. Add the index of this node to our children.
      for(std::string const& child_name : node_pair.second.children)
      {
        // Find the index of the child
        auto const& child_index_iter = node_indices.find(child_name);
        REDC_ASSERT(child_index_iter != node_indices.end());

        std::size_t child_index = child_index_iter->second;

        // That is one of our children.
        node.children.push_back(child_index);

        // And we are that node's parent.
        asset.nodes[child_index].parent = node_index_iter->second;
      }
    }
  }

  void load_accessors(Asset& asset, tinygltf::Scene const& scene)
  {
    asset.accessors.reserve(
      asset.accessors.size() + scene.accessors.size()
    );
    asset.accessor_names.reserve(
      asset.accessor_names.size() + scene.accessors.size()
    );

    std::size_t i = 0;
    for(auto pair : scene.accessors)
    {
      auto& in_accessor = pair.second;

      // Push the string name for cross-referencing
      asset.accessor_names.push_back(pair.first);

      // Make our accessor
      Accessor acc;

      // Just copy these fields.
      acc.count = in_accessor.count;
      acc.offset = in_accessor.byteOffset;
      acc.stride = in_accessor.byteStride;

      // Find the buffer by name
      std::size_t buf_i = find_string_index(
        asset.buf_names, in_accessor.bufferView,
        "Accessor references invalid bufferView"
      );
      acc.buffer = asset.buffers[buf_i].repr.get();

      // Now do data and attribute type
      acc.data_type = to_data_type(in_accessor.componentType);
      acc.attrib_type = to_attrib_type(in_accessor.type);

      asset.accessors.push_back(acc);

      ++i;
    }
  }

  void load_textures(IDriver& driver, Asset& asset, tinygltf::Scene const& scene)
  {
    // Starting index
    std::size_t i = asset.textures.size();
    // Add room for more textures.
    asset.textures.resize(i + scene.textures.size());

    driver.make_textures(scene.textures.size(), &asset.textures[i]);

    asset.texture_names.reserve(
      asset.texture_names.size() + scene.textures.size()
    );

    for(auto tex_pair : scene.textures)
    {
      auto& in_tex = tex_pair.second;
      asset.texture_names.push_back(tex_pair.first);

      // Find image by name
      auto image_find = scene.images.find(in_tex.source);
      REDC_ASSERT(image_find != scene.images.end());

      // Load as enums
      Texture_Target  target = to_texture_target(in_tex.target);
      Texture_Format dformat = to_texture_format(in_tex.format);
      Texture_Format iformat = to_texture_format(in_tex.internalFormat);
      Data_Type    data_type = to_data_type(in_tex.type);

      switch(image_find->second.component)
      {
      case 1:
        if(dformat != Texture_Format::Alpha)
        {
          log_w("Ignoring texture format because image has one component");
          dformat = Texture_Format::Alpha;
        }
        break;
      case 3:
        if(dformat != Texture_Format::Rgb && dformat != Texture_Format::Srgb)
          log_w("Ignoring texture format because image has three components");

        // Don't lose the fact that we are using srgb
        if(dformat == Texture_Format::Srgb_Alpha)
          dformat = Texture_Format::Srgb;
        else
          dformat = Texture_Format::Rgb;

        break;
      case 4:
        if(dformat != Texture_Format::Rgba &&
           dformat != Texture_Format::Srgb_Alpha)
          log_w("Ignoring texture format because image has four components");

        if(dformat == Texture_Format::Srgb)
          dformat = Texture_Format::Srgb_Alpha;
        else
          dformat = Texture_Format::Rgba;

        break;
      default:
        REDC_UNREACHABLE_MSG("Unsupported number of image components");
        break;
      }

      ITexture* tex = asset.textures[i].get();

      Vec<std::size_t> image_size(image_find->second.width,
                                  image_find->second.height);
      // Although we pass target from the glTF, we only really support 2D
      // textures. Cube maps have a whole different blitting process.
      tex->allocate(image_size, iformat, target);

      Volume<std::size_t> blit_size;
      blit_size.pos = Vec<std::size_t>();
      blit_size.width = image_size.x;
      blit_size.height = image_size.y;

      tex->blit_tex2d_data(blit_size, dformat, data_type,
                           &image_find->second.image[0]);

      // Find the sampler
      auto sampler_find = scene.samplers.find(in_tex.sampler);
      REDC_ASSERT(sampler_find != scene.samplers.end());

      tinygltf::Sampler const& sampler = sampler_find->second;
      tex->set_min_filter(to_texture_filter(sampler.minFilter));
      tex->set_mag_filter(to_texture_filter(sampler.magFilter));

      tex->set_wrap_s(to_texture_wrap(sampler.wrapS));
      tex->set_wrap_t(to_texture_wrap(sampler.wrapT));

      ++i;
    }
  }

  void load_materials(Asset& asset, tinygltf::Scene const& scene)
  {
    asset.materials.reserve(asset.materials.size() + scene.materials.size());

    asset.material_names.reserve(
      asset.material_names.size() + scene.materials.size()
    );

    for(auto mat_pair : scene.materials)
    {
      auto& in_mat = mat_pair.second;

      asset.material_names.push_back(mat_pair.first);

      Material mat;

      // Look up based on the name.
      mat.technique_i =
        find_string_index(asset.technique_names, in_mat.technique,
                          "Material references invalid technique '%'",
                          in_mat.technique);

      auto& technique = asset.techniques[mat.technique_i];

      // Now use the technique to find parameter bind points
      for(auto param_pair : in_mat.values)
      {
        // This the name of the technique parameter.
        auto& name = param_pair.first;

        // Find the type of that parameter using the string name
        auto param_decl_find = technique.parameters.find(name);
        REDC_ASSERT(param_decl_find != technique.parameters.end());

        // Get the value of this parameter
        Typed_Value param;
        param.type = param_decl_find->second.type;
        param.value = to_param_value(param_pair.second, param.type, asset);

        // Get the technique parameter from the name.
        auto tech_param_find = technique.parameters.find(name);
        REDC_ASSERT(tech_param_find != technique.parameters.end());

        // Add the parameter bind point with it's corrosponding value for this
        // material.
        mat.parameters.emplace_back(tech_param_find->second.bind, param);
      }

      asset.materials.push_back(std::move(mat));
    }
  }

  void load_meshes(IDriver& driver, Asset& asset, tinygltf::Scene const& scene)
  {
    asset.meshes.reserve(asset.meshes.size() + scene.meshes.size());
    asset.mesh_names.reserve(asset.mesh_names.size() + scene.meshes.size());

    for(auto mesh_pair : scene.meshes)
    {
      asset.mesh_names.push_back(mesh_pair.first);

      Mesh our_mesh;

      // Make a representation
      our_mesh.repr = driver.make_mesh_repr();

      Attrib_Bind max_bind = 0;
      for(auto& in_prim : mesh_pair.second.primitives)
      {
        Primitive prim;

        prim.mode = to_primitive_type(in_prim.mode);

        // Find the referenced material
        prim.mat_i =
          find_string_index(asset.material_names, in_prim.material,
                            "Primitive of mesh '%' references invalid material"
                            " name '%'", mesh_pair.first, in_prim.material);

        // Add references to any accessors and their semantics.
        for(auto accessor_pair : in_prim.attributes)
        {
          auto name = accessor_pair.first;
          auto accessor = accessor_pair.second;

          // String => Semantic
          auto semantic = to_attrib_semantic(name);

          // Find the index / ref of the accessor
          auto access_ref =
            find_string_index(asset.accessor_names, accessor,
                              "Primitive references invalid accessor");

          // We know have an accessor bound to some attribute point.
          prim.attributes.emplace(semantic, access_ref);
        }

        // Enable enough vertex attributes in the mesh repr to hold every
        // attribute in the primitive. With this implementation we are assuming
        // that they are contiguous and start at 0.

        // Make the size of attributes signed ;)
        max_bind = std::max(
          max_bind, static_cast<Attrib_Bind>(in_prim.attributes.size())
        );

        // Indices attribute
        if(!in_prim.indices.empty())
        {
          // Use this accessor for the element arrayxy
          prim.indices =
            find_string_index(asset.accessor_names, in_prim.indices,
                              "Primitive references invalid accessor");
        }

        our_mesh.primitives.push_back(prim);
      }
      for(Attrib_Bind bind = 0; bind < max_bind; ++bind)
      {
        our_mesh.repr->enable_attrib_bind(bind);
      }
      asset.meshes.push_back(std::move(our_mesh));
    }
  }
  void load_programs(IDriver& driver, Asset& asset, tinygltf::Scene const& scene)
  {
    asset.programs.reserve(asset.programs.size() + scene.programs.size());
    asset.program_names.reserve(
      asset.program_names.size() + scene.programs.size()
    );

    for(auto const& program_pair : scene.programs)
    {
      tinygltf::Program const& in_program = program_pair.second;

      // Make a shader repr
      Program program;
      program.repr = driver.make_shader_repr();

      try
      {
        // Find shaders and compile sources, etc.
        tinygltf::Shader const& vert =
          scene.shaders.at(in_program.vertexShader);

        program.repr->load_vertex_part(
          std::vector<char>(vert.source.begin(), vert.source.end()),
          in_program.vertexShader
        );

        tinygltf::Shader const& frag =
          scene.shaders.at(in_program.fragmentShader);

        program.repr->load_fragment_part(
          std::vector<char>(frag.source.begin(), frag.source.end()),
          in_program.fragmentShader
        );
      }
      catch(std::out_of_range& e)
      {
        log_e("Ignoring program that references invalid shaders '%' or '%'",
              in_program.vertexShader, in_program.fragmentShader);
        continue;
      }
      // Link
      program.repr->link();
      if(!program.repr->linked())
      {
        // Failed to link
        log_e("Failed to link program '%'", program_pair.first);
        continue;
      }

      // Link attribute names to binds.
      for(auto attribute_name : in_program.attributes)
      {
        // Find the bind and add it to the program's list of attributes
        program.attributes.emplace(
          attribute_name, program.repr->get_attrib_bind(attribute_name)
        );
      }

      // Add this program to our list
      asset.programs.push_back(std::move(program));
      asset.program_names.push_back(program_pair.first);
    }
  }

  void load_techniques(Asset& asset, tinygltf::Scene const& scene)
  {
    asset.techniques.reserve(asset.techniques.size() + scene.techniques.size());
    asset.technique_names.reserve(
      asset.technique_names.size() + scene.techniques.size()
    );

    for(auto const& technique_pair : scene.techniques)
    {
      tinygltf::Technique const& in_technique = technique_pair.second;

      asset.technique_names.push_back(technique_pair.first);

      Technique technique;

      auto is_deferred_find = in_technique.extras.find("is_deferred");
      if(is_deferred_find != in_technique.extras.end() &&
        is_deferred_find->second.is<bool>())
      {
        technique.is_deferred = is_deferred_find->second.get<bool>();
      }
      else
      {
        technique.is_deferred = false;
      }

      // Find program of this technique
      auto program_i = find_string_index(asset.program_names,
                                         in_technique.program,
                       "Technique references invalid program");
      technique.program_i = program_i;

      Program const& program = asset.programs[program_i];

      std::unordered_map<std::string, Attrib_Bind> attributes;
      for(auto const& attrib_pair : in_technique.attributes)
      {
        // We already have the bind in the program struct, we just need to find
        // it given the string, which is the key (first element) of this
        // dictionary. The value (second element) is what we are interested in,
        // so it will be the key of this new attributes map.

        auto find_bind = program.attributes.find(attrib_pair.first);
        REDC_ASSERT_MSG(find_bind != program.attributes.end(),
                        "Technique references invalid attribute");
        attributes.emplace(attrib_pair.second, find_bind->second);
      }

      std::unordered_map<std::string, Param_Bind> uniforms;
      for(auto const& uniform_pair : in_technique.uniforms)
      {
        // Uniform identifier
        auto const& uniform_ident = uniform_pair.second;
        // Uniform name, this is the name in the GLSL source code
        auto const& uniform_name = uniform_pair.first;

        // Get the bind of the uniform
        auto bind = program.repr->get_param_bind(uniform_name);

        uniforms.emplace(uniform_ident, bind);
      }

      for(auto const& param_pair : in_technique.parameters)
      {
        tinygltf::TechniqueParameter const& in_param = param_pair.second;

        // Is this parameter an attribute?
        if(attributes.count(param_pair.first))
        {
          // This parameter is an attribute
          Attrib_Decl param;

          param.type = to_param_type(in_param.type);

          // Does it have a semantic?
          if(in_param.semantic.size())
          {
            // Select the overload using the type
            param.semantic = to_attrib_semantic(in_param.semantic);
          }

          // Find the bind point
          param.bind = attributes[param_pair.first];

          technique.attributes.emplace(param_pair.first, param);
        }
        else
        {
          // This is a (uniform) parameter
          Param_Decl param;

          param.count = in_param.count;

          if(in_param.node.size())
          {
            auto node_ref = find_string_index(asset.node_names, in_param.node,
                            "Technique parameter references invalid node");
            param.node = node_ref;
          }

          param.type = to_param_type(in_param.type);

          // Does it have a semantic?
          if(in_param.semantic.size())
          {
            // Select the overload using the type
            param.semantic = to_param_semantic(in_param.semantic);
          }

          // Find the bind using the map
          param.bind = uniforms[param_pair.first];

          if(in_param.value.string_value.size() == 0 &&
             in_param.value.number_array.size() == 0)
          {
            param.default_value = boost::none;
          }
          else
          {
            param.default_value = to_param_value(in_param.value,param.type,asset);
          }

          technique.parameters.emplace(param_pair.first, param);
        }
      }

      asset.techniques.push_back(std::move(technique));
    }
  }

  std::size_t load_node_names(Asset& asset, tinygltf::Scene const& scene)
  {
    std::size_t initial_offset = asset.node_names.size();
    for(auto node_pair : scene.nodes)
    {
      asset.node_names.push_back(node_pair.first);
    }
    return initial_offset;
  }

  Asset load_asset(IDriver& driver, tinygltf::Scene const& scene)
  {
    Asset ret;
    append_to_asset(driver, ret, scene);
    return std::move(ret);
  }
  void append_to_asset(IDriver& driver, Asset& ret, tinygltf::Scene const& scene)
  {
    load_buffers(driver, ret, scene);

    load_accessors(ret, scene);

    load_textures(driver, ret, scene);

    load_programs(driver, ret, scene);

    std::size_t node_off = load_node_names(ret, scene);

    load_techniques(ret, scene);

    load_materials(ret, scene);

    load_meshes(driver, ret, scene);

    // Load nodes
    load_nodes_given_names(ret, node_off, scene);
  }

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

      cur_model = cur_model * local_transformation(node);
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

  void render_asset(Asset const& asset, gfx::Camera const& camera,
                    Rendering_State& cur_rendering_state)
  {
    IDriver& driver = *cur_rendering_state.driver;
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
          render_params.push_back(std::move(render));
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
            if(!cur_rendering_state.deferred)
            {
              cur_rendering_state.deferred =
                std::make_unique<gfx::Deferred_Shading>(
                  *cur_rendering_state.driver
                  );

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

              gfx::IDriver& driver = *cur_rendering_state.driver;
              cur_rendering_state.deferred->init(driver.window_extents(), oi);
            }

            if(!cur_rendering_state.deferred->is_active())
            {
              cur_rendering_state.deferred->use();
            }
          }
          else
          {
            // We need to do forward rendering
            if(cur_rendering_state.deferred &&
               cur_rendering_state.deferred->is_active())
            {
              // Disable and render
              cur_rendering_state.deferred->finish();
              ran_deferred = true;
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
      gfx::Light light;
      light.pos = glm::vec3(0.0f, 0.0f, 0.0f);
      light.power = 1.0f;
      light.diffuse_color = glm::vec3(1.0f, 1.0f, 1.0f);
      light.specular_color = glm::vec3(1.0f, 1.0f, 1.0f);

      cur_rendering_state.deferred->render(camera, std::vector<gfx::Light>{light});
    }

  }
} }
