#pragma once

#include <memory>

#include "context.h"
#include "rendering/mesh_manager.h"
#include "utils/shader.h"
#include "utils/texture.h"

class RenderSystem {
 public:
  RenderSystem(Context& context, MeshManager& mesh_manager);

  RenderSystem(const RenderSystem&) = delete;
  RenderSystem& operator=(const RenderSystem&) = delete;

  void initialize();
  void update(double delta_time);

 private:
  Context& context_;
  MeshManager& mesh_manager_;
  std::unique_ptr<fices::Shader> shader_;
  std::unique_ptr<fices::Texture> block_textures_;
};
