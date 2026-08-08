#include "systems/render_system.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "components/camera.h"
#include "components/mesh.h"
#include "components/transform.h"

RenderSystem::RenderSystem(Context& context, MeshManager& mesh_manager)
    : context_(context), mesh_manager_(mesh_manager) {}

void RenderSystem::initialize() {
  gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress));
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glViewport(0, 0, 800, 800);
  shader_ =
      std::make_unique<fices::Shader>("shaders/chunk.vert", "shaders/chunk.frag");
  block_textures_ =
      std::make_unique<fices::Texture>("resources/images/blocks.png");
}

void RenderSystem::update(double delta_time) {
  (void)delta_time;

  glClearColor(0.0f, 0.72f, 0.97f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (!shader_ || !block_textures_) {
    return;
  }

  entt::registry& registry = *context_.getRegistry();
  const auto mesh_view = registry.view<Mesh, Transform>();
  const auto camera_view = registry.view<Camera, Transform>();

  shader_->use();
  block_textures_->bind(0);
  for (const entt::entity entity : camera_view) {
    const Camera& camera = camera_view.get<Camera>(entity);
    const Transform& transform = camera_view.get<Transform>(entity);

    const glm::mat4 projection =
        glm::perspective(glm::radians(camera.fov), 1.0f, camera.near, camera.far);
    glm::vec3 front;
    front.x = std::cos(glm::radians(camera.yaw)) *
              std::cos(glm::radians(camera.pitch));
    front.y = std::sin(glm::radians(camera.pitch));
    front.z = std::sin(glm::radians(camera.yaw)) *
              std::cos(glm::radians(camera.pitch));
    const glm::mat4 view = glm::lookAt(
        glm::vec3(transform.x, transform.y, transform.z),
        glm::vec3(transform.x, transform.y, transform.z) + front,
        glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::vec3 light_direction = glm::normalize(glm::vec3(1.0f));

    shader_->setUniformMatrix4f("projection", projection);
    shader_->setUniformMatrix4f("view", view);
    shader_->setUniformVector3f("view_position", transform.x, transform.y,
                                transform.z);
    shader_->setUniformVector3f("light_direction", light_direction);
  }

  for (const entt::entity entity : mesh_view) {
    const Mesh& mesh = mesh_view.get<Mesh>(entity);
    const Transform& transform = mesh_view.get<Transform>(entity);
    const glm::mat4 model = glm::translate(
        glm::mat4(1.0f), glm::vec3(transform.x, transform.y, transform.z));
    shader_->setUniformMatrix4f("model", model);
    mesh_manager_.bind(mesh);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.vertex_count));
  }
}
