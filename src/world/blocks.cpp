#include "blocks.hpp"

#include <array>
#include <cstddef>

namespace aether {

namespace {

constexpr std::array<BlockProperties, static_cast<std::size_t>(BlockId::Count)> k_table = {{
    {BlockId::Air,        "air",        false, true,  false},
    {BlockId::Grass,      "grass",      true,  false, true},
    {BlockId::Dirt,       "dirt",       true,  false, true},
    {BlockId::Stone,      "stone",      true,  false, true},
    {BlockId::Sand,       "sand",       true,  false, true},
    {BlockId::Water,      "water",      false, true,  false},
    {BlockId::Wood,       "wood",       true,  false, true},
    {BlockId::Leaves,     "leaves",     true,  true,  false},
    {BlockId::Bedrock,    "bedrock",    true,  false, true},
    {BlockId::Planks,     "planks",     true,  false, true},
    {BlockId::Cobblestone,"cobblestone",true,  false, true},
    {BlockId::Glass,      "glass",      true,  true,  false},
}};

} // namespace

const BlockProperties &block_properties(BlockId id) {
    const auto i = static_cast<std::size_t>(id);
    return k_table[i < k_table.size() ? i : 0];
}

} // namespace aether
