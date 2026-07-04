#ifndef AETHER_BRIDGE_CHUNK_MESH_BRIDGE_HPP
#define AETHER_BRIDGE_CHUNK_MESH_BRIDGE_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>

#include "world/chunk.hpp"

namespace aether {

// Pure glue between the engine-agnostic SIM layer and Godot.
// Input: a PackedByteArray of kChunkVoxelCount bytes (one BlockId per voxel,
//        little-endian uint16 = two bytes if you want full palette — kept simple
//        here as low byte = BlockId, high byte ignored during demo).
// Output: vertex/index arrays ready for an ArrayMesh (positions, normals, uvs, indices).
class ChunkMeshBridge : public godot::Node {
    GDCLASS(ChunkMeshBridge, godot::Node)

protected:
    static void _bind_methods();

public:
    ChunkMeshBridge();
    ~ChunkMeshBridge() override;

    godot::PackedFloat32Array build_mesh_positions(const godot::PackedByteArray &voxels);
    godot::PackedFloat32Array build_mesh_normals(const godot::PackedByteArray &voxels);
    godot::PackedFloat32Array build_mesh_uvs(const godot::PackedByteArray &voxels);
    godot::PackedInt32Array build_mesh_indices(const godot::PackedByteArray &voxels);

    int chunk_voxel_count() const;
};

} // namespace aether

#endif // AETHER_BRIDGE_CHUNK_MESH_BRIDGE_HPP
