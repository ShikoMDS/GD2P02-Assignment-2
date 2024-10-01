#include "Level1.h"

Level1::Level1(SceneManager& Manager)
    : MWorld(b2Vec2(0.0f, 9.8f)),  // Set gravity for the Box2D world
    MSceneManager(Manager),
    MPixelsPerMeter(100.0f)  // 100 pixels per meter for scaling
{
    // Load the background texture
    if (!MBackgroundTexture.loadFromFile("resources/kenney physics assets/PNG/Backgrounds/colored_desert.png")) {
        // Handle loading error
    }
    MBackgroundSprite.setTexture(MBackgroundTexture);

    // Set background to stretch across the window size
    sf::Vector2u windowSize(1600, 900); // Assuming the window size is locked
    sf::Vector2u bgTextureSize = MBackgroundTexture.getSize();
    MBackgroundSprite.setScale(
        static_cast<float>(windowSize.x) / bgTextureSize.x,
        static_cast<float>(windowSize.y) / bgTextureSize.y
    );

    // Load the grass texture and create the tiles
    if (!MGrassTexture.loadFromFile("resources/kenney physics assets/PNG/Other/grass.png")) {
        // Handle loading error
    }

    // Create ground tiles by tiling the grass texture across the screen width
    for (int i = 0; i < 1600 / 70 + 1; ++i) {  // Assuming each grass tile is 70x70
        sf::Sprite grassTile;
        grassTile.setTexture(MGrassTexture);
        grassTile.setPosition(i * 70.0f, 830.0f);  // Position at the bottom of the window
        MGrassTiles.push_back(grassTile);
    }

    // Load block texture
    if (!MBlockTexture.loadFromFile("resources/kenney physics assets/PNG/Wood elements/elementWood010.png")) {
        // Handle error
    }

    // Initialize the dark overlay for pause state
    MDarkOverlay.setFillColor(sf::Color(0, 0, 0, 150));  // Black overlay with transparency

    // Set up buttons and font for the pause menu
    if (!MFont.loadFromFile("resources/sugar bread/Sugar Bread.otf")) {
        // Handle error
    }
    MResumeButton.setFont(MFont);
    MResumeButton.setString("Resume");
    MRestartButton.setFont(MFont);
    MRestartButton.setString("Restart");
    MMenuButton.setFont(MFont);
    MMenuButton.setString("Menu");
    MNextLevelButton.setFont(MFont);  // Use the preloaded font
    MNextLevelButton.setString("Next Level");
    MNextLevelButton.setCharacterSize(50);  // Initial text size, will be scaled later

    // Load projectile texture
    if (!MProjectileTexture.loadFromFile("resources/kenney animal pack redux/PNG/Round (outline)/frog.png")) {
        // Handle loading error
    }

    // Initialize the projectile shape
    MProjectileShape.setRadius(25.0f);
    MProjectileShape.setOrigin(25.0f, 25.0f);
    MProjectileShape.setTexture(&MProjectileTexture);

    // Drag line setup
    MDragLine = sf::VertexArray(sf::Lines, 2);

    isWin = false;
    isLose = false;
}

