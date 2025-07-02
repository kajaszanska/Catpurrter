#pragma once
#include <string>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;

// Player stores all persistent data about the player's progress, inventory, and customizations.
// Responsible for saving/loading state (coins, owned items, unlocked content, etc) to disk.
class Player {
public:
    // --- Player State Variables ---

    int coins;                                // The player's coin balance.
    std::string equippedHat;                  // The hat the player is currently wearing ("none" if unequipped).
    std::vector<std::string> unlockedHats;    // All hats the player has bought/unlocked.
    std::vector<std::string> ownedDecorations;// Big shelf decorations owned by the player.
    std::vector<std::string> shelfContents;   // Which shelf decorations are currently placed/displayed.
    std::vector<std::string> aquariumContents;// Fish and decorations the player owns for their aquarium.
    std::vector<std::string> ownedMiniGames;  // Which mini-games the player has bought/unlocked.

    // --- Constructors ---

    // Initializes a new player with default starting state.
    Player();

    // --- Persistence Methods ---

    // Loads all player data from the given save file (returns true on success).
    bool loadFromFile(const std::string& filename);

    // Saves all player data to the given save file.
    void saveToFile(const std::string& filename);
};
