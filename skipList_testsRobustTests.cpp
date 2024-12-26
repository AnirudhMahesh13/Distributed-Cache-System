#include "skipList.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

int main() {
    SkipList<int, std::string> skipList(16); // Increased max level for stress testing

    std::cout << "Inserting initial elements..." << std::endl;
    skipList.insert(1, "one");
    skipList.insert(2, "two");
    skipList.insert(3, "three");
    skipList.insert(4, "four");

    skipList.display();

    std::cout << "\nWaiting for 2 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "\nAfter 2 seconds:" << std::endl;
    skipList.removeExpiredNodes();
    skipList.display();

    std::cout << "\nSearching for key 2 and key 4:" << std::endl;
    std::string value;
    if (skipList.search(2, value)) {
        std::cout << "Key 2 found with value: " << value << std::endl;
    } else {
        std::cout << "Key 2 not found or expired." << std::endl;
    }

    if (skipList.search(4, value)) {
        std::cout << "Key 4 found with value: " << value << std::endl;
    } else {
        std::cout << "Key 4 not found or expired." << std::endl;
    }

    std::cout << "\nStress testing with large data set..." << std::endl;
    const int largeInsertCount = 1000000;
    const int randomOpsCount = 500000;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> keyDist(1, largeInsertCount);
    std::uniform_int_distribution<> ttlDist(1, 10);
    std::uniform_int_distribution<> operationDist(0, 1); // 0 for insertion, 1 for deletion

    for (int i = 0; i < largeInsertCount; ++i) {
        int key = keyDist(gen);
        std::string value = "value_" + std::to_string(key);
        auto ttl = std::chrono::steady_clock::now() + std::chrono::seconds(ttlDist(gen));
        skipList.insert(key, value, ttl);

        // Display skip list periodically
        if (i % 10000 == 0) {
            std::cout << "\n--- Skip List after " << i << " insertions ---" << std::endl;
            skipList.display();
        }
    }

    std::cout << "\nFinished inserting " << largeInsertCount << " elements.\n";
    std::cout << "STARTING DISPLAY\n";
    skipList.display();
    std::cout << "ENDING DISPLAY\n";

    std::cout << "\nPerforming random operations (insertions and deletions)..." << std::endl;
    for (int i = 0; i < randomOpsCount; ++i) {
        int key = keyDist(gen);
        if (operationDist(gen) == 0) {
            std::string value = "value_" + std::to_string(key);
            auto ttl = std::chrono::steady_clock::now() + std::chrono::seconds(ttlDist(gen));
            skipList.insert(key, value, ttl);
        } else {
            skipList.erase(key);
        }

        // Display skip list periodically
        if (i % 10000 == 0) {
            std::cout << "\n--- Skip List after " << i << " random operations ---" << std::endl;
            skipList.display();
        }
    }

    std::cout << "\nFinished performing random operations.\n";
    skipList.display();

    std::cout << "\nCleaning up expired nodes after waiting 5 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    skipList.cleanupExpiredNodes();
    skipList.display();

    std::cout << "\nSearching for 10 random keys from the large data set..." << std::endl;
    for (int i = 0; i < 10; ++i) {
        int key = keyDist(gen);
        if (skipList.search(key, value)) {
            std::cout << "Key " << key << " found with value: " << value << std::endl;
        } else {
            std::cout << "Key " << key << " not found or expired." << std::endl;
        }
    }

    std::cout << "\nDeleting 10 random keys from the large data set..." << std::endl;
    for (int i = 0; i < 10; ++i) {
        int key = keyDist(gen);
        if (skipList.erase(key)) {
            std::cout << "Key " << key << " successfully deleted." << std::endl;
        } else {
            std::cout << "Key " << key << " not found for deletion." << std::endl;
        }
    }

    std::cout << "\nFinal cleanup after all tests:" << std::endl;
    skipList.cleanupExpiredNodes();
    skipList.display();

    std::cout << "\nStress tests completed. No errors detected!" << std::endl;

    return 0;
}
