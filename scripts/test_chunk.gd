extends Node3D

const SIZE_X := 32
const SIZE_Y := 32
const SIZE_Z := 32

func _ready() -> void:
	# Bridge node lives in the same scene tree; grab the autoload-free singleton-less way.
	var bridge := _find_bridge()
	if bridge == null:
		printerr("[AetherForgeV] ChunkMeshBridge not found — GDExtension build produced no symbols?")
		return

	# Build a flat-ish test terrain: grass on top, dirt below, a stone pillar.
	var voxels := PackedByteArray()
	voxels.resize(bridge.chunk_voxel_count())
	voxels.fill(0)
	for z in range(SIZE_Z):
		for x in range(SIZE_X):
			var h := 4 + int(round(sin(x * 0.3) * 2.0 + cos(z * 0.3) * 2.0))
			h = clamp(h, 1, SIZE_Y - 4)
			for y in range(h + 1):
				var id := 1 # grass top
				if y < h:
					id = 2 # dirt
				if y == 0:
					id = 8 # bedrock
				voxels[z * SIZE_X * SIZE_Y + y * SIZE_X + x] = id

	# Pillar in the middle as a visual landmark.
	for y in range(6, 12):
		for x in range(15, 17):
			for z in range(15, 17):
				voxels[z * SIZE_X * SIZE_Y + y * SIZE_X + x] = 3 # stone

	var positions := bridge.build_mesh_positions(voxels)
	var normals   := bridge.build_mesh_normals(voxels)
	var uvs       := bridge.build_mesh_uvs(voxels)
	var indices   := bridge.build_mesh_indices(voxels)

	if positions.size() == 0:
		printerr("[AetherForgeV] Mesher produced no vertices for test chunk.")
		return

	var arr_mesh := ArrayMesh.new()
	var arrays := []
	arrays.resize(Mesh.ARRAY_MAX)
	arrays[Mesh.ARRAY_VERTEX] = PackedVector3Array(_zip3(positions))
	arrays[Mesh.ARRAY_NORMAL] = PackedVector3Array(_zip3(normals))
	arrays[Mesh.ARRAY_TEX_UV] = PackedVector2Array(_zip2(uvs))
	arrays[Mesh.ARRAY_INDEX]  = PackedInt32Array(indices)
	arr_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)

	var mi := MeshInstance3D.new()
	mi.mesh = arr_mesh
	mi.material_override = StandardMaterial3D.new()
	mi.material_override.albedo_color = Color(0.55, 0.78, 0.45)
	add_child(mi)

	print("[AetherForgeV] Test chunk meshed: verts=%d tris=%d" % [positions.size()/3, indices.size()/3])

func _find_bridge() -> Node:
	for c in get_children():
		if c.is_class("ChunkMeshBridge") or c.get_class() == "ChunkMeshBridge":
			return c
	return null

func _zip3(f: PackedFloat32Array) -> PackedVector3Array:
	var v := PackedVector3Array()
	v.resize(f.size() / 3)
	for i in range(v.size()):
		v[i] = Vector3(f[i*3], f[i*3+1], f[i*3+2])
	return v

func _zip2(f: PackedFloat32Array) -> PackedVector2Array:
	var v := PackedVector2Array()
	v.resize(f.size() / 2)
	for i in range(v.size()):
		v[i] = Vector2(f[i*2], f[i*2+1])
	return v
