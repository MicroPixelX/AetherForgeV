#ifndef AETHER_AETHER_TEST_HPP
#define AETHER_AETHER_TEST_HPP

#include <godot_cpp/classes/node.hpp>

namespace aether {

// Minimal node used to prove the GDExtension pipeline works across targets.
// Drop once building/crafting/saving modules land.
class AetherTest : public godot::Node {
    GDCLASS(AetherTest, godot::Node)

protected:
    static void _bind_methods();

public:
    AetherTest();
    ~AetherTest() override;

    void _ready() override;
};

} // namespace aether

#endif // AETHER_AETHER_TEST_HPP
