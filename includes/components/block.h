#pragma once

struct Block {
  enum class BlockType { STONE, DIRT, AIR };
  BlockType block_type;
};