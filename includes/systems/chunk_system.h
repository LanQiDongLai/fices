#pragma once
#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <vector>

#include "components/chunk_block_set.h"
#include "components/mesh.h"
#include "context.h"
#include "entities/chunk.h"
#include "events/chunk_generate_event.h"
#include "events/chunk_remove_event.h"

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
  void onGenerateChunk(ChunkGenerateEvent event);
  Mesh generateMesh(ChunkBlockSet& block_set);
  Mesh combineToMesh(const MeshData& mesh_data);

  void addFrontFace(MeshData* mesh_data, float x, float y, float z,
                    float texture_offset_x, float texture_offset_y);
  void addBehindFace(MeshData* mesh_data, float x, float y, float z,
                     float texture_offset_x, float texture_offset_y);
  void addLeftFace(MeshData* mesh_data, float x, float y, float z,
                   float texture_offset_x, float texture_offset_y);
  void addRightFace(MeshData* mesh_data, float x, float y, float z,
                    float texture_offset_x, float texture_offset_y);
  void addTopFace(MeshData* mesh_data, float x, float y, float z,
                  float texture_offset_x, float texture_offset_y);
  void addBottomFace(MeshData* mesh_data, float x, float y, float z,
                     float texture_offset_x, float texture_offset_y);
  std::pair<float, float> findTypeUV(Block::BlockType type);
  Context* context_;
};