void Level1::init() {
    // Define the ground body for Box2D
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(800.0f / MPixelsPerMeter, 850.0f / MPixelsPerMeter);  // Box2D works in meters
    MGroundBody = MWorld.CreateBody(&groundBodyDef);

    b2PolygonShape groundBox;
    groundBox.SetAsBox(800.0f / MPixelsPerMeter, 10.0f / MPixelsPerMeter);  // Half-width, half-height in meters
    MGroundBody->CreateFixture(&groundBox, 0.0f);  // Static ground with zero density

    // Define the block body
    b2BodyDef blockBodyDef;
    blockBodyDef.type = b2_dynamicBody;  // Dynamic body for the falling block
    blockBodyDef.position.Set(800.0f / MPixelsPerMeter, 200.0f / MPixelsPerMeter);  // Centered at the top
    MBlockBody = MWorld.CreateBody(&blockBodyDef);

    b2PolygonShape blockPhysicsShape;  // Renamed to avoid conflict
    blockPhysicsShape.SetAsBox(50.0f / MPixelsPerMeter, 50.0f / MPixelsPerMeter);  // 50x50 pixel block
    b2FixtureDef blockFixtureDef;
    blockFixtureDef.shape = &blockPhysicsShape;
    blockFixtureDef.density = 1.0f;
    blockFixtureDef.friction = 0.3f;
    MBlockBody->CreateFixture(&blockFixtureDef);

    // Set up the visual representation for the block
    MBlockShape.setSize(sf::Vector2f(100.0f, 100.0f));  // 100x100 pixels
    MBlockShape.setOrigin(50.0f, 50.0f);  // Set origin to center for proper rotation
    MBlockShape.setTexture(&MBlockTexture);  // Set the texture for the block

    // Define the projectile body for Box2D
    b2BodyDef projectileBodyDef;
    projectileBodyDef.type = b2_dynamicBody;
    projectileBodyDef.position.Set(200.0f / MPixelsPerMeter, 800.0f / MPixelsPerMeter);  // Set initial position in meters
    MProjectileBody = MWorld.CreateBody(&projectileBodyDef);

    b2CircleShape projectileShapeDef;
    projectileShapeDef.m_radius = 25.0f / MPixelsPerMeter;  // Set radius (in meters)

    b2FixtureDef projectileFixtureDef;
    projectileFixtureDef.shape = &projectileShapeDef;
    projectileFixtureDef.density = 1.0f;
    projectileFixtureDef.friction = 0.5f;  // Set appropriate friction
    projectileFixtureDef.restitution = 0.5f;  // Set restitution (bounciness)
    MProjectileBody->CreateFixture(&projectileFixtureDef);


    std::vector enemyPositions = {
    sf::Vector2f(1100.0f, 800.0f),  // First enemy position
    sf::Vector2f(1200.0f, 800.0f),  // Second enemy position
    sf::Vector2f(1300.0f, 800.0f),   // Third enemy position
    sf::Vector2f(1500.0f, 800.0f)   // Fourth enemy position
    };

    initEnemies(enemyPositions);

    MRemainingProjectiles = 3;  // Set the total number of projectiles to 3

    spawnProjectile();  // Spawn the first projectile
}

void Level1::update(float DeltaTime) {
    if (!isPaused && !isWin && !isLose) {
        MWorld.Step(DeltaTime, 8, 3);  // Update the Box2D world

        // Update enemy positions
        for (auto& enemy : MEnemies) {
            if (enemy.isAlive) {
                b2Vec2 enemyPos = enemy.MEnemyBody->GetPosition();
                enemy.MEnemySprite.setPosition(enemyPos.x * MPixelsPerMeter, enemyPos.y * MPixelsPerMeter);
                enemy.MEnemySprite.setRotation(enemy.MEnemyBody->GetAngle() * 180.0f / b2_pi);

                // Check if the enemy is off the screen (only left and right)
                if (enemyPos.x * MPixelsPerMeter < MScreenLeftBound || enemyPos.x * MPixelsPerMeter > MScreenRightBound) {
                    MWorld.DestroyBody(enemy.MEnemyBody);  // Remove from the world
                    enemy.isAlive = false;  // Mark the enemy as dead
                }
            }
        }

        // Update projectile positions
        if (isProjectileLaunched && !isProjectileStopped) {
            b2Vec2 velocity = MProjectileBody->GetLinearVelocity();

            // Check if the projectile's velocity is below a threshold
            if (velocity.Length() < 0.1f) {
                MStationaryTime += DeltaTime;
            }
            else {
                MStationaryTime = 0.0f;  // Reset the timer if the projectile moves again
            }

            // If the projectile has been stationary for more than 3 seconds, remove it
            if (MStationaryTime > 3.0f) {
                isProjectileStopped = true;
                MWorld.DestroyBody(MProjectileBody);
                MProjectileShape.setPosition(-100, -100);  // Move the projectile shape off-screen
                spawnProjectile();  // Spawn the next projectile
            }

            // Check if the projectile is off the screen (only left and right)
            b2Vec2 projectilePos = MProjectileBody->GetPosition();
            if (projectilePos.x * MPixelsPerMeter < MScreenLeftBound || projectilePos.x * MPixelsPerMeter > MScreenRightBound) {
                isProjectileStopped = true;
                MWorld.DestroyBody(MProjectileBody);  // Remove the projectile from the Box2D world
                MProjectileShape.setPosition(-100, -100);  // Move the projectile shape off-screen
                spawnProjectile();  // Spawn the next projectile
            }
        }

        // Handle collisions
        handleCollisions();

        // Check if all enemies are dead
        checkEnemiesAlive();

        // Update block and projectile positions and rotations
        b2Vec2 position = MBlockBody->GetPosition();
        float angle = MBlockBody->GetAngle();
        MBlockShape.setPosition(position.x * MPixelsPerMeter, position.y * MPixelsPerMeter);
        MBlockShape.setRotation(angle * 180.0f / 3.14159f);

        // Only update the projectile position if it hasn't been removed
        if (!isProjectileStopped) {
            b2Vec2 projectilePosition = MProjectileBody->GetPosition();
            float projectileAngle = MProjectileBody->GetAngle();
            MProjectileShape.setPosition(projectilePosition.x * MPixelsPerMeter, projectilePosition.y * MPixelsPerMeter);
            MProjectileShape.setRotation(projectileAngle * 180.0f / 3.14159f);
        }
    }
}

