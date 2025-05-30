#ifndef BTREENODE_HPP
#define BTREENODE_HPP

#include <memory>
#include <array>

template <typename K, class C, int T>
class BTreeNode {
public:
    std::array<K, 2 * T - 1> keys{}; 
    std::array<C, 2 * T - 1> values{};
    std::array<std::shared_ptr<BTreeNode<K, C, T>>, 2 * T> children{};
    int n;
    bool leaf;

    explicit BTreeNode(bool is_leaf = true) : n{}, leaf{ is_leaf } {}
};

#endif