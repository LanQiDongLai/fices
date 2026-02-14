#pragma once
#include "context.h"

#include <spdlog/spdlog.h>

#include "events/move_event.h"
#include "events/eye_move_event.h"

#include "components/camera.h"
#include "components/tag.h"
#include "components/transform.h"

class PlayerSystem {
 public:
  PlayerSystem(Context* context);
  void initialize();
  void update(double delta_time);

 private:
  void onMove(MoveEvent event);
  void onEyeMove(EyeMoveEvent event);
  Context* context_;
  std::optional<entt::entity> player_;
  bool is_moving_forward_;
  bool is_moving_backward_;
  bool is_moving_up_;
  bool is_moving_down_;
  bool is_moving_left_;
  bool is_moving_right_;
};