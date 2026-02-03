#pragma once
#include <glad/glad.h>

#include <string_view>

namespace fcies {

class Texture {
 public:
  Texture(std::string_view path);
  ~Texture();

  void bind(int slot);
  void unBind();
  int getWidth();
  int getHeight();
  int getTextureID();

 private:
  GLuint texture_id_;
  int width_;
  int height_;
  int channels_;
};

}  // namespace fcies