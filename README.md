# AetherForgeV

A voxel sandbox game (Minecraft-style) built in **C++ on Godot 4 (GDExtension)**, exporting to both **desktop** (Linux/Windows/macOS) and the **browser** (WebAssembly).

## Why Godot + GDExtension over raw Vulkan

- Same C++ project exports to PC and Web — no dual render paths.
- C++ GDExtension keeps chunk meshing, worldgen, and save logic in hot code.
- Godot handles windowing, input, audio, lighting, and export templates.

## Repo layout

```
AetherForgeV/
├─ project.godot              # Godot project (main scene: scenes/Main.tscn)
├─ export_presets.cfg         # Linux / Windows / Web export configs
├─ SConstruct                 # SCons build for the GDExtension library
├─ src/                       # C++ game code
│  ├─ register_types.cpp      # GDExtension entry (aetherforgev_library_init)
│  ├─ aether_test.cpp        # Proof node — drop once real modules land
├─ addons/aetherforgev/      # GDExtension manifest + built library bin/
└─ scenes/Main.tscn           # Entry scene (camera, light, AetherTest node)
```

## Prerequisites

1. **Godot 4.3+** (engine + export templates for Desktop and Web).
2. **SCons** (`pip install scons`) and a C++17 compiler (gcc/clang/MSVC/emscripten).
3. **godot-cpp** (binds), clone next to this repo:
   ```sh
   git clone -b godot-4.3-stable https://github.com/godotengine/godot-cpp ../godotcpp
   ```
4. For Web builds: **Emscripten** (`emsdk` activated in PATH).

## Build the C++ library

```sh
# Linux desktop (debug)
scons platform=linuxbsd target=template_debug -j$(nproc)
# Windows desktop (cross from Linux)
scons platform=windows target=template_release
# Web (wasm) — needs emscripten active
scons platform=web target=template_release
```

Built artifact lands in `addons/aetherforgev/bin/aetherforgev.<platform>.<target>.<ext>`.

## Run / export

```sh
# Editor
godot4 --editor
# Play from CLI
godot4
# Export (use the matching preset id)
godot4 --export-release "Linux Desktop"
godot4 --export-release "Web"
```

## Roadmap (MVP + crafting + saving, singleplayer-first)

1. ✅ Project bootstrap + GDExtension pipeline + cross-export config
2. ⬜ Voxel core: chunk data, face-culling/greedy meshing, load/unload radius
3. ⬜ Worldgen (FastNoiseLite), biomes, block palette
4. ⬜ Player controller + block raycast pick + place/break
5. ⬜ Inventory + simple crafting grid + recipes
6. ⬜ Save/load (chunk + player state)

Networking is intentionally deferred but the SIM layer (`src/`) is kept free of Godot types so it can later run server-authoritative.
