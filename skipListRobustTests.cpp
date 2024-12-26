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

// template <typename Key, typename Value>
// void SkipList<Key, Value>::display() const {
//     for (int i = currentLevel - 1; i >= 0; --i) { // Start from the highest level
//         Node* node = header->forward[i];
//         std::cout << "Level " << i << ": ";
//         while (node != nullptr) {
//             std::cout << "(" << node->key << ", " << node->value << ") ";
//             node = node->forward[i];
//         }
//         std::cout << std::endl;
//     }
// }

// template <typename Key, typename Value>
// void SkipList<Key, Value>::display() const {
//     for (int i = currentLevel - 1; i >= 0; --i) { // Start from the highest level
//         Node* node = header->forward[i];
//         int currentIndex = 0;
//         std::cout << "Level " << i << ": ";

//         while (node != nullptr) {
//             // Add placeholders for missing indices
//             while (currentIndex < node->key) {
//                 std::cout << "----- ";
//                 ++currentIndex;
//             }

//             // Print the current node
//             std::cout << "(" << node->key << ", " << node->value << ") ";
//             node = node->forward[i];
//             ++currentIndex;
//         }

//         std::cout << std::endl;
//     }
// }

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

// template <typename Key, typename Value>
// void SkipList<Key, Value>::display() const {
//     // Determine the maximum key for alignment purposes
//     int maxKey = 0;
//     Node* current = header->forward[0];
//     while (current != nullptr) {
//         maxKey = std::max(maxKey, current->key);
//         current = current->forward[0];
//     }

//     // Width of each column for neat display
//     const int columnWidth = 8;

//     // Display each level
//     for (int i = currentLevel - 1; i >= 0; --i) { // Start from the highest level
//         Node* node = header->forward[i];
//         int currentKey = 0;
//         std::cout << "Level " << i << ": ";

//         while (currentKey <= maxKey) {
//             if (node != nullptr && node->key == currentKey) {
//                 // Print the node key and value
//                 std::cout << std::setw(columnWidth) << "(" << node->key << ")";
//                 node = node->forward[i];
//             } else {
//                 // Print an empty space if no node exists at this key
//                 std::cout << std::setw(columnWidth) << " ";
//             }
//             ++currentKey;
//         }

//         std::cout << std::endl;
//     }
// }

// template <typename Key, typename Value>
// void SkipList<Key, Value>::display() const {
//     // Determine the maximum key length for alignment purposes
//     int maxKeyLength = 0;
//     Node* current = header->forward[0];
//     while (current != nullptr) {
//         int keyLength = std::to_string(current->key).length();
//         maxKeyLength = std::max(maxKeyLength, keyLength);
//         current = current->forward[0];
//     }

//     // Width of each column for neat display, based on max key length
//     const int columnWidth = maxKeyLength + 4; // Add some padding for better readability

//     // Display each level
//     for (int i = currentLevel - 1; i >= 0; --i) { // Start from the highest level
//         Node* node = header->forward[i];
//         std::cout << "Level " << i << ": ";

//         // Traverse and display nodes at this level
//         while (node != nullptr) {
//             // Print the node key and value in a formatted manner
//             std::cout << std::setw(columnWidth) << "(" << node->key << ", " << node->value << ")";
//             node = node->forward[i];
//         }

//         // If no nodes exist at this level, print a placeholder
//         if (node == nullptr) {
//             std::cout << std::setw(columnWidth) << " ";
//         }

//         std::cout << std::endl;
//     }
// }

// template <typename Key, typename Value>
// void SkipList<Key, Value>::display() const {
//     // Determine the maximum key length for alignment purposes
//     int maxKeyLength = 0;
//     Node* current = header->forward[0];
//     while (current != nullptr) {
//         int keyLength = std::to_string(current->key).length();
//         maxKeyLength = std::max(maxKeyLength, keyLength);
//         current = current->forward[0];
//     }

//     // Width of each column for neat display, based on max key length
//     const int columnWidth = maxKeyLength + 4; // Add some padding for better readability

//     // Traverse all levels from highest to lowest
//     for (int level = currentLevel - 1; level >= 0; --level) {
//         Node* node = header->forward[level];
//         std::cout << "Level " << level << ": ";

//         // Traverse the current level, tracking the current key
//         while (node != nullptr) {
//             // Print the node key and value at this level
//             std::cout << std::setw(columnWidth) << "(" << node->key << ", " << node->value << ")";
//             node = node->forward[level];
//         }

//         // If no node exists at this level, print empty space
//         std::cout << std::endl;
//     }
// }

template <typename Key, typename Value>
void SkipList<Key, Value>::display() const {
    // Find the maximum key to determine column width for display
    int maxKeyLength = 0;
    Node* current = header->forward[0];
    while (current != nullptr) {
        int keyLength = std::to_string(current->key).length();
        maxKeyLength = std::max(maxKeyLength, keyLength);
        current = current->forward[0];
    }

    // Width of each column for neat display, based on max key length
    const int columnWidth = maxKeyLength + 4; // Add padding for readability

    // Iterate over levels from top to bottom
    for (int level = currentLevel - 1; level >= 0; --level) {
        Node* node = header->forward[level];
        std::cout << "Level " << level << ": ";

        // Traverse each level, starting with the first node
        int currentKey = 1;  // Start from the first key
        while (node != nullptr || currentKey <= maxKeyLength) {
            // If the node exists and matches the current key
            if (node != nullptr && node->key == currentKey) {
                // Print the key and value at the current level
                std::cout << std::setw(columnWidth) << "(" << node->key << ", " << node->value << ")";
                node = node->forward[level];
            } else {
                // If no node exists at the current position, print an empty space
                std::cout << std::setw(columnWidth) << " ";
            }
            currentKey++;
        }
        std::cout << std::endl;
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
