#include "Level2.h"

Level2::Level2(SceneManager& Manager)
	: MWorld(b2Vec2(0.0f, 9.8f)), // Set gravity for Box2D world
	MPixelsPerMetre(100.0f),
	MSceneManager(Manager), MBlockBody(nullptr), MGroundBody(nullptr), MProjectileBody(nullptr),
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

void Level2::init()
{
	// Define ground body for Box2D
	b2BodyDef LGroundBodyDef;
	LGroundBodyDef.position.Set(800.0f / MPixelsPerMetre, 850.0f / MPixelsPerMetre); // Box2D works in metres
	MGroundBody = MWorld.CreateBody(&LGroundBodyDef);

	b2PolygonShape LGroundBox;
	LGroundBox.SetAsBox(800.0f / MPixelsPerMetre, 10.0f / MPixelsPerMetre); // Half-width, half-height in metres
	MGroundBody->CreateFixture(&LGroundBox, 0.0f); // Static ground with zero density

	// Define block body
	b2BodyDef LBlockBodyDef;
	LBlockBodyDef.type = b2_dynamicBody; // Dynamic body for falling block
	LBlockBodyDef.position.Set(800.0f / MPixelsPerMetre, 800.0f / MPixelsPerMetre); // Centered at top
	MBlockBody = MWorld.CreateBody(&LBlockBodyDef);

	b2PolygonShape LBlockPhysicsShape;
	LBlockPhysicsShape.SetAsBox(50.0f / MPixelsPerMetre, 50.0f / MPixelsPerMetre); // 50x50 pixel block
	b2FixtureDef LBlockFixtureDef;
	LBlockFixtureDef.shape = &LBlockPhysicsShape;
	LBlockFixtureDef.density = 1.0f;
	LBlockFixtureDef.friction = 0.3f;
	MBlockBody->CreateFixture(&LBlockFixtureDef);

	// Set up visual representation for block
	MBlockShape.setSize(sf::Vector2f(100.0f, 100.0f)); // 100x100 pixels
	MBlockShape.setOrigin(50.0f, 50.0f); // Set origin to center for proper rotation
	MBlockShape.setTexture(&MBlockTexture); // Set texture for block

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
		sf::Vector2f(400.0f, 800.0f), // First enemy position
		sf::Vector2f(500.0f, 800.0f), // Second enemy position
		sf::Vector2f(1300.0f, 800.0f), // Third enemy position
		sf::Vector2f(1500.0f, 800.0f) // Fourth enemy position
	};

	initEnemies(LEnemyPositions);

	MRemainingProjectiles = 3; // Set number of projectiles

	spawnProjectile(); // Spawn first projectile
}

