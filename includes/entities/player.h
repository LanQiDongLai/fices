#pragma once
#include "components/camera.h"
#include "components/tag.h"
#include "components/transform.h"
#include "context.h"

class Player {
 public:
  Player(Context* context, Camera& camera, Transform& transform) {
    using namespace entt::literals;
    auto* registry = context->getRegistry();
    id_ = registry->create();
    registry->emplace<Tag>(id_, "player"_hs, "player");
    registry->emplace<Transform>(id_, transform);
    registry->emplace<Camera>(id_, camera);
  }
  entt::entity getEntity() { return id_; }

 private:
  entt::entity id_;
};