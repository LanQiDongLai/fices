#pragma once

#include <vector>

struct MeshData {
  std::vector<float> positions;
  std::vector<float> texture_coordinates;
  std::vector<float> normals;
};
