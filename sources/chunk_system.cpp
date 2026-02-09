#include "systems/chunk_system.h"

ChunkSystem::ChunkSystem(Context* context) { context_ = context; }

void ChunkSystem::initialize() {
  context_->getDispatcher()->sink<ChunkGenerateEvent>().connect<&ChunkSystem::onGenerateChunk>(this);
}

void ChunkSystem::update(double delta_time) {}

void ChunkSystem::onGenerateChunk(ChunkGenerateEvent event) {
  ChunkBlockSet block_set;
  for (int i = 0; i < 256; i++) {
    for (int j = 0; j < 16; j++) {
      for (int k = 0; k < 16; k++) {
        Block block;
        if (i < 2)
          block.block_type = Block::BlockType::DIRT;
        else
          block.block_type = Block::BlockType::AIR;
        block_set.blocks[k][j][i] = block;
      }
    }
  }
  Mesh mesh = generateMesh();
  Transform transform{.x = 0, .y = 0, .z = 0};
  Chunk(context_, block_set, transform, mesh);
}

Mesh ChunkSystem::generateMesh() {
  //TODO
  return Mesh{};
}

Mesh ChunkSystem::combineToMesh(const MeshData& mesh_data) {
  GLuint VBO;
  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, mesh_data.points.size() * sizeof(float),
                  mesh_data.points.data());
  glBufferSubData(GL_ARRAY_BUFFER, mesh_data.points.size() * sizeof(float),
                  mesh_data.uv.size() * sizeof(float), mesh_data.uv.data());
  glBufferSubData(GL_ARRAY_BUFFER,
                  mesh_data.points.size() * sizeof(float) +
                      mesh_data.uv.size() * sizeof(float),
                  mesh_data.normal.size() * sizeof(double),
                  mesh_data.normal.data());

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                        (void*)(mesh_data.points.size() * sizeof(float)));
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        (void*)(mesh_data.points.size() * sizeof(float) +
                                mesh_data.uv.size() * sizeof(float)));
  Mesh mesh;
  mesh.VAO = VAO;
  mesh.triangle_count = mesh_data.points.size() / 3;
  return mesh;
}

void ChunkSystem::addFrontFace(MeshData* mesh_data, float x, float y, float z,
                               Block::BlockType type) {
  // left-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(-1.f);

  mesh_data->points.push_back(x - 0.5f);
  mesh_data->points.push_back(y + 0.5f);
  mesh_data->points.push_back(z - 0.5f);

  mesh_data->uv.push_back(0.f);
  mesh_data->uv.push_back(0.f);

  // right-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(-1.f);

  mesh_data->points.push_back(x + 0.5f);
  mesh_data->points.push_back(y + 0.5f);
  mesh_data->points.push_back(z - 0.5f);

  mesh_data->uv.push_back(0.f);
  mesh_data->uv.push_back(0.f);

  // left-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(-1.f);

  mesh_data->points.push_back(x - 0.5f);
  mesh_data->points.push_back(y - 0.5f);
  mesh_data->points.push_back(z - 0.5f);

  mesh_data->uv.push_back(0.f);
  mesh_data->uv.push_back(0.f);

  // right-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(-1.f);

  mesh_data->points.push_back(x + 0.5f);
  mesh_data->points.push_back(y + 0.5f);
  mesh_data->points.push_back(z - 0.5f);

  mesh_data->uv.push_back(0.f);
  mesh_data->uv.push_back(0.f);

  // right-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(-1.f);

  mesh_data->points.push_back(x + 0.5f);
  mesh_data->points.push_back(y - 0.5f);
  mesh_data->points.push_back(z - 0.5f);

  mesh_data->uv.push_back(0.f);
  mesh_data->uv.push_back(0.f);

  // left-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(-1.f);

  mesh_data->points.push_back(x - 0.5f);
  mesh_data->points.push_back(y - 0.5f);
  mesh_data->points.push_back(z - 0.5f);

  mesh_data->uv.push_back(0.f);
  mesh_data->uv.push_back(0.f);
}

