#pragma once

#include <chrono>

#include <entt/entt.hpp>

#include "context.h"
#include "events/game_quit_event.h"
#include "rendering/mesh_manager.h"
#include "scenes/game_scene.h"
#include "systems/chunk_system.h"
#include "systems/debug_system.h"
#include "systems/input_system.h"
#include "systems/player_system.h"
#include "systems/render_system.h"
#include "window/window.h"
#include "world/chunk_mesher.h"
#include "world/terrain_generator.h"

class GameApp {
 public:
  GameApp();
  ~GameApp();

  GameApp(const GameApp&) = delete;
  GameApp& operator=(const GameApp&) = delete;

  void initialize();
  void run();

 private:
  void update();
  void onCloseWindow();

  Window window_;
  entt::registry registry_;
  entt::dispatcher dispatcher_;
  Context context_;

  TerrainGenerator terrain_generator_;
  ChunkMesher chunk_mesher_;
  MeshManager mesh_manager_;

  RenderSystem render_system_;
  InputSystem input_system_;
  PlayerSystem player_system_;
  ChunkSystem chunk_system_;
  DebugSystem debug_system_;
  GameScene scene_;

  bool is_running_{true};
  std::chrono::time_point<std::chrono::high_resolution_clock> last_frame_time_;
};
