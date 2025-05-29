#pragma once
#include <SFML/Graphics.hpp>

class GameObject {
public:
    virtual void update() = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    virtual sf::FloatRect getBounds() const = 0;
    virtual ~GameObject() {}
};