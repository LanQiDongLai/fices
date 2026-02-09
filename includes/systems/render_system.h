#pragma once
#include "context.h"
#include <glad/glad.h>
#include <spdlog/spdlog.h>

class RenderSystem {
 public:
  RenderSystem(Context* registry);
  void initialize();
  void update(double delta_time);
 private:
  Context *context_;
};