#include "systems/input_system.h"

#include "events/game_quit_event.h"
#include "events/move_event.h"
#include "events/eye_move_event.h"

InputSystem::InputSystem(Context* context) {
  context_ = context;
  first_mouse_ = true;
}

void InputSystem::update(double delta_time) {
  Window* window = context_->getWindow();
  entt::dispatcher* dispatcher = context_->getDispatcher();
  SDL_Event event;
  while (window->pollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_QUIT:
        dispatcher->enqueue<GameQuitEvent>();
        break;
      case SDL_EVENT_KEY_DOWN: {
        MoveEvent player_move_event;
        if (event.key.scancode == SDL_SCANCODE_W) {
          player_move_event.direction = MoveEvent::Direction::FORWARD;
          dispatcher->enqueue<MoveEvent>(player_move_event);
        } else if (event.key.scancode == SDL_SCANCODE_S) {
          player_move_event.direction = MoveEvent::Direction::BACKWARD;
          dispatcher->enqueue<MoveEvent>(player_move_event);
        } else if (event.key.scancode == SDL_SCANCODE_A) {
          player_move_event.direction = MoveEvent::Direction::LEFT;
          dispatcher->enqueue<MoveEvent>(player_move_event);
        } else if (event.key.scancode == SDL_SCANCODE_D) {
          player_move_event.direction = MoveEvent::Direction::RIGHT;
          dispatcher->enqueue<MoveEvent>(player_move_event);
        } else if (event.key.scancode == SDL_SCANCODE_SPACE) {
          player_move_event.direction = MoveEvent::Direction::UP;
          dispatcher->enqueue<MoveEvent>(player_move_event);
        } else if (event.key.scancode == SDL_SCANCODE_LSHIFT) {
          player_move_event.direction = MoveEvent::Direction::DOWN;
          dispatcher->enqueue<MoveEvent>(player_move_event);
        }
        break;
      }
      case SDL_EVENT_MOUSE_MOTION: {
        spdlog::info("motion.xrel:{} motion.yrel:{}", event.motion.xrel, event.motion.yrel);
        EyeMoveEvent eye_move_event;
        eye_move_event.yaw = event.motion.xrel * 0.05f;
        eye_move_event.pitch = -event.motion.yrel * 0.05f;
        dispatcher->enqueue<EyeMoveEvent>(eye_move_event);
        break;
      }
      default:
        break;
    }
  }
}