void Level1::draw(sf::RenderWindow& Window) {
    // Draw background
    Window.draw(MBackgroundSprite);

    // Draw the ground tiles
    for (const auto& tile : MGrassTiles) {
        Window.draw(tile);
    }

    // Draw the block
    Window.draw(MBlockShape);

    // Draw the projectile if it's not removed
    if (!isProjectileStopped) {
        Window.draw(MProjectileShape);
    }

    // Draw the drag line only if the projectile has not been launched and dragging is happening
    if (isDragging && !isProjectileLaunched) {
        for (const auto& point : MTrajectoryPoints) {
            Window.draw(point);
        }
    }

    // Draw the enemy if alive
    for (auto& enemy : MEnemies) {
        if (enemy.isAlive) {
            Window.draw(enemy.MEnemySprite);
        }
    }

    // Calculate scaling factors based on window size
    float scaleX = static_cast<float>(Window.getSize().x) / 1600.0f;  // 1600 is the reference width
    float scaleY = static_cast<float>(Window.getSize().y) / 900.0f;   // 900 is the reference height

    // Handle win screen
    if (isWin) {
        // Draw dark overlay
        MDarkOverlay.setSize(sf::Vector2f(static_cast<float>(Window.getSize().x), static_cast<float>(Window.getSize().y)));
        Window.draw(MDarkOverlay);

        // "You Win" Title
        sf::Text winText;
        winText.setFont(MFont);
        winText.setString("You Win!");
        winText.setCharacterSize(static_cast<unsigned int>(100 * scaleY));
        sf::FloatRect winBounds = winText.getLocalBounds();
        winText.setOrigin(winBounds.width / 2.0f, winBounds.height / 2.0f);
        winText.setPosition(Window.getSize().x / 2.0f, Window.getSize().y / 4.0f);  // Centered title
        Window.draw(winText);

        // Draw the "Next Level" button
        MNextLevelButton.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
        sf::FloatRect nextBounds = MNextLevelButton.getLocalBounds();
        MNextLevelButton.setOrigin(nextBounds.width / 2.0f, nextBounds.height / 2.0f);
        MNextLevelButton.setPosition(Window.getSize().x / 2.0f, Window.getSize().y / 2.0f);  // Centered under the title
        Window.draw(MNextLevelButton);

        // Draw the "Restart" button
        MRestartButton.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
        sf::FloatRect restartBounds = MRestartButton.getLocalBounds();
        MRestartButton.setOrigin(restartBounds.width / 2.0f, restartBounds.height / 2.0f);
        MRestartButton.setPosition(Window.getSize().x / 2.0f, Window.getSize().y / 2.0f + 100 * scaleY);  // Centered and spaced
        Window.draw(MRestartButton);

        // Draw the "Menu" button
        MMenuButton.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
        sf::FloatRect menuBounds = MMenuButton.getLocalBounds();
        MMenuButton.setOrigin(menuBounds.width / 2.0f, menuBounds.height / 2.0f);
        MMenuButton.setPosition(Window.getSize().x / 2.0f, Window.getSize().y / 2.0f + 200 * scaleY);  // Centered and spaced
        Window.draw(MMenuButton);
    }

    if (isLose) {
        // Draw dark overlay
        MDarkOverlay.setSize(sf::Vector2f(static_cast<float>(Window.getSize().x), static_cast<float>(Window.getSize().y)));
        Window.draw(MDarkOverlay);

        // "You Lose!" Title
        sf::Text loseText;
        loseText.setFont(MFont);
        loseText.setString("You Lose!");
        loseText.setCharacterSize(static_cast<unsigned int>(100 * scaleY));
        sf::FloatRect loseBounds = loseText.getLocalBounds();
        loseText.setOrigin(loseBounds.width / 2.0f, loseBounds.height / 2.0f);
        loseText.setPosition(Window.getSize().x / 2.0f, Window.getSize().y / 4.0f);  // Centered title
        Window.draw(loseText);

        // Draw the "Restart" button
        MRestartButton.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
        sf::FloatRect restartBounds = MRestartButton.getLocalBounds();
        MRestartButton.setOrigin(restartBounds.width / 2.0f, restartBounds.height / 2.0f);
        MRestartButton.setPosition(Window.getSize().x / 2.0f, Window.getSize().y / 2.0f + 100 * scaleY);  // Centered and spaced
        Window.draw(MRestartButton);

        // Draw the "Menu" button
        MMenuButton.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
        sf::FloatRect menuBounds = MMenuButton.getLocalBounds();
        MMenuButton.setOrigin(menuBounds.width / 2.0f, menuBounds.height / 2.0f);
        MMenuButton.setPosition(Window.getSize().x / 2.0f, Window.getSize().y / 2.0f + 200 * scaleY);  // Centered and spaced
        Window.draw(MMenuButton);
    }

    // Handle pause screen if not on win screen
    if (isPaused && !isWin && !isLose) {
        // Draw dark overlay
        MDarkOverlay.setSize(sf::Vector2f(Window.getSize().x, Window.getSize().y));
        Window.draw(MDarkOverlay);

        // "Paused" Title
        sf::Text pausedText;
        pausedText.setFont(MFont);
        pausedText.setString("Paused");
        pausedText.setCharacterSize(static_cast<unsigned int>(100 * scaleY));
        sf::FloatRect pausedBounds = pausedText.getLocalBounds();
        pausedText.setOrigin(pausedBounds.width / 2.0f, pausedBounds.height / 2.0f);
        pausedText.setPosition(Window.getSize().x / 2.0f, Window.getSize().y / 4.0f);  // Centered title
        Window.draw(pausedText);

        // Draw the "Resume" button
        MResumeButton.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
        sf::FloatRect resumeBounds = MResumeButton.getLocalBounds();
        MResumeButton.setOrigin(resumeBounds.width / 2.0f, resumeBounds.height / 2.0f);
        MResumeButton.setPosition(Window.getSize().x / 2.0f, Window.getSize().y / 2.0f);  // Centered under the title
        Window.draw(MResumeButton);

        // Draw the "Restart" button
        MRestartButton.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
        sf::FloatRect restartBounds = MRestartButton.getLocalBounds();
        MRestartButton.setOrigin(restartBounds.width / 2.0f, restartBounds.height / 2.0f);
        MRestartButton.setPosition(Window.getSize().x / 2.0f, Window.getSize().y / 2.0f + 100 * scaleY);  // Centered and spaced
        Window.draw(MRestartButton);

        // Draw the "Menu" button
        MMenuButton.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
        sf::FloatRect menuBounds = MMenuButton.getLocalBounds();
        MMenuButton.setOrigin(menuBounds.width / 2.0f, menuBounds.height / 2.0f);
        MMenuButton.setPosition(Window.getSize().x / 2.0f, Window.getSize().y / 2.0f + 200 * scaleY);  // Centered and spaced
        Window.draw(MMenuButton);
    }
}

