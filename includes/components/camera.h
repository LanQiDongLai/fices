#pragma once

enum class CameraType {
  PERSPECTIVE,
  ORTHOGONAL
};

struct Camera {
  double fov;
  double near;
  double far;
};