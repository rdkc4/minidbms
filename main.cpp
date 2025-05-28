#include "BTree/BTree.hpp"
#include <format>

int main(){
    const int t = 4;
    static_assert(t > 1, "B-Tree minimum degree T must be greater than 1");
    BTree<int, int, 4> bt;

    bt.insert(10, 10);
    bt.insert(20, 20);
    bt.insert(5, 5);
    bt.insert(6, 6);
    bt.insert(12, 12);
    bt.insert(30, 30);
    bt.insert(7, 7);
    bt.insert(17, 17);

    bt.traverse();
    std::cout << "\n";

    int k = 6;
    {
        auto [ptr, idx] = bt.search(k);
        if(ptr == nullptr){
            std::cout << std::format("Key {} not found\n", k);
        }
        else {
            std::cout << std::format("Key {} found, value: {}\n", k, ptr->values[idx]);
        }
    }
    
    k = 15;
    {
        auto [ptr, idx] = bt.search(k);
        if(ptr == nullptr){
            std::cout << std::format("Key {} not found!\n", k);
        }
        else {
            std::cout << std::format("Key {} found! Value: {}\n", k, ptr->values[idx]);
        }
    }

    return 0;
}