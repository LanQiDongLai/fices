#include "utils/texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

fcies::Texture::Texture(std::string_view path) {
  stbi_set_flip_vertically_on_load(true);

  glGenTextures(1, &texture_id_);
  glBindTexture(GL_TEXTURE_2D, texture_id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  unsigned char* image =
      stbi_load(path.data(), &width_, &height_, &channels_, 4);

  GLenum format = GL_RGBA;
  switch (channels_) {
    case 1:
      format = GL_RED;
      break;
    case 2:
      format = GL_RG;
      break;
    case 3:
      format = GL_RGB;
      break;
    case 4:
      format = GL_RGBA;
      break;
    default:
      format = GL_RGBA;
      break;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0, format,
               GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);

  if (image) {
    stbi_image_free(image);
  }
}

fcies::Texture::~Texture() {
  glDeleteTextures(1, &texture_id_);
}

void fcies::Texture::bind(int slot) {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, texture_id_);
}

void fcies::Texture::unBind() {
  glBindTexture(GL_TEXTURE_2D, 0);
}

int fcies::Texture::getWidth() {
  return width_;
}

int fcies::Texture::getHeight() {
  return height_;
}

int fcies::Texture::getTextureID() {
  return texture_id_;
}