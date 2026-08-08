#pragma once

#include <entt/entt.hpp>

#include <utility>

#include "components/chunk_block_set.h"
#include "components/chunk_mesh_state.h"
#include "components/mesh.h"
#include "components/tag.h"
#include "components/transform.h"

inline entt::entity createChunkEntity(entt::registry& registry,
                                      ChunkBlockSet block_set,
                                      Transform transform, Mesh mesh) {
  using namespace entt::literals;

  const entt::entity entity = registry.create();
  registry.emplace<Tag>(entity, "chunk"_hs, "chunk");
  registry.emplace<ChunkBlockSet>(entity, std::move(block_set));
  registry.emplace<Transform>(entity, transform);
  registry.emplace<Mesh>(entity, mesh);
  registry.emplace<ChunkMeshState>(
      entity, ChunkMeshState{.state = ChunkMeshState::State::Available});
  return entity;
}
