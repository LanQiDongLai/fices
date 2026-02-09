#include "systems/input_system.h"

#include "events/move_event.h"
#include "events/game_quit_event.h"

InputSystem::InputSystem(Context* context) { context_ = context; }

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
          player_move_event.direction = MoveEvent::Direction::UP;
          dispatcher->enqueue<MoveEvent>(player_move_event);
        } else if (event.key.scancode == SDL_SCANCODE_S) {
          player_move_event.direction = MoveEvent::Direction::DOWN;
          dispatcher->enqueue<MoveEvent>(player_move_event);
        } else if (event.key.scancode == SDL_SCANCODE_A) {
          player_move_event.direction = MoveEvent::Direction::LEFT;
          dispatcher->enqueue<MoveEvent>(player_move_event);
        } else if (event.key.scancode == SDL_SCANCODE_D) {
          player_move_event.direction = MoveEvent::Direction::RIGHT;
          dispatcher->enqueue<MoveEvent>(player_move_event);
        }
        break;
      }
      default:
        break;
    }
  }
}