#pragma once
#include <vector>
#include <cstdint>

struct Mesh {
  std::vector<double> points; // x y z w
  std::vector<double> uv; // u v
  std::vector<double> normal; // x y z
  std::uint32_t VBO;
};