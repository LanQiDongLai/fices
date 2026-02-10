#pragma once
#include "context.h"
#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "utils/shader.h"

#include "components/camera.h"
#include "components/mesh.h"
#include "components/transform.h"

class RenderSystem {
 public:
  RenderSystem(Context* registry);
  void initialize();
  void update(double delta_time);
 private:
  Context *context_;
  fices::Shader* shader_;
};