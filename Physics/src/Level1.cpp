#include "Level1.h"

Level1::Level1(SceneManager& Manager)
	: MSceneManager(Manager), MWorld(b2Vec2(0.0f, 9.8f)), MGroundBody(nullptr), MProjectileBody(nullptr),
	  MSeesawBase(nullptr),
	  MSeesawPlank1(nullptr),
	  // Set gravity for Box2D world
	  MSeesawPlank2(nullptr),
	  MSeesawJoint(nullptr), MPlankJoint(nullptr), MPixelsPerMetre(100.0f),
	  MRemainingProjectiles(0)
// 100 pixels per meter for scaling
{
	// Load background texture
	if (!MBackgroundTexture.loadFromFile("resources/kenney physics assets/PNG/Backgrounds/colored_desert.png"))
	{
		// Handle loading error
	}
	MBackgroundSprite.setTexture(MBackgroundTexture);

	// Set background to stretch across window size
	const sf::Vector2u LWindowSize(1600, 900); // Window size is locked
	const sf::Vector2u LBgTextureSize = MBackgroundTexture.getSize();
	MBackgroundSprite.setScale(
		static_cast<float>(LWindowSize.x) / static_cast<float>(LBgTextureSize.x),
		static_cast<float>(LWindowSize.y) / static_cast<float>(LBgTextureSize.y)
	);

	// Load grass texture and create tiles
	if (!MGrassTexture.loadFromFile("resources/kenney physics assets/PNG/Other/grass.png"))
	{
		// Handle loading error
	}

	// Create ground tiles by tiling grass texture across screen width
	for (int I = 0; I < 1600 / 70 + 1; ++I)
	{
		// Grass tile is 70x70
		sf::Sprite LGrassTile;
		LGrassTile.setTexture(MGrassTexture);
		LGrassTile.setPosition(static_cast<float>(I) * 70.0f, 830.0f); // Position at bottom of window
		MGrassTiles.push_back(LGrassTile);
	}

	// Load block texture
	if (!MDestructibleBlockTexture.loadFromFile("resources/kenney physics assets/PNG/Wood elements/elementWood010.png"))
	{
		// Handle error
	}
	// Load block texture
	if (!MBlockTexture.loadFromFile("resources/kenney physics assets/PNG/Metal elements/elementMetal011.png"))
	{
		// Handle error
	}

	// Initialize dark overlay for pause state
	MDarkOverlay.setFillColor(sf::Color(0, 0, 0, 150)); // Black overlay with transparency

	// Set up buttons and font for pause menu
	if (!MFont.loadFromFile("resources/sugar bread/Sugar Bread.otf"))
	{
		// Handle error
	}
	MResumeButton.setFont(MFont);
	MResumeButton.setString("Resume");
	MRestartButton.setFont(MFont);
	MRestartButton.setString("Restart");
	MMenuButton.setFont(MFont);
	MMenuButton.setString("Menu");
	MNextLevelButton.setFont(MFont); // Use preloaded font
	MNextLevelButton.setString("Next Level");
	MNextLevelButton.setCharacterSize(50); // Initial text size

	// Load projectile texture
	if (!MProjectileTexture.loadFromFile("resources/kenney animal pack redux/PNG/Round (outline)/frog.png"))
	{
		// Handle loading error
	}

	// Initialize projectile shape
	MProjectileShape.setRadius(25.0f);
	MProjectileShape.setOrigin(25.0f, 25.0f);
	MProjectileShape.setTexture(&MProjectileTexture);

	// Drag line setup
	MDragLine = sf::VertexArray(sf::Lines, 2);

	isPaused = false;
	PKeyPressed = false;
	isDragging = false;
	isProjectileLaunched = false;
	isProjectileStopped = false;
	isWin = false;
	isLose = false;

	MStationaryTime = 0.0f;
	MScreenLeftBound = -50.0f; // Left edge (off-screen)
	MScreenRightBound = 1650.0f; // Right edge (off-screen)
}

