#pragma once
#include <vector>
#include <cstdint>

struct Mesh {
  std::uint32_t VAO;
  std::vector<std::uint32_t> VBOs;
  std::uint32_t triangle_count;
};