void Level1::updateButtonPositions(const sf::Vector2u& WindowSize)
{
	Scene::updateButtonPositions(WindowSize);
}

void Level1::handleInput(sf::RenderWindow& Window, sf::Event& Event) {

    // DEBUG: set lose
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::L) && !isPaused && !isWin)
    {
        isLose = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && !isPaused && !isWin && !isLose)
    {
        MSceneManager.setScene(std::make_shared<Level1>(MSceneManager));  // Restart the level
    }

    // Handle pausing with the 'P' key without using KeyReleased and avoiding key repeat
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && !isWin && !isLose) {
        if (!PKeyPressed) {
            PKeyPressed = true;  // Mark the key as pressed
            togglePause();  // Toggle pause when 'P' is pressed once

            // Clear any input states related to dragging
            isDragging = false;
            MTrajectoryPoints.clear();
        }
    }
    else {
        PKeyPressed = false;  // Reset the flag when the key is released
    }

    if (isPaused && !isWin && !isLose) {
        // Handle mouse interaction only for the pause menu buttons when paused
        if (Event.type == sf::Event::MouseButtonReleased) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(Window);
            if (MResumeButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                togglePause();  // Resume the game
            }
            else if (MRestartButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                MSceneManager.setScene(std::make_shared<Level1>(MSceneManager));  // Restart the level
            }
            else if (MMenuButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                MSceneManager.setScene(std::make_shared<TitleScreen>(MSceneManager));  // Go back to menu
            }
        }
    }
    if (!isPaused && !isWin && !isLose) {
        // Only allow input when the game is not paused
        if (Event.type == sf::Event::MouseButtonPressed && Event.mouseButton.button == sf::Mouse::Left) {
            isDragging = true;
            MDragStart = sf::Vector2f(sf::Mouse::getPosition(Window));
        }

        if (Event.type == sf::Event::MouseMoved && isDragging) {
            MDragEnd = sf::Vector2f(sf::Mouse::getPosition(Window));
            calculateParabolicTrajectory(MDragStart, MDragEnd);
        }

        if (Event.type == sf::Event::MouseButtonReleased && Event.mouseButton.button == sf::Mouse::Left) {
            isDragging = false;
            launchProjectile(MDragStart, MDragEnd);  // Launch the projectile
        }
    }
    if (isWin) {
        if (Event.type == sf::Event::MouseButtonReleased) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(Window);
            if (MNextLevelButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                MSceneManager.setScene(std::make_shared<Level2>(MSceneManager));  // Go to the next level (Level2)
            }
            else if (MRestartButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                MSceneManager.setScene(std::make_shared<Level1>(MSceneManager));  // Restart the level
            }
            else if (MMenuButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                MSceneManager.setScene(std::make_shared<TitleScreen>(MSceneManager));  // Go back to menu
            }
        }
    }
    if (isLose) {
        // Handle mouse interaction for the lose screen
        if (Event.type == sf::Event::MouseButtonReleased) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(Window);
            if (MRestartButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                MSceneManager.setScene(std::make_shared<Level1>(MSceneManager));  // Restart the level
            }
            else if (MMenuButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                MSceneManager.setScene(std::make_shared<TitleScreen>(MSceneManager));  // Go back to menu
            }
        }
    }
}

