#include "Obstacle.h"
#include <cstdlib>
#include <iostream>

Obstacle::Obstacle(float speed) : speed(speed), texture(std::make_shared<sf::Texture>()) {
    if (!texture->loadFromFile("obstacle.png")) {
        std::cerr << "Failed to load obstacle.png, using fallback sprite\n";
        sprite.setTextureRect(sf::IntRect(0, 0, 100, 60));
        sprite.setColor(sf::Color::Red);
    }
    else {
        sprite.setTexture(*texture);
        sprite.setScale(0.6f, 0.6f);
        std::cout << "Loaded obstacle.png successfully\n";
    }
    sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
    collisionBox.width = sprite.getGlobalBounds().width * 0.7f;
    collisionBox.height = sprite.getGlobalBounds().height * 0.7f;
}

void Obstacle::update() {
    sprite.move(0, speed);
    collisionBox.left = sprite.getPosition().x + (sprite.getGlobalBounds().width - collisionBox.width) / 2;
    collisionBox.top = sprite.getPosition().y + (sprite.getGlobalBounds().height - collisionBox.height) / 2;
}

void Obstacle::render(sf::RenderWindow& window) {
    window.draw(sprite);
}

void Obstacle::reset(float windowWidth, float windowHeight) {
    float minX = windowWidth * 0.2f;
    float maxX = windowWidth * 1.1f - sprite.getGlobalBounds().width;
    float randomX = minX + static_cast<float>(rand()) / RAND_MAX * (maxX - minX);
    sprite.setPosition(randomX, -sprite.getGlobalBounds().height);
    collisionBox.left = sprite.getPosition().x + (sprite.getGlobalBounds().width - collisionBox.width) / 2;
    collisionBox.top = sprite.getPosition().y + (sprite.getGlobalBounds().height - collisionBox.height) / 2;
    std::cout << "Obstacle reset: x=" << randomX << ", y=" << -sprite.getGlobalBounds().height << "\n";
}

sf::FloatRect Obstacle::getBounds() const {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    float scale = 0.4f;
    float widthScale = std::max(0.1f, scale - 0.2f);
    float heightScale = std::max(0.1f, scale + 0.4f);
    bounds.width *= widthScale;
    bounds.height *= heightScale;
    bounds.left += (sprite.getGlobalBounds().width - bounds.width) / 2;
    bounds.top += (sprite.getGlobalBounds().height - bounds.height) / 2;
    return bounds;
}

void Obstacle::increaseSpeed(float delta) {
    speed += delta;
}

void Obstacle::setXPosition(float x) {
    sprite.setPosition(x, sprite.getPosition().y);
    collisionBox.left = x + (sprite.getGlobalBounds().width - collisionBox.width) / 2;
}