#include "systems/render_system.h"

RenderSystem::RenderSystem(Context* context) { context_ = context; }

void RenderSystem::initialize() {
  gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, 800, 800);
  shader_ = new fices::Shader("shaders/chunk.vert", "shaders/chunk.frag");
}

void RenderSystem::update(double delta_time) {
  glClearColor(0.f, 0.72f, 0.97f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  auto* registry = context_->getRegistry();
  auto mesh_view = registry->view<Mesh>();
  auto camera_view = registry->view<Camera, Transform>();

  for (auto entity : camera_view) {
    Camera& camera = camera_view.get<Camera>(entity);
    Transform& transform = camera_view.get<Transform>(entity);

    glm::mat4 projection =
        glm::perspective(glm::radians(camera.fov), (float)800 / (float)800,
                         camera.near, camera.far);
    glm::mat4 view = glm::lookAt(glm::vec3(transform.x, transform.y, transform.z), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    shader_->setUniformMatrix4f("projection", projection);
    shader_->setUniformMatrix4f("view", view);
  }
  for (auto entity : mesh_view) {
    Mesh& mesh = mesh_view.get<Mesh>(entity);
    shader_->use();
    glBindVertexArray(mesh.VAO);
    glDrawArrays(GL_TRIANGLES, 0, mesh.triangle_count);
  }
  context_->getWindow()->present();
}