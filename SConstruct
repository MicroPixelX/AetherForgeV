#!/usr/bin/env python
# SConstruct for the AetherForgeV GDExtension library.
# Build:
#   scons platform=linuxbsd target=template_debug -j$(nproc)
#   scons platform=windows target=template_release
#   scons platform=web    target=template_release
#
# Requires the Godotcpp sources next to this project at ../godotcpp (or set GODOTCPP=path).

import os
from pathlib import Path

# ---------------------------------------------------------------------------
# Environment
# ---------------------------------------------------------------------------
env = Environment(ENV=os.environ)

platform = ARGUMENTS.get("platform", "linuxbsd")
target = ARGUMENTS.get("target", "template_debug")
bits = ARGUMENTS.get("bits", 64)

libname = "aetherforgev"
out_dir = Dir(f"#addons/{libname}/bin")

# ---------------------------------------------------------------------------
# Locate godot-cpp
# ---------------------------------------------------------------------------
godotcpp = ARGUMENTS.get(
    "GODOTCPP",
    os.environ.get("GODOTCPP", str(Path("#..").abspath) + "/godotcpp"),
)
if not os.path.isdir(godotcpp):
    print(f"FATAL: godot-cpp not found at '{godotcpp}'. Clone https://github.com/godotengine/godot-cpp (godot-4.3-stable branch) and set GODOTCPP=path/to/godot-cpp.")
    Exit(1)

print(f"[AetherForgeV] Using godot-cpp from: {godotcpp}")

# Build godot-cpp once with the same target/profile, then link against it.
env.SConscript(
    f"{godotcpp}/SConstruct",
    variant_dir=f"#.scons_build/godot-cpp/{platform}_{target}",
    duplicate=False,
    exports={
        "env": env,
        "platform": platform,
        "target": target,
        "bits": bits,
    },
)

cpp_lib = env["godotcpp_bin"]
env.Append(LIBS=cpp_lib)

# ---------------------------------------------------------------------------
# Compiler settings
# ---------------------------------------------------------------------------
env.Append(CPPPATH=[f"{godotcpp}/gdextension", f"{godotcpp}/gen/include"])
env.Append(CPPPATH=["#src"])

if target in ("template_debug", "debug"):
    env.Append(CPPDEFINES=["DEBUG_ENABLED", "TOOLS_ENABLED"])
    env.Append(CCFLAGS=["-g", "-O0"])
else:
    env.Append(CCFLAGS=["-O3"])

env.Append(CXXFLAGS=["-std=c++17", "-Wno-switch", "-Wno-unused-variable"])

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------
sources = Glob("src/*.cpp") + Glob("src/**/*.cpp")

if not sources:
    print("FATAL: no C++ sources found under src/")
    Exit(1)

# Output library per platform/target so Desktop and Web builds don't clobber each other.
out_lib = out_dir.File(f"{libname}.{platform}.{target}.{"so"}")
if platform == "windows":
    out_lib = out_dir.File(f"{libname}.windows.{target}.dll")
elif platform == "web":
    out_lib = out_dir.File(f"{libname}.web.{target}.wasm")
    env.Append(LINKFLAGS=["--no-entry", "-sSIDE_MODULE", "-sEXPORTED_FUNCTIONS=[]", "-sSUPPORT_LONGJMP=wasm"])
elif platform == "macos":
    out_lib = out_dir.File(f"{libname}.macos.{target}.framework/{libname}")

Default(env.SharedLibrary(target=out_lib, source=sources))
