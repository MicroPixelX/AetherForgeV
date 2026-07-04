#ifndef AETHER_WORLD_CHUNK_HPP
#define AETHER_WORLD_CHUNK_HPP

#include "world/blocks.hpp"

#include <array>
#include <cstdint>

namespace aether {

// Chunk dimensions. 32x32x32 keeps memory flat (32 KiB at 1 byte/voxel) and
// matches a clean power-of-two so the broader mesher can later group greedily.
inline constexpr int kChunkSizeX = 32;
inline constexpr int kChunkSizeY = 32;
inline constexpr int kChunkSizeZ = 32;
inline constexpr int kChunkVoxelCount = kChunkSizeX * kChunkSizeY * kChunkSizeZ;

struct ChunkCoord {
    int x;
    int y;
    int z;

    bool operator==(const ChunkCoord &o) const { return x == o.x && y == o.y && z == o.z; }
    bool operator!=(const ChunkCoord &o) const { return !(*this == o); }
};

// Local voxel coordinate inside a chunk. Each axis in [0, kChunkSize*).
struct LocalVoxel {
    int x, y, z;
};

inline constexpr int voxel_index(int x, int y, int z) {
    return (y * kChunkSizeZ + z) * kChunkSizeX + x;
}

inline constexpr LocalVoxel index_to_voxel(int i) {
    const int x = i % kChunkSizeX;
    const int z = (i / kChunkSizeX) % kChunkSizeZ;
    const int y = i / (kChunkSizeX * kChunkSizeZ);
    return {x, y, z};
}

inline constexpr bool in_chunk_bounds(int x, int y, int z) {
    return x >= 0 && x < kChunkSizeX && y >= 0 && y < kChunkSizeY && z >= 0 && z < kChunkSizeZ;
}

class Chunk {
public:
    Chunk() = default;

    Chunk(const Chunk &) = default;
    Chunk &operator=(const Chunk &) = default;
    Chunk(Chunk &&) noexcept = default;
    Chunk &operator=(Chunk &&) noexcept = default;

    void clear();

    BlockId get(int x, int y, int z) const;
    BlockId get_safe(int x, int y, int z, BlockId fallback = BlockId::Air) const;
    void set(int x, int y, int z, BlockId id);

    bool is_empty() const { return occupied_ == 0; }
    std::uint32_t occupied_count() const { return occupied_; }

private:
    std::array<std::uint16_t, kChunkVoxelCount> voxels_{};
    std::uint32_t occupied_ = 0;
};

} // namespace aether

#endif // AETHER_WORLD_CHUNK_HPP
