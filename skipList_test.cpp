#include "skipList.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    SkipList<int, std::string> skipList(4);

    std::cout << "Inserting elements..." << std::endl;
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

    std::string value;
    std::cout << "\nSearching for key 2 and key 4:" << std::endl;
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

    std::cout << "\nInserting more elements..." << std::endl;
    skipList.insert(5, "five", std::chrono::steady_clock::now() + std::chrono::seconds(3));
    skipList.insert(6, "six", std::chrono::steady_clock::now() + std::chrono::seconds(3));

    skipList.display();

    std::cout << "\nWaiting for cleanup..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    skipList.cleanupExpiredNodes();

    std::cout << "\nAfter cleanup:" << std::endl;
    skipList.display();

    return 0;
}
