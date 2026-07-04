#include "aether_test.hpp"

#include <godot_cpp/variant/utility_functions.hpp>

namespace aether {

void AetherTest::_bind_methods() {
}

AetherTest::AetherTest() = default;
AetherTest::~AetherTest() = default;

void AetherTest::_ready() {
    godot::UtilityFunctions::print("[AetherForgeV] C++ GDExtension loaded — pipeline OK. Target: ", __FILE__);
}

} // namespace aether