void Level1::init()
{
	// Define ground body for Box2D
	b2BodyDef LGroundBodyDef;
	LGroundBodyDef.position.Set(800.0f / MPixelsPerMetre, 850.0f / MPixelsPerMetre); // Box2D works in metres
	MGroundBody = MWorld.CreateBody(&LGroundBodyDef);

	b2PolygonShape LGroundBox;
	LGroundBox.SetAsBox(800.0f / MPixelsPerMetre, 10.0f / MPixelsPerMetre); // Half-width, half-height in metres
	MGroundBody->CreateFixture(&LGroundBox, 0.0f); // Static ground with zero density

	// Define projectile body for Box2D
	b2BodyDef LProjectileBodyDef;
	LProjectileBodyDef.type = b2_dynamicBody;
	LProjectileBodyDef.position.Set(200.0f / MPixelsPerMetre, 800.0f / MPixelsPerMetre);
	// Set initial position in meters
	MProjectileBody = MWorld.CreateBody(&LProjectileBodyDef);

	b2CircleShape LProjectileShapeDef;
	LProjectileShapeDef.m_radius = 25.0f / MPixelsPerMetre; // Set radius (in metres)

	b2FixtureDef LProjectileFixtureDef;
	LProjectileFixtureDef.shape = &LProjectileShapeDef;
	LProjectileFixtureDef.density = 1.0f;
	LProjectileFixtureDef.friction = 0.5f; // Set appropriate friction
	//LProjectileFixtureDef.restitution = 0.5f; // Set restitution (bounciness)
	MProjectileBody->CreateFixture(&LProjectileFixtureDef);


	std::vector LEnemyPositions = {
		sf::Vector2f(1100.0f, 800.0f), // First enemy position
		sf::Vector2f(1200.0f, 800.0f), // Second enemy position
		sf::Vector2f(1300.0f, 800.0f), // Third enemy position
		sf::Vector2f(1500.0f, 800.0f) // Fourth enemy position
	};

	initEnemies(LEnemyPositions);

	// Initialize blocks at specific positions
	// Add block data with destructible information
	const std::vector<std::tuple<sf::Vector2f, bool, int>> LBlockData = {
		// Destructible blocks
		{{800.0f, 800.0f}, true, 1},
		{{425.0f, 600.0f}, true, 1},
		{{575.0f, 600.0f}, true, 1},
		{{900.0f, 800.0f}, true, 1},
		// Regular blocks
		{{850.0f, 705.0f}, false, -1},
		{{450.0f, 505.0f}, false, -1},
		{{550.0f, 505.0f}, false, -1},
		{{425.0f, 695.0f}, false, -1},
		{{575.0f, 695.0f}, false, -1},
		{{500.0f, 410.0f}, false, -1}
	};
	initBlocks(LBlockData);

	initSeesaw();

	MRemainingProjectiles = 3; // Set number of projectiles

	spawnProjectile(); // Spawn first projectile
}

