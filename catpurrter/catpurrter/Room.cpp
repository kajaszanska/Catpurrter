#include "Room.h"
#include <iostream>

Room::Room(sf::Font& fontRef, Player& playerRef)
    : font(fontRef), playerData(playerRef), selectionIndex(0), changeView(false), selectedView(RoomSelection::Computer) {
}

void Room::init() {
    roomObjects.clear();
    std::vector<std::string> labels = {
        "Aquarium", "Computer", "Storage Rack", "Shelves", "Doors"
    };

    for (size_t i = 0; i < labels.size(); ++i) {
        sf::Text item;
        item.setFont(font);
        item.setString(labels[i]);
        item.setCharacterSize(24);
        item.setPosition(100.f + i * 130.f, 500.f);
        item.setFillColor(i == selectionIndex ? sf::Color::Yellow : sf::Color::White);
        roomObjects.push_back(item);
    }
}

void Room::update() {
    for (size_t i = 0; i < roomObjects.size(); ++i) {
        roomObjects[i].setFillColor(i == selectionIndex ? sf::Color::Yellow : sf::Color::White);
    }
}

void Room::render(sf::RenderWindow& window) {
    sf::Text roomText("Welcome to your room!", font, 30);
    roomText.setPosition(200.f, 250.f);
    roomText.setFillColor(sf::Color::Cyan);
    window.draw(roomText);

    for (const auto& obj : roomObjects) {
        window.draw(obj);
    }
}

void Room::handleInput(sf::Keyboard::Key key) {
    if ((key == sf::Keyboard::A || key == sf::Keyboard::Left) && selectionIndex > 0) {
        selectionIndex--;
    }
    else if ((key == sf::Keyboard::D || key == sf::Keyboard::Right) && selectionIndex < static_cast<int>(roomObjects.size()) - 1) {
        selectionIndex++;
    }

    if (key == sf::Keyboard::Enter) {
        std::string selected = roomObjects[selectionIndex].getString().toAnsiString();
        std::cout << "Selected: " << selected << "\n";

        if (selected == "Computer") {
            selectedView = RoomSelection::Computer;
            changeView = true;
        }
        else if (selected == "Doors") {
            selectedView = RoomSelection::Doors;
            changeView = true;
        }
        else if (selected == "Storage Rack") {
            selectedView = RoomSelection::StorageRack;
            changeView = true;
        }
        else if (selected == "Shelves") {
            selectedView = RoomSelection::Shelves;
            changeView = true;
        }
        else if (selected == "Aquarium") {
            selectedView = RoomSelection::Aquarium;
            changeView = true;
        }
    }
}

bool Room::shouldChangeView() const {
    return changeView;
}

RoomSelection Room::getSelectedView() const {
    return selectedView;
}

void Room::resetViewChangeFlag() {
    changeView = false;
}
