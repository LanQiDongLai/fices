#include "systems/render_system.h"

#include "components/camera.h"
#include "components/mesh.h"

RenderSystem::RenderSystem(Context* context) { context_ = context; }

void RenderSystem::initialize() {
  gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
}

void RenderSystem::update(double delta_time) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(1.f, 1.f, 1.f, 1.f);
  auto* registry = context_->getRegistry();
  auto mesh_view = registry->view<Mesh>();
  auto camera_view = registry->view<Camera>();
  for (auto entity : mesh_view) {
    Mesh& mesh = mesh_view.get<Mesh>(entity);
    glBindVertexArray(mesh.VAO);
    glDrawArrays(GL_TRIANGLES, 0, mesh.triangle_count);
  }
  context_->getWindow()->present();
}