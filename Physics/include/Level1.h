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
	struct Block
	{
		sf::RectangleShape MShape;
		b2Body* MBody;
		bool isDestructible;
		int MHealth;
	};

	struct Enemy
	{
		sf::Sprite MEnemySprite;
		b2Body* MEnemyBody;
		bool isAlive;
	};

	void initSeesaw();
	void initBlocks(const std::vector<std::tuple<sf::Vector2f, bool, int>>& BlockData);
	void initEnemies(const std::vector<sf::Vector2f>& Positions);
	void spawnProjectile();
	void launchProjectile(const sf::Vector2f& Start, const sf::Vector2f& End);
	void calculateParabolicTrajectory(const sf::Vector2f& Start, const sf::Vector2f& End);
	void handleCollisions();
	void checkEnemiesAlive();

	SceneManager& MSceneManager;

	b2World MWorld;

	b2Body *MGroundBody, *MProjectileBody, *MSeesawBase, *MSeesawPlank1, *MSeesawPlank2;

	b2RevoluteJoint* MSeesawJoint;

	b2WeldJoint* MPlankJoint;

	float MPixelsPerMetre, MStationaryTime, MScreenLeftBound, MScreenRightBound;

	int MRemainingProjectiles;

	bool isPaused, PKeyPressed, isDragging, isProjectileLaunched, isProjectileStopped, isWin, isLose;

	std::vector<Block> MBlocks;
	std::vector<Enemy> MEnemies;
	std::vector<sf::CircleShape> MTrajectoryPoints;
	std::vector<sf::Sprite> MGrassTiles;

	sf::CircleShape MProjectileShape;

	sf::Font MFont;

	sf::RectangleShape MDarkOverlay;

	sf::Sprite MBackgroundSprite, MSeesawBaseSprite, MSeesawPlankSprite1, MSeesawPlankSprite2;

	sf::Text MResumeButton, MRestartButton, MMenuButton, MNextLevelButton;

	sf::Texture MBackgroundTexture, MGrassTexture, MBlockTexture, MDestructibleBlockTexture, MProjectileTexture,
	            MEnemyTexture, MSeesawBaseTexture, MSeesawPlankTexture;

	sf::Vector2f MDragStart, MDragEnd;

	sf::VertexArray MDragLine;
};
