#pragma once
#include "context.h"

#include <spdlog/spdlog.h>

class InputSystem {
 public:
  InputSystem(Context* context);
  void update(double delta_time);

 private:
  Context* context_;
  bool first_mouse_;
  bool last_mouse_x_;
  bool last_mouse_y_;
};