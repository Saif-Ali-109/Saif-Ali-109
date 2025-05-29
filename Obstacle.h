#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "GameObject.h"

class Obstacle : public GameObject {
private:
    sf::Sprite sprite;
    std::shared_ptr<sf::Texture> texture;
    float speed;
    sf::FloatRect collisionBox;

public:
    Obstacle(float speed = 0.1f);
    void update() override;
    void render(sf::RenderWindow& window) override;
    void reset(float windowWidth, float windowHeight);
    sf::FloatRect getBounds() const override;
    void increaseSpeed(float delta);
    void setXPosition(float x);
};