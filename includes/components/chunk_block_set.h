#pragma once
#include <array>
#include "components/block.h"
const static int CHUNK_SIZE_X = 16;
const static int CHUNK_SIZE_Y = 256;
const static int CHUNK_SIZE_Z = 16;

struct ChunkBlockSet {
  std::array<std::array<std::array<Block, CHUNK_SIZE_X>, CHUNK_SIZE_Z>, CHUNK_SIZE_Y> blocks;
};