void Level2::draw(sf::RenderWindow& Window)
{
	// Draw background
	Window.draw(MBackgroundSprite);

	// Draw ground tiles
	for (const auto& Tile : MGrassTiles)
	{
		Window.draw(Tile);
	}

	// Draw block
	Window.draw(MBlockShape);

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

		// Remove the "Next Level" button code to avoid displaying it

		// Draw "Restart" button
		MRestartButton.setCharacterSize(static_cast<unsigned int>(50 * LScaleY));
		sf::FloatRect LRestartBounds = MRestartButton.getLocalBounds();
		MRestartButton.setOrigin(LRestartBounds.width / 2.0f, LRestartBounds.height / 2.0f);
		MRestartButton.setPosition(static_cast<float>(Window.getSize().x) / 2.0f,
			static_cast<float>(Window.getSize().y) / 2.0f + 100 * LScaleY); // Centered and spaced
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

void Level2::update(const float DeltaTime)
{
	if (!isPaused && !isWin && !isLose)
	{
		MWorld.Step(DeltaTime, 8, 3); // Update Box2D world

		// Update enemy positions
		for (auto& Enemy : MEnemies)
		{
			if (Enemy.isAlive)
			{
				const b2Vec2 LEnemyPos = Enemy.MEnemyBody->GetPosition();
				Enemy.MEnemySprite.setPosition(LEnemyPos.x * MPixelsPerMetre, LEnemyPos.y * MPixelsPerMetre);
				Enemy.MEnemySprite.setRotation(Enemy.MEnemyBody->GetAngle() * 180.0f / b2_pi);

				// Check if enemy is off screen (left and right)
				if (LEnemyPos.x * MPixelsPerMetre < MScreenLeftBound || LEnemyPos.x * MPixelsPerMetre >
					MScreenRightBound)
				{
					MWorld.DestroyBody(Enemy.MEnemyBody); // Remove from world
					Enemy.isAlive = false; // Mark enemy as dead
				}
			}
		}

		// Update projectile positions
		if (isProjectileLaunched && !isProjectileStopped)
		{
			const b2Vec2 LVelocity = MProjectileBody->GetLinearVelocity();

			// Check if projectile velocity is below a threshold
			if (LVelocity.Length() < 0.1f)
			{
				MStationaryTime += DeltaTime;
			}
			else
			{
				MStationaryTime = 0.0f; // Reset timer if projectile moves again
			}

			// If projectile has been stationary for more than 3 seconds, remove it
			if (MStationaryTime > 3.0f)
			{
				isProjectileStopped = true;
				MWorld.DestroyBody(MProjectileBody);
				MProjectileShape.setPosition(-100, -100); // Move projectile shape off-screen
				spawnProjectile(); // Spawn next projectile
			}

			// Check if projectile is off screen (only left and right)
			const b2Vec2 LProjectilePos = MProjectileBody->GetPosition();
			if (LProjectilePos.x * MPixelsPerMetre < MScreenLeftBound || LProjectilePos.x * MPixelsPerMetre >
				MScreenRightBound)
			{
				isProjectileStopped = true;
				MWorld.DestroyBody(MProjectileBody); // Remove projectile from Box2D world
				MProjectileShape.setPosition(-100, -100); // Move projectile shape off-screen
				spawnProjectile(); // Spawn next projectile
			}
		}

		// Handle collisions
		handleCollisions();

		// Check if all enemies are dead
		checkEnemiesAlive();

		// Update block and projectile positions and rotations
		const b2Vec2 LPosition = MBlockBody->GetPosition();
		const float LAngle = MBlockBody->GetAngle();
		MBlockShape.setPosition(LPosition.x * MPixelsPerMetre, LPosition.y * MPixelsPerMetre);
		MBlockShape.setRotation(LAngle * 180.0f / 3.14159f);

		// Only update projectile position if it hasn't been removed
		if (!isProjectileStopped)
		{
			const b2Vec2 LProjectilePosition = MProjectileBody->GetPosition();
			const float LProjectileAngle = MProjectileBody->GetAngle();
			MProjectileShape.setPosition(LProjectilePosition.x * MPixelsPerMetre,
				LProjectilePosition.y * MPixelsPerMetre);
			MProjectileShape.setRotation(LProjectileAngle * 180.0f / 3.14159f);
		}
	}
}

void Level2::handleInput(sf::RenderWindow& Window, sf::Event& Event)
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
		MSceneManager.setScene(std::make_shared<Level2>(MSceneManager)); // Restart level
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
				MSceneManager.setScene(std::make_shared<Level2>(MSceneManager)); // Restart level
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
			if (MRestartButton.getGlobalBounds().contains(static_cast<float>(LMousePos.x),
				static_cast<float>(LMousePos.y)))
			{
				MSceneManager.setScene(std::make_shared<Level2>(MSceneManager)); // Restart level
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
				MSceneManager.setScene(std::make_shared<Level2>(MSceneManager)); // Restart level
			}
			else if (MMenuButton.getGlobalBounds().contains(static_cast<float>(LMousePos.x),
				static_cast<float>(LMousePos.y)))
			{
				MSceneManager.setScene(std::make_shared<TitleScreen>(MSceneManager)); // Go back to menu
			}
		}
	}
}

void Level2::updateButtonPositions(const sf::Vector2u& WindowSize)
{
	Scene::updateButtonPositions(WindowSize);
}

void Level2::togglePause()
{
	isPaused = !isPaused;
}

