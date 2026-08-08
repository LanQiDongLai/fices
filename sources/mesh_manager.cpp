#include "rendering/mesh_manager.h"

#include <glad/glad.h>

#include <cstddef>

MeshManager::~MeshManager() { clear(); }

Mesh MeshManager::upload(const MeshData& mesh_data) {
  GpuMesh gpu_mesh;
  glGenVertexArrays(1, &gpu_mesh.vertex_array);
  glBindVertexArray(gpu_mesh.vertex_array);

  std::uint32_t vertex_buffer{};
  glGenBuffers(1, &vertex_buffer);
  gpu_mesh.vertex_buffers.push_back(vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

  const std::size_t positions_size = mesh_data.positions.size() * sizeof(float);
  const std::size_t texture_coordinates_size =
      mesh_data.texture_coordinates.size() * sizeof(float);
  const std::size_t normals_size = mesh_data.normals.size() * sizeof(float);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(positions_size +
                                      texture_coordinates_size + normals_size),
               nullptr, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(positions_size),
                  mesh_data.positions.data());
  glBufferSubData(GL_ARRAY_BUFFER, static_cast<GLintptr>(positions_size),
                  static_cast<GLsizeiptr>(texture_coordinates_size),
                  mesh_data.texture_coordinates.data());
  glBufferSubData(GL_ARRAY_BUFFER,
                  static_cast<GLintptr>(positions_size +
                                       texture_coordinates_size),
                  static_cast<GLsizeiptr>(normals_size),
                  mesh_data.normals.data());

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                        reinterpret_cast<void*>(positions_size));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
      2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
      reinterpret_cast<void*>(positions_size + texture_coordinates_size));
  glEnableVertexAttribArray(2);

  const std::uint32_t handle = next_handle_++;
  meshes_.emplace(handle, std::move(gpu_mesh));
  return Mesh{.handle = handle,
              .vertex_count = static_cast<std::uint32_t>(
                  mesh_data.positions.size() / 3)};
}

void MeshManager::release(Mesh mesh) {
  const auto iterator = meshes_.find(mesh.handle);
  if (iterator == meshes_.end()) {
    return;
  }

  GpuMesh& gpu_mesh = iterator->second;
  glDeleteVertexArrays(1, &gpu_mesh.vertex_array);
  glDeleteBuffers(static_cast<GLsizei>(gpu_mesh.vertex_buffers.size()),
                  gpu_mesh.vertex_buffers.data());
  meshes_.erase(iterator);
}

void MeshManager::bind(Mesh mesh) const {
  const auto iterator = meshes_.find(mesh.handle);
  glBindVertexArray(iterator == meshes_.end() ? 0
                                              : iterator->second.vertex_array);
}

void MeshManager::clear() {
  for (auto& [handle, gpu_mesh] : meshes_) {
    glDeleteVertexArrays(1, &gpu_mesh.vertex_array);
    glDeleteBuffers(static_cast<GLsizei>(gpu_mesh.vertex_buffers.size()),
                    gpu_mesh.vertex_buffers.data());
  }
  meshes_.clear();
}
