#include "world/world.hpp"

#include <utility>

namespace aether {

World::World() = default;

const Chunk *World::get_chunk(const ChunkCoord &c) const {
    return get_chunk_or_null(c);
}

Chunk *World::get_chunk(const ChunkCoord &c) {
    return get_chunk_or_null(c);
}

const Chunk *World::get_chunk_or_null(const ChunkCoord &c) const {
    const auto it = chunks_.find(c);
    return it == chunks_.end() ? nullptr : it->second.get();
}

Chunk *World::get_chunk_or_null(const ChunkCoord &c) {
    const auto it = chunks_.find(c);
    return it == chunks_.end() ? nullptr : it->second.get();
}

Chunk &World::ensure_chunk(const ChunkCoord &c) {
    auto &slot = chunks_[c];
    if (!slot) slot = std::make_unique<Chunk>();
    return *slot;
}

BlockId World::get_block(int x, int y, int z) const {
    const ChunkCoord cc = world_to_chunk(x, y, z);
    const Chunk *ch = get_chunk_or_null(cc);
    if (!ch) return BlockId::Air;
    const LocalVoxel lv = world_to_local(x, y, z);
    return ch->get(lv.x, lv.y, lv.z);
}

void World::set_block(int x, int y, int z, BlockId id) {
    const ChunkCoord cc = world_to_chunk(x, y, z);
    Chunk &ch = ensure_chunk(cc);
    const LocalVoxel lv = world_to_local(x, y, z);
    ch.set(lv.x, lv.y, lv.z, id);
}

} // namespace aether
