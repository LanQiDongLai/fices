#pragma once

#include <entt/entt.hpp>

#include "components/camera.h"
#include "components/tag.h"
#include "components/transform.h"

inline entt::entity createPlayerEntity(entt::registry& registry,
                                       Camera camera, Transform transform) {
  using namespace entt::literals;

  const entt::entity entity = registry.create();
  registry.emplace<Tag>(entity, "player"_hs, "player");
  registry.emplace<Transform>(entity, transform);
  registry.emplace<Camera>(entity, camera);
  return entity;
}