void Level2::initEnemies(const std::vector<sf::Vector2f>& Positions)
{
	// Load enemy texture once for all enemies
	if (!MEnemyTexture.loadFromFile("resources/kenney physics assets/PNG/Aliens/alienGreen_round.png"))
	{
		std::cerr << "Failed to load enemy texture!" << std::endl;
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

void Level2::spawnProjectile()
{
	if (MRemainingProjectiles > 0)
	{
		// Create a new projectile
		b2BodyDef LProjectileBodyDef;
		LProjectileBodyDef.type = b2_dynamicBody;
		LProjectileBodyDef.position.Set(100.0f / MPixelsPerMetre, 800.0f / MPixelsPerMetre);
		// Starting position for each projectile
		MProjectileBody = MWorld.CreateBody(&LProjectileBodyDef);

		// Add linear damping to slow down projectile over time (resistance)
		MProjectileBody->SetLinearDamping(0.8f);

		b2CircleShape LProjectileShapeDef;
		LProjectileShapeDef.m_radius = 25.0f / MPixelsPerMetre; // Set radius of projectile

		b2FixtureDef LProjectileFixtureDef;
		LProjectileFixtureDef.shape = &LProjectileShapeDef;
		LProjectileFixtureDef.density = 1.0f;
		LProjectileFixtureDef.friction = 0.8f;
		//LProjectileFixtureDef.restitution = 0.2f; // Bounce
		MProjectileBody->CreateFixture(&LProjectileFixtureDef);

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

void Level2::launchProjectile(const sf::Vector2f& Start, const sf::Vector2f& End)
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

	// Mark projectile as launched
	isProjectileLaunched = true;
	isDragging = false; // Disable dragging after launch

	// Clear trajectory points after launching
	MTrajectoryPoints.clear();
}

void Level2::calculateParabolicTrajectory(const sf::Vector2f& Start, const sf::Vector2f& End)
{
	// Clear previous trajectory points
	MTrajectoryPoints.clear();

	// Start from projectile's current position
	const sf::Vector2f LProjectilePosition = MProjectileShape.getPosition();

	// Calculate direction and velocity, reversed to get proper trajectory
	const sf::Vector2f LDirection = Start - End; // Reverse direction so it's from mouse to projectile
	const float LVelocityX = (LDirection.x * 3.0f) / MPixelsPerMetre; // Increased by a factor of 3
	const float LVelocityY = (LDirection.y * 3.0f) / MPixelsPerMetre; // Y-axis is no longer inverted

	const float LGravity = MWorld.GetGravity().y; // Get gravity from Box2D world

	constexpr int LNumPoints = 30; // Number of points to simulate for trajectory
	constexpr float LTimeStep = 0.1f; // Time step for each point in simulation

	bool LApexReached = false; // Flag to detect apex

	for (int I = 0; I < LNumPoints; ++I)
	{
		const float T = static_cast<float>(I) * LTimeStep;

		// Calculate vertical velocity at time t
		const float LCurrentVelocityY = LVelocityY + LGravity * T;

		// Stop drawing points if apex is reached (when vertical velocity is near zero)
		if (LCurrentVelocityY > 0 && !LApexReached)
		{
			LApexReached = true; // Mark apex as reached and stop drawing further points
		}

		// Stop drawing trajectory after apex
		if (LApexReached)
		{
			break;
		}

		// Calculate position of projectile at time t
		const float X = LVelocityX * T;
		const float Y = LVelocityY * T + 0.5f * LGravity * T * T;

		sf::CircleShape LPoint(5.0f); // Increased size of point for more visibility
		LPoint.setFillColor(sf::Color::Red);
		LPoint.setPosition((LProjectilePosition.x / MPixelsPerMetre + X) * MPixelsPerMetre,
			(LProjectilePosition.y / MPixelsPerMetre + Y) * MPixelsPerMetre);

		MTrajectoryPoints.push_back(LPoint);
	}
}

void Level2::handleCollisions()
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
						std::cout << "Enemy killed by collision!" << std::endl;
					}
				}
			}
		}
	}
}

void Level2::checkEnemiesAlive()
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
