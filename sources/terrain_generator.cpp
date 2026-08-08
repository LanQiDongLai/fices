#include "world/terrain_generator.h"

TerrainGenerator::TerrainGenerator(std::int64_t seed) : random_(seed) {}

ChunkBlockSet TerrainGenerator::generate(int chunk_x, int chunk_z) {
  ChunkBlockSet block_set{};

  for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
      const float height =
          random_.fractalNoise((chunk_x * CHUNK_SIZE_X + x) / 16.0f,
                               (chunk_z * CHUNK_SIZE_Z + z) / 16.0f, 6, 0.3,
                               3.0) *
              10.0f +
          20.0f;

      for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
        Block::BlockType type = Block::BlockType::AIR;
        if (y < height) {
          type = Block::BlockType::STONE;
        } else if (y < height + 2.0f) {
          type = Block::BlockType::DIRT;
        }
        block_set.blocks[y][z][x].block_type = type;
      }
    }
  }

  return block_set;
}
