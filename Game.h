#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <fstream>

// Forward declarations to avoid circular dependencies
class Player;
class Obstacle;

enum class GameState {
    Player_Name,
    MENU,
    MODE_SELECT,
    INSTRUCTIONS,
    PLAYING,
    PAUSED,
    GAME_OVER,
    HIGH_SCORES,
    SETTINGS // Added for settings menu
};

class Game {
private:
    sf::RenderWindow window;
    GameState state;
    Player* player;
    Obstacle* obstacle;
    float roadSpeed;
    float roadHeight;
    std::shared_ptr<sf::Texture> roadTexture;
    sf::Sprite roadSprite1;
    sf::Sprite roadSprite2;
    std::shared_ptr<sf::Texture> menuTexture;
    sf::Sprite menuBackground;
    sf::Font font;
    sf::Text inputText;
    sf::Text nameDisplay;
    sf::Text gameOverText;
    sf::Text scoreText;
    sf::Text instructionText;
    sf::Text finalScoreText;
    sf::Text highScoreText;
    std::vector<sf::Text> menuItems;
    std::vector<sf::Text> pauseMenuItems;
    std::vector<sf::Text> gameOverMenuItems;
    std::vector<sf::Text> modeMenuItems;
    std::vector<sf::Text> settingsMenuItems; // Added for settings menu
    std::shared_ptr<sf::SoundBuffer> crashSoundBuffer;
    sf::Sound crashSound;
    std::shared_ptr<sf::SoundBuffer> menuChangeSoundBuffer;
    sf::Sound menuChangeSound;
    sf::Music backgroundMusic;
    std::string player_name;
    std::vector<std::pair<std::string, int>> highScores;
    int selectedItemIndex;
    int pauseSelectedIndex;
    int modeSelectedIndex;
    int settingsSelectedIndex; // Added for settings menu
    int score;
    float lastObstacleY;
    float initialObstacleSpeed;
    float speedIncreaseRate;
    sf::Clock gameClock;
    bool backgroundMusicEnabled; // Added to toggle background music
    bool sfxEnabled; // Added to toggle SFX (crashSound, menuChangeSound)

    void initializeTextElements();
    void initializeMenuItems();
    void initializeSettingsMenuItems(); // Added for settings menu
    void updateMenuColors(std::vector<sf::Text>& items, int selectedIndex);
    void handleNameInput(sf::Event& event);
    void handleMenuInput(const sf::Event& event);
    void handleModeSelectInput(const sf::Event& event);
    void handlePauseMenuInput(const sf::Event& event);
    void handleGameOverInput(const sf::Event& event);
    void handleHighScoreInput(const sf::Event& event);
    void handleSettingsInput(const sf::Event& event); // Added for settings menu
    void loadHighScores();
    void saveHighScores();
    void resetHighScores();
    void manageBackgroundMusic();

public:
    Game();
    ~Game();
    void run();
    void handleEvents();
    void update();
    void render();
    void reset();
};

#include "Player.h"
#include "Obstacle.h"
