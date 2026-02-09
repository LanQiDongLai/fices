#pragma once
#include <entt/entt.hpp>

#include "window/window.h"

class Context {
 public:
  Context(Window* window, entt::registry* registry,
          entt::dispatcher* dispatcher);
  ~Context();

  entt::registry* getRegistry();
  entt::dispatcher* getDispatcher();
  Window* getWindow();

 private:
  Window* window_;
  entt::registry* registry_;
  entt::dispatcher* dispatcher_;
};