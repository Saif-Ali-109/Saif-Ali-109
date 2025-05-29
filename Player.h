#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "GameObject.h"

class Player : public GameObject {
private:
    float speed;
    sf::Sprite sprite;
    std::shared_ptr<sf::Texture> texture;
    sf::FloatRect collisionBox;

public:
    Player();
    void update() override;
    void render(sf::RenderWindow& window) override;
    void reset();
    sf::FloatRect getBounds() const override;
};