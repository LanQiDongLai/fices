#pragma once
#include <cstdint>
#include <glad/glad.h>

namespace fices {

struct Vertex {
  GLfloat position[4];
  GLfloat texture_coords[3];
  GLfloat normal[4];
};

}  // namespace fices
