#include "systems/chunk_system.h"

#include <cmath>

#include <spdlog/spdlog.h>

#include "components/chunk_block_set.h"
#include "components/chunk_mesh_state.h"
#include "components/mesh.h"
#include "components/tag.h"
#include "components/transform.h"
#include "entities/chunk.h"

namespace {

int chunkCoordinate(int world_coordinate) {
  return static_cast<int>(
      std::floor(static_cast<double>(world_coordinate) / CHUNK_SIZE_X));
}

int localCoordinate(int world_coordinate, int chunk_coordinate) {
  return world_coordinate - chunk_coordinate * CHUNK_SIZE_X;
}

}  // namespace

ChunkSystem::ChunkSystem(Context& context,
                         TerrainGenerator& terrain_generator,
                         ChunkMesher& chunk_mesher, MeshManager& mesh_manager)
    : context_(context),
      terrain_generator_(terrain_generator),
      chunk_mesher_(chunk_mesher),
      mesh_manager_(mesh_manager) {}

ChunkSystem::~ChunkSystem() {
  context_.getDispatcher()->sink<PlaceBlockEvent>().disconnect(this);
  while (!chunks_.empty()) {
    const auto position = chunks_.begin()->first;
    removeChunk(position.first, position.second);
  }
}

void ChunkSystem::initialize() {
  context_.getDispatcher()
      ->sink<PlaceBlockEvent>()
      .connect<&ChunkSystem::onPlaceBlock>(this);
}

void ChunkSystem::update(double delta_time) {
  (void)delta_time;

  entt::registry& registry = *context_.getRegistry();
  if (!player_.has_value() || !registry.valid(*player_) ||
      !registry.all_of<Transform>(*player_)) {
    player_ = findPlayer();
  }
  if (!player_.has_value()) {
    return;
  }

  const Transform& player_position = registry.get<Transform>(*player_);
  const int player_chunk_x = static_cast<int>(
      std::floor(player_position.x / static_cast<float>(CHUNK_SIZE_X)));
  const int player_chunk_z = static_cast<int>(
      std::floor(player_position.z / static_cast<float>(CHUNK_SIZE_Z)));
  manageChunks(player_chunk_x, player_chunk_z);
  updateMeshes();
}

void ChunkSystem::generateChunk(int chunk_x, int chunk_z) {
  if (chunks_.contains({chunk_x, chunk_z})) {
    return;
  }

  ChunkBlockSet block_set = terrain_generator_.generate(chunk_x, chunk_z);
  const MeshData mesh_data = chunk_mesher_.build(block_set);
  const Mesh mesh = mesh_manager_.upload(mesh_data);
  const Transform transform{.x = chunk_x * static_cast<float>(CHUNK_SIZE_X),
                            .y = 0.0f,
                            .z = chunk_z * static_cast<float>(CHUNK_SIZE_Z)};
  const entt::entity entity = createChunkEntity(
      *context_.getRegistry(), std::move(block_set), transform, mesh);
  chunks_.emplace(std::pair{chunk_x, chunk_z}, entity);
}

void ChunkSystem::removeChunk(int chunk_x, int chunk_z) {
  const auto iterator = chunks_.find({chunk_x, chunk_z});
  if (iterator == chunks_.end()) {
    return;
  }

  entt::registry& registry = *context_.getRegistry();
  const entt::entity entity = iterator->second;
  if (registry.valid(entity)) {
    if (registry.all_of<Mesh>(entity)) {
      mesh_manager_.release(registry.get<Mesh>(entity));
    }
    registry.destroy(entity);
  }
  chunks_.erase(iterator);
}

