#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <memory>

template <typename Key, typename Value>
class SkipList {
public:
    struct Node {
        Key key;
        Value value;
        std::vector<Node*> forward;
        std::chrono::steady_clock::time_point ttl;

        Node(Key k, Value v, int level, std::chrono::steady_clock::time_point ttl = std::chrono::steady_clock::time_point::max())
            : key(k), value(v), forward(level, nullptr), ttl(ttl) {}
    };

    SkipList(int maxLevel);
    ~SkipList();

    void insert(Key key, Value value, std::chrono::steady_clock::time_point ttl = std::chrono::steady_clock::time_point::max());
    bool search(Key key, Value& value);
    bool erase(Key key);
    void display() const;
    void removeExpiredNodes();
    void cleanupExpiredNodes();

private:
    Node* createNode(Key key, Value value, int level, std::chrono::steady_clock::time_point ttl);
    int randomLevel();

    const int maxLevel;
    Node* header;
    int currentLevel;
};

#endif // SKIPLIST_H
