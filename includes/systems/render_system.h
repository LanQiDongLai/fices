#pragma once
#include "context.h"
#include <glad/glad.h>

class RenderSystem {
 public:
  RenderSystem(Context* registry);
  void update(double delta_time);
 private:
  Context *context_;
};