void Level1::launchProjectile(const sf::Vector2f& Start, const sf::Vector2f& End) {
    if (isPaused || isWin || isLose || isProjectileLaunched) {
        return;  // Don't launch projectiles if the game is paused, already won/lost, or the projectile is already launched
    }

    // Calculate direction and magnitude of the launch force
    sf::Vector2f force = Start - End;

    // Apply force to the projectile body
    b2Vec2 launchForce((force.x * 0.75f) / MPixelsPerMeter, (force.y * 0.75f) / MPixelsPerMeter);
    MProjectileBody->ApplyLinearImpulseToCenter(launchForce, true);

    // Mark the projectile as launched
    isProjectileLaunched = true;
    isDragging = false;  // Disable dragging after launch

    // Clear the trajectory points after launching
    MTrajectoryPoints.clear();
}

void Level1::calculateParabolicTrajectory(const sf::Vector2f& Start, const sf::Vector2f& End) {
    // Clear previous trajectory points
    MTrajectoryPoints.clear();

    // Start from the projectile's current position
    sf::Vector2f projectilePosition = MProjectileShape.getPosition();

    // Calculate direction and velocity, reversed to get the proper trajectory
    sf::Vector2f direction = Start - End;  // Reverse the direction so it's from mouse to projectile
    float velocityX = (direction.x * 3.0f) / MPixelsPerMeter;  // Increased by a factor of 3
    float velocityY = (direction.y * 3.0f) / MPixelsPerMeter;  // Y-axis is no longer inverted

    float gravity = MWorld.GetGravity().y;  // Get gravity from Box2D world

    const int numPoints = 30;  // Number of points to simulate for the trajectory
    const float timeStep = 0.1f;  // Time step for each point in the simulation

    bool apexReached = false;  // Flag to detect the apex

    for (int i = 0; i < numPoints; ++i) {
        float t = i * timeStep;

        // Calculate vertical velocity at time t
        float currentVelocityY = velocityY + gravity * t;

        // Stop drawing points if the apex is reached (when vertical velocity is near zero)
        if (currentVelocityY > 0 && !apexReached) {
            apexReached = true;  // Mark the apex as reached and stop drawing further points
        }

        // Stop drawing the trajectory after the apex
        if (apexReached) {
            break;
        }

        // Calculate the position of the projectile at time t
        float x = velocityX * t;
        float y = velocityY * t + 0.5f * gravity * t * t;

        sf::CircleShape point(5.0f);  // Increased size of the point for more visibility
        point.setFillColor(sf::Color::Red);
        point.setPosition((projectilePosition.x / MPixelsPerMeter + x) * MPixelsPerMeter,
            (projectilePosition.y / MPixelsPerMeter + y) * MPixelsPerMeter);

        MTrajectoryPoints.push_back(point);
    }
}

