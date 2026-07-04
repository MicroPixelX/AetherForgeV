#include "bridge/chunk_mesh_bridge.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "world/blocks.hpp"
#include "world/chunk.hpp"
#include "world/chunk_mesher.hpp"

#include <cstring>

namespace aether {

namespace {

// Fill a Chunk from a Godot byte array (one byte per voxel, low byte = BlockId).
void load_voxels(const godot::PackedByteArray &bytes, Chunk &out) {
    out.clear();
    const auto *data = bytes.ptr();
    const int n = bytes.size() < kChunkVoxelCount ? bytes.size() : kChunkVoxelCount;
    if (n <= 0) return;
    for (int i = 0; i < n; ++i) {
        const uint8_t v = static_cast<uint8_t>(data[i]);
        if (v != 0) {
            const LocalVoxel lv = index_to_voxel(i);
            out.set(lv.x, lv.y, lv.z, static_cast<BlockId>(v));
        }
    }
}

} // namespace

void ChunkMeshBridge::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("build_mesh_positions", "voxels"),
                               &ChunkMeshBridge::build_mesh_positions);
    godot::ClassDB::bind_method(godot::D_METHOD("build_mesh_normals", "voxels"),
                               &ChunkMeshBridge::build_mesh_normals);
    godot::ClassDB::bind_method(godot::D_METHOD("build_mesh_uvs", "voxels"),
                               &ChunkMeshBridge::build_mesh_uvs);
    godot::ClassDB::bind_method(godot::D_METHOD("build_mesh_indices", "voxels"),
                               &ChunkMeshBridge::build_mesh_indices);
    godot::ClassDB::bind_method(godot::D_METHOD("chunk_voxel_count"),
                               &ChunkMeshBridge::chunk_voxel_count);
}

ChunkMeshBridge::ChunkMeshBridge() = default;
ChunkMeshBridge::~ChunkMeshBridge() = default;

int ChunkMeshBridge::chunk_voxel_count() const {
    return kChunkVoxelCount;
}

namespace {

// Single source of meshing so the four arrays stay consistent. Returns the
// cached mesh for this call only. `voxels` is the authoritative input.
ChunkMesh mesh_from_bytes(const godot::PackedByteArray &voxels) {
    Chunk chunk;
    load_voxels(voxels, chunk);
    ChunkMesh mesh;
    mesh_chunk(chunk, nullptr, nullptr, mesh);
    return mesh;
}

} // namespace

godot::PackedFloat32Array
ChunkMeshBridge::build_mesh_positions(const godot::PackedByteArray &voxels) {
    const ChunkMesh mesh = mesh_from_bytes(voxels);
    godot::PackedFloat32Array out;
    out.resize(static_cast<int64_t>(mesh.verts.size() * 3));
    float *w = out.ptrw();
    for (std::size_t i = 0; i < mesh.verts.size(); ++i) {
        const auto &v = mesh.verts[i];
        w[i * 3 + 0] = v.x;
        w[i * 3 + 1] = v.y;
        w[i * 3 + 2] = v.z;
    }
    return out;
}

godot::PackedFloat32Array
ChunkMeshBridge::build_mesh_normals(const godot::PackedByteArray &voxels) {
    const ChunkMesh mesh = mesh_from_bytes(voxels);
    godot::PackedFloat32Array out;
    out.resize(static_cast<int64_t>(mesh.verts.size() * 3));
    float *w = out.ptrw();
    for (std::size_t i = 0; i < mesh.verts.size(); ++i) {
        const auto &v = mesh.verts[i];
        w[i * 3 + 0] = v.nx;
        w[i * 3 + 1] = v.ny;
        w[i * 3 + 2] = v.nz;
    }
    return out;
}

godot::PackedFloat32Array
ChunkMeshBridge::build_mesh_uvs(const godot::PackedByteArray &voxels) {
    const ChunkMesh mesh = mesh_from_bytes(voxels);
    godot::PackedFloat32Array out;
    out.resize(static_cast<int64_t>(mesh.verts.size() * 2));
    float *w = out.ptrw();
    for (std::size_t i = 0; i < mesh.verts.size(); ++i) {
        const auto &v = mesh.verts[i];
        w[i * 2 + 0] = v.u;
        w[i * 2 + 1] = v.v;
    }
    return out;
}

godot::PackedInt32Array
ChunkMeshBridge::build_mesh_indices(const godot::PackedByteArray &voxels) {
    const ChunkMesh mesh = mesh_from_bytes(voxels);
    godot::PackedInt32Array out;
    out.resize(static_cast<int64_t>(mesh.indices.size()));
    int32_t *w = out.ptrw();
    for (std::size_t i = 0; i < mesh.indices.size(); ++i) {
        w[i] = static_cast<int32_t>(mesh.indices[i]);
    }
    return out;
}

} // namespace aether
