#pragma once

enum class CameraType {
  PERSPECTIVE,
  ORTHOGONAL
};

struct Camera {
  float fov;
  float near;
  float far;
};