void Level1::togglePause() {
    isPaused = !isPaused;
}

void Level1::handleCollisions() {
    for (b2Contact* contact = MWorld.GetContactList(); contact; contact = contact->GetNext()) {
        if (contact->IsTouching()) {
            b2Body* bodyA = contact->GetFixtureA()->GetBody();
            b2Body* bodyB = contact->GetFixtureB()->GetBody();

            // Iterate through all enemies
            for (auto& enemy : MEnemies) {
                if ((bodyA == enemy.MEnemyBody || bodyB == enemy.MEnemyBody) && enemy.isAlive) {
                    // Get the velocity and mass of the other body
                    b2Body* otherBody = (bodyA == enemy.MEnemyBody) ? bodyB : bodyA;
                    float mass = otherBody->GetMass();
                    b2Vec2 velocity = otherBody->GetLinearVelocity();
                    float speed = velocity.Length();

                    // If speed and mass exceed the threshold, kill the enemy
                    const float speedThreshold = 0.2f;
                    const float massThreshold = 1.0f;

                    if (speed > speedThreshold || mass > massThreshold) {
                        enemy.isAlive = false;
                        MWorld.DestroyBody(enemy.MEnemyBody);  // Destroy the enemy body
                        std::cout << "Enemy killed by collision!" << std::endl;
                    }
                }
            }
        }
    }
}

