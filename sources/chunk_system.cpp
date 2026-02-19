#include "systems/chunk_system.h"

ChunkSystem::ChunkSystem(Context* context) {
  random_ = new fices::Random(123);
  context_ = context;
}

void ChunkSystem::initialize() {
  context_->getDispatcher()
      ->sink<ChunkGenerateEvent>()
      .connect<&ChunkSystem::onGenerateChunk>(this);
  context_->getDispatcher()
      ->sink<ChunkRemoveEvent>()
      .connect<&ChunkSystem::onRemoveChunk>(this);
  context_->getDispatcher()
      ->sink<PlaceBlockEvent>()
      .connect<&ChunkSystem::onPlaceBlock>(this);
}

void ChunkSystem::update(double delta_time) {
  using namespace entt::literals;
  entt::registry* registry = context_->getRegistry();
  auto view = registry->view<Tag, Transform>();
  if (!player_.has_value()) {
    for (auto entity : view) {
      Tag& tag = view.get<Tag>(entity);
      if (tag.id == "player"_hs) {
        player_ = entity;
      }
    }
  }
  Transform& player_position = view.get<Transform>(player_.value());
  int player_in_chunk_x = (int)std::floor(player_position.x) / 16;
  int player_in_chunk_z = (int)std::floor(player_position.z) / 16;
  manageChunk(player_in_chunk_x, player_in_chunk_z);
  updateMesh();
}

void ChunkSystem::onGenerateChunk(ChunkGenerateEvent event) {
  ChunkBlockSet block_set;
  for (int j = 0; j < 16; j++) {
    for (int k = 0; k < 16; k++) {
      float height =
          random_->fractalNoise((event.chunk_x * 16 + j) / 16.f,
                                (event.chunk_z * 16 + k) / 16.f, 6, 0.3, 3.0) *
              10 +
          20.f;
      for (int i = 0; i < 256; i++) {
        Block block;
        if (i < height) {
          block.block_type = Block::BlockType::STONE;
        } else if (i < height + 2) {
          block.block_type = Block::BlockType::DIRT;
        } else {
          block.block_type = Block::BlockType::AIR;
        }
        block_set.blocks[i][j][k] = block;
      }
    }
  }
  Mesh mesh = generateMesh(block_set);
  Transform transform{
      .x = event.chunk_x * 16.f, .y = 0, .z = event.chunk_z * 16.f};
  Chunk chunk(context_, block_set, transform, mesh);
  position_to_chunks_cache_.insert({{event.chunk_x, event.chunk_z}, chunk.getEntityId()});
}

Mesh ChunkSystem::generateMesh(ChunkBlockSet& block_set) {
  MeshData mesh_data;
  for (int i = 0; i < 256; i++) {     // y
    for (int j = 0; j < 16; j++) {    // z
      for (int k = 0; k < 16; k++) {  // x
        Block::BlockType type = block_set.blocks[i][j][k].block_type;
        if (type == Block::BlockType::AIR) {
          continue;
        }
        auto offset = findTypeUV(type);
        if (k == 0 ||
            block_set.blocks[i][j][k - 1].block_type == Block::BlockType::AIR) {
          addLeftFace(&mesh_data, k, i, j, offset.first, offset.second);
        }
        if (j == 0 ||
            block_set.blocks[i][j - 1][k].block_type == Block::BlockType::AIR) {
          addFrontFace(&mesh_data, k, i, j, offset.first, offset.second);
        }
        if (i == 0 ||
            block_set.blocks[i - 1][j][k].block_type == Block::BlockType::AIR) {
          addBottomFace(&mesh_data, k, i, j, offset.first, offset.second);
        }
        if (k == 15 ||
            block_set.blocks[i][j][k + 1].block_type == Block::BlockType::AIR) {
          addRightFace(&mesh_data, k, i, j, offset.first, offset.second);
        }
        if (j == 15 ||
            block_set.blocks[i][j + 1][k].block_type == Block::BlockType::AIR) {
          addBehindFace(&mesh_data, k, i, j, offset.first, offset.second);
        }
        if (i == 255 ||
            block_set.blocks[i + 1][j][k].block_type == Block::BlockType::AIR) {
          addTopFace(&mesh_data, k, i, j, offset.first, offset.second);
        }
      }
    }
  }

  return combineToMesh(mesh_data);
}

