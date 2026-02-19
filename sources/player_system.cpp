#include "systems/player_system.h"

PlayerSystem::PlayerSystem(Context* context) {
  context_ = context;
  is_moving_forward_ = false;
  is_moving_backward_ = false;
  is_moving_up_ = false;
  is_moving_down_ = false;
  is_moving_left_ = false;
  is_moving_right_ = false;
}

void PlayerSystem::initialize() {
  context_->getDispatcher()->sink<MoveEvent>().connect<&PlayerSystem::onMove>(
      this);
  context_->getDispatcher()
      ->sink<EyeMoveEvent>()
      .connect<&PlayerSystem::onEyeMove>(this);
}

void PlayerSystem::update(double delta_time) {
  using namespace entt::literals;
  entt::registry* registry = context_->getRegistry();
  auto view = registry->view<Tag, Transform, Camera>();
  if (!player_.has_value()) {
    for (auto entity : view) {
      Tag& tag = view.get<Tag>(entity);
      if (tag.id == "player"_hs) {
        player_ = entity;
      }
    }
  }

  Camera& camera = view.get<Camera>(player_.value());
  glm::vec3 camera_front;
  camera_front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
  camera_front.y = sin(glm::radians(camera.pitch));
  camera_front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
  camera_front = glm::normalize(camera_front);
  glm::vec3 right = glm::cross(camera_front, glm::vec3(0., 1., 0.));
  glm::vec3 up = glm::vec3(0., 1., 0.);
  glm::vec3 front = glm::cross(up, right);

  glm::vec3 move_direction(0.);

  if (is_moving_forward_ == true) {
    move_direction += front;
  }
  if (is_moving_backward_ == true) {
    move_direction -= front;
  }
  if (is_moving_left_ == true) {
    move_direction -= right;
  }
  if (is_moving_right_ == true) {
    move_direction += right;
  }
  if (is_moving_up_ == true) {
    move_direction += up;
  }
  if (is_moving_down_ == true) {
    move_direction -= up;
  }
  if(move_direction.x == 0 && move_direction.y == 0 && move_direction.z == 0) {
    return;
  }
  move_direction = glm::normalize(move_direction);
  
  Transform& transform = view.get<Transform>(player_.value());
  transform.x += move_direction.x * delta_time / 30000;
  transform.y += move_direction.y * delta_time / 30000;
  transform.z += move_direction.z * delta_time / 30000;
}

void PlayerSystem::onMove(MoveEvent event) {
  switch(event.direction) {
    case MoveEvent::Direction::FORWARD: {
      if(event.cancel) {
        is_moving_forward_ = false;
      }
      else {
        is_moving_forward_ = true;
      }
      break;
    }
    case MoveEvent::Direction::BACKWARD: {
      if(event.cancel) {
        is_moving_backward_ = false;
      }
      else {
        is_moving_backward_ = true;
      }
      break;
    }
    case MoveEvent::Direction::LEFT: {
      if(event.cancel) {
        is_moving_left_ = false;
      }
      else {
        is_moving_left_ = true;
      }
      break;
    }
    case MoveEvent::Direction::RIGHT: {
      if(event.cancel) {
        is_moving_right_ = false;
      }
      else {
        is_moving_right_ = true;
      }
      break;
    }
    case MoveEvent::Direction::UP: {
      if(event.cancel) {
        is_moving_up_ = false;
      }
      else {
        is_moving_up_ = true;
      }
      break;
    }
    case MoveEvent::Direction::DOWN: {
      if(event.cancel) {
        is_moving_down_ = false;
      }
      else {
        is_moving_down_ = true;
      }
      break;
    }
  }
}

void PlayerSystem::onEyeMove(EyeMoveEvent event) {
  using namespace entt::literals;
  entt::registry* registry = context_->getRegistry();
  auto view = registry->view<Tag, Camera>();
  if (!player_.has_value()) {
    for (auto entity : view) {
      Tag& tag = view.get<Tag>(entity);
      if (tag.id == "player"_hs) {
        player_ = entity;
      }
    }
  }
  Camera& camera = view.get<Camera>(player_.value());
  camera.yaw += event.yaw;
  camera.pitch += event.pitch;
  if (camera.pitch > 89.0f) camera.pitch = 89.0f;
  if (camera.pitch < -89.0f) camera.pitch = -89.0f;
}