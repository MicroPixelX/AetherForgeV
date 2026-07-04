#ifndef AETHER_WORLD_BLOCKS_HPP
#define AETHER_WORLD_BLOCKS_HPP

#include <cstdint>

namespace aether {

// Keep these dense and stable: they are persisted to save files and sent over the wire.
enum class BlockId : std::uint16_t {
    Air = 0,
    Grass = 1,
    Dirt = 2,
    Stone = 3,
    Sand = 4,
    Water = 5,
    Wood = 6,
    Leaves = 7,
    Bedrock = 8,
    Planks = 9,
    Cobblestone = 10,
    Glass = 11,
    Count,
};

struct BlockProperties {
    BlockId id;
    const char *name;
    bool solid;       // has collision
    bool transparent; // neighbours should still draw the shared face
    bool opaque_cube; // mesh face-culls against same-id opaque neighbours
};

// Compile-time table indexed by BlockId. Air must always be index 0.
const BlockProperties &block_properties(BlockId id);

inline bool is_air(BlockId id) { return id == BlockId::Air; }
inline bool is_renderable(BlockId id) { return id != BlockId::Air; }

} // namespace aether

#endif // AETHER_WORLD_BLOCKS_HPP
