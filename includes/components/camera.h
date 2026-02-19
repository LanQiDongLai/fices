#pragma once
#include <glm/glm.hpp>

enum class CameraType {
  PERSPECTIVE,
  ORTHOGONAL
};

struct Camera {
  float fov;
  float near;
  float far;
  float yaw;
  float pitch;
};