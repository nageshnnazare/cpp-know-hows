// Module 11 — unique_ptr ownership transfer, shared_ptr refcount, weak_ptr cycle break.
#include <iostream>
#include <memory>
#include <vector>

struct Widget {
    int id;
    explicit Widget(int id) : id(id) { std::cout << "  Widget(" << id << ") ctor\n"; }
    ~Widget() { std::cout << "  Widget(" << id << ") dtor\n"; }
};

// A tree node demonstrating the shared/weak ownership split (no leak).
struct Node {
    int value;
    std::vector<std::shared_ptr<Node>> children;
    std::weak_ptr<Node> parent;           // weak ⇒ breaks the parent<->child cycle
    explicit Node(int v) : value(v) {}
    ~Node() { std::cout << "  Node(" << value << ") dtor\n"; }
};

int main() {
    std::cout << "unique_ptr:\n";
    auto u = std::make_unique<Widget>(1);
    auto u2 = std::move(u);                // ownership transferred; u is now null
    std::cout << "  u is " << (u ? "non-null" : "null") << '\n';

    std::cout << "shared_ptr:\n";
    auto s1 = std::make_shared<Widget>(2);
    {
        auto s2 = s1;                      // refcount 2
        std::cout << "  use_count inside = " << s1.use_count() << '\n';
    }                                      // s2 gone, refcount 1
    std::cout << "  use_count after = " << s1.use_count() << '\n';

    std::cout << "weak_ptr breaks cycles:\n";
    {
        auto root  = std::make_shared<Node>(10);
        auto child = std::make_shared<Node>(20);
        root->children.push_back(child);
        child->parent = root;              // weak: no ownership cycle
        std::cout << "  child's parent value = "
                  << child->parent.lock()->value << '\n';
    }                                      // both Nodes correctly destroyed
    std::cout << "done\n";
}
