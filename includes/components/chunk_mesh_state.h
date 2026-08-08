#pragma once

struct ChunkMeshState {
  enum class State { Available, Dirty, Ungenerated } state;
};