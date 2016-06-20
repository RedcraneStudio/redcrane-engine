/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "scene.h"
#include "../common/debugging.h"
#include <boost/variant/get.hpp>

namespace redc
{

#ifdef REDC_USE_OPENGL
  std::vector<Buf> make_buffers(std::size_t num)
  {
    std::vector<Buf> bufs;
    if(num > 0)
    {
      bufs.resize(num);

      // We reference the buf member of the first element, assuming they are
      // tightly packed, etc.
      glGenBuffers(num, &bufs[0].buf);
    }
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
    if(num > 0)
    {
      texs.resize(num);
      glGenTextures(num, &texs[0].tex);
    }
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
    if(num > 0)
    {
      meshs.resize(num);
      glGenVertexArrays(num, &meshs[0].vao);
    }
    return meshs;
  }

  Shader make_shader(Shader_Type type)
  {
    Shader ret;
    ret.type = type;
    ret.repr = {glCreateShader((GLenum) type)};
    return ret;
  }

  Program_Repr make_program()
  {
    Program_Repr ret;
    ret.program = glCreateProgram();
    return ret;
  }

  void upload_shader_source(Shader shade, char const * source,
                            int source_length)
  {
    glShaderSource(shade.repr.shader, 1, &source, &source_length);
  }
  bool compile_shader(Shader shader, std::vector<char>* info_log)
  {
    // Compile
    glCompileShader(shader.repr.shader);

    // Get info log
    if(info_log)
    {
      GLint length = 0;
      glGetShaderiv(shader.repr.shader, GL_INFO_LOG_LENGTH, &length);

      GLsizei actual_length = 0;
      info_log->resize(length);
      glGetShaderInfoLog(shader.repr.shader, length, &actual_length,
                         &(*info_log)[0]);
    }

    // Return compile status
    GLint val;
    glGetShaderiv(shader.repr.shader, GL_COMPILE_STATUS, &val);
    return val == GL_TRUE;
  }
  void attach_shader(Program_Repr program, Shader_Repr shader)
  {
    glAttachShader(program.program, shader.shader);
  }
  bool link_program(Program_Repr program, std::vector<char>* link_log)
  {
    // Link
    glLinkProgram(program.program);

    // Get info log
    if(link_log)
    {
      GLint length;
      glGetProgramiv(program.program, GL_INFO_LOG_LENGTH, &length);

      link_log->resize(length);
      glGetProgramInfoLog(program.program, length, NULL, &(*link_log)[0]);
    }

    // Return compile status
    GLint val;
    glGetProgramiv(program.program, GL_LINK_STATUS, &val);
    return val == GL_TRUE;
  }

  Attribute_Bind get_attrib_bind(Program_Repr program, std::string const& name)
  {
    Attribute_Bind ret;
    ret.loc = glGetAttribLocation(program.program, name.c_str());
    return ret;
  }
  Parameter_Bind get_param_bind(Program_Repr program, std::string const& name)
  {
    Parameter_Bind bind;
    bind.loc = glGetUniformLocation(program.program, name.c_str());
    return bind;
  }

  void destroy_shader(Shader_Repr shader)
  {
    glDeleteShader(shader.shader);
  }
  void destroy_program(Program_Repr program)
  {
    glDeleteProgram(program.program);
  }

  void destroy_bufs(std::size_t num, Buf* bufs)
  {
    if(num >= 1)
    {
      glDeleteBuffers(num, &bufs->buf);
    }
  }
  void destroy_textures(std::size_t num, Texture_Repr* textures)
  {
    if(num >= 1)
    {
      glDeleteTextures(num, &textures->tex);
    }
  }
  void destroy_meshes(std::size_t num, Mesh_Repr* reprs)
  {
    if(num >= 1)
    {
      glDeleteVertexArrays(num, &reprs->vao);
    }
  }

  void use_mesh(Mesh_Repr mesh)
  {
    glBindVertexArray(mesh.vao);
  }


  // Use this for rendering. It would be nice to pass in some OpenGL state
  // structure so we don't have to redundantly set this information if it has
  // already been set.
  void use_array_accessor(Attribute_Bind bind, Buf buf, Accessor const& acc)
  {
    // We have some data
    glBindBuffer(GL_ARRAY_BUFFER, buf.buf);

    // We can't be dealing with matrices of any kind
    REDC_ASSERT((int) acc.attrib_type & 0x10);

    // Bind the buffer to a given attribute, at this point we know exactly
    // what part of the buffer needs to be referenced.
    glVertexAttribPointer(bind.loc, (GLint) acc.attrib_type & 0x0f,
                          (GLenum) acc.data_type, GL_FALSE,
                          acc.stride, (void*) acc.offset);
  }
  void use_element_array_accessor(Buf buf, Accessor const& acc)
  {
    // Use this as our element array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.buf);
  }

#endif

