#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Player.h"

enum class RoomSelection {
    Aquarium,
    Computer,
    StorageRack,
    Shelves,
    Doors
};

class Room {
public:
    Room(sf::Font& font, Player& player);

    void init();
    void update();
    void render(sf::RenderWindow& window);
    void handleInput(sf::Keyboard::Key key);

    // View transition trigger
    bool shouldChangeView() const;
    RoomSelection getSelectedView() const;
    void resetViewChangeFlag();

private:
    sf::Font& font;
    Player& playerData;

    std::vector<sf::Text> roomObjects;
    int selectionIndex;
    bool changeView;
    RoomSelection selectedView;
};
