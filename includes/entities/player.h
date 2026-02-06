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
    Transform transform{.x = 0, .y = 0, .z = 0};
    registry->emplace<Transform>(id_, transform);
    Camera camera{.far = 1000., .near = 1., .fov = 40.};
    registry->emplace<Camera>(id_, camera);
  }
  entt::entity getEntity() { return id_; }

 private:
  entt::entity id_;
};