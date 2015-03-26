/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "pipeline.h"
#include <algorithm>
#include "../common/log.h"
namespace survive
{
  namespace gfx
  {
    /*!
     * Swallow a mesh for our own purposes.
     */
    Pipeline_Mesh* Pipeline::prepare_mesh(Mesh&& mesh) noexcept
    {
      // Make a "pipeline mesh" or rather something that will store some
      // useful things about the mesh for the purposes of rendering it with
      // opengl.
      auto pipemesh = Pipeline_Mesh{std::move(mesh)};

      // Initialize the vao and buffer and such.
      glGenVertexArrays(1, &pipemesh.vao);
      glBindVertexArray(pipemesh.vao);

      glGenBuffers(1, &pipemesh.vertex_buffer);
      glBindBuffer(GL_ARRAY_BUFFER, pipemesh.vertex_buffer);
      glBufferData(GL_ARRAY_BUFFER,
                   pipemesh.mesh.vertices.size() * sizeof(glm::vec3),
                   &pipemesh.mesh.vertices[0][0], GL_STATIC_DRAW);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      glEnableVertexAttribArray(0);

      glGenBuffers(1, &pipemesh.face_index_buffer);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pipemesh.face_index_buffer);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   pipemesh.mesh.faces.size() * sizeof(glm::ivec3),
                   &pipemesh.mesh.faces[0][0],
                   GL_STATIC_DRAW);

      glBindVertexArray(0);

      // Move *that* into the vector.
      mesh_.push_back(std::move(pipemesh));

      // Now that we have a permanent spot for it. Give it to the client.
      return &mesh_.back();
    }
    Mesh Pipeline::remove_mesh(Pipeline_Mesh& mesh) noexcept
    {
      using std::begin; using std::end;
      auto new_end = std::remove_if(begin(mesh_), end(mesh_),
      [&mesh](auto& val)
      {
        // Check to see which element the pointer is pointing to.
        return &mesh == &val;
      });

      // First uninitialize the vao and buffers.
      uninit_pipeline_mesh_(mesh);

      // Capture that mesh!
      Mesh&& old_mesh_data = std::move(mesh.mesh);
      // Remove it from our vector.
      mesh_.erase(new_end, end(mesh_));

      // Move the original mesh out.
      return old_mesh_data;
    }
    void Pipeline::render_pipeline_mesh(Pipeline_Mesh* mesh) noexcept
    {
      glBindVertexArray(mesh->vao);
      glDrawElements(GL_TRIANGLES, mesh->mesh.faces.size() * 3,
                     GL_UNSIGNED_INT, 0);
      // Unbind vao?
      //glBindVertexArray(0);
    }
    // Uninitialize a mesh as far as opengl is concerned *unprepare it*.
    void Pipeline::uninit_pipeline_mesh_(Pipeline_Mesh& mesh) noexcept
    {
      glDeleteBuffers(1, &mesh.vertex_buffer);
      glDeleteBuffers(1, &mesh.face_index_buffer);
      glDeleteVertexArrays(1, &mesh.vao);
    }
  }
}