void ChunkSystem::addBehindFace(MeshData* mesh_data, float x, float y, float z,
                                Block::BlockType type) {
  // left-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);

  mesh_data->points.push_back(x - 0.5f);
  mesh_data->points.push_back(y + 0.5f);
  mesh_data->points.push_back(z + 0.5f);

  mesh_data->uv.push_back(0.f);
  mesh_data->uv.push_back(0.f);

  // left-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);

  mesh_data->points.push_back(x - 0.5f);
  mesh_data->points.push_back(y - 0.5f);
  mesh_data->points.push_back(z + 0.5f);

  mesh_data->uv.push_back(0.f);
  mesh_data->uv.push_back(0.f);

  // right-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);

  mesh_data->points.push_back(x + 0.5f);
  mesh_data->points.push_back(y + 0.5f);
  mesh_data->points.push_back(z + 0.5f);

  mesh_data->uv.push_back(0.f);
  mesh_data->uv.push_back(0.f);

  // left-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);

  mesh_data->points.push_back(x - 0.5f);
  mesh_data->points.push_back(y - 0.5f);
  mesh_data->points.push_back(z + 0.5f);

  mesh_data->uv.push_back(0.f);
  mesh_data->uv.push_back(0.f);

  // right-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);

  mesh_data->points.push_back(x + 0.5f);
  mesh_data->points.push_back(y - 0.5f);
  mesh_data->points.push_back(z + 0.5f);

  mesh_data->uv.push_back(0.f);
  mesh_data->uv.push_back(0.f);

  // right-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);

  mesh_data->points.push_back(x + 0.5f);
  mesh_data->points.push_back(y + 0.5f);
  mesh_data->points.push_back(z + 0.5f);

  mesh_data->uv.push_back(0.f);
  mesh_data->uv.push_back(0.f);
}

void ChunkSystem::addLeftFace(MeshData* mesh_data, float x, float y, float z,
                              Block::BlockType type) {
  // left-top
  mesh_data->normal.push_back(-1.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);

  mesh_data->points.push_back(x - 0.5f);
  mesh_data->points.push_back(y + 0.5f);
  mesh_data->points.push_back(z + 0.5f);

  mesh_data->uv.push_back(0.f);
  mesh_data->uv.push_back(0.f);

  // left-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);

  mesh_data->points.push_back(x - 0.5f);
  mesh_data->points.push_back(y - 0.5f);
  mesh_data->points.push_back(z + 0.5f);

  mesh_data->uv.push_back(0.f);
  mesh_data->uv.push_back(0.f);

  // right-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);

  mesh_data->points.push_back(x + 0.5f);
  mesh_data->points.push_back(y + 0.5f);
  mesh_data->points.push_back(z - 0.5f);

  mesh_data->uv.push_back(0.f);
  mesh_data->uv.push_back(0.f);

  // left-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);

  mesh_data->points.push_back(x - 0.5f);
  mesh_data->points.push_back(y - 0.5f);
  mesh_data->points.push_back(z + 0.5f);

  mesh_data->uv.push_back(0.f);
  mesh_data->uv.push_back(0.f);

  // right-bottom
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);

  mesh_data->points.push_back(x + 0.5f);
  mesh_data->points.push_back(y - 0.5f);
  mesh_data->points.push_back(z + 0.5f);

  mesh_data->uv.push_back(0.f);
  mesh_data->uv.push_back(0.f);

  // right-top
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(0.f);
  mesh_data->normal.push_back(1.f);

  mesh_data->points.push_back(x + 0.5f);
  mesh_data->points.push_back(y + 0.5f);
  mesh_data->points.push_back(z + 0.5f);

  mesh_data->uv.push_back(0.f);
  mesh_data->uv.push_back(0.f);
}

void ChunkSystem::addRightFace(MeshData* mesh_data, float x, float y, float z,
                              Block::BlockType type) {}

void ChunkSystem::addTopFace(MeshData* mesh_data, float x, float y, float z,
                              Block::BlockType type) {}

void ChunkSystem::addBottomFace(MeshData* mesh_data, float x, float y, float z,
                              Block::BlockType type) {}