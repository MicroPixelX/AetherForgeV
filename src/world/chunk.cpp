#include "world/chunk.hpp"

#include <algorithm>

namespace aether {

void Chunk::clear() {
    voxels_.fill(0);
    occupied_ = 0;
}

BlockId Chunk::get(int x, int y, int z) const {
    return static_cast<BlockId>(voxels_[voxel_index(x, y, z)]);
}

BlockId Chunk::get_safe(int x, int y, int z, BlockId fallback) const {
    if (!in_chunk_bounds(x, y, z)) {
        return fallback;
    }
    return get(x, y, z);
}

void Chunk::set(int x, int y, int z, BlockId id) {
    const int i = voxel_index(x, y, z);
    const bool was_air = voxels_[i] == 0;
    const bool is_air = static_cast<std::uint16_t>(id) == 0;
    voxels_[i] = static_cast<std::uint16_t>(id);
    if (was_air && !is_air) ++occupied_;
    else if (!was_air && is_air) --occupied_;
}

} // namespace aether
