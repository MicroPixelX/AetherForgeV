#include "world/chunk_mesher.hpp"

#include <array>
#include <cstdint>

namespace aether {

namespace {

struct Face {
    int dx, dy, dz;          // neighbour offset
    float nx, ny, nz;        // face normal
    float corner[4][3];      // 4 corner offsets (CCW viewed from outside)
};

// 6 cube faces, corners ordered so a quad (0,1,2,3) triangulates as (0,1,2)+(0,2,3).
// Position of each corner is the corner of the unit cube relative to (x,y,z).
constexpr std::array<Face, 6> k_faces = {{
    // +X
    { 1, 0, 0,  1, 0, 0,
      {{1,0,1},{1,0,0},{1,1,0},{1,1,1}} },
    // -X
    {-1, 0, 0, -1, 0, 0,
      {{0,0,0},{0,0,1},{0,1,1},{0,1,0}} },
    // +Y
    { 0, 1, 0,  0, 1, 0,
      {{0,1,1},{1,1,1},{1,1,0},{0,1,0}} },
    // -Y
    { 0,-1, 0,  0,-1, 0,
      {{0,0,0},{1,0,0},{1,0,1},{0,0,1}} },
    // +Z
    { 0, 0, 1,  0, 0, 1,
      {{0,0,1},{1,0,1},{1,1,1},{0,1,1}} },
    // -Z
    { 0, 0,-1,  0, 0,-1,
      {{1,0,0},{0,0,0},{0,1,0},{1,1,0}} },
}};

// Per-face UVs matching corner order above (tl, tr, br, bl on the quad).
constexpr float k_face_uv[6][4][2] = {
    { {0,0},{1,0},{1,1},{0,1} },
    { {0,0},{1,0},{1,1},{0,1} },
    { {0,0},{1,0},{1,1},{0,1} },
    { {0,0},{1,0},{1,1},{0,1} },
    { {0,0},{1,0},{1,1},{0,1} },
    { {0,0},{1,0},{1,1},{0,1} },
};

bool should_draw_face(BlockId self, BlockId neighbour) {
    if (is_air(neighbour)) return true;
    const auto &n = block_properties(neighbour);
    if (n.opaque_cube && neighbour != self) return true;
    return false;
}

} // namespace

void mesh_chunk(const Chunk &chunk,
                NeighborLookup neighbor,
                void *neighbor_user,
                ChunkMesh &out) {
    out.clear();

    for (int y = 0; y < kChunkSizeY; ++y) {
        for (int z = 0; z < kChunkSizeZ; ++z) {
            for (int x = 0; x < kChunkSizeX; ++x) {
                const BlockId self = chunk.get(x, y, z);
                if (!is_renderable(self)) continue;

                for (std::size_t f = 0; f < k_faces.size(); ++f) {
                    const Face &face = k_faces[f];

                    const int nx = x + face.dx;
                    const int ny = y + face.dy;
                    const int nz = z + face.dz;

                    BlockId neighbour_id;
                    if (in_chunk_bounds(nx, ny, nz)) {
                        neighbour_id = chunk.get(nx, ny, nz);
                    } else if (neighbor) {
                        // Caller resolves cross-chunk queries at global voxel coords.
                        neighbour_id = neighbor(nx, ny, nz, neighbor_user);
                    } else {
                        neighbour_id = BlockId::Air;
                    }

                    if (!should_draw_face(self, neighbour_id)) continue;

                    const std::uint32_t base = static_cast<std::uint32_t>(out.verts.size());
                    for (int c = 0; c < 4; ++c) {
                        ChunkMesh::Vert v{};
                        v.x = static_cast<float>(x) + face.corner[c][0];
                        v.y = static_cast<float>(y) + face.corner[c][1];
                        v.z = static_cast<float>(z) + face.corner[c][2];
                        v.nx = face.nx; v.ny = face.ny; v.nz = face.nz;
                        v.u = k_face_uv[f][c][0];
                        v.v = k_face_uv[f][c][1];
                        v.block = static_cast<std::uint16_t>(self);
                        out.verts.push_back(v);
                    }
                    out.indices.push_back(base + 0);
                    out.indices.push_back(base + 1);
                    out.indices.push_back(base + 2);
                    out.indices.push_back(base + 0);
                    out.indices.push_back(base + 2);
                    out.indices.push_back(base + 3);
                }
            }
        }
    }
}

} // namespace aether
