#pragma warning(disable : 26812) // Disable enum class warnings from SFML
#include "Game.h"
#include <SFML/Audio.hpp>
#include "Player.h"
#include "Obstacle.h"
#include "Font.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <iostream>

Game::Game() : window(sf::VideoMode(800, 600), "Car Dodging Game", sf::Style::Default),
state(GameState::Player_Name), player(new Player()), obstacle(new Obstacle(10.f)),
roadSpeed(6.f), roadHeight(600.f), roadTexture(std::make_shared<sf::Texture>()),
menuTexture(std::make_shared<sf::Texture>()), crashSoundBuffer(std::make_shared<sf::SoundBuffer>()),
menuChangeSoundBuffer(std::make_shared<sf::SoundBuffer>()), selectedItemIndex(0),
pauseSelectedIndex(0), modeSelectedIndex(0), settingsSelectedIndex(0), score(0),
lastObstacleY(-250.f), initialObstacleSpeed(10.f), speedIncreaseRate(0.1f),
backgroundMusicEnabled(true), sfxEnabled(true) {
    window.setFramerateLimit(60);
    srand(static_cast<unsigned>(time(0)));
    if (!roadTexture->loadFromFile("road_background.png")) {
        std::cerr << "Error: Failed to load road_background.png\n";
        roadSprite1.setColor(sf::Color::White);
        roadSprite2.setColor(sf::Color(150, 150, 150));
    }
    else {
        roadSprite1.setTexture(*roadTexture);
        roadSprite2.setTexture(*roadTexture);
        roadSprite1.setScale(3.3f, 5.0f);
        roadSprite2.setScale(3.3f, 5.0f);
    }
    roadSprite1.setPosition(0, 0);
    roadSprite2.setPosition(0, -roadHeight);
    if (!menuTexture->loadFromFile("menu_background.png")) {
        std::cerr << "Error: Failed to load menu_background.png\n";
        menuBackground.setColor(sf::Color(200, 200, 200));
    }
    else {
        menuBackground.setTexture(*menuTexture);
        menuBackground.setScale(0.7f, 0.7f);
    }
    if (!font.loadFromFile("arial.ttf")) {
        if (!font.loadFromMemory(&default_font_data, default_font_size)) {
            std::cerr << "Error: Failed to load default font\n";
        }
    }
    initializeTextElements();
    if (!crashSoundBuffer->loadFromFile("crash_sound.ogg")) {
        std::cerr << "Error: Failed to load crash_sound.ogg\n";
    }
    crashSound.setBuffer(*crashSoundBuffer);
    if (!menuChangeSoundBuffer->loadFromFile("menu_change_sound.ogg")) {
        std::cerr << "Error: Failed to load menu_change_sound.ogg\n";
    }
    menuChangeSound.setBuffer(*menuChangeSoundBuffer);
    backgroundMusic.setLoop(true);
    if (!backgroundMusic.openFromFile("background_music.ogg")) {
        std::cerr << "Error: Failed to load background_music.ogg\n";
    }
    loadHighScores();
}

Game::~Game() {
    crashSound.stop();
    menuChangeSound.stop();
    backgroundMusic.stop();
    delete player;
    delete obstacle;
}

