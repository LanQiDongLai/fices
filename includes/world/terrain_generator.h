#pragma once

#include <cstdint>

#include "components/chunk_block_set.h"
#include "utils/random.h"

class TerrainGenerator {
 public:
  explicit TerrainGenerator(std::int64_t seed);

  ChunkBlockSet generate(int chunk_x, int chunk_z);

 private:
  fices::Random random_;
};