  template <class Bind_T, unsigned int Which_N, class Semantic_T>
  Bind_T get_semantic_bind(
    std::unordered_map<std::string, Technique_Parameter> const& parameters,
    Semantic_T attrib_semantic)
  {
    auto param_find = std::find_if(parameters.begin(),
                                   parameters.end(),
    [&](auto const& tech_param)
    {
      if(tech_param.second.semantic == boost::none) return false;
      if(tech_param.second.semantic.value().which() != Which_N) return false;

      auto const& param_semantic =
              boost::get<Semantic_T>(tech_param.second.semantic.value());

      // If these are equal we found our parameter
      return param_semantic == attrib_semantic;
    });

    // We should have found a parameter
    REDC_ASSERT(param_find != parameters.end());

    // It should be an attribute bind
    REDC_ASSERT_MSG(param_find->second.bind.which() == Which_N,
                    "Parameter '%' must be referencing an attribute",
                    param_find->first);

    // Now get the bind
    return boost::get<Bind_T>(param_find->second.bind);
  }

  Attribute_Bind get_attrib_semantic_bind(Technique const& tech,
                                          Attrib_Semantic attrib_semantic)
  {
    return get_semantic_bind<Attribute_Bind, 1>(tech.parameters,
                                                attrib_semantic);
  }

  // TODO: Add template that accepts a type either Attribute_Bind or
  // Parameter_Bind and a number either 1 or zero and does the above search but
  // for either one. Move get_param_semantic_bind down here out of the Opengl
  // ifdef.

  Parameter_Bind get_param_semantic_bind(Technique const& tech,
                                         Param_Semantic param_semantic)
  {
    return get_semantic_bind<Parameter_Bind, 0>(tech.parameters,
                                                param_semantic);

  }

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
    buf_view_names.reserve(bufs.size());

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

  Asset::~Asset()
  {
    for(auto program : programs)
    {
      destroy_program(program.repr);
    }
    for(auto shader : shaders)
    {
      destroy_shader(shader.repr);
    }
    for(auto& mesh : meshes)
    {
      destroy_meshes(1, &mesh.repr);
    }
    destroy_bufs(buffers.size(), &buffers[0]);
    destroy_textures(textures.size(), &textures[0]);
  }

