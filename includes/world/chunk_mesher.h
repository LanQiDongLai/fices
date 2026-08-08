#pragma once

#include "components/chunk_block_set.h"
#include "rendering/mesh_data.h"

class ChunkMesher {
 public:
  [[nodiscard]] MeshData build(const ChunkBlockSet& block_set) const;
};
