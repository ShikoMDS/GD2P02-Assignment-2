#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>  // For background sprite

class Level2 : public Scene {
private:
    b2World world;
    SceneManager& sceneManager;

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    bool isPaused = false;
    bool pKeyPressed = false;  // To track the state of the 'P' key
    sf::RectangleShape darkOverlay;
    sf::Text resumeButton;
    sf::Text restartButton;
    sf::Text menuButton;
    sf::Font font;

public:
    Level2(SceneManager& manager);

    void init() override;
    void handleInput(sf::RenderWindow& window, sf::Event& event) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;

    void togglePause();
};
