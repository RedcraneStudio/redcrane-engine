/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
// NOT COMPILED
#include "program_cache.h"
#include <vector>
#include <algorithm>
#include "../../common/json.h"
namespace strat
{
  namespace gfx
  {
    namespace gl
    {
      std::shared_ptr<Program> load_program(std::string decl) noexcept
      {
        static std::vector<std::shared_ptr<Program> > progs;

        using std::begin; using std::end;
        auto prog_find = std::find_if(begin(progs), end(progs),
        [&decl](auto const& val)
        {
          return val->name == decl;
        });

        std::shared_ptr<Program> ret;

        if(prog_find == end(progs))
        {
          auto doc = load_json(decl);

          auto vs = std::string{doc["vshade"].GetString()};
          auto fs = std::string{doc["fshade"].GetString()};

          ret = std::make_shared<Program>(Program::from_files(vs, fs));
          ret->name = decl;
          progs.push_back(ret);
        }
        else
        {
          ret = *prog_find;
        }
        return ret;
      }
    }
  }
}
