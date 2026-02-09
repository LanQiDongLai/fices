#include "systems/debug_system.h"

DebugSystem::DebugSystem(Context* context) { context_ = context; }

void DebugSystem::update(double delta_time) {
  using namespace entt::literals;
  auto* registry = context_->getRegistry();
  auto view = registry->view<Tag, Transform>();
  for (auto entity : view) {
    Tag& tag = view.get<Tag>(entity);
    Transform& transform = view.get<Transform>(entity);
    if (tag.id != "player"_hs) {
      continue;
    }
    spdlog::debug("Player x:{}, y:{}, z:{}", transform.x, transform.y,
                  transform.z);
  }
}