void Level1::draw(sf::RenderWindow& Window)
{
	// Draw background
	Window.draw(MBackgroundSprite);

	// Draw ground tiles
	for (const auto& Tile : MGrassTiles)
	{
		Window.draw(Tile);
	}

	// Draw each block
	for (const auto& Block : MBlocks)
	{
		Window.draw(Block.MShape);
	}

	// Draw seesaw components
	Window.draw(MSeesawBaseSprite);
	Window.draw(MSeesawPlankSprite1);
	Window.draw(MSeesawPlankSprite2);

	// Draw projectile if it's not removed
	if (!isProjectileStopped)
	{
		Window.draw(MProjectileShape);
	}

	// Draw drag line only if projectile has not been launched and dragging is happening
	if (isDragging && !isProjectileLaunched)
	{
		for (const auto& Point : MTrajectoryPoints)
		{
			Window.draw(Point);
		}
	}

	// Draw enemy if alive
	for (auto& Enemy : MEnemies)
	{
		if (Enemy.isAlive)
		{
			Window.draw(Enemy.MEnemySprite);
		}
	}

	float LScaleY = static_cast<float>(Window.getSize().y) / 900.0f; // 900 is reference height

	// Handle win screen
	if (isWin)
	{
		// Draw dark overlay
		MDarkOverlay.setSize(sf::Vector2f(static_cast<float>(Window.getSize().x),
		                                  static_cast<float>(Window.getSize().y)));
		Window.draw(MDarkOverlay);

		// "You Win" Title
		sf::Text LWinText;
		LWinText.setFont(MFont);
		LWinText.setString("You Win!");
		LWinText.setCharacterSize(static_cast<unsigned int>(100 * LScaleY));
		sf::FloatRect LWinBounds = LWinText.getLocalBounds();
		LWinText.setOrigin(LWinBounds.width / 2.0f, LWinBounds.height / 2.0f);
		LWinText.setPosition(static_cast<float>(Window.getSize().x) / 2.0f,
		                     static_cast<float>(Window.getSize().y) / 4.0f); // Centered title
		Window.draw(LWinText);

		// Draw "Next Level" button
		MNextLevelButton.setCharacterSize(static_cast<unsigned int>(50 * LScaleY));
		sf::FloatRect LNextBounds = MNextLevelButton.getLocalBounds();
		MNextLevelButton.setOrigin(LNextBounds.width / 2.0f, LNextBounds.height / 2.0f);
		MNextLevelButton.setPosition(static_cast<float>(Window.getSize().x) / 2.0f,
		                             static_cast<float>(Window.getSize().y) / 2.0f); // Centered under title
		Window.draw(MNextLevelButton);

		// Draw "Restart" button
		MRestartButton.setCharacterSize(static_cast<unsigned int>(50 * LScaleY));
		sf::FloatRect LRestartBounds = MRestartButton.getLocalBounds();
		MRestartButton.setOrigin(LRestartBounds.width / 2.0f, LRestartBounds.height / 2.0f);
		MRestartButton.setPosition(static_cast<float>(Window.getSize().x) / 2.0f,
		                           static_cast<float>(Window.getSize().y) / 2.0f + 100 * LScaleY);
		// Centered and spaced
		Window.draw(MRestartButton);

		// Draw "Menu" button
		MMenuButton.setCharacterSize(static_cast<unsigned int>(50 * LScaleY));
		sf::FloatRect LMenuBounds = MMenuButton.getLocalBounds();
		MMenuButton.setOrigin(LMenuBounds.width / 2.0f, LMenuBounds.height / 2.0f);
		MMenuButton.setPosition(static_cast<float>(Window.getSize().x) / 2.0f,
		                        static_cast<float>(Window.getSize().y) / 2.0f + 200 * LScaleY); // Centered and spaced
		Window.draw(MMenuButton);
	}

	if (isLose)
	{
		// Draw dark overlay
		MDarkOverlay.setSize(sf::Vector2f(static_cast<float>(Window.getSize().x),
		                                  static_cast<float>(Window.getSize().y)));
		Window.draw(MDarkOverlay);

		// "You Lose!" Title
		sf::Text LLoseText;
		LLoseText.setFont(MFont);
		LLoseText.setString("You Lose!");
		LLoseText.setCharacterSize(static_cast<unsigned int>(100 * LScaleY));
		sf::FloatRect LLoseBounds = LLoseText.getLocalBounds();
		LLoseText.setOrigin(LLoseBounds.width / 2.0f, LLoseBounds.height / 2.0f);
		LLoseText.setPosition(static_cast<float>(Window.getSize().x) / 2.0f,
		                      static_cast<float>(Window.getSize().y) / 4.0f); // Centered title
		Window.draw(LLoseText);

		// Draw "Restart" button
		MRestartButton.setCharacterSize(static_cast<unsigned int>(50 * LScaleY));
		sf::FloatRect LRestartBounds = MRestartButton.getLocalBounds();
		MRestartButton.setOrigin(LRestartBounds.width / 2.0f, LRestartBounds.height / 2.0f);
		MRestartButton.setPosition(static_cast<float>(Window.getSize().x) / 2.0f,
		                           static_cast<float>(Window.getSize().y) / 2.0f + 100 * LScaleY);
		// Centered and spaced
		Window.draw(MRestartButton);

		// Draw "Menu" button
		MMenuButton.setCharacterSize(static_cast<unsigned int>(50 * LScaleY));
		sf::FloatRect LMenuBounds = MMenuButton.getLocalBounds();
		MMenuButton.setOrigin(LMenuBounds.width / 2.0f, LMenuBounds.height / 2.0f);
		MMenuButton.setPosition(static_cast<float>(Window.getSize().x) / 2.0f,
		                        static_cast<float>(Window.getSize().y) / 2.0f + 200 * LScaleY); // Centered and spaced
		Window.draw(MMenuButton);
	}

	// Handle pause screen if not on win screen
	if (isPaused && !isWin && !isLose)
	{
		// Draw dark overlay
		MDarkOverlay.setSize(sf::Vector2f(static_cast<float>(Window.getSize().x),
		                                  static_cast<float>(Window.getSize().y)));
		Window.draw(MDarkOverlay);

		// "Paused" Title
		sf::Text LPausedText;
		LPausedText.setFont(MFont);
		LPausedText.setString("Paused");
		LPausedText.setCharacterSize(static_cast<unsigned int>(100 * LScaleY));
		sf::FloatRect LPausedBounds = LPausedText.getLocalBounds();
		LPausedText.setOrigin(LPausedBounds.width / 2.0f, LPausedBounds.height / 2.0f);
		LPausedText.setPosition(static_cast<float>(Window.getSize().x) / 2.0f,
		                        static_cast<float>(Window.getSize().y) / 4.0f); // Centered title
		Window.draw(LPausedText);

		// Draw "Resume" button
		MResumeButton.setCharacterSize(static_cast<unsigned int>(50 * LScaleY));
		sf::FloatRect LResumeBounds = MResumeButton.getLocalBounds();
		MResumeButton.setOrigin(LResumeBounds.width / 2.0f, LResumeBounds.height / 2.0f);
		MResumeButton.setPosition(static_cast<float>(Window.getSize().x) / 2.0f,
		                          static_cast<float>(Window.getSize().y) / 2.0f); // Centered under title
		Window.draw(MResumeButton);

		// Draw "Restart" button
		MRestartButton.setCharacterSize(static_cast<unsigned int>(50 * LScaleY));
		sf::FloatRect LRestartBounds = MRestartButton.getLocalBounds();
		MRestartButton.setOrigin(LRestartBounds.width / 2.0f, LRestartBounds.height / 2.0f);
		MRestartButton.setPosition(static_cast<float>(Window.getSize().x) / 2.0f,
		                           static_cast<float>(Window.getSize().y) / 2.0f + 100 * LScaleY);
		// Centered and spaced
		Window.draw(MRestartButton);

		// Draw "Menu" button
		MMenuButton.setCharacterSize(static_cast<unsigned int>(50 * LScaleY));
		sf::FloatRect LMenuBounds = MMenuButton.getLocalBounds();
		MMenuButton.setOrigin(LMenuBounds.width / 2.0f, LMenuBounds.height / 2.0f);
		MMenuButton.setPosition(static_cast<float>(Window.getSize().x) / 2.0f,
		                        static_cast<float>(Window.getSize().y) / 2.0f + 200 * LScaleY); // Centered and spaced
		Window.draw(MMenuButton);
	}
}

