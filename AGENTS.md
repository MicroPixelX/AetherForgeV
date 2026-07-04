# AGENTS.md — AetherForgeV

Hands-on guide for any agent (AI or human) working in this repo.

## Project

C++ voxel sandbox on Godot 4 (GDExtension). Exports to desktop + browser.
`src/` is the authoritative game SIM layer and must stay free of Godot types so it can later run server-side. The GDExtension bridge in `register_types.cpp` is the only place Godot types should leak in.

## Commands

### Build C++ library
```sh
# Debug desktop build (Linux)
scons platform=linuxbsd target=template_debug -j$(nproc)
# Release web build (Emscripten must be active: `source <emsdk>/emsdk_env.sh`)
scons platform=web target=template_release
```
Requires godot-cpp cloned at `../godotcpp` (or pass `GODOTCPP=/path`):
```sh
git clone -b godot-4.3-stable https://github.com/godotengine/godot-cpp ../godotcpp
```

### Run / export from CLI
```sh
godot4 --editor                       # open editor
godot4                                # play in editor run
godot4 --export-release "Linux Desktop"
godot4 --export-release "Web"
```

## Lint / typecheck

No C++ linter is wired yet. When adding one, prefer `clang-format` (`-style=file`, 4-space indent, `BasedOnStyle: LLVM`) and `cppcheck --enable=warning,style src/`. Add the command here so other agents run it:
```sh
clang-format --dry-run -Werror src/**/*.{cpp,hpp}
```

## Conventions

- **C++17**, 4-space indent, `snake_case` for files, `PascalCase` for classes.
- Headers use `#ifndef AETHER_<PATH>_HPP` guards.
- All game classes live in namespace `aether`.
- Never include Godot headers from non-registration files; keep SIM pure.
- No comments unless requested. No emojis in code.
- Don't commit built libraries (`bin/` is gitignored).
- Don't amend or force-push without explicit request.

## Where things go

- New C++ source → `src/<module>/<file>.cpp` + `.hpp`, add to `register_types.cpp` under `MODULE_INITIALIZATION_LEVEL_SCENE`.
- New libraries/binaries after build → `addons/aetherforgev/bin/` (gitignored); update `aetherforgev.gdextension` `[libraries]` map if naming changes.
- New export targets → `export_presets.cfg`.
