#include "skipList.h"

template <typename Key, typename Value>
SkipList<Key, Value>::SkipList(int maxLevel) 
    : maxLevel(maxLevel), currentLevel(0) {
    header = new Node(Key{}, Value{}, maxLevel);
}

template <typename Key, typename Value>
SkipList<Key, Value>::~SkipList() {
    Node* current = header;
    while (current != nullptr) {
        Node* temp = current;
        current = current->forward[0];
        delete temp;
    }
}

template <typename Key, typename Value>
typename SkipList<Key, Value>::Node* SkipList<Key, Value>::createNode(Key key, Value value, int level, std::chrono::steady_clock::time_point ttl) {
    return new Node(key, value, level, ttl);
}

template <typename Key, typename Value>
void SkipList<Key, Value>::insert(Key key, Value value, std::chrono::steady_clock::time_point ttl) {
    Node* update[maxLevel];
    Node* current = header;

    for (int i = currentLevel - 1; i >= 0; --i) {
        while (current->forward[i] != nullptr && current->forward[i]->key < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];

    if (current == nullptr || current->key != key) {
        int level = randomLevel();
        if (level > currentLevel) {
            for (int i = currentLevel; i < level; ++i) {
                update[i] = header;
            }
            currentLevel = level;
        }

        Node* newNode = createNode(key, value, level, ttl);

        for (int i = 0; i < level; ++i) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
    } else {
        current->value = value;
    }
}

template <typename Key, typename Value>
bool SkipList<Key, Value>::search(Key key, Value& value) {
    Node* current = header;

    for (int i = currentLevel - 1; i >= 0; --i) {
        while (current->forward[i] != nullptr && current->forward[i]->key < key) {
            current = current->forward[i];
        }
    }

    current = current->forward[0];

    if (current != nullptr && current->key == key) {
        value = current->value;
        return true;
    }
    return false;
}

template <typename Key, typename Value>
bool SkipList<Key, Value>::erase(Key key) {
    Node* update[maxLevel];
    Node* current = header;

    for (int i = currentLevel - 1; i >= 0; --i) {
        while (current->forward[i] != nullptr && current->forward[i]->key < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];

    if (current != nullptr && current->key == key) {
        for (int i = 0; i < currentLevel; ++i) {
            if (update[i]->forward[i] != current) break;
            update[i]->forward[i] = current->forward[i];
        }

        delete current;
        while (currentLevel > 0 && header->forward[currentLevel - 1] == nullptr) {
            --currentLevel;
        }

        return true;
    }
    return false;
}

template <typename Key, typename Value>
void SkipList<Key, Value>::display() const {
    for (int i = 0; i < currentLevel; ++i) {
        Node* node = header->forward[i];
        std::cout << "Level " << i << ": ";
        while (node != nullptr) {
            std::cout << "(" << node->key << ", " << node->value << ") ";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

template <typename Key, typename Value>
void SkipList<Key, Value>::removeExpiredNodes() {
    Node* current = header->forward[0];
    while (current != nullptr) {
        if (current->ttl != std::chrono::steady_clock::time_point::max() && 
            std::chrono::steady_clock::now() > current->ttl) {
            erase(current->key);
        }
        current = current->forward[0];
    }
}

template <typename Key, typename Value>
void SkipList<Key, Value>::cleanupExpiredNodes() {
    Node* current = header->forward[0];
    while (current != nullptr) {
        if (current->ttl != std::chrono::steady_clock::time_point::max() && 
            std::chrono::steady_clock::now() > current->ttl) {
            Node* expiredNode = current;
            current = current->forward[0];
            erase(expiredNode->key);
        } else {
            current = current->forward[0];
        }
    }
}

template <typename Key, typename Value>
int SkipList<Key, Value>::randomLevel() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    int level = 1;
    while (dis(gen) == 1 && level < maxLevel) {
        ++level;
    }
    return level;
}

template class SkipList<int, std::string>;  // Explicit instantiation
