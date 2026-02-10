#pragma once

struct MoveEvent {
  enum class Direction{ LEFT, RIGHT, UP, DOWN, FORWARD, BACKWARD };
  Direction direction;
};