#ifndef BTREE_HPP
#define BTREE_HPP 

#include <cstdlib>
#include <memory>
#include <iostream>

#include "BTreeNode.hpp"

template <typename K, class C, int T>
class BTree {
private:
    using btn = BTreeNode<K, C, T>;

    std::shared_ptr<btn> root;

    void split(std::shared_ptr<btn> x, int i, std::shared_ptr<btn> y) {
        std::shared_ptr<btn> z = std::make_shared<btn>(btn(y->leaf));
        z->n = T - 1;

        for(int j = 0; j < T - 1; ++j) {
            z->keys[j] = y->keys[T + j];
            z->values[j] = y->values[T + j];
        }
        if(!y->leaf) {
            for(int j = 0; j < T; ++j) {
                z->children[j] = y->children[T + j];
            }
        }
        y->n = T - 1;

        for(int j = x->n; j >= i + 1; --j) {
            x->children[j + 1] = x->children[j];
        }
        x->children[i + 1] = z;

        for(int j = x->n - 1; j >= i; --j){
            x->keys[j + 1] = x->keys[j];
            x->values[j + 1] = x->values[j];
        }
        x->keys[i] = y->keys[T - 1];
        x->values[i] = y->values[T - 1];
        ++x->n;
    }

    void insert_nonfull(std::shared_ptr<btn> x, K key, C obj){
        int i = x->n - 1;
        if(x->leaf) {
            while(i >= 0 && key < x->keys[i]) {
                x->keys[i + 1] = x->keys[i];
                x->values[i + 1] = x->values[i];
                --i;
            }
            x->keys[i + 1] = key;
            x->values[i + 1] = obj;
            ++x->n; 
        }
        else {
            while(i >= 0 && key < x->keys[i]) {
                --i;
            }
            ++i;
            if(x->children[i]->n == 2 * T - 1) {
                split(x, i, x->children[i]);
                if(key > x->keys[i]) ++i;
            }
            insert_nonfull(x->children[i], key, obj);
        }
    }

    void traverse(std::shared_ptr<btn> x, int level) {
        std::cout << "level " << level << ":\n";
        for(int i = 0; i < x->n; ++i) {
            std::cout << x->keys[i] << " " << x->values[i] << "\n";
        }
        if(!x->leaf){
            for(int i = 0; i < x->n + 1; ++i){
                traverse(x->children[i], level + 1);
            }
        }
    }

    std::pair<std::shared_ptr<btn>, int> search(std::shared_ptr<btn> x, K key) {
        int i{};
        while(i < x->n && key > x->keys[i]) {
            ++i;
        }
        if(i < x->n && key == x->keys[i]){
            return {x, i};
        }
        else if(x->leaf) {
            return {nullptr, -1};
        }
        return search(x->children[i], key);
    }

public:
    BTree() : root{std::make_shared<btn>(btn())} {}
    void insert(K key, C obj) {
        if(root->n == 2 * T - 1) {
            std::shared_ptr<btn> s = std::make_shared<btn>(btn(false));
            s->children[0] = root;
            root = s;
            split(root, 0, root->children[0]);
            insert_nonfull(s, key, obj);
        }
        else {
            insert_nonfull(root, key, obj);
        }
    }

    void traverse(){
        std::cout << "B-Tree:\n";
        if(root != nullptr){
            traverse(root, 0);
        }
    }

    std::pair<std::shared_ptr<btn>, int> search(K key) {
        return root == nullptr ? std::pair<std::shared_ptr<btn>, int>{nullptr, -1} : search(root, key);
    }

};

#endif
