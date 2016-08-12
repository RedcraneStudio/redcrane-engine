/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "json.h"

#include <cstdlib>

#include "log.h"

#include "rapidjson/filereadstream.h"
#include "rapidjson/error/en.h"

#define REDC_TEMP_JSON_BUFFER_SIZE 4096

namespace redc
{
  struct File_Wrapper
  {
    File_Wrapper(char const* const filename, char const* const mode) noexcept
                 : fd(std::fopen(filename, mode)) {}
    inline ~File_Wrapper()
    {
      if(fd) std::fclose(fd);
    }

    std::FILE* fd;
  };

  bool load_json(rapidjson::Document& doc, std::string const& fn)
  {
    File_Wrapper file(fn.c_str(), "r");
    if(!file.fd)
    {
      log_e("Could not open file: %", fn);
      return false;
    }

    char buffer[REDC_TEMP_JSON_BUFFER_SIZE];
    rapidjson::FileReadStream file_stream(file.fd, buffer,
      REDC_TEMP_JSON_BUFFER_SIZE);

    doc.ParseStream(file_stream);

    if(doc.HasParseError())
    {
      log_e("Error parsing %:%\n%", fn, doc.GetErrorOffset(),
            rapidjson::GetParseError_En(doc.GetParseError()));
      return false;
    }

    return true;
  }

  bool has_json_members(rapidjson::Value const& val,
                        std::vector<std::string> const& mems)
  {
    for(auto str : mems)
    {
      if(!val.HasMember(str.c_str())) return false;
    }
    return true;
  }
}