void Level1::update(const float DeltaTime)
{
	if (!isPaused && !isWin && !isLose)
	{
		MWorld.Step(DeltaTime, 8, 8);

		// Block and projectile collision update 
		for (auto It = MBlocks.begin(); It != MBlocks.end();)
		{
			Block& Block = *It;
			if (Block.isDestructible)
			{
				b2Vec2 Velocity = Block.MBody->GetLinearVelocity();
				const float ImpactForce = Velocity.Length();

				// Break threshold
				if (ImpactForce > 2.0f)
				{
					Block.MHealth--;
					if (Block.MHealth <= 0)
					{
						MWorld.DestroyBody(Block.MBody);
						It = MBlocks.erase(It);
						continue;
					}
				}
			}
			const b2Vec2 Position = Block.MBody->GetPosition();
			const float Angle = Block.MBody->GetAngle();
			Block.MShape.setPosition(Position.x * MPixelsPerMetre, Position.y * MPixelsPerMetre);
			Block.MShape.setRotation(Angle * 180.0f / b2_pi);

			++It;
		}

		// Update and draw seesaw components
		if (MSeesawBase && MSeesawPlank1 && MSeesawPlank2)
		{
			// Update base position
			const b2Vec2 BasePos = MSeesawBase->GetPosition();
			MSeesawBaseSprite.setPosition(BasePos.x * MPixelsPerMetre, BasePos.y * MPixelsPerMetre);

			// Update plank 1 position and rotation
			const b2Vec2 Plank1Pos = MSeesawPlank1->GetPosition();
			const float Plank1Angle = MSeesawPlank1->GetAngle();
			MSeesawPlankSprite1.setPosition(Plank1Pos.x * MPixelsPerMetre, Plank1Pos.y * MPixelsPerMetre);
			MSeesawPlankSprite1.setRotation(Plank1Angle * 180.0f / b2_pi);

			// Update plank 2 position and rotation
			const b2Vec2 Plank2Pos = MSeesawPlank2->GetPosition();
			const float Plank2Angle = MSeesawPlank2->GetAngle();
			MSeesawPlankSprite2.setPosition(Plank2Pos.x * MPixelsPerMetre, Plank2Pos.y * MPixelsPerMetre);
			MSeesawPlankSprite2.setRotation(Plank2Angle * 180.0f / b2_pi);
		}

		// Remaining projectile update and collision handling
		if (isProjectileLaunched && !isProjectileStopped)
		{
			const b2Vec2 Velocity = MProjectileBody->GetLinearVelocity();
			if (Velocity.Length() < 0.1f)
			{
				MStationaryTime += DeltaTime;
			}
			else
			{
				MStationaryTime = 0.0f;
			}
			if (MStationaryTime > 3.0f)
			{
				isProjectileStopped = true;
				MWorld.DestroyBody(MProjectileBody);
				MProjectileShape.setPosition(-100, -100); // Off-screen
				spawnProjectile();
			}
			const b2Vec2 ProjectilePosition = MProjectileBody->GetPosition();
			if (ProjectilePosition.x * MPixelsPerMetre < MScreenLeftBound || ProjectilePosition.x * MPixelsPerMetre >
				MScreenRightBound)
			{
				isProjectileStopped = true;
				MWorld.DestroyBody(MProjectileBody);
				MProjectileShape.setPosition(-100, -100);
				spawnProjectile();
			}
		}

		// General collision handling
		handleCollisions();
		checkEnemiesAlive();

		// Projectile rendering update
		if (!isProjectileStopped)
		{
			const b2Vec2 ProjectilePosition = MProjectileBody->GetPosition();
			const float ProjectileAngle = MProjectileBody->GetAngle();
			MProjectileShape.setPosition(ProjectilePosition.x * MPixelsPerMetre,
			                             ProjectilePosition.y * MPixelsPerMetre);
			MProjectileShape.setRotation(ProjectileAngle * 180.0f / b2_pi);
		}
	}
}