void Game::initializeTextElements() {
    inputText.setFont(font);
    inputText.setCharacterSize(36);
    inputText.setFillColor(sf::Color::White);
    inputText.setOutlineColor(sf::Color::Black);
    inputText.setOutlineThickness(2.f);
    inputText.setPosition(200, 200);
    inputText.setString("Enter your name: ");
    nameDisplay.setFont(font);
    nameDisplay.setCharacterSize(36);
    nameDisplay.setFillColor(sf::Color::White);
    nameDisplay.setOutlineColor(sf::Color::Black);
    nameDisplay.setOutlineThickness(2.f);
    nameDisplay.setPosition(100, 250);
    nameDisplay.setString("");
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setOutlineColor(sf::Color::Black);
    gameOverText.setOutlineThickness(1.f);
    gameOverText.setString("Game Over!");
    gameOverText.setPosition(240, 150);
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setOutlineColor(sf::Color::Black);
    scoreText.setOutlineThickness(2.f);
    scoreText.setPosition(20.f, 20.f);
    scoreText.setString("Score: 0");
    instructionText.setFont(font);
    instructionText.setCharacterSize(24);
    instructionText.setFillColor(sf::Color::White);
    instructionText.setOutlineColor(sf::Color::Black);
    instructionText.setOutlineThickness(2.f);
    instructionText.setString("Instructions:\n\n- Use arrow keys to move\n- Avoid red blocks\n- ESC to pause\n\nPress Escape to go back");
    instructionText.setPosition(200, 150);
    finalScoreText.setFont(font);
    finalScoreText.setCharacterSize(36);
    finalScoreText.setFillColor(sf::Color::White);
    finalScoreText.setOutlineColor(sf::Color::Black);
    finalScoreText.setOutlineThickness(2.f);
    finalScoreText.setPosition(250, 220);
    highScoreText.setFont(font);
    highScoreText.setCharacterSize(36);
    highScoreText.setFillColor(sf::Color::White);
    highScoreText.setOutlineColor(sf::Color::Black);
    highScoreText.setOutlineThickness(2.f);
    highScoreText.setPosition(200, 150);
    highScoreText.setString("High Scores:\n\nPress Escape to go back\nPress R to reset");
    initializeMenuItems();
    initializeSettingsMenuItems();
}

void Game::initializeMenuItems() {
    menuItems.clear();
    pauseMenuItems.clear();
    gameOverMenuItems.clear();
    modeMenuItems.clear();
    std::string labels[] = { "Play Game", "Instructions", "High Scores", "Settings", "Exit" };
    for (int i = 0; i < 5; ++i) {
        sf::Text item;
        item.setFont(font);
        item.setString(labels[i]);
        item.setCharacterSize(40);
        item.setPosition(300.f, 250.f + i * 60.f);
        item.setOutlineColor(sf::Color::Black);
        item.setOutlineThickness(2.f);
        item.setFillColor(i == selectedItemIndex ? sf::Color::Blue : sf::Color::White);
        menuItems.push_back(item);
    }
    std::string pauseLabels[] = { "Resume", "Restart", "Main Menu", "Exit" };
    for (int i = 0; i < 4; ++i) {
        sf::Text item;
        item.setFont(font);
        item.setString(pauseLabels[i]);
        item.setCharacterSize(40);
        item.setPosition(300.f, 250.f + i * 60.f);
        item.setOutlineColor(sf::Color::Black);
        item.setOutlineThickness(2.f);
        item.setFillColor(i == pauseSelectedIndex ? sf::Color::Blue : sf::Color::White);
        pauseMenuItems.push_back(item);
    }
    std::string gameOverLabels[] = { "Restart", "Main Menu", "Exit" };
    for (int i = 0; i < 3; ++i) {
        sf::Text item;
        item.setFont(font);
        item.setString(gameOverLabels[i]);
        item.setCharacterSize(40);
        item.setPosition(300.f, 300.f + i * 60.f);
        item.setOutlineColor(sf::Color::Black);
        item.setOutlineThickness(2.f);
        item.setFillColor(i == pauseSelectedIndex ? sf::Color::Blue : sf::Color::White);
        gameOverMenuItems.push_back(item);
    }
    std::string modeLabels[] = { "Easy", "Medium", "Hard" };
    for (int i = 0; i < 3; ++i) {
        sf::Text item;
        item.setFont(font);
        item.setString(modeLabels[i]);
        item.setCharacterSize(40);
        item.setPosition(300.f, 250.f + i * 60.f);
        item.setOutlineColor(sf::Color::Black);
        item.setOutlineThickness(2.f);
        item.setFillColor(i == modeSelectedIndex ? sf::Color::Blue : sf::Color::White);
        modeMenuItems.push_back(item);
    }
}