void ChunkSystem::manageChunks(int player_chunk_x, int player_chunk_z,
                               int distance) {
  std::vector<std::pair<int, int>> chunks_to_remove;
  for (const auto& [position, entity] : chunks_) {
    (void)entity;
    const int chunk_distance = std::abs(position.first - player_chunk_x) +
                               std::abs(position.second - player_chunk_z);
    if (chunk_distance > distance) {
      chunks_to_remove.push_back(position);
    }
  }

  for (const auto& position : chunks_to_remove) {
    removeChunk(position.first, position.second);
  }

  for (int dx = -distance; dx <= distance; ++dx) {
    const int max_dz = distance - std::abs(dx);
    for (int dz = -max_dz; dz <= max_dz; ++dz) {
      generateChunk(player_chunk_x + dx, player_chunk_z + dz);
    }
  }
}

void ChunkSystem::setBlock(int x, int y, int z, Block block) {
  if (y < 0 || y >= CHUNK_SIZE_Y) {
    return;
  }

  const int chunk_x = chunkCoordinate(x);
  const int chunk_z = chunkCoordinate(z);
  const auto iterator = chunks_.find({chunk_x, chunk_z});
  if (iterator == chunks_.end()) {
    return;
  }

  entt::registry& registry = *context_.getRegistry();
  const entt::entity entity = iterator->second;
  ChunkBlockSet& block_set = registry.get<ChunkBlockSet>(entity);
  ChunkMeshState& mesh_state = registry.get<ChunkMeshState>(entity);
  block_set.blocks[y][localCoordinate(z, chunk_z)][localCoordinate(x, chunk_x)] =
      block;
  mesh_state.state = ChunkMeshState::State::Dirty;
}

Block ChunkSystem::getBlock(int x, int y, int z) const {
  if (y < 0 || y >= CHUNK_SIZE_Y) {
    return Block{.block_type = Block::BlockType::AIR};
  }

  const int chunk_x = chunkCoordinate(x);
  const int chunk_z = chunkCoordinate(z);
  const auto iterator = chunks_.find({chunk_x, chunk_z});
  if (iterator == chunks_.end()) {
    return Block{.block_type = Block::BlockType::AIR};
  }

  const ChunkBlockSet& block_set =
      context_.getRegistry()->get<ChunkBlockSet>(iterator->second);
  return block_set
      .blocks[y][localCoordinate(z, chunk_z)][localCoordinate(x, chunk_x)];
}

void ChunkSystem::updateMeshes() {
  entt::registry& registry = *context_.getRegistry();
  for (const auto& [position, entity] : chunks_) {
    (void)position;
    ChunkMeshState& mesh_state = registry.get<ChunkMeshState>(entity);
    if (mesh_state.state == ChunkMeshState::State::Available) {
      continue;
    }

    const MeshData mesh_data =
        chunk_mesher_.build(registry.get<ChunkBlockSet>(entity));
    const Mesh new_mesh = mesh_manager_.upload(mesh_data);
    if (registry.all_of<Mesh>(entity)) {
      mesh_manager_.release(registry.get<Mesh>(entity));
      registry.replace<Mesh>(entity, new_mesh);
    } else {
      registry.emplace<Mesh>(entity, new_mesh);
    }
    mesh_state.state = ChunkMeshState::State::Available;
  }
}

std::optional<entt::entity> ChunkSystem::findPlayer() const {
  using namespace entt::literals;

  entt::registry& registry = *context_.getRegistry();
  const auto view = registry.view<Tag, Transform>();
  for (const entt::entity entity : view) {
    if (view.get<Tag>(entity).id == "player"_hs) {
      return entity;
    }
  }
  return std::nullopt;
}

void ChunkSystem::onPlaceBlock(PlaceBlockEvent event) {
  (void)event;
  spdlog::info("place");

  Block block{.block_type = Block::BlockType::STONE};
  for (int x = 0; x < 20; ++x) {
    for (int z = 0; z < 20; ++z) {
      for (int y = 0; y < 20; ++y) {
        setBlock(x, y + 20, z, block);
      }
    }
  }

  block.block_type = Block::BlockType::AIR;
  for (int x = 1; x < 19; ++x) {
    for (int z = 1; z < 19; ++z) {
      for (int y = 1; y < 19; ++y) {
        setBlock(x, y + 20, z, block);
      }
    }
  }
  setBlock(0, 30, 10, block);
  setBlock(0, 29, 10, block);
}
