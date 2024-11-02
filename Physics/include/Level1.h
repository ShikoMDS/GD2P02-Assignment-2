#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include "TitleScreen.h"
#include "Level2.h"
#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

class Level1 final : public Scene
{
public:
	explicit Level1(SceneManager& Manager);

	void init() override;
	void draw(sf::RenderWindow& Window) override;
	void update(float DeltaTime) override;
	void handleInput(sf::RenderWindow& Window, sf::Event& Event) override;
	void updateButtonPositions(const sf::Vector2u& WindowSize) override;

	void togglePause();

private:
	// Multiple enemy support (same type for now)
	struct Enemy
	{
		sf::Sprite MEnemySprite;
		b2Body* MEnemyBody;
		bool isAlive;
	};

	std::vector<Enemy> MEnemies; // Store multiple enemies

	struct Block {
		sf::RectangleShape shape;
		b2Body* body;
		bool isDestructible;
		int health;  // Health for destructible blocks
	};

	std::vector<Block> MBlocks;
	void initBlocks(const std::vector<std::tuple<sf::Vector2f, bool, int>>& blockData);  // Vector includes position, destructibility, and health

	void initEnemies(const std::vector<sf::Vector2f>& Positions);
	void spawnProjectile();
	void launchProjectile(const sf::Vector2f& Start, const sf::Vector2f& End);
	void calculateParabolicTrajectory(const sf::Vector2f& Start, const sf::Vector2f& End);
	void handleCollisions();
	void checkEnemiesAlive();


	b2World MWorld;
	float MPixelsPerMetre, MStationaryTime, MScreenLeftBound, MScreenRightBound;
	SceneManager& MSceneManager;

	//b2Body* MBlockBody;
	b2Body* MGroundBody;
	b2Body* MProjectileBody;

	sf::CircleShape MProjectileShape; // Visual representation of projectile
	sf::RectangleShape /*MBlockShape, */MDarkOverlay;
	sf::Texture MBackgroundTexture, MGrassTexture, MBlockTexture, MDestructibleBlockTexture, MProjectileTexture, MEnemyTexture;
	sf::Sprite MBackgroundSprite;

	sf::Font MFont;
	sf::Text MResumeButton, MRestartButton, MMenuButton, MNextLevelButton;

	sf::Vector2f MDragStart, MDragEnd;
	sf::VertexArray MDragLine; // Visual indicator for drag direction
	std::vector<sf::CircleShape> MTrajectoryPoints; // Parabolic trajectory visualisation

	std::vector<sf::Sprite> MGrassTiles;

	bool isPaused, PKeyPressed, isDragging, isProjectileLaunched, isProjectileStopped, isWin, isLose;

	int MRemainingProjectiles;
};