void Game::initializeSettingsMenuItems() {
    settingsMenuItems.clear();
    std::string labels[] = {
        "Background Music: " + std::string(backgroundMusicEnabled ? "On" : "Off"),
        "SFX Sound: " + std::string(sfxEnabled ? "On" : "Off"),
        "Back"
    };
    for (int i = 0; i < 3; ++i) {
        sf::Text item;
        item.setFont(font);
        item.setString(labels[i]);
        item.setCharacterSize(40);
        item.setPosition(230.f, 250.f + i * 60.f);
        item.setOutlineColor(sf::Color::Black);
        item.setOutlineThickness(2.f);
        item.setFillColor(i == settingsSelectedIndex ? sf::Color::Blue : sf::Color::White);
        settingsMenuItems.push_back(item);
    }
}

void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        if (state == GameState::PLAYING)
            update();
        render();
    }
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        if (state == GameState::Player_Name) {
            handleNameInput(event);
        }
        else if (event.type == sf::Event::KeyPressed) {
            switch (state) {
            case GameState::Player_Name:
                handleNameInput(event);
                break;
            case GameState::MENU:
                handleMenuInput(event);
                break;
            case GameState::INSTRUCTIONS:
                if (event.key.code == sf::Keyboard::Escape)
                    state = GameState::MENU;
                break;
            case GameState::MODE_SELECT:
                handleModeSelectInput(event);
                break;
            case GameState::PLAYING:
                if (event.key.code == sf::Keyboard::Escape)
                    state = GameState::PAUSED;
                break;
            case GameState::PAUSED:
                handlePauseMenuInput(event);
                break;
            case GameState::GAME_OVER:
                handleGameOverInput(event);
                break;
            case GameState::HIGH_SCORES:
                handleHighScoreInput(event);
                break;
            case GameState::SETTINGS:
                handleSettingsInput(event);
                break;
            }
        }
    }
}

void Game::handleNameInput(sf::Event& event) {
    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode >= 32 && event.text.unicode < 127) {
            player_name += static_cast<char>(event.text.unicode);
        }
        else if (event.text.unicode == 8 && !player_name.empty()) {
            player_name.pop_back();
        }
    }
    else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Enter && !player_name.empty()) {
            state = GameState::MENU;
        }
    }
    nameDisplay.setString(player_name);
}

void Game::handleMenuInput(const sf::Event& event) {
    if (event.key.code == sf::Keyboard::Up) {
        if (selectedItemIndex > 0) {
            selectedItemIndex--;
            if (sfxEnabled) {
                menuChangeSound.play(); // Play sound without checking status
            }
            updateMenuColors(menuItems, selectedItemIndex);
        }
    }
    else if (event.key.code == sf::Keyboard::Down) {
        if (selectedItemIndex < menuItems.size() - 1) {
            selectedItemIndex++;
            if (sfxEnabled) {
                menuChangeSound.play(); // Play sound without checking status
            }
            updateMenuColors(menuItems, selectedItemIndex);
        }
    }
    else if (event.key.code == sf::Keyboard::Enter) {
        if (selectedItemIndex == 0) {
            state = GameState::MODE_SELECT;
            modeSelectedIndex = 0;
            updateMenuColors(modeMenuItems, modeSelectedIndex);
        }
        else if (selectedItemIndex == 1) state = GameState::INSTRUCTIONS;
        else if (selectedItemIndex == 2) state = GameState::HIGH_SCORES;
        else if (selectedItemIndex == 3) {
            state = GameState::SETTINGS;
            settingsSelectedIndex = 0;
            initializeSettingsMenuItems();
            updateMenuColors(settingsMenuItems, settingsSelectedIndex);
        }
        else if (selectedItemIndex == 4) window.close();
    }
}
void Game::handleModeSelectInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up) {
            if (modeSelectedIndex > 0&& sfxEnabled) {
                modeSelectedIndex--;
                menuChangeSound.play();
                if ( menuChangeSound.getStatus() != sf::Sound::Playing) {
                    menuChangeSound.play();
                }
                updateMenuColors(modeMenuItems, modeSelectedIndex);
            }
        }
        else if (event.key.code == sf::Keyboard::Down) {
            if (modeSelectedIndex < modeMenuItems.size() - 1) {
                modeSelectedIndex++;
                if (sfxEnabled) {
                    menuChangeSound.play();
                }
                if (menuChangeSound.getStatus() != sf::Sound::Playing) {
                    menuChangeSound.play();
                }
                updateMenuColors(modeMenuItems, modeSelectedIndex);
            }
        }
        else if (event.key.code == sf::Keyboard::Enter) {
            if (modeSelectedIndex == 0) initialObstacleSpeed = 5.f; // Easy
            else if (modeSelectedIndex == 1) initialObstacleSpeed = 10.f; // Medium
            else if (modeSelectedIndex == 2) initialObstacleSpeed = 15.f; // Hard
            state = GameState::PLAYING;
            reset();
        }
        else if (event.key.code == sf::Keyboard::Escape) {
            state = GameState::MENU;
            selectedItemIndex = 0;
            updateMenuColors(menuItems, selectedItemIndex);
        }
    }
}

