#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "components/mesh.h"
#include "rendering/mesh_data.h"

class MeshManager {
 public:
  MeshManager() = default;
  ~MeshManager();

  MeshManager(const MeshManager&) = delete;
  MeshManager& operator=(const MeshManager&) = delete;

  [[nodiscard]] Mesh upload(const MeshData& mesh_data);
  void release(Mesh mesh);
  void bind(Mesh mesh) const;
  void clear();

 private:
  struct GpuMesh {
    std::uint32_t vertex_array{};
    std::vector<std::uint32_t> vertex_buffers;
  };

  std::uint32_t next_handle_{1};
  std::unordered_map<std::uint32_t, GpuMesh> meshes_;
};
