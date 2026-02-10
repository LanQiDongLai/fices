#include "systems/player_system.h"

PlayerSystem::PlayerSystem(Context* context) { context_ = context; }

void PlayerSystem::initialize() {
  context_->getDispatcher()->sink<MoveEvent>().connect<&PlayerSystem::onMove>(this);
  context_->getDispatcher()->sink<EyeMoveEvent>().connect<&PlayerSystem::onEyeMove>(this);
}

void PlayerSystem::update(double delta_time) {}

void PlayerSystem::onMove(MoveEvent event) {
  using namespace entt::literals;
  entt::registry* registry = context_->getRegistry();
  auto view = registry->view<Tag, Transform>();
  for (auto entity : view) {
    Tag& tag = view.get<Tag>(entity);
    if (tag.id != "player"_hs) {
      continue;
    }
    Transform& transform = view.get<Transform>(entity);
    if (event.direction == MoveEvent::Direction::LEFT) {
      transform.x -= 0.3f;
    } else if (event.direction == MoveEvent::Direction::RIGHT) {
      transform.x += 0.3f;
    } else if (event.direction == MoveEvent::Direction::UP) {
      transform.y += 0.3f;
    } else if (event.direction == MoveEvent::Direction::DOWN) {
      transform.y -= 0.3f;
    } else if (event.direction == MoveEvent::Direction::FORWARD) {
      transform.z -= 0.3f;
    } else if (event.direction == MoveEvent::Direction::BACKWARD) {
      transform.z += 0.3f;
    }
  }
}

void PlayerSystem::onEyeMove(EyeMoveEvent event) {
  spdlog::info("reviced eye move event");
  using namespace entt::literals;
  entt::registry* registry = context_->getRegistry();
  auto view = registry->view<Tag, Camera>();
  for (auto entity : view) {
    Tag& tag = view.get<Tag>(entity);
    if (tag.id != "player"_hs) {
      continue;
    }
    Camera& camera = view.get<Camera>(entity);
    camera.yaw += event.yaw;
    camera.pitch += event.pitch;
    if(camera.pitch > 89.0f)
        camera.pitch = 89.0f;
    if(camera.pitch < -89.0f)
        camera.pitch = -89.0f;
  }
}