void Game::handlePauseMenuInput(const sf::Event& event) {
    auto& currentMenu = pauseMenuItems;
    int maxIndex = static_cast<int>(currentMenu.size()) - 1;
    if (event.key.code == sf::Keyboard::Up) {
        if (pauseSelectedIndex > 0&&sfxEnabled) {
            pauseSelectedIndex--;
            menuChangeSound.play();
            if (menuChangeSound.getStatus() != sf::Sound::Playing) {
                menuChangeSound.play();
            }
            updateMenuColors(currentMenu, pauseSelectedIndex);
        }
    }
    else if (event.key.code == sf::Keyboard::Down) {
        if (pauseSelectedIndex < maxIndex&&sfxEnabled) {
            menuChangeSound.play();
            pauseSelectedIndex++;
            if ( menuChangeSound.getStatus() != sf::Sound::Playing) {
                menuChangeSound.play();
            }
            updateMenuColors(currentMenu, pauseSelectedIndex);
        }
    }
    else if (event.key.code == sf::Keyboard::Escape) {
        state = GameState::PLAYING;
    }
    else if (event.key.code == sf::Keyboard::Enter) {
        if (pauseSelectedIndex == 0) state = GameState::PLAYING;
        else if (pauseSelectedIndex == 1) reset();
        else if (pauseSelectedIndex == 2) state = GameState::MENU;
        else if (pauseSelectedIndex == 3) window.close();
    }
}

void Game::handleGameOverInput(const sf::Event& event) {
    auto& currentMenu = gameOverMenuItems;
    int maxIndex = static_cast<int>(currentMenu.size()) - 1;
    if (event.key.code == sf::Keyboard::Up) {
        if (pauseSelectedIndex > 0) {
            pauseSelectedIndex--;
            if (sfxEnabled) {
                menuChangeSound.play(); // Play sound without checking status
            }
            updateMenuColors(currentMenu, pauseSelectedIndex);
        }
    }
    else if (event.key.code == sf::Keyboard::Down) {
        if (pauseSelectedIndex < maxIndex) {
            pauseSelectedIndex++;
            if (sfxEnabled) {
                menuChangeSound.play(); // Play sound without checking status
            }
            updateMenuColors(currentMenu, pauseSelectedIndex);
        }
    }
    else if (event.key.code == sf::Keyboard::Enter) {
        if (pauseSelectedIndex == 0) reset();
        else if (pauseSelectedIndex == 1) state = GameState::MENU;
        else if (pauseSelectedIndex == 2) window.close();
    }
}
void Game::handleHighScoreInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            state = GameState::MENU;
            selectedItemIndex = 0;
            updateMenuColors(menuItems, selectedItemIndex);
        }
        else if (event.key.code == sf::Keyboard::R) {
            resetHighScores();
            if (sfxEnabled && menuChangeSound.getStatus() != sf::Sound::Playing) {
                menuChangeSound.play();
            }
            highScoreText.setString("High Scores Cleared!\n\nPress B to go back\nPress R to reset");
        }
    }
}

