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
    std::vector<std::string> ownedDecorations;
    std::vector<std::string> shelfContents;
    std::vector<std::string> aquariumContents;


    Player();

    bool loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename);
};
