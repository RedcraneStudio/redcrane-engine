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
  Buffer load_buffer(Desc const& d, std::string const& name,
                     rapidjson::Value& js)
  {
    Buffer b;

    b.name = name;

    // Load the path relative to the description filepath
    auto& uri_obj = js["uri"];
    auto uri = std::string(uri_obj.GetString(), uri_obj.GetStringLength());

    // We don't support data URIs.
    auto url = fs::path{uri};
    if(d.filepath)
    {
      url = d.filepath.value().parent_path() / url;
    }

    b.buf = load_buf_bin(url, js["byteLength"].GetUint());

    return b;
  }
  std::string get_js_string(rapidjson::Value const& name)
  {
    return {name.GetString(), name.GetStringLength()};
  }
  Desc load_desc(rapidjson::Value& js, boost::optional<fs::path> path)
  {
    Desc d;
    d.filepath = path;

    auto& buffers = js["buffers"];
    for(auto iter = buffers.MemberBegin(); iter != buffers.MemberEnd(); ++iter)
    {
      auto name = get_js_string(iter->name);
      auto buf = load_buffer(d, name, iter->value);
      d.buffers.emplace(name, std::move(buf));
    }

    auto& buf_views = js["bufferViews"];
    for(auto iter = buf_views.MemberBegin(); iter != buf_views.MemberEnd();
        ++iter)
    {
      auto name = get_js_string(iter->name);
      Buffer_View buf_view;
      buf_view.name = name;

      auto buffer_name = get_js_string(iter->value["buffer"]);
      auto buffer_iter = d.buffers.find(buffer_name);
      if(buffer_iter == d.buffers.end())
      {
        // No buffer found, this buffer view is fucked
        log_e("% bufferView references bad buffer: '%'", name, buffer_name);
        continue;
      }

      buf_view.base = &buffer_iter->second.buf[0];

      buf_view.size = iter->value["byteLength"].GetUint();
      buf_view.offset = iter->value["byteOffset"].GetUint();

      auto& target_js = iter->value["target"];
      if(target_js.IsString())
      {
        if(strcmp("ARRAY_BUFFER", target_js.GetString()) == 0)
          buf_view.target = Buf_View_Target::Array;
        if(strcmp("ELEMENT_ARRAY_BUFFER", target_js.GetString()) == 0)
          buf_view.target = Buf_View_Target::Element_Array;
      }
      else if(target_js.IsUint())
      {
        switch(target_js.GetUint())
        {
        case 34962:
          buf_view.target = Buf_View_Target::Array;
          break;
        case 34963:
          buf_view.target = Buf_View_Target::Element_Array;
          break;
        }
      }
      else
      {
        log_e("Could not load target for bufferView: '%'", name);
        continue;
      }
      d.buf_views.emplace(name, std::move(buf_view));
    }

    return d;
  }

  Desc load_desc_file(std::string name)
  {
    auto file = std::fopen(name.c_str(), "rb");
    if(!file)
    {
      log_e("Failed to load scene file: '%'", name);
      return Desc{};
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
      return Desc{};
    }

    return load_desc(d, fs::path{name});
  }
}
