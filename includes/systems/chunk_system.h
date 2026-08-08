#pragma once

#include <entt/entt.hpp>

#include <map>
#include <optional>
#include <utility>
#include <vector>

#include "components/block.h"
#include "context.h"
#include "events/place_block_event.h"
#include "rendering/mesh_manager.h"
#include "world/chunk_mesher.h"
#include "world/terrain_generator.h"

class ChunkSystem {
 public:
  ChunkSystem(Context& context, TerrainGenerator& terrain_generator,
              ChunkMesher& chunk_mesher, MeshManager& mesh_manager);
  ~ChunkSystem();

  ChunkSystem(const ChunkSystem&) = delete;
  ChunkSystem& operator=(const ChunkSystem&) = delete;

  void initialize();
  void update(double delta_time);
  void setBlock(int x, int y, int z, Block block);
  [[nodiscard]] Block getBlock(int x, int y, int z) const;

 private:
  void generateChunk(int chunk_x, int chunk_z);
  void removeChunk(int chunk_x, int chunk_z);
  void manageChunks(int player_chunk_x, int player_chunk_z,
                    int distance = kRenderDistance);
  void updateMeshes();
  void onPlaceBlock(PlaceBlockEvent event);
  [[nodiscard]] std::optional<entt::entity> findPlayer() const;

  static constexpr int kRenderDistance = 16;

  Context& context_;
  TerrainGenerator& terrain_generator_;
  ChunkMesher& chunk_mesher_;
  MeshManager& mesh_manager_;
  std::map<std::pair<int, int>, entt::entity> chunks_;
  std::optional<entt::entity> player_;
};
