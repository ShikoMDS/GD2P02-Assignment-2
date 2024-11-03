#include "Level1.h"

Level1::Level1(SceneManager& Manager)
	: MWorld(b2Vec2(0.0f, 9.8f)), // Set gravity for Box2D world
	  MPixelsPerMetre(100.0f),
	  MSceneManager(Manager), MGroundBody(nullptr), MProjectileBody(nullptr),
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
	std::vector<std::tuple<sf::Vector2f, bool, int>> blockData = {
		{ {800.0f, 800.0f}, true, 1 },   // Destructible block with health of 1
		{ {900.0f, 800.0f}, true, 1 },     // Destructible block with health of 1
		{ {1000.0f, 400.0f}, false, -1 }     // Regular block
	};
	initBlocks(blockData);

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
	for (const auto& block : MBlocks) {
		Window.draw(block.shape);
	}

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

void Level1::update(const float DeltaTime) {
	if (!isPaused && !isWin && !isLose) {
		// Increase Box2D timestep parameters to handle high-speed collisions more accurately
		MWorld.Step(DeltaTime, 8, 8); // Balanced iterations

		// Block and projectile collision update logic
		for (auto it = MBlocks.begin(); it != MBlocks.end();) {
			Block& block = *it;
			if (block.isDestructible) {
				b2Vec2 velocity = block.body->GetLinearVelocity();
				float impactForce = velocity.Length();

				// Adjust the force threshold as needed
				if (impactForce > 2.0f) {
					block.health--;
					if (block.health <= 0) {
						MWorld.DestroyBody(block.body);
						it = MBlocks.erase(it);
						continue;
					}
				}
			}
			b2Vec2 position = block.body->GetPosition();
			float angle = block.body->GetAngle();
			block.shape.setPosition(position.x * MPixelsPerMetre, position.y * MPixelsPerMetre);
			block.shape.setRotation(angle * 180.0f / b2_pi);

			++it;
		}

		// Remaining projectile update and collision handling
		if (isProjectileLaunched && !isProjectileStopped) {
			b2Vec2 velocity = MProjectileBody->GetLinearVelocity();
			if (velocity.Length() < 0.1f) {
				MStationaryTime += DeltaTime;
			}
			else {
				MStationaryTime = 0.0f;
			}
			if (MStationaryTime > 3.0f) {
				isProjectileStopped = true;
				MWorld.DestroyBody(MProjectileBody);
				MProjectileShape.setPosition(-100, -100); // Off-screen
				spawnProjectile();
			}
			b2Vec2 projectilePosition = MProjectileBody->GetPosition();
			if (projectilePosition.x * MPixelsPerMetre < MScreenLeftBound || projectilePosition.x * MPixelsPerMetre > MScreenRightBound) {
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
		if (!isProjectileStopped) {
			b2Vec2 projectilePosition = MProjectileBody->GetPosition();
			float projectileAngle = MProjectileBody->GetAngle();
			MProjectileShape.setPosition(projectilePosition.x * MPixelsPerMetre, projectilePosition.y * MPixelsPerMetre);
			MProjectileShape.setRotation(projectileAngle * 180.0f / b2_pi);
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

void Level1::initBlocks(const std::vector<std::tuple<sf::Vector2f, bool, int>>& blockData) {
	for (const auto& [pos, destructible, health] : blockData) {
		Block block;

		// Set up the SFML rectangle shape
		block.shape.setSize(sf::Vector2f(100.0f, 100.0f));
		block.shape.setOrigin(50.0f, 50.0f);
		block.shape.setTexture(destructible ? &MDestructibleBlockTexture : &MBlockTexture); // Use a different texture for destructible blocks

		// Set up Box2D body
		b2BodyDef blockBodyDef;
		blockBodyDef.type = b2_dynamicBody;
		blockBodyDef.position.Set(pos.x / MPixelsPerMetre, pos.y / MPixelsPerMetre);
		block.body = MWorld.CreateBody(&blockBodyDef);

		// Set up Box2D shape and fixture
		b2PolygonShape blockPhysicsShape;
		blockPhysicsShape.SetAsBox(50.0f / MPixelsPerMetre, 50.0f / MPixelsPerMetre);

		b2FixtureDef blockFixtureDef;
		blockFixtureDef.shape = &blockPhysicsShape;
		blockFixtureDef.density = 1.0f;
		blockFixtureDef.friction = 0.3f;
		block.body->CreateFixture(&blockFixtureDef);

		// Set destructible properties
		block.isDestructible = destructible;
		block.health = destructible ? health : -1;  // Non-destructible blocks have no health

		// Add to vector
		MBlocks.push_back(block);
	}
}

void Level1::initEnemies(const std::vector<sf::Vector2f>& Positions)
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

void Level1::spawnProjectile()
{
	if (MRemainingProjectiles > 0)
	{
		// Create a new projectile
		b2BodyDef LProjectileBodyDef;
		LProjectileBodyDef.type = b2_dynamicBody;
		LProjectileBodyDef.position.Set(100.0f / MPixelsPerMetre, 700.0f / MPixelsPerMetre);  // Spawn at 700 units high
		LProjectileBodyDef.bullet = true;
		MProjectileBody = MWorld.CreateBody(&LProjectileBodyDef);

		// Add linear damping to slow down projectile over time (resistance)
		MProjectileBody->SetLinearDamping(0.5f);

		b2CircleShape LProjectileShapeDef;
		LProjectileShapeDef.m_radius = 25.0f / MPixelsPerMetre; // Set radius of projectile

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


void Level1::calculateParabolicTrajectory(const sf::Vector2f& start, const sf::Vector2f& end)
{
    // Clear previous trajectory points
    MTrajectoryPoints.clear();

    // Start from projectile's current position
    const sf::Vector2f LProjectilePosition = MProjectileShape.getPosition();

    // Calculate direction and velocity
    const sf::Vector2f LDirection = start - end;
    const float LVelocityX = (LDirection.x * 2.0f) / MPixelsPerMetre;  // Adjusted horizontal scaling factor
    const float LVelocityY = (LDirection.y * 3.0f) / MPixelsPerMetre;  // Kept vertical scaling as before

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

        //// Stop at the peak to avoid overshooting (optional)
        //if (I > 0 && MTrajectoryPoints[I].getPosition().y > MTrajectoryPoints[I - 1].getPosition().y) {
        //    break;
        //}
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
						std::cout << "Enemy killed by collision!" << std::endl;
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