Mesh ChunkSystem::combineToMesh(const MeshData& mesh_data) {
  GLuint VBO;
  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               mesh_data.points.size() * sizeof(float) +
                   mesh_data.uv.size() * sizeof(float) +
                   mesh_data.normal.size() * sizeof(float),
               nullptr, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, mesh_data.points.size() * sizeof(float),
                  mesh_data.points.data());
  glBufferSubData(GL_ARRAY_BUFFER, mesh_data.points.size() * sizeof(float),
                  mesh_data.uv.size() * sizeof(float), mesh_data.uv.data());
  glBufferSubData(GL_ARRAY_BUFFER,
                  mesh_data.points.size() * sizeof(float) +
                      mesh_data.uv.size() * sizeof(float),
                  mesh_data.normal.size() * sizeof(float),
                  mesh_data.normal.data());

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                        (void*)(mesh_data.points.size() * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        (void*)(mesh_data.points.size() * sizeof(float) +
                                mesh_data.uv.size() * sizeof(float)));
  glEnableVertexAttribArray(2);
  Mesh mesh;
  mesh.VAO = VAO;
  mesh.VBOs.push_back(VBO);
  mesh.triangle_count = mesh_data.points.size() / 3;
  return mesh;
}

void ChunkSystem::addFrontFace(MeshData* mesh_data, float x, float y, float z,
                               float texture_offset_x, float texture_offset_y) {
  // right-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(-1.f);

  mesh_data->points.push_back(x);
  mesh_data->points.push_back(y + 1.f);
  mesh_data->points.push_back(z);

  mesh_data->uv.push_back(texture_offset_x + 1.f);
  mesh_data->uv.push_back(texture_offset_y + 1.f);

  // left-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(-1.f);

  mesh_data->points.push_back(x + 1.f);
  mesh_data->points.push_back(y + 1.f);
  mesh_data->points.push_back(z);

  mesh_data->uv.push_back(texture_offset_x + 0.f);
  mesh_data->uv.push_back(texture_offset_y + 1.f);

  // right-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(-1.f);

  mesh_data->points.push_back(x);
  mesh_data->points.push_back(y);
  mesh_data->points.push_back(z);

  mesh_data->uv.push_back(texture_offset_x + 1.f);
  mesh_data->uv.push_back(texture_offset_y + 0.f);

  // left-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(-1.f);

  mesh_data->points.push_back(x + 1.f);
  mesh_data->points.push_back(y + 1.f);
  mesh_data->points.push_back(z);

  mesh_data->uv.push_back(texture_offset_x + 0.f);
  mesh_data->uv.push_back(texture_offset_y + 1.f);

  // left-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(-1.f);

  mesh_data->points.push_back(x + 1.f);
  mesh_data->points.push_back(y);
  mesh_data->points.push_back(z);

  mesh_data->uv.push_back(texture_offset_x + 0.f);
  mesh_data->uv.push_back(texture_offset_y + 0.f);

  // right-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(-1.f);

  mesh_data->points.push_back(x);
  mesh_data->points.push_back(y);
  mesh_data->points.push_back(z);

  mesh_data->uv.push_back(texture_offset_x + 1.f);
  mesh_data->uv.push_back(texture_offset_y + 0.f);
}

void ChunkSystem::addBehindFace(MeshData* mesh_data, float x, float y, float z,
                                float texture_offset_x,
                                float texture_offset_y) {
  // left-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);

  mesh_data->points.push_back(x);
  mesh_data->points.push_back(y + 1.f);
  mesh_data->points.push_back(z + 1.f);

  mesh_data->uv.push_back(texture_offset_x + 0.f);
  mesh_data->uv.push_back(texture_offset_y + 1.f);

  // left-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);

  mesh_data->points.push_back(x);
  mesh_data->points.push_back(y);
  mesh_data->points.push_back(z + 1.f);

  mesh_data->uv.push_back(texture_offset_x + 0.f);
  mesh_data->uv.push_back(texture_offset_y + 0.f);

  // right-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);

  mesh_data->points.push_back(x + 1.f);
  mesh_data->points.push_back(y + 1.f);
  mesh_data->points.push_back(z + 1.f);

  mesh_data->uv.push_back(texture_offset_x + 1.f);
  mesh_data->uv.push_back(texture_offset_y + 1.f);

  // left-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);

  mesh_data->points.push_back(x);
  mesh_data->points.push_back(y);
  mesh_data->points.push_back(z + 1.f);

  mesh_data->uv.push_back(texture_offset_x + 0.f);
  mesh_data->uv.push_back(texture_offset_y + 0.f);

  // right-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);

  mesh_data->points.push_back(x + 1.f);
  mesh_data->points.push_back(y);
  mesh_data->points.push_back(z + 1.f);

  mesh_data->uv.push_back(texture_offset_x + 1.f);
  mesh_data->uv.push_back(texture_offset_y + 0.f);

  // right-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);

  mesh_data->points.push_back(x + 1.f);
  mesh_data->points.push_back(y + 1.f);
  mesh_data->points.push_back(z + 1.f);

  mesh_data->uv.push_back(texture_offset_x + 1.f);
  mesh_data->uv.push_back(texture_offset_y + 1.f);
}

