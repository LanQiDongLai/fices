#include "systems/render_system.h"

#include "components/camera.h"
#include "components/mesh.h"

RenderSystem::RenderSystem(Context* context) { context_ = context; }

void RenderSystem::update(double delta_time) {
  auto* registry = context_->getRegistry();
  auto mesh_view = registry->view<Mesh>();
  auto camera_view = registry->view<Camera>();
  for (auto entity : mesh_view) {
    Mesh& mesh = mesh_view.get<Mesh>(entity);
    glBindVertexArray(mesh.VAO);
    glDrawArrays(GL_TRIANGLES, 0, mesh.triangle_count);
  }
}