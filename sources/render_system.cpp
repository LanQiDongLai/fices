#include "systems/render_system.h"

RenderSystem::RenderSystem(Context* context) { context_ = context; }

void RenderSystem::initialize() {
  gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glViewport(0, 0, 800, 800);
  shader_ = new fices::Shader("shaders/chunk.vert", "shaders/chunk.frag");
  block_textures_ = new fices::Texture("resources/images/blocks.png");
}

void RenderSystem::update(double delta_time) {
  glClearColor(0.f, 0.72f, 0.97f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  auto* registry = context_->getRegistry();
  auto mesh_view = registry->view<Mesh, Transform>();
  auto camera_view = registry->view<Camera, Transform>();

  for (auto entity : camera_view) {
    Camera& camera = camera_view.get<Camera>(entity);
    Transform& transform = camera_view.get<Transform>(entity);

    glm::mat4 projection =
        glm::perspective(glm::radians(camera.fov), (float)800 / (float)800,
                         camera.near, camera.far);
    glm::vec3 front;
    front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    front.y = sin(glm::radians(camera.pitch));
    front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    glm::mat4 view =
        glm::lookAt(glm::vec3(transform.x, transform.y, transform.z),
                    glm::vec3(transform.x, transform.y, transform.z) + front, glm::vec3(0.0f, 1.0f, 0.0f));
    shader_->setUniformMatrix4f("projection", projection);
    shader_->setUniformMatrix4f("view", view);
  }
  for (auto entity : mesh_view) {
    Mesh& mesh = mesh_view.get<Mesh>(entity);
    Transform& transform = mesh_view.get<Transform>(entity);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(transform.x, transform.y, transform.z));
    shader_->setUniformMatrix4f("model", model);
    shader_->use();
    block_textures_->bind(0);
    glBindVertexArray(mesh.VAO);
    glDrawArrays(GL_TRIANGLES, 0, mesh.triangle_count);
  }
}