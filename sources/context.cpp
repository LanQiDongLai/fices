#include "context.h"

Context::Context(Window* window, entt::registry* registry,
          entt::dispatcher* dispatcher) {
  window_ = window;
  registry_ = registry;
  dispatcher_ = dispatcher;
}

entt::registry *Context::getRegistry() {
  return registry_;
}

entt::dispatcher *Context::getDispatcher() {
  return dispatcher_;
}

Window* Context::getWindow() {
  return window_;
}

Context::~Context() {
  delete dispatcher_;
  delete registry_;
  delete window_;
}