void ChunkSystem::addLeftFace(MeshData* mesh_data, float x, float y, float z,
                              float texture_offset_x, float texture_offset_y) {
  // left-top
  mesh_data->normal.push_back(-1.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x);
  mesh_data->points.push_back(y + 1.f);
  mesh_data->points.push_back(z);

  mesh_data->uv.push_back(texture_offset_x + 0.f);
  mesh_data->uv.push_back(texture_offset_y + 1.f);

  // left-bottom
  mesh_data->normal.push_back(-1.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x);
  mesh_data->points.push_back(y);
  mesh_data->points.push_back(z);

  mesh_data->uv.push_back(texture_offset_x + 0.f);
  mesh_data->uv.push_back(texture_offset_y + 0.f);

  // right-top
  mesh_data->normal.push_back(-1.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x);
  mesh_data->points.push_back(y + 1.f);
  mesh_data->points.push_back(z + 1.f);

  mesh_data->uv.push_back(texture_offset_x + 1.f);
  mesh_data->uv.push_back(texture_offset_y + 1.f);

  // left-bottom
  mesh_data->normal.push_back(-1.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x);
  mesh_data->points.push_back(y);
  mesh_data->points.push_back(z);

  mesh_data->uv.push_back(texture_offset_x + 0.f);
  mesh_data->uv.push_back(texture_offset_y + 0.f);

  // right-bottom
  mesh_data->normal.push_back(-1.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x);
  mesh_data->points.push_back(y);
  mesh_data->points.push_back(z + 1.f);

  mesh_data->uv.push_back(texture_offset_x + 1.f);
  mesh_data->uv.push_back(texture_offset_y + 0.f);

  // right-top
  mesh_data->normal.push_back(-1.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x);
  mesh_data->points.push_back(y + 1.f);
  mesh_data->points.push_back(z + 1.f);

  mesh_data->uv.push_back(texture_offset_x + 1.f);
  mesh_data->uv.push_back(texture_offset_y + 1.f);
}

void ChunkSystem::addRightFace(MeshData* mesh_data, float x, float y, float z,
                               float texture_offset_x, float texture_offset_y) {
  // left-top
  mesh_data->normal.push_back(1.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x + 1.f);
  mesh_data->points.push_back(y + 1.f);
  mesh_data->points.push_back(z + 1.f);

  mesh_data->uv.push_back(texture_offset_x + 0.f);
  mesh_data->uv.push_back(texture_offset_y + 1.f);

  // left-bottom
  mesh_data->normal.push_back(1.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x + 1.f);
  mesh_data->points.push_back(y);
  mesh_data->points.push_back(z + 1.f);

  mesh_data->uv.push_back(texture_offset_x + 0.f);
  mesh_data->uv.push_back(texture_offset_y + 0.f);

  // right-top
  mesh_data->normal.push_back(1.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x + 1.f);
  mesh_data->points.push_back(y + 1.f);
  mesh_data->points.push_back(z);

  mesh_data->uv.push_back(texture_offset_x + 1.f);
  mesh_data->uv.push_back(texture_offset_y + 1.f);

  // left-bottom
  mesh_data->normal.push_back(1.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x + 1.f);
  mesh_data->points.push_back(y);
  mesh_data->points.push_back(z + 1.f);

  mesh_data->uv.push_back(texture_offset_x + 0.f);
  mesh_data->uv.push_back(texture_offset_y + 0.f);

  // right-bottom
  mesh_data->normal.push_back(1.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x + 1.f);
  mesh_data->points.push_back(y);
  mesh_data->points.push_back(z);

  mesh_data->uv.push_back(texture_offset_x + 1.f);
  mesh_data->uv.push_back(texture_offset_y + 0.f);

  // right-top
  mesh_data->normal.push_back(1.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x + 1.f);
  mesh_data->points.push_back(y + 1.f);
  mesh_data->points.push_back(z);

  mesh_data->uv.push_back(texture_offset_x + 1.f);
  mesh_data->uv.push_back(texture_offset_y + 1.f);
}

