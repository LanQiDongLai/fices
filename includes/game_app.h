#pragma once
#include "context.h"
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>
#include <chrono>

#include "systems/render_system.h"
#include "systems/chunk_system.h"
#include "systems/input_system.h"
#include "systems/player_system.h"
#include "systems/debug_system.h"

#include "scenes/game_scene.h"

#include "events/game_quit_event.h"

class GameApp {
  GameApp(const GameApp&) = delete;
  GameApp& operator=(const GameApp&) = delete;

 public:
  GameApp();
  ~GameApp();
  void run();
  void initialize();

 private:
  void update();
  void onCloseWindow();
  Context* context_;
  bool is_running_;
  RenderSystem *render_system_;
  ChunkSystem *chunk_system_;
  InputSystem *input_system_;
  PlayerSystem* player_system_;
  DebugSystem* debug_system_;
  
  GameScene *scene_;
  std::chrono::time_point<std::chrono::high_resolution_clock> last_frame_time_;
};