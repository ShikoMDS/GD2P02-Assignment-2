#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include "TitleScreen.h"
#include "Level2.h"
#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

class Level1 : public Scene {
public:
    Level1(SceneManager& manager);

    void init() override;                     // Initialize the level
    void handleInput(sf::RenderWindow& window, sf::Event& event) override;  // Handle player input
    void update(float deltaTime) override;    // Update the game objects
    void draw(sf::RenderWindow& window) override;  // Render the game objects
    void updateButtonPositions(const sf::Vector2u& windowSize) override;   // Update positions of UI elements based on window size

    void togglePause();  // Function to toggle the pause state

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

    b2Body* projectileBody;        // Box2D body for the projectile
    sf::Texture projectileTexture;  // Texture for the projectile character
    sf::CircleShape projectileShape; // SFML representation of the projectile

    sf::Vector2f dragStart;         // Start position of drag
    sf::Vector2f dragEnd;           // End position of drag
    bool isDragging = false;        // Track if currently dragging
    sf::VertexArray dragLine;       // Visual indicator for drag direction
    std::vector<sf::CircleShape> trajectoryPoints; // Parabolic trajectory visualization

    void launchProjectile(const sf::Vector2f& start, const sf::Vector2f& end);
    void calculateParabolicTrajectory(const sf::Vector2f& start, const sf::Vector2f& end);

    // Multiple enemy support
    struct Enemy {
        sf::Sprite sprite;
        b2Body* body;
        bool isAlive;
    };
    std::vector<Enemy> enemies;    // Store multiple enemies

    void handleCollisions();       // New function to handle collision logic
    void initEnemies(const std::vector<sf::Vector2f>& positions);
    // Initialize the enemy entities

    bool isWin;

    sf::Text nextLevelButton;  // Button to transition to the next level

    void checkEnemiesAlive();      // Check if all enemies are dead

    sf::Texture enemyTexture;

    bool isLose;               // Track if the player has lost

    bool isProjectileLaunched = false;  // New flag to track if the projectile has been launched
    bool isProjectileStopped = false;   // Flag to track if projectile has stopped moving

    float stationaryTime = 0.0f;  // Track how long the projectile has been stationary
	float removalTimeThreshold = 3.0f;  // Time (in seconds) before removal

    int remainingProjectiles;  // Tracks how many projectiles are left
    void spawnProjectile();    // Function to spawn a new projectile

    const float screenLeftBound = -100.0f;      // Left edge (off-screen)
    const float screenRightBound = 1600.0f;     // Right edge (off-screen, adjust based on screen width)

};