void Level1::handleInput(sf::RenderWindow& Window, sf::Event& Event)
{
	/*
	// DEBUG: set lose
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::L) && !isPaused && !isWin)
	{
	    isLose = true;
	}
	*/

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && !isPaused && !isWin && !isLose)
	{
		MSceneManager.setScene(std::make_shared<Level1>(MSceneManager)); // Restart level
	}

	// Handle pausing with 'P' key avoiding key repeat
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && !isWin && !isLose)
	{
		if (!PKeyPressed)
		{
			PKeyPressed = true; // Mark key as pressed
			togglePause(); // Toggle pause when 'P' is pressed once

			// Clear any input states related to dragging
			isDragging = false;
			MTrajectoryPoints.clear();
		}
	}
	else
	{
		PKeyPressed = false; // Reset flag when key is released
	}

	if (isPaused && !isWin && !isLose)
	{
		// Handle mouse interaction only for pause menu buttons when paused
		if (Event.type == sf::Event::MouseButtonReleased)
		{
			const sf::Vector2i LMousePos = sf::Mouse::getPosition(Window);
			if (MResumeButton.getGlobalBounds().contains(static_cast<float>(LMousePos.x),
			                                             static_cast<float>(LMousePos.y)))
			{
				togglePause(); // Resume game
			}
			else if (MRestartButton.getGlobalBounds().contains(static_cast<float>(LMousePos.x),
			                                                   static_cast<float>(LMousePos.y)))
			{
				MSceneManager.setScene(std::make_shared<Level1>(MSceneManager)); // Restart level
			}
			else if (MMenuButton.getGlobalBounds().contains(static_cast<float>(LMousePos.x),
			                                                static_cast<float>(LMousePos.y)))
			{
				MSceneManager.setScene(std::make_shared<TitleScreen>(MSceneManager)); // Go back to menu
			}
		}
	}
	if (!isPaused && !isWin && !isLose)
	{
		// Only allow input when game is not paused
		if (Event.type == sf::Event::MouseButtonPressed && Event.mouseButton.button == sf::Mouse::Left)
		{
			isDragging = true;
			MDragStart = sf::Vector2f(sf::Mouse::getPosition(Window));
		}

		if (Event.type == sf::Event::MouseMoved && isDragging)
		{
			MDragEnd = sf::Vector2f(sf::Mouse::getPosition(Window));
			calculateParabolicTrajectory(MDragStart, MDragEnd);
		}

		if (Event.type == sf::Event::MouseButtonReleased && Event.mouseButton.button == sf::Mouse::Left)
		{
			isDragging = false;
			launchProjectile(MDragStart, MDragEnd); // Launch projectile
		}
	}
	if (isWin)
	{
		if (Event.type == sf::Event::MouseButtonReleased)
		{
			const sf::Vector2i LMousePos = sf::Mouse::getPosition(Window);
			if (MNextLevelButton.getGlobalBounds().contains(static_cast<float>(LMousePos.x),
			                                                static_cast<float>(LMousePos.y)))
			{
				MSceneManager.setScene(std::make_shared<Level2>(MSceneManager)); // Go to next level (Level2)
			}
			else if (MRestartButton.getGlobalBounds().contains(static_cast<float>(LMousePos.x),
			                                                   static_cast<float>(LMousePos.y)))
			{
				MSceneManager.setScene(std::make_shared<Level1>(MSceneManager)); // Restart level
			}
			else if (MMenuButton.getGlobalBounds().contains(static_cast<float>(LMousePos.x),
			                                                static_cast<float>(LMousePos.y)))
			{
				MSceneManager.setScene(std::make_shared<TitleScreen>(MSceneManager)); // Go back to menu
			}
		}
	}
	if (isLose)
	{
		// Handle mouse interaction for lose screen
		if (Event.type == sf::Event::MouseButtonReleased)
		{
			const sf::Vector2i LMousePos = sf::Mouse::getPosition(Window);
			if (MRestartButton.getGlobalBounds().contains(static_cast<float>(LMousePos.x),
			                                              static_cast<float>(LMousePos.y)))
			{
				MSceneManager.setScene(std::make_shared<Level1>(MSceneManager)); // Restart level
			}
			else if (MMenuButton.getGlobalBounds().contains(static_cast<float>(LMousePos.x),
			                                                static_cast<float>(LMousePos.y)))
			{
				MSceneManager.setScene(std::make_shared<TitleScreen>(MSceneManager)); // Go back to menu
			}
		}
	}
}

void Level1::updateButtonPositions(const sf::Vector2u& WindowSize)
{
	Scene::updateButtonPositions(WindowSize);
}

void Level1::togglePause()
{
	isPaused = !isPaused;
}

