#pragma once

struct MoveEvent {
  enum class Direction{ LEFT, RIGHT, UP, DOWN };
  Direction direction;
};