#include "world/chunk_mesher.h"

#include <array>
#include <utility>

namespace {

struct Position {
  float x;
  float y;
  float z;
};

using Face = std::array<Position, 6>;
using TextureCoordinates = std::array<std::pair<float, float>, 6>;

constexpr Face kFrontFace{{{0, 1, 0}, {1, 1, 0}, {0, 0, 0},
                           {1, 1, 0}, {1, 0, 0}, {0, 0, 0}}};
constexpr Face kBackFace{{{0, 1, 1}, {0, 0, 1}, {1, 1, 1},
                          {0, 0, 1}, {1, 0, 1}, {1, 1, 1}}};
constexpr Face kLeftFace{{{0, 1, 0}, {0, 0, 0}, {0, 1, 1},
                          {0, 0, 0}, {0, 0, 1}, {0, 1, 1}}};
constexpr Face kRightFace{{{1, 1, 1}, {1, 0, 1}, {1, 1, 0},
                           {1, 0, 1}, {1, 0, 0}, {1, 1, 0}}};
constexpr Face kTopFace{{{0, 1, 0}, {0, 1, 1}, {1, 1, 0},
                         {0, 1, 1}, {1, 1, 1}, {1, 1, 0}}};
constexpr Face kBottomFace{{{0, 0, 1}, {0, 0, 0}, {1, 0, 1},
                            {0, 0, 0}, {1, 0, 0}, {1, 0, 1}}};

constexpr TextureCoordinates kTextureCoordinates{{
    {0, 1}, {0, 0}, {1, 1}, {0, 0}, {1, 0}, {1, 1}}};
constexpr TextureCoordinates kFlippedTextureCoordinates{{
    {1, 1}, {0, 1}, {1, 0}, {0, 1}, {0, 0}, {1, 0}}};

std::pair<float, float> textureOffset(Block::BlockType type) {
  switch (type) {
    case Block::BlockType::DIRT:
      return {1.0f, 3.0f};
    case Block::BlockType::STONE:
      return {0.0f, 3.0f};
    case Block::BlockType::AIR:
      return {0.0f, 0.0f};
  }
  return {0.0f, 0.0f};
}

void appendFace(MeshData& mesh_data, const Face& face,
                const TextureCoordinates& texture_coordinates, float x,
                float y, float z, float texture_offset_x,
                float texture_offset_y, const Position& normal) {
  for (std::size_t index = 0; index < face.size(); ++index) {
    mesh_data.positions.push_back(x + face[index].x);
    mesh_data.positions.push_back(y + face[index].y);
    mesh_data.positions.push_back(z + face[index].z);

    mesh_data.texture_coordinates.push_back(
        texture_offset_x + texture_coordinates[index].first);
    mesh_data.texture_coordinates.push_back(
        texture_offset_y + texture_coordinates[index].second);

    mesh_data.normals.push_back(normal.x);
    mesh_data.normals.push_back(normal.y);
    mesh_data.normals.push_back(normal.z);
  }
}

bool isAir(const ChunkBlockSet& block_set, int x, int y, int z) {
  return block_set.blocks[y][z][x].block_type == Block::BlockType::AIR;
}

}  // namespace

MeshData ChunkMesher::build(const ChunkBlockSet& block_set) const {
  MeshData mesh_data;

  for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
    for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
      for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        const Block::BlockType type = block_set.blocks[y][z][x].block_type;
        if (type == Block::BlockType::AIR) {
          continue;
        }

        const auto [texture_x, texture_y] = textureOffset(type);
        if (x == 0 || isAir(block_set, x - 1, y, z)) {
          appendFace(mesh_data, kLeftFace, kTextureCoordinates, x, y, z,
                     texture_x, texture_y, {-1, 0, 0});
        }
        if (z == 0 || isAir(block_set, x, y, z - 1)) {
          appendFace(mesh_data, kFrontFace, kFlippedTextureCoordinates, x, y,
                     z, texture_x, texture_y, {0, 0, -1});
        }
        if (y == 0 || isAir(block_set, x, y - 1, z)) {
          appendFace(mesh_data, kBottomFace, kTextureCoordinates, x, y, z,
                     texture_x, texture_y, {0, -1, 0});
        }
        if (x == CHUNK_SIZE_X - 1 || isAir(block_set, x + 1, y, z)) {
          appendFace(mesh_data, kRightFace, kTextureCoordinates, x, y, z,
                     texture_x, texture_y, {1, 0, 0});
        }
        if (z == CHUNK_SIZE_Z - 1 || isAir(block_set, x, y, z + 1)) {
          appendFace(mesh_data, kBackFace, kTextureCoordinates, x, y, z,
                     texture_x, texture_y, {0, 0, 1});
        }
        if (y == CHUNK_SIZE_Y - 1 || isAir(block_set, x, y + 1, z)) {
          appendFace(mesh_data, kTopFace, kTextureCoordinates, x, y, z,
                     texture_x, texture_y, {0, 1, 0});
        }
      }
    }
  }

  return mesh_data;
}
