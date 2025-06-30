#include "Player.h"
#include <fstream>
#include <iostream>
// Im using <filesystem> for file existence, directory creation, disk space checks, and save path validation.
#include <filesystem> 

using json = nlohmann::json;

Player::Player() : coins(100), equippedHat("none") {}

bool Player::loadFromFile(const std::string& filename) {
    // Im using filesystem to check if the file exists before opening
    if (!std::filesystem::exists(filename)) {
        std::cerr << "File does not exist: " << filename << std::endl;
        return false;
    }

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
    std::cout << "Loaded hats: ";
    for (const auto& hat : unlockedHats) std::cout << hat << ", ";
    std::cout << std::endl;

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
    // --- Restrict save location for security ---
    std::filesystem::path saveDir = "saves";
    std::filesystem::create_directories(saveDir); // Making sure save dir exists
    std::filesystem::path filePath(filename);

    // I'm getting absolute paths for comparison
    std::filesystem::path absSaveDir = std::filesystem::absolute(saveDir);
    std::filesystem::path absFilePath = std::filesystem::absolute(filePath);

    // I'm checking if the file path is inside the save directory
    if (absFilePath.string().find(absSaveDir.string()) != 0) {
        std::cerr << "Error: Save file must be inside the 'saves' directory!" << std::endl;
        return;
    }

    // Im checkinf free disk space before saving
    auto space = std::filesystem::space(absSaveDir);
    const size_t minRequired = 10 * 1024; // 10 KB as example
    if (space.available < minRequired) {
        std::cerr << "Error: Not enough disk space to save the file!" << std::endl;
        return;
    }

    json data;
    data["coins"] = coins;
    data["equippedHat"] = equippedHat;
    data["unlockedHats"] = unlockedHats;
    data["ownedDecorations"] = ownedDecorations;
    data["shelfContents"] = shelfContents;
    data["aquariumContents"] = aquariumContents;
    data["ownedMiniGames"] = ownedMiniGames;

    // Im ensuring the directory for the file exists
    std::filesystem::create_directories(filePath.parent_path());

    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open file for writing!" << std::endl;
        return;
    }
    outFile << data.dump(4);
}

