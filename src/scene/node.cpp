/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */

#include "node.h"
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
  Buffer load_buffer(Desc const& d, std::string const& name, rapidjson::Value& js)
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
  Desc load_desc(rapidjson::Value& js, boost::optional<fs::path> path)
  {
    Desc d;
    d.filepath = path;

    auto& buffers = js["buffers"];
    for(auto iter = buffers.MemberBegin(); iter != buffers.MemberEnd(); ++iter)
    {
      std::string name(iter->name.GetString(), iter->name.GetStringLength());
      auto buf = load_buffer(d, name, iter->value);
      d.buffers.emplace(name, std::move(buf));
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