void Level1::initSeesaw()
{
	// Load textures for the base and plank
	if (!MSeesawBaseTexture.loadFromFile("resources/kenney physics assets/PNG/Metal elements/elementMetal000.png") ||
		!MSeesawPlankTexture.loadFromFile("resources/kenney physics assets/PNG/Metal elements/elementMetal013.png"))
	{
		// Handle texture loading failure
		return;
	}

	// Set up base sprite
	MSeesawBaseSprite.setTexture(MSeesawBaseTexture);
	MSeesawBaseSprite.setOrigin(MSeesawBaseSprite.getLocalBounds().width / 2,
	                            MSeesawBaseSprite.getLocalBounds().height);
	MSeesawBaseSprite.setPosition(500.0f, 850.0f);

	// Define base body in Box2D
	b2BodyDef LBaseBodyDef;
	LBaseBodyDef.position.Set(500.0f / MPixelsPerMetre, 850.0f / MPixelsPerMetre);
	MSeesawBase = MWorld.CreateBody(&LBaseBodyDef);

	b2PolygonShape LBaseShape;
	b2Vec2 LVertices[3];
	LVertices[0].Set(0, -30.0f / MPixelsPerMetre); // Top of the triangle
	LVertices[1].Set(-30.0f / MPixelsPerMetre, 30.0f / MPixelsPerMetre); // Bottom left
	LVertices[2].Set(30.0f / MPixelsPerMetre, 30.0f / MPixelsPerMetre); // Bottom right
	LBaseShape.Set(LVertices, 3);

	b2FixtureDef LBaseFixtureDef;
	LBaseFixtureDef.shape = &LBaseShape;
	LBaseFixtureDef.density = 1.0f;
	LBaseFixtureDef.friction = 0.5f;
	MSeesawBase->CreateFixture(&LBaseFixtureDef);

	// Set up plank sprites with the same texture
	MSeesawPlankSprite1.setTexture(MSeesawPlankTexture);
	MSeesawPlankSprite1.setOrigin(MSeesawPlankSprite1.getLocalBounds().width / 2,
	                              MSeesawPlankSprite1.getLocalBounds().height / 2);

	MSeesawPlankSprite2.setTexture(MSeesawPlankTexture);
	MSeesawPlankSprite2.setOrigin(MSeesawPlankSprite2.getLocalBounds().width / 2,
	                              MSeesawPlankSprite2.getLocalBounds().height / 2);

	// Define the two plank bodies in Box2D
	b2BodyDef LPlank1BodyDef;
	LPlank1BodyDef.type = b2_dynamicBody;
	LPlank1BodyDef.position.Set((500.0f - 100.0f) / MPixelsPerMetre, 780.0f / MPixelsPerMetre); // Left plank
	MSeesawPlank1 = MWorld.CreateBody(&LPlank1BodyDef);

	b2BodyDef LPlank2BodyDef;
	LPlank2BodyDef.type = b2_dynamicBody;
	LPlank2BodyDef.position.Set((500.0f + 100.0f) / MPixelsPerMetre, 780.0f / MPixelsPerMetre); // Right plank
	MSeesawPlank2 = MWorld.CreateBody(&LPlank2BodyDef);

	// Define plank shapes
	b2PolygonShape LPlankShape;
	LPlankShape.SetAsBox(100.0f / MPixelsPerMetre, 10.0f / MPixelsPerMetre);

	// Create fixtures for each plank
	b2FixtureDef LPlankFixtureDef;
	LPlankFixtureDef.shape = &LPlankShape;
	LPlankFixtureDef.density = 1.0f;
	LPlankFixtureDef.friction = 0.5f;
	LPlankFixtureDef.restitution = 0.3f;
	MSeesawPlank1->CreateFixture(&LPlankFixtureDef);
	MSeesawPlank2->CreateFixture(&LPlankFixtureDef);

	// Weld the two planks together
	b2WeldJointDef LWeldJointDef;
	LWeldJointDef.bodyA = MSeesawPlank1;
	LWeldJointDef.bodyB = MSeesawPlank2;
	LWeldJointDef.localAnchorA.Set(100.0f / MPixelsPerMetre, 0); // Anchor at the end of the left plank
	LWeldJointDef.localAnchorB.Set(-100.0f / MPixelsPerMetre, 0); // Anchor at the end of the right plank
	MPlankJoint = dynamic_cast<b2WeldJoint*>(MWorld.CreateJoint(&LWeldJointDef));

	// Create the joint between base and planks
	b2RevoluteJointDef LJointDef;
	LJointDef.bodyA = MSeesawBase;
	LJointDef.bodyB = MSeesawPlank1;
	LJointDef.localAnchorA.Set(0, -90.0f / MPixelsPerMetre); // Attach at the tip of the triangle
	LJointDef.localAnchorB.Set(100.0f / MPixelsPerMetre, 0); // Center of the combined plank structure
	LJointDef.enableLimit = true;
	LJointDef.lowerAngle = -0.25f * b2_pi; // -45 degrees
	LJointDef.upperAngle = 0.25f * b2_pi; // 45 degrees

	MSeesawJoint = dynamic_cast<b2RevoluteJoint*>(MWorld.CreateJoint(&LJointDef));
}

