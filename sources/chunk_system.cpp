#include "systems/chunk_system.h"

ChunkSystem::ChunkSystem(Context* context) {
  random_ = new fices::Random(123);
  context_ = context;
}

void ChunkSystem::initialize() {
  context_->getDispatcher()
      ->sink<ChunkGenerateEvent>()
      .connect<&ChunkSystem::onGenerateChunk>(this);
  ChunkGenerateEvent event;
  event.chunk_x = 0;
  event.chunk_z = 0;
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      event.chunk_x = i;
      event.chunk_z = j;
      onGenerateChunk(event);
    }
  }
}

void ChunkSystem::update(double delta_time) {}

void ChunkSystem::onGenerateChunk(ChunkGenerateEvent event) {
  ChunkBlockSet block_set;
  for (int j = 0; j < 16; j++) {
    for (int k = 0; k < 16; k++) {
      float height =
          random_->perlinNoise((event.chunk_x * 16 + j) / 16.f / 128.f, (event.chunk_z * 16 + k) / 16.f / 128.f) * 10 + 20.5;
      for (int i = 0; i < 256; i++) {
        Block block;
        if (i < height) {
          block.block_type = Block::BlockType::STONE;
        } else {
          block.block_type = Block::BlockType::AIR;
        }
        block_set.blocks[i][j][k] = block;
      }
    }
  }
  // for (int i = 0; i < 256; i++) {
  //   for (int j = 0; j < 16; j++) {
  //     for (int k = 0; k < 16; k++) {
  //       Block block;
  //       if (i < 20)
  //         block.block_type = Block::BlockType::STONE;
  //       else if(i < 50)
  //         block.block_type = Block::BlockType::DIRT;
  //       else
  //         block.block_type = Block::BlockType::AIR;
  //       block_set.blocks[i][j][k] = block;
  //     }
  //   }
  // }
  Mesh mesh = generateMesh(block_set);
  Transform transform{.x = event.chunk_x * 16.f, .y = 0, .z = event.chunk_z * 16.f};
  Chunk(context_, block_set, transform, mesh);
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