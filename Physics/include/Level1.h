#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <vector>

class Level1 : public Scene {
private:
    b2World world;              // Box2D world
    float pixelsPerMeter;        // Scaling factor for SFML pixels to Box2D meters
    SceneManager& sceneManager;  // Reference to scene manager

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    sf::Texture grassTexture;  // Texture for the grass ground
    std::vector<sf::Sprite> grassTiles;  // Tiles for the grass ground

    b2Body* groundBody;   // Box2D body for the ground
    b2Body* blockBody;    // Box2D body for the block

    sf::Texture blockTexture;    // Texture for the falling block
    sf::RectangleShape blockShape; // Visual representation of the block

    bool isPaused = false;        // Pause state
    sf::RectangleShape darkOverlay;  // Dark overlay when paused
    sf::Text resumeButton;        // Resume button in pause menu
    sf::Text restartButton;       // Restart button in pause menu
    sf::Text menuButton;          // Menu button in pause menu
    sf::Font font;                // Font for the buttons
    bool pKeyPressed = false;     // To track the state of the 'P' key

public:
    Level1(SceneManager& manager);

    void init() override;                     // Initialize the level
    void handleInput(sf::RenderWindow& window, sf::Event& event) override;  // Handle player input
    void update(float deltaTime) override;    // Update the game objects
    void draw(sf::RenderWindow& window) override;  // Render the game objects
    void updateButtonPositions(const sf::Vector2u& windowSize) override;   // Update positions of UI elements based on window size

    void togglePause();  // Function to toggle the pause state
};