void Game::handleSettingsInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up) {
            if (settingsSelectedIndex > 0) {
                settingsSelectedIndex--;
                if (sfxEnabled) {
                    menuChangeSound.play(); // Play sound without checking status
                }
                updateMenuColors(settingsMenuItems, settingsSelectedIndex);
            }
        }
        else if (event.key.code == sf::Keyboard::Down) {
            if (settingsSelectedIndex < settingsMenuItems.size() - 1) {
                settingsSelectedIndex++;
                if (sfxEnabled) {
                    menuChangeSound.play(); // Play sound without checking status
                }
                updateMenuColors(settingsMenuItems, settingsSelectedIndex);
            }
        }
        else if (event.key.code == sf::Keyboard::Enter) {
            if (settingsSelectedIndex == 0) {
                backgroundMusicEnabled = !backgroundMusicEnabled;
                if (!backgroundMusicEnabled) {
                    backgroundMusic.stop();
                }
                initializeSettingsMenuItems();
                updateMenuColors(settingsMenuItems, settingsSelectedIndex);
            }
            else if (settingsSelectedIndex == 1) {
                sfxEnabled = !sfxEnabled;
                if (!sfxEnabled) {
                    crashSound.stop();
                    menuChangeSound.stop();
                }
                initializeSettingsMenuItems();
                updateMenuColors(settingsMenuItems, settingsSelectedIndex);
            }
            else if (settingsSelectedIndex == 2) {
                state = GameState::MENU;
                selectedItemIndex = 0;
                updateMenuColors(menuItems, selectedItemIndex);
            }
        }
        else if (event.key.code == sf::Keyboard::Escape ) {
            state = GameState::MENU;
            selectedItemIndex = 0;
            updateMenuColors(menuItems, selectedItemIndex);
        }
    }
}

void Game::updateMenuColors(std::vector<sf::Text>& items, int selectedIndex) {
    for (size_t i = 0; i < items.size(); ++i)
        items[i].setFillColor(i == selectedIndex ? sf::Color::Blue : sf::Color::White);
}

void Game::loadHighScores() {
    highScores.clear();
    std::ifstream file("highscores.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            size_t commaPos = line.find(',');
            if (commaPos != std::string::npos) {
                std::string name = line.substr(0, commaPos);
                int score = std::stoi(line.substr(commaPos + 1));
                highScores.emplace_back(name, score);
            }
        }
        file.close();
    }
    std::sort(highScores.begin(), highScores.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    if (highScores.size() > 5) highScores.resize(5);
}