void Level1::initBlocks(const std::vector<std::tuple<sf::Vector2f, bool, int>>& BlockData)
{
	for (const auto& [pos, destructible, health] : BlockData)
	{
		Block LBlock;

		// Set up the SFML rectangle shape
		LBlock.MShape.setSize(sf::Vector2f(100.0f, 100.0f));
		LBlock.MShape.setOrigin(50.0f, 50.0f);
		LBlock.MShape.setTexture(destructible ? &MDestructibleBlockTexture : &MBlockTexture);
		// Use a different texture for destructible blocks

		// Set up Box2D body
		b2BodyDef LBlockBodyDef;
		LBlockBodyDef.type = b2_dynamicBody;
		LBlockBodyDef.position.Set(pos.x / MPixelsPerMetre, pos.y / MPixelsPerMetre);
		LBlock.MBody = MWorld.CreateBody(&LBlockBodyDef);

		// Set up Box2D shape and fixture
		b2PolygonShape LBlockPhysicsShape;
		LBlockPhysicsShape.SetAsBox(50.0f / MPixelsPerMetre, 50.0f / MPixelsPerMetre);

		b2FixtureDef LBlockFixtureDef;
		LBlockFixtureDef.shape = &LBlockPhysicsShape;
		LBlockFixtureDef.density = 1.0f;
		LBlockFixtureDef.friction = 0.3f;
		LBlock.MBody->CreateFixture(&LBlockFixtureDef);

		// Set destructible properties
		LBlock.isDestructible = destructible;
		LBlock.MHealth = destructible ? health : -1; // Non-destructible blocks have no health

		MBlocks.push_back(LBlock);
	}
}

void Level1::initEnemies(const std::vector<sf::Vector2f>& Positions)
{
	// Load enemy texture once for all enemies
	if (!MEnemyTexture.loadFromFile("resources/kenney physics assets/PNG/Aliens/alienGreen_round.png"))
	{
		std::cerr << "Failed to load enemy texture!" << '\n';
		return;
	}

	// Loop through provided positions to create enemies
	for (const auto& Pos : Positions)
	{
		Enemy LEnemy;

		// Set texture and scaling for enemy sprite
		LEnemy.MEnemySprite.setTexture(MEnemyTexture);
		LEnemy.MEnemySprite.setScale(1.0f, 1.0f);
		LEnemy.MEnemySprite.setOrigin(LEnemy.MEnemySprite.getGlobalBounds().width / 2.0f,
		                              LEnemy.MEnemySprite.getGlobalBounds().height / 2.0f);

		// Set initial position for each enemy
		const float LxPosition = Pos.x;
		const float LyPosition = Pos.y;
		LEnemy.MEnemySprite.setPosition(LxPosition, LyPosition);

		// Define enemy body in Box2D
		b2BodyDef LEnemyBodyDef;
		LEnemyBodyDef.type = b2_dynamicBody;
		LEnemyBodyDef.position.Set(LxPosition / MPixelsPerMetre, LyPosition / MPixelsPerMetre);
		LEnemy.MEnemyBody = MWorld.CreateBody(&LEnemyBodyDef);

		// Define shape and properties
		b2PolygonShape LEnemyShape;
		LEnemyShape.SetAsBox((LEnemy.MEnemySprite.getGlobalBounds().width / 2.0f) / MPixelsPerMetre,
		                     (LEnemy.MEnemySprite.getGlobalBounds().height / 2.0f) / MPixelsPerMetre);

		b2FixtureDef LEnemyFixtureDef;
		LEnemyFixtureDef.shape = &LEnemyShape;
		LEnemyFixtureDef.density = 1.0f;
		LEnemyFixtureDef.friction = 0.5f;
		LEnemy.MEnemyBody->CreateFixture(&LEnemyFixtureDef);

		// Set user data for collision detection
		LEnemy.MEnemyBody->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);

		// Mark enemy as alive initially
		LEnemy.isAlive = true;

		// Add enemy to list of enemies
		MEnemies.push_back(LEnemy);
	}
}

