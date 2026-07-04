#ifndef AETHER_WORLD_CHUNK_MESHER_HPP
#define AETHER_WORLD_CHUNK_MESHER_HPP

#include "world/blocks.hpp"
#include "world/chunk.hpp"

#include <cstdint>
#include <vector>

namespace aether {

// Output of one meshing pass. All arrays are parallel and indexed by `indices`.
// Position is in chunk-local space [0, kChunkSize*). Normals are unit vectors.
// `block_id` per vertex lets the renderer (or greedy merge) group faces by block.
struct ChunkMesh {
    struct Vert {
        float x, y, z;       // position
        float nx, ny, nz;    // normal
        float u, v;          // uv in tile-atlas space [0,1] per face
        std::uint16_t block; // BlockId, for per-block dispatch / texture choice
    };

    std::vector<Vert> verts;
    std::vector<std::uint32_t> indices;

    void clear() {
        verts.clear();
        indices.clear();
    }

    bool empty() const { return verts.empty(); }

    std::size_t vertex_bytes() const { return verts.size() * sizeof(Vert); }
    std::size_t index_bytes() const { return indices.size() * sizeof(std::uint32_t); }
};

// Neighbor lookup that crosses chunk borders via a callback. Returning a
// non-Air block lets the mesher skip the boundary face. Use nullptr when
// the chunk has no neighbours yet (treats out-of-bounds as air).
using NeighborLookup = BlockId (*)(int gx, int gy, int gz, void *user);

// Builds a face-culled mesh for `chunk`. Emits a quad per exposed face
// (the simplest correct mesher; greedy merge is a later optimisation).
// `origin` is the world-space chunk origin position to add to vertices.
// `neighbor` may be null; otherwise it must answer queries at chunk-local
// coordinates possibly outside [0,kChunkSize*) — i.e. crossing the border.
void mesh_chunk(const Chunk &chunk,
                NeighborLookup neighbor,
                void *neighbor_user,
                ChunkMesh &out);

} // namespace aether

#endif // AETHER_WORLD_CHUNK_MESHER_HPP
