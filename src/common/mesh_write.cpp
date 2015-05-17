/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include <fstream>
#include "mesh_write.h"
#include "translate.h"
namespace game
{
  void write_obj_stream(std::ostream& stream, Mesh_Data const& mesh) noexcept
  {
    stream << "# Generating by <insert-engine-version-here>\n";

    // For now don't worry about doubles.

    for(Vertex const& v : mesh.vertices)
    {
      write_format_str(stream, "v % % %\n", v.position.x, v.position.y,
                       v.position.z);
      write_format_str(stream, "vt % %\n", v.uv.x, v.uv.y);
      write_format_str(stream, "vn % % %\n", v.normal.x, v.normal.y,
                       v.normal.z);
    }
    for(unsigned int i = 0; i < mesh.elements.size(); i += 3)
    {
      std::array<unsigned int, 3> indices;

      indices[0] = mesh.elements[i] + 1;
      indices[1] = mesh.elements[i + 1] + 1;
      indices[2] = mesh.elements[i + 2] + 1;

      write_format_str(stream, "f % % %\n", indices[0], indices[1], indices[2]);
#if 0
      write_format_str(stream, "f %/%/% %/%/% %/%/%\n",
                       indices[0], indices[0], indices[0],
                       indices[1], indices[1], indices[1],
                       indices[2], indices[2], indices[2]);
#endif
    }
  }
  void write_obj(std::string fn, Mesh_Data const& mesh) noexcept
  {
    std::ofstream file{fn};
    write_obj_stream(file, mesh);
    file.flush();
  }
}