void Game::saveHighScores() {
    bool playerFound = false;
    for (auto& entry : highScores) {
        if (entry.first == player_name) {
            if (score > entry.second) {
                entry.second = score;
            }
            playerFound = true;
            break;
        }
    }
    if (!playerFound) {
        highScores.emplace_back(player_name, score);
    }
    std::sort(highScores.begin(), highScores.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    if (highScores.size() > 5) highScores.resize(5);
    std::ofstream file("highscores.txt");
    if (file.is_open()) {
        for (const auto& entry : highScores) {
            file << entry.first << "," << entry.second << "\n";
        }
        file.close();
    }
}

void Game::resetHighScores() {
    highScores.clear();
    std::ofstream file("highscores.txt", std::ios::trunc);
    if (file.is_open()) {
        file.close();
    }
}

void Game::update() {
    roadSprite1.move(0, roadSpeed);
    roadSprite2.move(0, roadSpeed);
    if (roadSprite1.getPosition().y >= roadHeight) {
        roadSprite1.setPosition(0, roadSprite2.getPosition().y - roadHeight);
    }
    if (roadSprite2.getPosition().y >= roadHeight) {
        roadSprite2.setPosition(0, roadSprite1.getPosition().y - roadHeight);
    }
    player->update();
    obstacle->update();
    obstacle->increaseSpeed(speedIncreaseRate * gameClock.getElapsedTime().asSeconds());
    gameClock.restart();

    // Check if obstacle has moved off-screen (bottom edge past window height)
    sf::FloatRect bounds = obstacle->getBounds();
    if (bounds.top + bounds.height > 600.f) {
        obstacle->reset(800.f, 600.f); // Respawn at top with random X
        score++;
        scoreText.setString("Score: " + std::to_string(score));
    }

    if (player->getBounds().intersects(obstacle->getBounds())) {
        if (sfxEnabled && crashSound.getStatus() != sf::Sound::Playing) {
            crashSound.play();
        }
        saveHighScores();
        state = GameState::GAME_OVER;
        pauseSelectedIndex = 0;
        updateMenuColors(gameOverMenuItems, pauseSelectedIndex);
        finalScoreText.setString(player_name + "'s Score: " + std::to_string(score));
    }
}

void Game::render() {
    window.clear(sf::Color::Black);
    manageBackgroundMusic();
    switch (state) {
    case GameState::Player_Name:
        window.draw(menuBackground);
        window.draw(inputText);
        window.draw(nameDisplay);
        break;
    case GameState::MENU:
        window.draw(menuBackground);
        for (const auto& item : menuItems) window.draw(item);
        break;
    case GameState::MODE_SELECT:
        window.draw(menuBackground);
        for (const auto& item : modeMenuItems) window.draw(item);
        break;
    case GameState::INSTRUCTIONS:
        window.draw(menuBackground);
        window.draw(instructionText);
        break;
    case GameState::PLAYING:
        window.draw(roadSprite1);
        window.draw(roadSprite2);
        player->render(window);
        obstacle->render(window);
        window.draw(scoreText);
        break;
    case GameState::PAUSED:
        window.draw(roadSprite1);
        window.draw(roadSprite2);
        player->render(window);
        obstacle->render(window);
        for (const auto& item : pauseMenuItems) window.draw(item);
        break;
    case GameState::GAME_OVER:
        window.draw(roadSprite1);
        window.draw(roadSprite2);
        player->render(window);
        obstacle->render(window);
        window.draw(gameOverText);
        window.draw(finalScoreText);
        for (const auto& item : gameOverMenuItems) window.draw(item);
        break;
    case GameState::HIGH_SCORES:
        window.draw(menuBackground);
        if (highScores.empty()) {
            highScoreText.setString("High Scores Cleared!\n\nPress Escape to go back\nPress R to reset");
        }
        else {
            std::string highScoreString = "High Scores:\n\n";
            for (size_t i = 0; i < highScores.size(); ++i) {
                highScoreString += std::to_string(i + 1) + ". " + highScores[i].first + ": " + std::to_string(highScores[i].second) + "\n";
            }
            highScoreString += "\nPress Escape to go back\nPress R to reset";
            highScoreText.setString(highScoreString);
        }
        window.draw(highScoreText);
        break;
    case GameState::SETTINGS:
        window.draw(menuBackground);
        for (const auto& item : settingsMenuItems) window.draw(item);
        break;
    }
    window.display();
}

void Game::manageBackgroundMusic() {
    if (!backgroundMusicEnabled) {
        if (backgroundMusic.getStatus() == sf::Music::Playing) {
            backgroundMusic.stop();
        }
        return;
    }
    if (state == GameState::PLAYING) {
        if (backgroundMusic.getStatus() == sf::Music::Paused) {
            backgroundMusic.play();
        }
        else if (backgroundMusic.getStatus() != sf::Music::Playing) {
            backgroundMusic.play();
        }
    }
    else if (state == GameState::PAUSED) {
        if (backgroundMusic.getStatus() == sf::Music::Playing) {
            backgroundMusic.pause();
        }
    }
    else {
        if (backgroundMusic.getStatus() == sf::Music::Playing) {
            backgroundMusic.stop();
        }
    }
}

void Game::reset() {
    state = GameState::PLAYING;
    player->reset();
    delete obstacle;
    obstacle = new Obstacle(initialObstacleSpeed);
    obstacle->reset(800.f, 600.f);
    roadSprite1.setPosition(0, 0);
    roadSprite2.setPosition(0, -roadHeight);
    score = 0;
    scoreText.setString("Score: 0");
    selectedItemIndex = 0;
    pauseSelectedIndex = 0;
    updateMenuColors(menuItems, selectedItemIndex);
    updateMenuColors(pauseMenuItems, pauseSelectedIndex);
    updateMenuColors(gameOverMenuItems, pauseSelectedIndex);
    gameClock.restart();
}