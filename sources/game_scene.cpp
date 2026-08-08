#include "scenes/game_scene.h"

#include "components/camera.h"
#include "components/transform.h"
#include "entities/player.h"

GameScene::GameScene(Context& context) : context_(context) {
  const Camera camera{.fov = 45.0f,
                      .near = 1.0f,
                      .far = 1000.0f,
                      .yaw = 45.0f,
                      .pitch = 0.0f};
  const Transform transform{.x = 0.0f, .y = 30.0f, .z = 0.0f};
  player_ = createPlayerEntity(*context_.getRegistry(), camera, transform);
}

GameScene::~GameScene() {
  entt::registry& registry = *context_.getRegistry();
  if (registry.valid(player_)) {
    registry.destroy(player_);
  }
}

void GameScene::update(double delta_time) { (void)delta_time; }
