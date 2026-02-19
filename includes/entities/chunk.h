#pragma once
#include <entt/entt.hpp>

#include "components/chunk_block_set.h"
#include "components/mesh.h"
#include "components/tag.h"
#include "components/transform.h"
#include "context.h"

class Chunk {
  Chunk(const Chunk&) = delete;
  Chunk& operator=(const Chunk&) = delete;

 public:
  Chunk(Context* context, ChunkBlockSet& block_set, Transform& transform,
        Mesh& mesh) {
    using namespace entt::literals;
    auto* registry = context->getRegistry();
    id_ = registry->create();
    registry->emplace<Tag>(id_, "chunk"_hs, "chunk");
    registry->emplace<ChunkBlockSet>(id_, block_set);
    registry->emplace<Transform>(id_, transform);
    registry->emplace<Mesh>(id_, mesh);
  }
  entt::entity getEntityId() { return id_; }

 private:
  entt::entity id_;
};