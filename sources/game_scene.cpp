#include "scenes/game_scene.h"

GameScene::GameScene(Context* context) {
  using namespace entt::literals;
  context_ = context;
  Camera camera{.fov = 45.f, .near = 1.f, .far = 1000.f};
  Transform transform{.x = 0.f, .y = 0.f, .z = 0.f};
  player_ = new Player(context, camera, transform);
}

GameScene::~GameScene() { delete player_; }

void GameScene::update(double delta_time) {}