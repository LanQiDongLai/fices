#pragma once
#include "context.h"

#include "events/move_event.h"
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
  Context* context_;
};