void Level1::initEnemies(const std::vector<sf::Vector2f>& Positions) {
    // Load the enemy texture once for all enemies
    if (!MEnemyTexture.loadFromFile("resources/kenney physics assets/PNG/Aliens/alienGreen_round.png")) {
        std::cerr << "Failed to load enemy texture!" << std::endl;
        return;
    }

    // Loop through the provided positions to create the enemies
    for (const auto& pos : Positions) {
        Enemy enemy;

        // Set texture and scaling for the enemy sprite
        enemy.MEnemySprite.setTexture(MEnemyTexture);
        enemy.MEnemySprite.setScale(1.0f, 1.0f);  // Adjust scaling if needed
        enemy.MEnemySprite.setOrigin(enemy.MEnemySprite.getGlobalBounds().width / 2.0f, enemy.MEnemySprite.getGlobalBounds().height / 2.0f);

        // Set initial position for each enemy
        float xPosition = pos.x;
        float yPosition = pos.y;
        enemy.MEnemySprite.setPosition(xPosition, yPosition);

        // Define the enemy body in Box2D
        b2BodyDef enemyBodyDef;
        enemyBodyDef.type = b2_dynamicBody;
        enemyBodyDef.position.Set(xPosition / MPixelsPerMeter, yPosition / MPixelsPerMeter);
        enemy.MEnemyBody = MWorld.CreateBody(&enemyBodyDef);

        // Define the shape and properties
        b2PolygonShape enemyShape;
        enemyShape.SetAsBox((enemy.MEnemySprite.getGlobalBounds().width / 2.0f) / MPixelsPerMeter,
            (enemy.MEnemySprite.getGlobalBounds().height / 2.0f) / MPixelsPerMeter);

        b2FixtureDef enemyFixtureDef;
        enemyFixtureDef.shape = &enemyShape;
        enemyFixtureDef.density = 1.0f;
        enemyFixtureDef.friction = 0.5f;
        enemy.MEnemyBody->CreateFixture(&enemyFixtureDef);

        // Set user data for collision detection
        enemy.MEnemyBody->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);

        // Mark the enemy as alive initially
        enemy.isAlive = true;

        // Add the enemy to the list of enemies
        MEnemies.push_back(enemy);
    }
}

void Level1::checkEnemiesAlive() {
    bool allDead = true;

    for (const auto& enemy : MEnemies) {
        if (enemy.isAlive) {
            allDead = false;
            break;  // Exit early if we find an enemy that is still alive
        }
    }

    if (allDead) {
        isWin = true;  // Trigger win condition
    }
}

void Level1::spawnProjectile() {
    if (MRemainingProjectiles > 0) {
        // Create a new projectile
        b2BodyDef projectileBodyDef;
        projectileBodyDef.type = b2_dynamicBody;
        projectileBodyDef.position.Set(100.0f / MPixelsPerMeter, 800.0f / MPixelsPerMeter);  // Starting position for each projectile
        MProjectileBody = MWorld.CreateBody(&projectileBodyDef);

        // Add linear damping to slow down the projectile over time (like resistance)
        MProjectileBody->SetLinearDamping(0.8f);  // Adjust this value for more/less gradual slowing

        b2CircleShape projectileShapeDef;
        projectileShapeDef.m_radius = 25.0f / MPixelsPerMeter;  // Set radius of the projectile

        b2FixtureDef projectileFixtureDef;
        projectileFixtureDef.shape = &projectileShapeDef;
        projectileFixtureDef.density = 1.0f;           // Adjust density based on mass
        projectileFixtureDef.friction = 0.8f;          // High friction to simulate grass resistance
        projectileFixtureDef.restitution = 0.2f;       // Low restitution for less bounce on grass
        MProjectileBody->CreateFixture(&projectileFixtureDef);

        // Reset projectile flags and decrease the projectile count
        isProjectileLaunched = false;
        isProjectileStopped = false;
        MStationaryTime = 0.0f;
        MRemainingProjectiles--;
    }
    else {
        // If no projectiles remain, trigger the lose condition
        isLose = true;
    }
}