  void load_nodes_given_names(tinygltf::Scene const& scene,
                              std::vector<std::string> const& node_names,
                              std::vector<Node>& nodes,
                              std::vector<std::string> const& mesh_names)
  {
    // This maps names to indexes into the nodes array parameter.
    std::unordered_map<std::string, std::size_t> node_indices;
    for(std::string const& name : node_names)
    {
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
        auto mesh_find = std::find(mesh_names.begin(), mesh_names.end(),
                                   req_mesh);
        REDC_ASSERT_MSG(mesh_find != mesh_names.end(),
                        "Node references invalid mesh '%'", req_mesh);
        node.meshes.push_back(mesh_find - mesh_names.begin());
      }

      // Add this node to the main vector of nodes
      nodes.push_back(node);
      // Add its index for later cross-referencing.
      node_indices.insert({name, nodes.size()});
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
      Node& node = nodes[node_index_iter->second];

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
        nodes[child_index].parent = node_index_iter->second;
      }
    }
  }

  void load_accessors(tinygltf::Scene const& scene,
                      std::vector<Accessor>& accessors,
                      std::vector<std::string>& accessor_names,
                      std::vector<std::string> const& buf_names)
  {
    accessors.reserve(scene.accessors.size());
    accessor_names.reserve(scene.accessors.size());

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
                                    "-Accessor references invalid bufferView");

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
    texture_names.reserve(textures.size());

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

  Parameter load_parameter_value(tinygltf::Parameter const& param,
                                 std::vector<std::string> const& texture_names,
                                 std::string thing)
  {
    Parameter ret;

    // A parameter either references a texture or its a vector of values
    if(!param.string_value.empty())
    {
      // Reference the texture
      auto texture_i =
        find_string_index(texture_names, param.string_value,
                          thing + " references invalid texture");
      ret.value = texture_i;
    }
    else
    {
      // Load in the values
      std::vector<float> values;
      values.reserve(param.number_array.size());
      for(double d : param.number_array)
      {
        // Do an explicit cast.
        values.push_back((float) d);
      }
      ret.value = values;
    }

    return ret;
  }

  void load_materials(tinygltf::Scene const& scene,
                      std::vector<Material>& materials,
                      std::vector<std::string>& material_names,
                      std::vector<Technique> const& techs,
                      std::vector<std::string> const& tech_names,
                      std::vector<std::string> const& texture_names,
                      std::vector<Program> const& programs)
  {
    materials.reserve(scene.materials.size());
    material_names.reserve(scene.materials.size());

    for(auto mat_pair : scene.materials)
    {
      auto& in_mat = mat_pair.second;

      material_names.push_back(mat_pair.first);

      Material mat;

      // Look up based on the name.
      mat.technique_i = find_string_index(tech_names, in_mat.technique,
                        "Material references invalid semantic");

      auto& technique = techs[mat.technique_i];
      auto& program = programs[technique.program_i];

      // Now use the technique to find parameter bind points
      for(auto param_pair : in_mat.values)
      {
        // This is like the name of the uniform, or tag in our case.
        auto& name = param_pair.first;

        Parameter param = load_parameter_value(param_pair.second, texture_names,
                                               "Material");

        // Find the bind point for this parameter.
        auto bind = get_param_bind(program.repr, name);
        mat.params.emplace_back(bind, param);
      }

      materials.push_back(std::move(mat));
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
      else REDC_ASSERT_MSG(false, "Invalid attribute semantic");

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
      ret.index = std::stoi(str.substr(offset, str.size()));
      return ret;
    }
  }

  void load_meshes(tinygltf::Scene const& scene, std::vector<Mesh>& meshes,
                   std::vector<std::string>& mesh_names,
                   std::vector<std::string> const& accessor_names,
                   std::vector<std::string> const& mat_names)
  {
    meshes.reserve(scene.meshes.size());
    mesh_names.reserve(scene.meshes.size());

    for(auto mesh_pair : scene.meshes)
    {
      mesh_names.push_back(mesh_pair.first);

      Mesh our_mesh;

      // Make a representation for our mesh
      our_mesh.repr = make_mesh_reprs(1)[0];

      for(auto& in_prim : mesh_pair.second.primitives)
      {
        Primitive prim;

        prim.mode = to_render_mode(in_prim.mode);

        // Find the referenced material
        prim.mat_i =
          find_string_index(mat_names, in_prim.material,
                            "Primitive references invalid material name");

        // Add references to any accessors and their semantics.
        for(auto accessor_pair : in_prim.attributes)
        {
          auto name = accessor_pair.first;
          auto accessor = accessor_pair.second;

          // String => Semantic
          auto semantic = to_attrib_semantic(name);

          // Find the index / ref of the accessor
          auto access_ref =
            find_string_index(accessor_names, accessor,
                              "Primitive references invalid accessor");

          // We know have an accessor bound to some attribute point.
          prim.attributes.emplace(semantic, access_ref);
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

  void load_shaders(tinygltf::Scene const& scene,
                    std::vector<Shader>& shaders,
                    std::vector<std::string>& shader_names)
  {
    shaders.reserve(scene.shaders.size());
    shader_names.reserve(scene.shaders.size());

    for(auto shader_pair : scene.shaders)
    {
      tinygltf::Shader const& in_shader = shader_pair.second;

      Shader_Type shader_type;
      switch(in_shader.type)
      {
      case TINYGLTF_SHADER_TYPE_VERTEX_SHADER:
        shader_type = Shader_Type::Vertex;
        break;
      case TINYGLTF_SHADER_TYPE_FRAGMENT_SHADER:
        shader_type = Shader_Type::Fragment;
        break;
      }

      // Make the shader (representation)
      auto shader = make_shader(shader_type);

      // Upload source
      upload_shader_source(shader, (char*) &in_shader.source[0],
                           in_shader.source.size());

      // Now compile the shader and handle errors, if there are errors refuse to
      // add the shader, log and continue to the next shader
      std::vector<char> info_log;
      if(compile_shader(shader, &info_log))
      {
        if(info_log.size() > 0)
        {
          std::string log(info_log.begin(), info_log.end());
          log_i("Shader '%' compiled successfully with info log:\n%",
                shader_pair.first, log);
        }
        shader_names.push_back(shader_pair.first);
        shaders.push_back(shader);
      }
      else
      {
        std::string log(info_log.begin(), info_log.end());
        log_e("Failed to compile shader '%':\n%", shader_pair.first, log);
        continue;
      }
    }
  }

  void load_programs(tinygltf::Scene const& scene,
                     std::vector<Program>& programs,
                     std::vector<std::string>& program_names,
                     std::vector<Shader> const& shaders,
                     std::vector<std::string> const& shader_names)
  {
    programs.reserve(scene.programs.size());
    program_names.reserve(scene.programs.size());

    for(auto const& program_pair : scene.programs)
    {
      tinygltf::Program const& in_program = program_pair.second;

      // Make a program repr
      Program program;
      program.repr = make_program();

      // Find vertex shader
      auto vert_i = find_string_index(shader_names, in_program.vertexShader,
                    "Program references invalid vertex shader");
      // Find the shader repr and attach that to our program repr
      attach_shader(program.repr, shaders[vert_i].repr);

      auto frag_i = find_string_index(shader_names, in_program.fragmentShader,
                    "Program references invalid fragment shader");
      attach_shader(program.repr, shaders[frag_i].repr);

      std::vector<char> link_log;
      if(link_program(program.repr, &link_log))
      {
        if(link_log.size())
        {
          std::string log(link_log.begin(), link_log.end());
          log_i("Information while linking program '%':\n%",
                program_pair.first, log);
        }

        // Link attribute names to binds.
        for(auto attribute_name : in_program.attributes)
        {
          // Find the bind and add it to the program's list of attributes
          program.attributes.emplace(attribute_name,
                                get_attrib_bind(program.repr, attribute_name));
        }

        // Add this program to our list
        programs.push_back(program);
        program_names.push_back(program_pair.first);
      }
      else
      {
        std::string log(link_log.begin(), link_log.end());
        log_e("Failed to link program '%':\n%",
              program_pair.first, log);
      }
    }
  }

  Technique_Parameter_Type to_technique_parameter_type(int ty)
  {
    switch(ty)
    {
    case TINYGLTF_PARAMETER_TYPE_BYTE:
      return Technique_Parameter_Type::Byte;
    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
      return Technique_Parameter_Type::UByte;
    case TINYGLTF_PARAMETER_TYPE_SHORT:
      return Technique_Parameter_Type::Short;
    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
      return Technique_Parameter_Type::UShort;
    case TINYGLTF_PARAMETER_TYPE_INT:
      return Technique_Parameter_Type::Int;
    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
      return Technique_Parameter_Type::UInt;
    case TINYGLTF_PARAMETER_TYPE_FLOAT:
      return Technique_Parameter_Type::Float;
    case TINYGLTF_PARAMETER_TYPE_FLOAT_VEC2:
      return Technique_Parameter_Type::Vec2;
    case TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3:
      return Technique_Parameter_Type::Vec3;
    case TINYGLTF_PARAMETER_TYPE_FLOAT_VEC4:
      return Technique_Parameter_Type::Vec4;
    case TINYGLTF_PARAMETER_TYPE_INT_VEC2:
      return Technique_Parameter_Type::IVec2;
    case TINYGLTF_PARAMETER_TYPE_INT_VEC3:
      return Technique_Parameter_Type::IVec3;
    case TINYGLTF_PARAMETER_TYPE_INT_VEC4:
      return Technique_Parameter_Type::IVec4;
    case TINYGLTF_PARAMETER_TYPE_BOOL:
      return Technique_Parameter_Type::Bool;
    case TINYGLTF_PARAMETER_TYPE_BOOL_VEC2:
      return Technique_Parameter_Type::BVec2;
    case TINYGLTF_PARAMETER_TYPE_BOOL_VEC3:
      return Technique_Parameter_Type::BVec3;
    case TINYGLTF_PARAMETER_TYPE_BOOL_VEC4:
      return Technique_Parameter_Type::BVec4;
    case TINYGLTF_PARAMETER_TYPE_FLOAT_MAT2:
      return Technique_Parameter_Type::Mat2;
    case TINYGLTF_PARAMETER_TYPE_FLOAT_MAT3:
      return Technique_Parameter_Type::Mat3;
    case TINYGLTF_PARAMETER_TYPE_FLOAT_MAT4:
      return Technique_Parameter_Type::Mat4;
    case TINYGLTF_PARAMETER_TYPE_SAMPLER_2D:
      return Technique_Parameter_Type::Sampler2D;
    default:
      REDC_ASSERT_MSG(false, "Invalid technique parameter type");
      break;
    }
  }

  Param_Semantic to_param_semantic(std::string str)
  {
    if(str == "LOCAL") return Param_Semantic::Local;
    if(str == "MODEL") return Param_Semantic::Model;
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

    REDC_ASSERT_MSG(false, "Invalid parameter semantic");
  }

  void load_techniques(tinygltf::Scene const& scene,
                       std::vector<Technique>& techniques,
                       std::vector<std::string>& technique_names,
                       std::vector<Program> const& programs,
                       std::vector<std::string> const& program_names,
                       std::vector<std::string> const& texture_names,
                       std::vector<std::string> const& node_names)
  {
    techniques.reserve(scene.techniques.size());
    technique_names.reserve(scene.techniques.size());

    for(auto const& technique_pair : scene.techniques)
    {
      tinygltf::Technique const& in_technique = technique_pair.second;

      technique_names.push_back(technique_pair.first);

      Technique technique;

      // Find program of this technique
      auto program_i = find_string_index(program_names, in_technique.program,
                       "Technique references invalid program");
      technique.program_i = program_i;

      Program const& program = programs[program_i];

      std::unordered_map<std::string, Attribute_Bind> attributes;
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

      std::unordered_map<std::string, Parameter_Bind> uniforms;
      for(auto const& uniform_pair : in_technique.uniforms)
      {
        // Uniform identifier
        auto const& uniform_ident = uniform_pair.second;
        // Uniform name, this is the name in the GLSL source code
        auto const& uniform_name = uniform_pair.first;

        // Get the bind of the uniform
        auto bind = get_param_bind(program.repr, uniform_name);

        uniforms.emplace(uniform_ident, bind);
      }

      for(auto const& param_pair : in_technique.parameters)
      {
        tinygltf::TechniqueParameter const& in_param = param_pair.second;

        Technique_Parameter param;

        param.count = in_param.count;

        if(in_param.node.size())
        {
          auto node_ref = find_string_index(node_names, in_param.node,
                          "Technique parameter references invalid node");
          param.node = node_ref;
        }

        param.type = to_technique_parameter_type(in_param.type);

        if(in_param.semantic.size())
        {
          // Is this an attribute parameter?
          if(attributes.count(param_pair.first))
          {
            // Try to make an attribute semantic
            param.semantic = to_attrib_semantic(in_param.semantic);

            // We already know the attribute bind
            param.bind = attributes[param_pair.first];
          }
          else
          {
            // Try to make a parameter semantic
            param.semantic = to_param_semantic(in_param.semantic);

            // We already know the parameter bind
            param.bind = uniforms[param_pair.first];
          }
        }

        param.value = load_parameter_value(in_param.value, texture_names,
                                           "Technique");

        technique.parameters.emplace(param_pair.first, std::move(param));
      }

      techniques.push_back(std::move(technique));
    }
  }

  void load_node_names(tinygltf::Scene const& scene,
                       std::vector<std::string>& node_names)
  {
    for(auto node_pair : scene.nodes)
    {
      node_names.push_back(node_pair.first);
    }
  }

  Asset load_asset(tinygltf::Scene const& scene)
  {
    Asset ret;

    std::vector<std::string> buf_view_names;
    load_buffers(scene, ret.buffers, buf_view_names);

    std::vector<std::string> accessor_names;
    load_accessors(scene, ret.accessors, accessor_names, buf_view_names);

    std::vector<std::string> texture_names;
    load_textures(scene, ret.textures, texture_names);

    // Load shaders and programs

    std::vector<std::string> shader_names;
    load_shaders(scene, ret.shaders, shader_names);

    std::vector<std::string> program_names;
    load_programs(scene, ret.programs, program_names, ret.shaders,shader_names);

    std::vector<std::string> node_names;
    load_node_names(scene, node_names);

    std::vector<std::string> technique_names;
    load_techniques(scene, ret.techniques, technique_names, ret.programs,
                    program_names, texture_names, node_names);

    std::vector<std::string> material_names;
    load_materials(scene, ret.materials, material_names, ret.techniques,
                   technique_names, texture_names, ret.programs);

    std::vector<std::string> mesh_names;
    load_meshes(scene, ret.meshes, mesh_names, accessor_names, material_names);

    // Load nodes
    load_nodes_given_names(scene, node_names, ret.nodes, mesh_names);
    return ret;
  }
}
