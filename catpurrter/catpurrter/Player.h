#pragma once
#include <string>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;

class Player {
public:
    int coins;
    std::string equippedHat;
    std::vector<std::string> unlockedHats;

    Player();

    bool loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename);
};
