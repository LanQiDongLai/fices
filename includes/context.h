#pragma once

#include <entt/entt.hpp>

#include "window/window.h"

class Context {
 public:
  Context(Window& window, entt::registry& registry,
          entt::dispatcher& dispatcher);

  [[nodiscard]] entt::registry* getRegistry();
  [[nodiscard]] entt::dispatcher* getDispatcher();
  [[nodiscard]] Window* getWindow();

 private:
  Window* window_;
  entt::registry* registry_;
  entt::dispatcher* dispatcher_;
};
