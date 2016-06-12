/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */

#include "minigltf.h"
#include <cstdio>
#include "rapidjson/filereadstream.h"
#include "../common/log.h"
#include "rapidjson/error/en.h"
namespace redc
{
  std::vector<uint8_t> load_buf_bin(fs::path const& path, unsigned int size)
  {
    std::FILE* f = std::fopen(path.native().c_str(), "rb");
    if(!f)
    {
      log_e("Could not open bin file: '%'", path.native());
      return {};
    }

    std::vector<uint8_t> buf(size);
    auto nsz = std::fread(&buf[0], 1, size, f);

    buf.resize(nsz);
    return buf;
  }
  Buffer load_buffer(Asset const& a, std::string const& name,
                     rapidjson::Value& js)
  {
    Buffer b;

    b.name = name;

    // Load the path relative to the description filepath
    auto& uri_obj = js["uri"];
    auto uri = std::string(uri_obj.GetString(), uri_obj.GetStringLength());

    // We don't support data URIs.
    auto url = fs::path{uri};
    if(a.filepath)
    {
      url = a.filepath.value().parent_path() / url;
    }

    b.buf = load_buf_bin(url, js["byteLength"].GetUint());

    return b;
  }
  std::string get_js_string(rapidjson::Value const& name)
  {
    return {name.GetString(), name.GetStringLength()};
  }
  Asset load_gltf(rapidjson::Value& js, boost::optional<fs::path> path)
  {
    Asset a;
    a.filepath = path;

    auto& buffers = js["buffers"];
    for(auto iter = buffers.MemberBegin(); iter != buffers.MemberEnd(); ++iter)
    {
      auto name = get_js_string(iter->name);
      auto buf = load_buffer(a, name, iter->value);
      a.buffers.emplace(name, std::move(buf));
    }

    auto& buf_views = js["bufferViews"];
    for(auto iter = buf_views.MemberBegin(); iter != buf_views.MemberEnd();
        ++iter)
    {
      auto name = get_js_string(iter->name);
      Buffer_View buf_view;
      buf_view.name = name;

      auto buffer_name = get_js_string(iter->value["buffer"]);
      auto buffer_iter = a.buffers.find(buffer_name);
      if(buffer_iter == a.buffers.end())
      {
        // No buffer found, this buffer view is fucked
        log_e("% bufferView references bad buffer: '%'", name, buffer_name);
        continue;
      }

      auto offset = iter->value["byteOffset"].GetUint();

      buf_view.base_ptr = &buffer_iter->second.buf[0] + offset;
      buf_view.size = iter->value["byteLength"].GetUint();

      switch(iter->value["target"].GetUint())
      {
      case 34962:
        buf_view.target = Buf_View_Target::Array;
        break;
      case 34963:
        buf_view.target = Buf_View_Target::Element_Array;
        break;
      default:
        log_e("Invalid target for bufferView: '%'", name);
        continue;
      }

      a.buf_views.emplace(name, std::move(buf_view));
    }

    auto& accessor_arr = js["accessors"];
    for(auto iter = accessor_arr.MemberBegin();
        iter != accessor_arr.MemberEnd(); ++iter)
    {
      Accessor ac;

      auto name = get_js_string(iter->name);
      ac.name = name;

      auto buffer_name = get_js_string(iter->value["bufferView"]);
      auto buffer_iter = a.buf_views.find(buffer_name);
      if(buffer_iter == a.buf_views.end())
      {
        // No buffer found, this buffer view is fucked
        log_e("% accessor references bad bufferView: '%'", name, buffer_name);
        continue;
      }

      auto offset = iter->value["byteOffset"].GetUint();

      ac.base_ptr = buffer_iter->second.base_ptr + offset;

      ac.count = iter->value["count"].GetUint();
      ac.stride = iter->value["byteStride"].GetUint();

      switch(iter->value["componentType"].GetUint())
      {
      case 5120:
        ac.component_type = Component_Type::Byte;
        break;
      case 5121:
        ac.component_type = Component_Type::UByte;
        break;
      case 5122:
        ac.component_type = Component_Type::Short;
        break;
      case 5123:
        ac.component_type = Component_Type::UShort;
        break;
      case 5126:
        ac.component_type = Component_Type::Float;
        break;
      default:
        log_e("Invalid componentType for accessorType: '%'", name);
        continue;
      }

      const char* ty_str = iter->value["type"].GetString();
      if(std::strcmp(ty_str, "SCALAR") == 0)
        ac.attribute_type = Attribute_Type::Scalar;
      else if(std::strcmp(ty_str, "VEC2") == 0)
        ac.attribute_type = Attribute_Type::Vec2;
      else if(std::strcmp(ty_str, "VEC3") == 0)
        ac.attribute_type = Attribute_Type::Vec3;
      else if(std::strcmp(ty_str, "VEC4") == 0)
        ac.attribute_type = Attribute_Type::Vec4;
      else if(std::strcmp(ty_str, "MAT2") == 0)
        ac.attribute_type = Attribute_Type::Mat2;
      else if(std::strcmp(ty_str, "MAT3") == 0)
        ac.attribute_type = Attribute_Type::Mat3;
      else if(std::strcmp(ty_str, "MAT4") == 0)
        ac.attribute_type = Attribute_Type::Mat4;

      a.accessors.emplace(name, std::move(ac));
    }

    return a;
  }

  Asset load_gltf_file(std::string name)
  {
    auto file = std::fopen(name.c_str(), "rb");
    if(!file)
    {
      log_e("Failed to load scene file: '%'", name);
      return Asset{};
    }

    char buf[65536];
    rapidjson::FileReadStream file_stream(file, buf, sizeof(buf));

    rapidjson::Document d;
    d.ParseStream(file_stream);

    std::fclose(file);

    if(d.HasParseError())
    {
      log_e("Json error in scene file (offset: %): %",
            d.GetErrorOffset(), rapidjson::GetParseError_En(d.GetParseError()));
      return Asset{};
    }

    return load_gltf(d, fs::path{name});
  }
}
