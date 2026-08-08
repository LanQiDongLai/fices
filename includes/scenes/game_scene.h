#pragma once

#include <entt/entt.hpp>

#include "context.h"
#include "scenes/scene.h"

class GameScene : public Scene {
 public:
  explicit GameScene(Context& context);
  ~GameScene() override;

  GameScene(const GameScene&) = delete;
  GameScene& operator=(const GameScene&) = delete;

  void update(double delta) override;

 private:
  Context& context_;
  entt::entity player_;
};
