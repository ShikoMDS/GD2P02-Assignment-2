#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include "TitleScreen.h"
#include "Level2.h"
#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

class Level1 final : public Scene {
public:
	explicit Level1(SceneManager& Manager);

    void init() override;                     
    void handleInput(sf::RenderWindow& Window, sf::Event& Event) override; 
    void update(float DeltaTime) override;   
    void draw(sf::RenderWindow& Window) override;  
    void updateButtonPositions(const sf::Vector2u& WindowSize) override; 

    void togglePause(); 

private:
    b2World MWorld;
    float MPixelsPerMeter; // Scaling factor for SFML pixels to Box2D meters
    SceneManager& MSceneManager;

    sf::Texture MBackgroundTexture;
    sf::Sprite MBackgroundSprite;

    sf::Texture MGrassTexture;  
    std::vector<sf::Sprite> MGrassTiles; 

    b2Body* MGroundBody; 
    b2Body* MBlockBody; 

    sf::Texture MBlockTexture;
    sf::RectangleShape MBlockShape; // Visual representation of block

    bool isPaused = false; 
    sf::RectangleShape MDarkOverlay; 
    sf::Text MResumeButton; 
    sf::Text MRestartButton;
    sf::Text MMenuButton;   
    sf::Font MFont;         
    bool PKeyPressed = false;     // State of 'P' key

    b2Body* MProjectileBody;        
    sf::Texture MProjectileTexture; 
    sf::CircleShape MProjectileShape; // Visual representation of projectile

    sf::Vector2f MDragStart;       
    sf::Vector2f MDragEnd;         
    bool isDragging = false;       
    sf::VertexArray MDragLine;       // Visual indicator for drag direction
    std::vector<sf::CircleShape> MTrajectoryPoints; // Parabolic trajectory visualization


    // Multiple enemy support (same type for now)
    struct Enemy {
        sf::Sprite MEnemySprite;
        b2Body* MEnemyBody;
        bool isAlive;
    };
    std::vector<Enemy> MEnemies;    // Store multiple enemies


    bool isWin;

    sf::Text MNextLevelButton; 


    sf::Texture MEnemyTexture;


    bool isProjectileLaunched = false;  
    bool isProjectileStopped = false;   
    int MRemainingProjectiles;
    float MStationaryTime = 0.0f; 

    float MScreenLeftBound = -50.0f;      // Left edge (off-screen)
    float MScreenRightBound = 1650.0f;     // Right edge (off-screen)

    bool isLose;

    void initEnemies(const std::vector<sf::Vector2f>& Positions);
    void spawnProjectile();   
    void launchProjectile(const sf::Vector2f& Start, const sf::Vector2f& End);
    void calculateParabolicTrajectory(const sf::Vector2f& Start, const sf::Vector2f& End);
    void handleCollisions();    
    void checkEnemiesAlive();
};