void ChunkSystem::addTopFace(MeshData* mesh_data, float x, float y, float z,
                             float texture_offset_x, float texture_offset_y) {
  // left-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x);
  mesh_data->points.push_back(y + 1.f);
  mesh_data->points.push_back(z);

  mesh_data->uv.push_back(texture_offset_x + 0.f);
  mesh_data->uv.push_back(texture_offset_y + 1.f);

  // left-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x);
  mesh_data->points.push_back(y + 1.f);
  mesh_data->points.push_back(z + 1.f);

  mesh_data->uv.push_back(texture_offset_x + 0.f);
  mesh_data->uv.push_back(texture_offset_y + 0.f);

  // right-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x + 1.f);
  mesh_data->points.push_back(y + 1.f);
  mesh_data->points.push_back(z);

  mesh_data->uv.push_back(texture_offset_x + 1.f);
  mesh_data->uv.push_back(texture_offset_y + 1.f);

  // left-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x);
  mesh_data->points.push_back(y + 1.f);
  mesh_data->points.push_back(z + 1.f);

  mesh_data->uv.push_back(texture_offset_x + 0.f);
  mesh_data->uv.push_back(texture_offset_y + 0.f);

  // right-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x + 1.f);
  mesh_data->points.push_back(y + 1.f);
  mesh_data->points.push_back(z + 1.f);

  mesh_data->uv.push_back(texture_offset_x + 1.f);
  mesh_data->uv.push_back(texture_offset_y + 0.f);

  // right-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x + 1.f);
  mesh_data->points.push_back(y + 1.f);
  mesh_data->points.push_back(z);

  mesh_data->uv.push_back(texture_offset_x + 1.f);
  mesh_data->uv.push_back(texture_offset_y + 1.f);
}

void ChunkSystem::addBottomFace(MeshData* mesh_data, float x, float y, float z,
                                float texture_offset_x,
                                float texture_offset_y) {
  // left-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(-1.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x);
  mesh_data->points.push_back(y);
  mesh_data->points.push_back(z + 1.f);

  mesh_data->uv.push_back(texture_offset_x + 0.f);
  mesh_data->uv.push_back(texture_offset_y + 1.f);

  // left-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(-1.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x);
  mesh_data->points.push_back(y);
  mesh_data->points.push_back(z);

  mesh_data->uv.push_back(texture_offset_x + 0.f);
  mesh_data->uv.push_back(texture_offset_y + 0.f);

  // right-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(-1.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x + 1.f);
  mesh_data->points.push_back(y);
  mesh_data->points.push_back(z + 1.f);

  mesh_data->uv.push_back(texture_offset_x + 1.f);
  mesh_data->uv.push_back(texture_offset_y + 1.f);

  // left-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(-1.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x);
  mesh_data->points.push_back(y);
  mesh_data->points.push_back(z);

  mesh_data->uv.push_back(texture_offset_x + 0.f);
  mesh_data->uv.push_back(texture_offset_y + 0.f);

  // right-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(-1.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x + 1.f);
  mesh_data->points.push_back(y);
  mesh_data->points.push_back(z);

  mesh_data->uv.push_back(texture_offset_x + 1.f);
  mesh_data->uv.push_back(texture_offset_y + 0.f);

  // right-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(-1.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x + 1.f);
  mesh_data->points.push_back(y);
  mesh_data->points.push_back(z + 1.f);

  mesh_data->uv.push_back(texture_offset_x + 1.f);
  mesh_data->uv.push_back(texture_offset_y + 1.f);
}

std::pair<float, float> ChunkSystem::findTypeUV(Block::BlockType type) {
  switch (type) {
    case Block::BlockType::DIRT:
      return {1.f, 3.f};
    case Block::BlockType::STONE:
      return {0.f, 3.f};
    default:
      return {0.f, 0.f};
  }
}

void ChunkSystem::onRemoveChunk(ChunkRemoveEvent event) {
  if(!position_to_chunks_cache_.count({event.chunk_x, event.chunk_z})) {
    return;
  }
  entt::entity chunk = position_to_chunks_cache_[{event.chunk_x, event.chunk_z}];
  auto *registry = context_->getRegistry();
  auto view = registry->view<Mesh>();
  Mesh& mesh = view.get<Mesh>(chunk);
  glDeleteVertexArrays(1, &mesh.VAO);
  glDeleteBuffers(mesh.VBOs.size(), mesh.VBOs.data());
  registry->destroy(chunk);
  position_to_chunks_cache_.erase({event.chunk_x, event.chunk_z});
}

