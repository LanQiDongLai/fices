#include "systems/player_system.h"

PlayerSystem::PlayerSystem(Context* context) { context_ = context; }

void PlayerSystem::initialize() {
  context_->getDispatcher()->sink<MoveEvent>().connect<&PlayerSystem::onMove>(this);
}

void PlayerSystem::update(double delta_time) {}

void PlayerSystem::onMove(MoveEvent event) {
  using namespace entt::literals;
  entt::registry* registry = context_->getRegistry();
  auto view = registry->view<Tag, Transform>();
  for (auto entity : view) {
    Tag tag = view.get<Tag>(entity);
    if (tag.id != "player"_hs) {
      continue;
    }
    Transform& transform = view.get<Transform>(entity);
    if (event.direction == MoveEvent::Direction::LEFT) {
      transform.x -= 10.f;
    } else if (event.direction == MoveEvent::Direction::RIGHT) {
      transform.x += 10.f;
    } else if (event.direction == MoveEvent::Direction::UP) {
      transform.z -= 10.f;
    } else if (event.direction == MoveEvent::Direction::DOWN) {
      transform.z += 10.f;
    }
  }
}