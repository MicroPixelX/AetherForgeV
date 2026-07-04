#ifndef AETHER_WORLD_WORLD_HPP
#define AETHER_WORLD_WORLD_HPP

#include "world/blocks.hpp"
#include "world/chunk.hpp"

#include <unordered_map>
#include <memory>

namespace aether {

struct ChunkCoordHash {
    std::size_t operator()(const ChunkCoord &c) const noexcept {
        const std::uint64_t h =
            (static_cast<std::uint64_t>(static_cast<std::uint32_t>(c.x)) << 32) ^
            (static_cast<std::uint64_t>(static_cast<std::uint32_t>(c.y)) << 16) ^
            static_cast<std::uint64_t>(static_cast<std::uint32_t>(c.z));
        return std::hash<std::uint64_t>{}(h);
    }
};

// World-to-chunk and world-to-local voxel math.
inline ChunkCoord world_to_chunk(int vx, int vy, int vz) {
    const auto div = [](int v) {
        const int d = v % kChunkSizeX;
        return (v < 0 && d != 0) ? (v / kChunkSizeX - 1) : (v / kChunkSizeX);
    };
    return {div(vx), div(vy), div(vz)};
}

inline LocalVoxel world_to_local(int vx, int vy, int vz) {
    const auto mod = [](int v) {
        const int m = v % kChunkSizeX;
        return (m < 0) ? m + kChunkSizeX : m;
    };
    return {mod(vx), mod(vy), mod(vz)};
}

class World {
public:
    World();

    BlockId get_block(int x, int y, int z) const;
    void set_block(int x, int y, int z, BlockId id);

    const Chunk *get_chunk(const ChunkCoord &c) const;
    Chunk *get_chunk(const ChunkCoord &c);
    const Chunk *get_chunk_or_null(const ChunkCoord &c) const;
    Chunk *get_chunk_or_null(const ChunkCoord &c);

    Chunk &ensure_chunk(const ChunkCoord &c);

    std::size_t chunk_count() const { return chunks_.size(); }

private:
    std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkCoordHash> chunks_;
};

} // namespace aether

#endif // AETHER_WORLD_WORLD_HPP