void Level1::spawnProjectile()
{
	if (MRemainingProjectiles > 0)
	{
		// Create a new projectile
		b2BodyDef LProjectileBodyDef;
		LProjectileBodyDef.type = b2_dynamicBody;
		LProjectileBodyDef.position.Set(100.0f / MPixelsPerMetre, 700.0f / MPixelsPerMetre); // Spawn at 700 units high
		LProjectileBodyDef.bullet = true;
		MProjectileBody = MWorld.CreateBody(&LProjectileBodyDef);

		MProjectileBody->SetLinearDamping(0.5f);

		b2CircleShape LProjectileShapeDef;
		LProjectileShapeDef.m_radius = 25.0f / MPixelsPerMetre;

		b2FixtureDef LProjectileFixtureDef;
		LProjectileFixtureDef.shape = &LProjectileShapeDef;
		LProjectileFixtureDef.density = 1.0f;
		LProjectileFixtureDef.friction = 0.8f;
		LProjectileFixtureDef.restitution = 0.8f; // Bounce
		MProjectileBody->CreateFixture(&LProjectileFixtureDef);

		// Disable gravity initially
		MProjectileBody->SetGravityScale(0.0f);

		// Reset projectile flags and decrease projectile count
		isProjectileLaunched = false;
		isProjectileStopped = false;
		MStationaryTime = 0.0f;
		MRemainingProjectiles--;
	}
	else
	{
		// If no projectiles remain, trigger lose condition
		isLose = true;
	}
}

void Level1::launchProjectile(const sf::Vector2f& Start, const sf::Vector2f& End)
{
	if (isPaused || isWin || isLose || isProjectileLaunched)
	{
		return; // Don't launch projectiles if game is paused, already won/lost, or projectile is already launched
	}

	// Calculate direction and magnitude of launch force
	const sf::Vector2f LForce = Start - End;

	// Apply force to projectile body
	const b2Vec2 LLaunchForce((LForce.x * 0.75f) / MPixelsPerMetre, (LForce.y * 0.75f) / MPixelsPerMetre);
	MProjectileBody->ApplyLinearImpulseToCenter(LLaunchForce, true);

	// Enable gravity for the projectile after launch
	MProjectileBody->SetGravityScale(1.0f);

	// Mark projectile as launched
	isProjectileLaunched = true;
	isDragging = false; // Disable dragging after launch

	// Clear trajectory points after launching
	MTrajectoryPoints.clear();
}


void Level1::calculateParabolicTrajectory(const sf::Vector2f& Start, const sf::Vector2f& End)
{
	// Clear previous trajectory points
	MTrajectoryPoints.clear();

	// Start from projectile's current position
	const sf::Vector2f LProjectilePosition = MProjectileShape.getPosition();

	// Calculate direction and velocity
	const sf::Vector2f LDirection = Start - End;
	const float LVelocityX = (LDirection.x * 2.0f) / MPixelsPerMetre;
	const float LVelocityY = (LDirection.y * 3.0f) / MPixelsPerMetre;

	const float LGravity = MWorld.GetGravity().y;

	constexpr int LNumPoints = 30;
	constexpr float LTimeStep = 0.1f;

	for (int I = 0; I < LNumPoints; ++I)
	{
		const float T = static_cast<float>(I) * LTimeStep;

		// Calculate projectile position at time T
		const float X = LVelocityX * T;
		const float Y = LVelocityY * T + 0.5f * LGravity * T * T;

		sf::CircleShape LPoint(5.0f);
		LPoint.setFillColor(sf::Color::Red);
		LPoint.setPosition(
			(LProjectilePosition.x / MPixelsPerMetre + X) * MPixelsPerMetre,
			(LProjectilePosition.y / MPixelsPerMetre + Y) * MPixelsPerMetre
		);

		MTrajectoryPoints.push_back(LPoint);
	}
}

void Level1::handleCollisions()
{
	for (b2Contact* LContact = MWorld.GetContactList(); LContact; LContact = LContact->GetNext())
	{
		if (LContact->IsTouching())
		{
			b2Body* LBodyA = LContact->GetFixtureA()->GetBody();
			b2Body* LBodyB = LContact->GetFixtureB()->GetBody();

			// Iterate through all enemies
			for (auto& Enemy : MEnemies)
			{
				if ((LBodyA == Enemy.MEnemyBody || LBodyB == Enemy.MEnemyBody) && Enemy.isAlive)
				{
					// Get velocity and mass of other body
					const b2Body* LOtherBody = (LBodyA == Enemy.MEnemyBody) ? LBodyB : LBodyA;
					const float LMass = LOtherBody->GetMass();
					b2Vec2 LVelocity = LOtherBody->GetLinearVelocity();
					const float LSpeed = LVelocity.Length();

					// If speed and mass exceed threshold, kill enemy
					constexpr float LSpeedThreshold = 0.2f;
					constexpr float LMassThreshold = 1.0f;

					if (LSpeed > LSpeedThreshold || LMass > LMassThreshold)
					{
						Enemy.isAlive = false;
						MWorld.DestroyBody(Enemy.MEnemyBody); // Destroy enemy body
						std::cout << "Enemy killed by collision!" << '\n';
					}
				}
			}
		}
	}
}

void Level1::checkEnemiesAlive()
{
	bool LAllDead = true;

	for (const auto& Enemy : MEnemies)
	{
		if (Enemy.isAlive)
		{
			LAllDead = false;
			break; // Exit early if we find an enemy that is still alive
		}
	}

	if (LAllDead)
	{
		isWin = true; // Trigger win condition
	}
}
