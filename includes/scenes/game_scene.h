#pragma once
#include "scenes/scene.h"
#include "entities/player.h"
#include "context.h"

class GameScene : public Scene {
  GameScene(const GameScene&) = delete;
  GameScene& operator=(const GameScene&) = delete;

 public:
  GameScene(Context *context);
  ~GameScene();
  void update(double delta);
 private:
  Context* context_;
  Player* player_;
};