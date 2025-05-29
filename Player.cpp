#include "Player.h"

Player::Player() : speed(12.f), texture(std::make_shared<sf::Texture>()) {
    if (!texture->loadFromFile("player.png")) {
        sprite.setTextureRect(sf::IntRect(0, 0, 50, 30));
        sprite.setColor(sf::Color::Blue);
    }
    else {
        sprite.setTexture(*texture);
        sprite.setScale(0.3f, 0.3f);
    }
    sprite.setPosition(400.f, 500.f);
    sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
    collisionBox.width = sprite.getGlobalBounds().width * 0.7f;
    collisionBox.height = sprite.getGlobalBounds().height * 0.7f;
}

void Player::update() {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    sf::Vector2u windowSize = sprite.getTexture()->getSize(); // Use dynamic window size if needed
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && sprite.getPosition().x > 150)
        sprite.move(-speed, 0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && sprite.getPosition().x + bounds.width < 800)
        sprite.move(speed, 0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && sprite.getPosition().y > 0)
        sprite.move(0, -speed);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && sprite.getPosition().y + bounds.height < 600)
        sprite.move(0, speed);
    collisionBox.left = sprite.getPosition().x + (sprite.getGlobalBounds().width - collisionBox.width) / 2;
    collisionBox.top = sprite.getPosition().y + (sprite.getGlobalBounds().height - collisionBox.height) / 2;
}

void Player::render(sf::RenderWindow& window) {
    window.draw(sprite);
}

void Player::reset() {
    sprite.setPosition(425.f, 520.f);
}

sf::FloatRect Player::getBounds() const {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    float scale = 0.5f;
    float widthScale = std::max(0.1f, scale - 0.1f); // Prevent negative/zero scaling
    float heightScale = std::max(0.1f, scale + 0.4f);
    bounds.width *= widthScale;
    bounds.height *= heightScale;
    bounds.left += (sprite.getGlobalBounds().width - bounds.width) / 2;
    bounds.top += (sprite.getGlobalBounds().height - bounds.height) / 2;
    return bounds;
}