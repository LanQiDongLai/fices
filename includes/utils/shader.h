#pragma once
#include <string_view>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

namespace fices {

class Shader {
 public:
  Shader(std::string_view vertex_shader_path,
         std::string_view fragment_shader_path);
  ~Shader();
  GLuint getProgramID();
  void use();
  void setUniformFloat(std::string_view name, GLfloat value);
  void setUniformInt(std::string_view name, GLint value);
  void setUniformVector2f(std::string_view name, GLfloat x, GLfloat y);
  void setUniformVector2f(std::string_view name, const glm::vec2& value);
  void setUniformVector3f(std::string_view name, GLfloat x, GLfloat y, GLfloat z);
  void setUniformVector3f(std::string_view name, const glm::vec3& value);
  void setUniformVector4f(std::string_view name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
  void setUniformVector4f(std::string_view name, const glm::vec4& value);
  void setUniformMatrix4f(std::string_view name, const glm::mat4& value);
 private:
  void compileShader();
  GLuint getUniformLocation(std::string_view name);
  GLuint program_id_;
  GLuint vertex_shader_id_;
  GLuint fragment_shader_id_;
  std::unordered_map<std::string_view, GLuint> uniform_cache_;
  std::string vertex_shader_;
  std::string fragment_shader_;
  std::string vertex_shader_path_;
  std::string fragment_shader_path_;
};

}  // namespace fices