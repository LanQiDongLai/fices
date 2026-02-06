#pragma once
#include <glad/glad.h>
#include <vector>

#include "components/chunk_block_set.h"
#include "components/mesh.h"
#include "context.h"
#include "entities/chunk.h"

struct MeshData {
  std::vector<float> points;
  std::vector<float> uv;
  std::vector<float> normal;
};

class ChunkSystem {
 public:
  ChunkSystem(Context* context);
  void initialize();
  void update(double delta_time);

 private:
  void onGenerateChunk(float x, float y);
  Mesh generateMesh();
  Mesh combineToMesh(const MeshData& mesh_data);

  void addFrontFace(MeshData* mesh_data, float x, float y, float z,
                    Block::BlockType type);
  void addBehindFace(MeshData* mesh_data, float x, float y, float z,
                     Block::BlockType type);
  void addLeftFace(MeshData* mesh_data, float x, float y, float z,
                   Block::BlockType type);
  void addRightFace(MeshData* mesh_data, float x, float y, float z,
                    Block::BlockType type);
  void addTopFace(MeshData* mesh_data, float x, float y, float z,
                  Block::BlockType type);
  void addBottomFace(MeshData* mesh_data, float x, float y, float z,
                     Block::BlockType type);

  Context* context_;
};