#include "Player.h"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

Player::Player() : coins(100), equippedHat("none") {}

bool Player::loadFromFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) return false;

    json data;
    inFile >> data;

    coins = data["coins"];
    equippedHat = data["equippedHat"];

    unlockedHats.clear();
    for (const auto& hat : data["unlockedHats"]) {
        unlockedHats.push_back(hat);
    }
    ownedDecorations.clear();
    for (const auto& item : data["ownedDecorations"]) {
        ownedDecorations.push_back(item);
    }

    shelfContents.clear();
    for (const auto& item : data["shelfContents"]) {
        shelfContents.push_back(item);
    }

    aquariumContents.clear();
    for (const auto& item : data["aquariumContents"]) {
        aquariumContents.push_back(item);
    }

    ownedMiniGames.clear();
    for (const auto& game : data["ownedMiniGames"])
        ownedMiniGames.push_back(game);

    return true;
}

void Player::saveToFile(const std::string& filename) {
    json data;
    data["coins"] = coins;
    data["equippedHat"] = equippedHat;
    data["unlockedHats"] = unlockedHats;
    data["ownedDecorations"] = ownedDecorations;
    data["shelfContents"] = shelfContents;
    data["aquariumContents"] = aquariumContents;
    data["ownedMiniGames"] = ownedMiniGames;

    std::ofstream outFile(filename);
    outFile << data.dump(4);
}