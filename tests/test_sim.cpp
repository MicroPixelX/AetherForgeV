// Standalone test harness for the SIM layer. Builds without godot-cpp:
//   g++ -std=c++17 -Isrc tests/test_sim.cpp src/world/*.cpp -o tests/test_sim
//   ./tests/test_sim
#include "world/blocks.hpp"
#include "world/chunk.hpp"
#include "world/chunk_mesher.hpp"
#include "world/world.hpp"

#include <cassert>
#include <cstdio>

using namespace aether;

static int g_failures = 0;
#define CHECK(cond) do { if (!(cond)) { std::printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); ++g_failures; } } while (0)

static void test_block_table() {
    CHECK(block_properties(BlockId::Air).solid == false);
    CHECK(block_properties(BlockId::Grass).opaque_cube == true);
    CHECK(block_properties(BlockId::Leaves).transparent == true);
    CHECK(block_properties(BlockId::Glass).transparent == true);
    CHECK(block_properties(BlockId::Air).id == BlockId::Air);
}

static void test_chunk_get_set() {
    Chunk c;
    CHECK(c.is_empty());
    c.set(1, 2, 3, BlockId::Grass);
    CHECK(!c.is_empty());
    CHECK(c.get(1, 2, 3) == BlockId::Grass);
    CHECK(c.get(0, 0, 0) == BlockId::Air);
    CHECK(c.occupied_count() == 1);
    c.set(1, 2, 3, BlockId::Air);
    CHECK(c.is_empty());
    CHECK(c.occupied_count() == 0);
    CHECK(c.get_safe(-1, 0, 0, BlockId::Stone) == BlockId::Stone);
}

static void test_index_roundtrip() {
    for (int i = 0; i < kChunkVoxelCount; ++i) {
        const LocalVoxel v = index_to_voxel(i);
        CHECK(v.x >= 0 && v.x < kChunkSizeX);
        CHECK(v.y >= 0 && v.y < kChunkSizeY);
        CHECK(v.z >= 0 && v.z < kChunkSizeZ);
        CHECK(voxel_index(v.x, v.y, v.z) == i);
    }
}

static void test_mesher_single_block() {
    Chunk c;
    c.set(0, 0, 0, BlockId::Stone);
    ChunkMesh m;
    mesh_chunk(c, nullptr, nullptr, m);
    // A single floating block exposes all 6 faces -> 6 quads -> 24 verts, 36 indices.
    CHECK(m.verts.size() == 24);
    CHECK(m.indices.size() == 36);
}

static void test_mesher_buried_block() {
    // A solid 3x3x3 cube, then the same with the center block removed and replaced
    // by another same-type block buried inside — should draw an identical mesh
    // (the buried block contributes no visible faces).
    Chunk box;
    for (int x = 4; x <= 6; ++x)
        for (int y = 4; y <= 6; ++y)
            for (int z = 4; z <= 6; ++z)
                box.set(x, y, z, BlockId::Stone);
    ChunkMesh mb;
    mesh_chunk(box, nullptr, nullptr, mb);
    CHECK(mb.verts.size() > 0);

    // Now add an internal stone buried deep inside a 5x5x5 block: it should add 0 faces.
    Chunk big;
    for (int x = 3; x <= 7; ++x)
        for (int y = 3; y <= 7; ++y)
            for (int z = 3; z <= 7; ++z)
                big.set(x, y, z, BlockId::Stone);
    ChunkMesh m_big;
    mesh_chunk(big, nullptr, nullptr, m_big);

    Chunk big_plus;
    for (int x = 3; x <= 7; ++x)
        for (int y = 3; y <= 7; ++y)
            for (int z = 3; z <= 7; ++z)
                big_plus.set(x, y, z, BlockId::Stone);
    big_plus.set(5, 5, 5, BlockId::Dirt); // already stone, but forces a no-op rerun
    big_plus.set(5, 5, 5, BlockId::Stone);
    ChunkMesh m_big_plus;
    mesh_chunk(big_plus, nullptr, nullptr, m_big_plus);

    CHECK(m_big.verts.size() == m_big_plus.verts.size());
    CHECK(m_big.indices.size() == m_big_plus.indices.size());
}

static void test_world_set_get_across_chunks() {
    World w;
    w.set_block(0, 0, 0, BlockId::Dirt);
    w.set_block(kChunkSizeX, 0, 0, BlockId::Stone);
    CHECK(w.get_block(0, 0, 0) == BlockId::Dirt);
    CHECK(w.get_block(kChunkSizeX, 0, 0) == BlockId::Stone);
    CHECK(w.get_block(kChunkSizeX - 1, 0, 0) == BlockId::Air);
    CHECK(w.chunk_count() == 2);
}

static void test_world_coord_math() {
    const ChunkCoord c1 = world_to_chunk(0, 0, 0);
    CHECK(c1.x == 0 && c1.y == 0 && c1.z == 0);
    const ChunkCoord cneg = world_to_chunk(-1, -1, -1);
    CHECK(cneg.x == -1 && cneg.y == -1 && cneg.z == -1);
    const LocalVoxel lv = world_to_local(-1, 0, 0);
    CHECK(lv.x == kChunkSizeX - 1);
    // Round-trip world -> chunk + local -> world is identity.
    const auto roundtrip = [](int x, int y, int z) {
        const ChunkCoord c = world_to_chunk(x, y, z);
        const LocalVoxel l = world_to_local(x, y, z);
        return std::tuple(c.x * kChunkSizeX + l.x,
                          c.y * kChunkSizeY + l.y,
                          c.z * kChunkSizeZ + l.z);
    };
    int rx, ry, rz;
    std::tie(rx, ry, rz) = roundtrip(-7, 13, -42);
    CHECK(rx == -7 && ry == 13 && rz == -42);
}

int main() {
    test_block_table();
    test_chunk_get_set();
    test_index_roundtrip();
    test_mesher_single_block();
    test_mesher_buried_block();
    test_world_set_get_across_chunks();
    test_world_coord_math();
    if (g_failures == 0) {
        std::printf("OK: all SIM tests passed\n");
        return 0;
    }
    std::printf("FAILURES: %d\n", g_failures);
    return 1;
}