void ChunkSystem::manageChunk(int player_in_chunk_x, int player_in_chunk_z, int distance) {
  std::vector<std::pair<int, int>> remove_chunks;
  for(auto& [position, chunk]: position_to_chunks_cache_) {
    int chunk_distance = 0;
    chunk_distance += abs(position.first - player_in_chunk_x);
    chunk_distance += abs(position.second - player_in_chunk_z);
    if(chunk_distance > distance) {
      remove_chunks.emplace_back(position.first, position.second);
    }
  }
  for(auto& position: remove_chunks) {
    ChunkRemoveEvent event;
    event.chunk_x = position.first;
    event.chunk_z = position.second;
    onRemoveChunk(event);
  }
  for(int dx = -distance; dx <= distance; dx++) {
    int max_dz = distance - abs(dx);
    for(int dz = -max_dz; dz <= max_dz; dz++) {
      int chunk_x = dx + player_in_chunk_x;
      int chunk_z = dz + player_in_chunk_z;
      if(position_to_chunks_cache_.count({chunk_x, chunk_z})) {
        continue;
      }
      ChunkGenerateEvent event;
      event.chunk_x = chunk_x;
      event.chunk_z = chunk_z;
      onGenerateChunk(event);
    }
  }
}

void ChunkSystem::setBlock(int x, int y, int z, Block block) {
  auto* registry = context_->getRegistry();
  int chunk_x = x / 16;
  int chunk_z = z / 16;
  if(!position_to_chunks_cache_.count({chunk_x, chunk_z})) {
    // TODO: write into file
    return;
  }
  entt::entity chunk = position_to_chunks_cache_[{chunk_x, chunk_z}];
  auto view = registry->view<ChunkBlockSet, Mesh, ChunkMeshState>();
  auto& block_set = view.get<ChunkBlockSet>(chunk);
  auto& mesh = view.get<Mesh>(chunk);
  auto& mesh_state = view.get<ChunkMeshState>(chunk);
  mesh_state.state = ChunkMeshState::STATE::DIRT;
  block_set.blocks[y][x % 16][z % 16] = block;
}

Block ChunkSystem::getBlock(int x, int y, int z) {
  auto* registry = context_->getRegistry();
  int chunk_x = x / 16;
  int chunk_z = z / 16;
  if(!position_to_chunks_cache_.count({chunk_x, chunk_z})) {
    // TODO: read from file
    return Block{Block::BlockType::AIR};
  }
  entt::entity chunk = position_to_chunks_cache_[{chunk_x, chunk_z}];
  auto view = registry->view<ChunkBlockSet>();
  auto& block_set = view.get<ChunkBlockSet>(chunk);
  return block_set.blocks[y][x % 16][z % 16];
}

void ChunkSystem::updateMesh() {
  auto* registry = context_->getRegistry();
  for(auto& [position, chunk]: position_to_chunks_cache_) {
    auto view = registry->view<ChunkMeshState, ChunkBlockSet>();
    auto& chunk_mesh_state = view.get<ChunkMeshState>(chunk);
    if(chunk_mesh_state.state == ChunkMeshState::STATE::AVALIBLE) {
      continue;
    }
    if(chunk_mesh_state.state == ChunkMeshState::STATE::DIRT) {
      Mesh new_mesh = generateMesh(view.get<ChunkBlockSet>(chunk));
      auto mesh_view = registry->view<Mesh>();
      Mesh& mesh = mesh_view.get<Mesh>(chunk);
      glDeleteBuffers(mesh.VBOs.size(), mesh.VBOs.data());
      glDeleteVertexArrays(1, &mesh.VAO);
      mesh = new_mesh;
      chunk_mesh_state.state = ChunkMeshState::STATE::AVALIBLE;
    }
    if(chunk_mesh_state.state == ChunkMeshState::STATE::UNGENERATED) {
      Mesh mesh = generateMesh(view.get<ChunkBlockSet>(chunk));
      registry->emplace<Mesh>(chunk, mesh);
      chunk_mesh_state.state = ChunkMeshState::STATE::AVALIBLE;
    }
  }
}

void ChunkSystem::onPlaceBlock(PlaceBlockEvent event) {
  spdlog::info("place");
  Block block;
  block.block_type = Block::BlockType::STONE;
  setBlock(0, 40, 0, block);
}