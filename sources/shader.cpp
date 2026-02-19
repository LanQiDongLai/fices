#include "utils/shader.h"

fices::Shader::Shader(std::string_view vertex_shader_path,
                      std::string_view fragment_shader_path) {
  std::ifstream vertex_file;
  std::ifstream fragment_file;
  std::stringstream vertex_buffer;
  std::stringstream fragment_buffer;

  vertex_file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
  fragment_file.exceptions(std::ifstream::badbit | std::ifstream::failbit);

  vertex_file.open(vertex_shader_path.data(), std::ios::in);
  fragment_file.open(fragment_shader_path.data(), std::ios::in);

  if (!vertex_file.good()) {
    spdlog::error("File {} is not good", vertex_shader_path);
  }
  if (!fragment_file.good()) {
    spdlog::error("File {} is not good", fragment_shader_path);
  }
  vertex_buffer << vertex_file.rdbuf();
  fragment_buffer << fragment_file.rdbuf();

  vertex_shader_ = vertex_buffer.str();
  fragment_shader_ = fragment_buffer.str();

  vertex_file.close();
  fragment_file.close();

  vertex_shader_path_ = vertex_shader_path;
  fragment_shader_path_ = fragment_shader_path;

  compileShader();
}

void fices::Shader::compileShader() {
  GLuint vertex_shader_id;
  GLuint fragment_shader_id;
  GLint successful;
  GLchar infoLog[512];

  vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
  fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

  const char* vertex_shader_content = vertex_shader_.c_str();
  const char* fragment_shader_content = fragment_shader_.c_str();

  glShaderSource(vertex_shader_id, 1, &vertex_shader_content, nullptr);
  glShaderSource(fragment_shader_id, 1, &fragment_shader_content, nullptr);

  glCompileShader(vertex_shader_id);
  glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &successful);
  if (!successful) {
    glGetShaderInfoLog(vertex_shader_id, 511, nullptr, infoLog);
    spdlog::error("Compile vertex shader [{}] error: {}",
                  vertex_shader_path_.c_str(), infoLog);
  }

  glCompileShader(fragment_shader_id);
  glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &successful);
  if (!successful) {
    glGetShaderInfoLog(fragment_shader_id, 511, nullptr, infoLog);
    spdlog::error("Compile fragment shader [{}] error: {}",
                  fragment_shader_path_.c_str(), infoLog);
  }

  program_id_ = glCreateProgram();
  glAttachShader(program_id_, vertex_shader_id);
  glAttachShader(program_id_, fragment_shader_id);
  glLinkProgram(program_id_);

  glGetShaderiv(program_id_, GL_LINK_STATUS, &successful);
  if (!successful) {
    glGetShaderInfoLog(program_id_, 511, nullptr, infoLog);
    spdlog::error(
        "Link shader error, vertex shader path: {}, fragment shader path: {}",
        vertex_shader_path_.c_str(), fragment_shader_path_.c_str(), infoLog);
  }

  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);
}

fices::Shader::~Shader() {
  glUseProgram(0);
  glDeleteProgram(program_id_);
}

void fices::Shader::use() { glUseProgram(program_id_); }

void fices::Shader::setUniformFloat(std::string_view name, GLfloat value) {
  GLuint location = getUniformLocation(name);
  glUniform1f(location, value);
}

void fices::Shader::setUniformInt(std::string_view name, GLint value) {
  GLuint location = getUniformLocation(name);
  glUniform1i(location, value);
}

void fices::Shader::setUniformVector2f(std::string_view name, GLfloat x,
                                       GLfloat y) {
  GLuint location = getUniformLocation(name);
  glUniform2f(location, x, y);
}

void fices::Shader::setUniformVector2f(std::string_view name,
                                       const glm::vec2& value) {
  GLuint location = getUniformLocation(name);
  glUniform2fv(location, 1, glm::value_ptr(value));
}

void fices::Shader::setUniformVector3f(std::string_view name, GLfloat x,
                                       GLfloat y, GLfloat z) {
  GLuint location = getUniformLocation(name);
  glUniform3f(location, x, y, z);
}

void fices::Shader::setUniformVector3f(std::string_view name,
                                       const glm::vec3& value) {
  GLuint location = getUniformLocation(name);
  glUniform3fv(location, 1, glm::value_ptr(value));
}

void fices::Shader::setUniformVector4f(std::string_view name, GLfloat x,
                                       GLfloat y, GLfloat z, GLfloat w) {
  GLuint location = getUniformLocation(name);
  glUniform4f(location, x, y, z, w);
}

void fices::Shader::setUniformVector4f(std::string_view name,
                                       const glm::vec4& value) {
  GLuint location = getUniformLocation(name);
  glUniform4fv(location, 1, glm::value_ptr(value));
}

void fices::Shader::setUniformMatrix4f(std::string_view name,
                                       const glm::mat4& value) {
  GLuint location = getUniformLocation(name);
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

GLuint fices::Shader::getUniformLocation(std::string_view name) {
  if (uniform_cache_.count(name)) {
    return uniform_cache_[name];
  }
  GLuint location = glGetUniformLocation(program_id_, name.data());
  if (location == -1) {
    spdlog::error("Can't find uniform name {} in shader {}", name, program_id_);
    return -1;
  }
  uniform_cache_.insert({name, location});
  return location;
}

GLuint fices::Shader::getProgramID() { return program_id_; }