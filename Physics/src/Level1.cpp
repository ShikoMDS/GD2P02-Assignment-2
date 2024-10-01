#include "Level1.h"

Level1::Level1(SceneManager& manager)
    : world(b2Vec2(0.0f, 9.8f)),  // Set gravity for the Box2D world
    sceneManager(manager),
    pixelsPerMeter(100.0f)  // 100 pixels per meter for scaling
{
    // Load the background texture
    if (!backgroundTexture.loadFromFile("resources/kenney physics assets/PNG/Backgrounds/colored_desert.png")) {
        // Handle loading error
    }
    backgroundSprite.setTexture(backgroundTexture);

    // Set background to stretch across the window size
    sf::Vector2u windowSize(1600, 900); // Assuming the window size is locked
    sf::Vector2u bgTextureSize = backgroundTexture.getSize();
    backgroundSprite.setScale(
        static_cast<float>(windowSize.x) / bgTextureSize.x,
        static_cast<float>(windowSize.y) / bgTextureSize.y
    );

    // Load the grass texture and create the tiles
    if (!grassTexture.loadFromFile("resources/kenney physics assets/PNG/Other/grass.png")) {
        // Handle loading error
    }

    // Create ground tiles by tiling the grass texture across the screen width
    for (int i = 0; i < 1600 / 70 + 1; ++i) {  // Assuming each grass tile is 70x70
        sf::Sprite grassTile;
        grassTile.setTexture(grassTexture);
        grassTile.setPosition(i * 70.0f, 830.0f);  // Position at the bottom of the window
        grassTiles.push_back(grassTile);
    }

    // Load block texture
    if (!blockTexture.loadFromFile("resources/kenney physics assets/PNG/Wood elements/elementWood010.png")) {
        // Handle error
    }

    // Initialize the dark overlay for pause state
    darkOverlay.setFillColor(sf::Color(0, 0, 0, 150));  // Black overlay with transparency

    // Set up buttons and font for the pause menu
    if (!font.loadFromFile("resources/sugar bread/Sugar Bread.otf")) {
        // Handle error
    }
    resumeButton.setFont(font);
    resumeButton.setString("Resume");
    restartButton.setFont(font);
    restartButton.setString("Restart");
    menuButton.setFont(font);
    menuButton.setString("Menu");
    nextLevelButton.setFont(font);  // Use the preloaded font
    nextLevelButton.setString("Next Level");
    nextLevelButton.setCharacterSize(50);  // Initial text size, will be scaled later

    // Load projectile texture
    if (!projectileTexture.loadFromFile("resources/kenney animal pack redux/PNG/Round (outline)/frog.png")) {
        // Handle loading error
    }

    // Initialize the projectile shape
    projectileShape.setSize(sf::Vector2f(50.0f, 50.0f));  // 50x50 pixels
    projectileShape.setOrigin(25.0f, 25.0f);
    projectileShape.setTexture(&projectileTexture);

    // Drag line setup
    dragLine = sf::VertexArray(sf::Lines, 2);

    isWin = false;
}

void Level1::init() {
    // Define the ground body for Box2D
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(800.0f / pixelsPerMeter, 850.0f / pixelsPerMeter);  // Box2D works in meters
    groundBody = world.CreateBody(&groundBodyDef);

    b2PolygonShape groundBox;
    groundBox.SetAsBox(800.0f / pixelsPerMeter, 10.0f / pixelsPerMeter);  // Half-width, half-height in meters
    groundBody->CreateFixture(&groundBox, 0.0f);  // Static ground with zero density

    // Define the block body
    b2BodyDef blockBodyDef;
    blockBodyDef.type = b2_dynamicBody;  // Dynamic body for the falling block
    blockBodyDef.position.Set(800.0f / pixelsPerMeter, 200.0f / pixelsPerMeter);  // Centered at the top
    blockBody = world.CreateBody(&blockBodyDef);

    b2PolygonShape blockPhysicsShape;  // Renamed to avoid conflict
    blockPhysicsShape.SetAsBox(50.0f / pixelsPerMeter, 50.0f / pixelsPerMeter);  // 50x50 pixel block
    b2FixtureDef blockFixtureDef;
    blockFixtureDef.shape = &blockPhysicsShape;
    blockFixtureDef.density = 1.0f;
    blockFixtureDef.friction = 0.3f;
    blockBody->CreateFixture(&blockFixtureDef);

    // Set up the visual representation for the block
    blockShape.setSize(sf::Vector2f(100.0f, 100.0f));  // 100x100 pixels
    blockShape.setOrigin(50.0f, 50.0f);  // Set origin to center for proper rotation
    blockShape.setTexture(&blockTexture);  // Set the texture for the block

    // Define the projectile body for Box2D
    b2BodyDef projectileBodyDef;
    projectileBodyDef.type = b2_dynamicBody;  // Dynamic body for the projectile
    projectileBodyDef.position.Set(200.0f / pixelsPerMeter, 800.0f / pixelsPerMeter);  // Positioned on the ground
    projectileBody = world.CreateBody(&projectileBodyDef);

    b2PolygonShape projectilePhysicsShape;
    projectilePhysicsShape.SetAsBox(25.0f / pixelsPerMeter, 25.0f / pixelsPerMeter);  // 50x50 pixel projectile

    b2FixtureDef projectileFixtureDef;
    projectileFixtureDef.shape = &projectilePhysicsShape;
    projectileFixtureDef.density = 1.0f;
    projectileFixtureDef.friction = 0.3f;
    projectileBody->CreateFixture(&projectileFixtureDef);

    std::vector enemyPositions = {
    sf::Vector2f(1100.0f, 800.0f),  // First enemy position
    sf::Vector2f(1200.0f, 800.0f),  // Second enemy position
    sf::Vector2f(1300.0f, 800.0f),   // Third enemy position
    sf::Vector2f(1500.0f, 800.0f)   // Fourth enemy position
    };

    initEnemies(enemyPositions);

}

void Level1::update(float deltaTime) {
    if (!isPaused && !isWin) {
        world.Step(deltaTime, 8, 3);  // Update the Box2D world

        // Update enemy positions
        for (auto& enemy : enemies) {
            if (enemy.isAlive) {
                b2Vec2 enemyPos = enemy.body->GetPosition();
                enemy.sprite.setPosition(enemyPos.x * pixelsPerMeter, enemyPos.y * pixelsPerMeter);
                enemy.sprite.setRotation(enemy.body->GetAngle() * 180.0f / b2_pi);
            }
        }

        // Handle collisions
        handleCollisions();

        // Check if all enemies are dead
        checkEnemiesAlive();

        // Update block and projectile positions and rotations
        b2Vec2 position = blockBody->GetPosition();
        float angle = blockBody->GetAngle();
        blockShape.setPosition(position.x * pixelsPerMeter, position.y * pixelsPerMeter);
        blockShape.setRotation(angle * 180.0f / 3.14159f);

        b2Vec2 projectilePosition = projectileBody->GetPosition();
        float projectileAngle = projectileBody->GetAngle();
        projectileShape.setPosition(projectilePosition.x * pixelsPerMeter, projectilePosition.y * pixelsPerMeter);
        projectileShape.setRotation(projectileAngle * 180.0f / 3.14159f);
    }
}

void Level1::draw(sf::RenderWindow& window) {
    // Draw background
    window.draw(backgroundSprite);

    // Draw the ground tiles
    for (const auto& tile : grassTiles) {
        window.draw(tile);
    }

    // Draw the block
    window.draw(blockShape);

    // Draw the projectile
    window.draw(projectileShape);

    // Draw the drag line if dragging
    if (isDragging) {
        for (const auto& point : trajectoryPoints) {
            window.draw(point);
        }
    }

    // Draw all enemies that are alive
    for (const auto& enemy : enemies) {
        if (enemy.isAlive) {
            window.draw(enemy.sprite);
        }
    }

    // Calculate scaling factors based on window size
    float scaleX = static_cast<float>(window.getSize().x) / 1600.0f;  // 1600 is the reference width
    float scaleY = static_cast<float>(window.getSize().y) / 900.0f;   // 900 is the reference height

    // Handle win screen
    if (isWin) {
        // Draw dark overlay
        darkOverlay.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
        window.draw(darkOverlay);

        // "You Win" Title
        sf::Text winText;
        winText.setFont(font);
        winText.setString("You Win!");
        winText.setCharacterSize(static_cast<unsigned int>(100 * scaleY));
        sf::FloatRect winBounds = winText.getLocalBounds();
        winText.setOrigin(winBounds.width / 2.0f, winBounds.height / 2.0f);
        winText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 4.0f);  // Centered title
        window.draw(winText);

        // Draw the "Next Level" button
        nextLevelButton.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
        sf::FloatRect nextBounds = nextLevelButton.getLocalBounds();
        nextLevelButton.setOrigin(nextBounds.width / 2.0f, nextBounds.height / 2.0f);
        nextLevelButton.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);  // Centered under the title
        window.draw(nextLevelButton);

        // Draw the "Restart" button
        restartButton.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
        sf::FloatRect restartBounds = restartButton.getLocalBounds();
        restartButton.setOrigin(restartBounds.width / 2.0f, restartBounds.height / 2.0f);
        restartButton.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f + 100 * scaleY);  // Centered and spaced
        window.draw(restartButton);

        // Draw the "Menu" button
        menuButton.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
        sf::FloatRect menuBounds = menuButton.getLocalBounds();
        menuButton.setOrigin(menuBounds.width / 2.0f, menuBounds.height / 2.0f);
        menuButton.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f + 200 * scaleY);  // Centered and spaced
        window.draw(menuButton);
    }

    // Handle pause screen if not on win screen
    if (isPaused && !isWin) {
        // Draw dark overlay
        darkOverlay.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
        window.draw(darkOverlay);

        // "Paused" Title
        sf::Text pausedText;
        pausedText.setFont(font);
        pausedText.setString("Paused");
        pausedText.setCharacterSize(static_cast<unsigned int>(100 * scaleY));
        sf::FloatRect pausedBounds = pausedText.getLocalBounds();
        pausedText.setOrigin(pausedBounds.width / 2.0f, pausedBounds.height / 2.0f);
        pausedText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 4.0f);  // Centered title
        window.draw(pausedText);

        // Draw the "Resume" button
        resumeButton.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
        sf::FloatRect resumeBounds = resumeButton.getLocalBounds();
        resumeButton.setOrigin(resumeBounds.width / 2.0f, resumeBounds.height / 2.0f);
        resumeButton.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);  // Centered under the title
        window.draw(resumeButton);

        // Draw the "Restart" button
        restartButton.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
        sf::FloatRect restartBounds = restartButton.getLocalBounds();
        restartButton.setOrigin(restartBounds.width / 2.0f, restartBounds.height / 2.0f);
        restartButton.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f + 100 * scaleY);  // Centered and spaced
        window.draw(restartButton);

        // Draw the "Menu" button
        menuButton.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
        sf::FloatRect menuBounds = menuButton.getLocalBounds();
        menuButton.setOrigin(menuBounds.width / 2.0f, menuBounds.height / 2.0f);
        menuButton.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f + 200 * scaleY);  // Centered and spaced
        window.draw(menuButton);
    }
}

void Level1::updateButtonPositions(const sf::Vector2u& windowSize)
{
	Scene::updateButtonPositions(windowSize);
}

void Level1::handleInput(sf::RenderWindow& window, sf::Event& event) {
    // Handle pausing with the 'P' key without using KeyReleased and avoiding key repeat
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && !isWin) {
        if (!pKeyPressed) {
            pKeyPressed = true;  // Mark the key as pressed
            togglePause();  // Toggle pause when 'P' is pressed once

            // Clear any input states related to dragging
            isDragging = false;
            trajectoryPoints.clear();
        }
    }
    else {
        pKeyPressed = false;  // Reset the flag when the key is released
    }

    if (isPaused && !isWin) {
        // Handle mouse interaction only for the pause menu buttons when paused
        if (event.type == sf::Event::MouseButtonReleased) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (resumeButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                togglePause();  // Resume the game
            }
            else if (restartButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                sceneManager.setScene(std::make_shared<Level1>(sceneManager));  // Restart the level
            }
            else if (menuButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                sceneManager.setScene(std::make_shared<TitleScreen>(sceneManager));  // Go back to menu
            }
        }
    }
    if (!isPaused && !isWin) {
        // Only allow input when the game is not paused
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            isDragging = true;
            dragStart = sf::Vector2f(sf::Mouse::getPosition(window));
        }

        if (event.type == sf::Event::MouseMoved && isDragging) {
            dragEnd = sf::Vector2f(sf::Mouse::getPosition(window));
            calculateParabolicTrajectory(dragStart, dragEnd);
        }

        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            isDragging = false;
            launchProjectile(dragStart, dragEnd);  // Launch the projectile
        }
    }
    if (isWin) {
        if (event.type == sf::Event::MouseButtonReleased) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (nextLevelButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                sceneManager.setScene(std::make_shared<Level2>(sceneManager));  // Go to the next level (Level2)
            }
            else if (restartButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                sceneManager.setScene(std::make_shared<Level1>(sceneManager));  // Restart the level
            }
            else if (menuButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                sceneManager.setScene(std::make_shared<TitleScreen>(sceneManager));  // Go back to menu
            }
        }
    }


}

void Level1::launchProjectile(const sf::Vector2f& start, const sf::Vector2f& end) {
    if (isPaused) {
        return;  // Don't launch projectiles if the game is paused
    }

    // Calculate direction and magnitude of the launch force
    sf::Vector2f force = start - end;

    // Reduce the strength of the force applied to the projectile
    b2Vec2 launchForce((force.x * 0.75f) / pixelsPerMeter, (force.y * 0.75f) / pixelsPerMeter); // Reduced by lowering factor to 0.75
    projectileBody->ApplyLinearImpulseToCenter(launchForce, true);

    // Clear the trajectory points after launching
    trajectoryPoints.clear();
}
void Level1::calculateParabolicTrajectory(const sf::Vector2f& start, const sf::Vector2f& end) {
    // Clear previous trajectory points
    trajectoryPoints.clear();

    // Start from the projectile's current position
    sf::Vector2f projectilePosition = projectileShape.getPosition();

    // Calculate direction and velocity, reversed to get the proper trajectory
    sf::Vector2f direction = start - end;  // Reverse the direction so it's from mouse to projectile
    float velocityX = (direction.x * 3.0f) / pixelsPerMeter;  // Increased by a factor of 3
    float velocityY = (direction.y * 3.0f) / pixelsPerMeter;  // Y-axis is no longer inverted

    float gravity = world.GetGravity().y;  // Get gravity from Box2D world

    const int numPoints = 30;  // Number of points to simulate for the trajectory
    const float timeStep = 0.1f;  // Time step for each point in the simulation

    for (int i = 0; i < numPoints; ++i) {
        float t = i * timeStep;

        // Calculate the position of the projectile at time t
        float x = velocityX * t;
        float y = velocityY * t + 0.5f * gravity * t * t;

        sf::CircleShape point(5.0f);  // Increased size of the point for more visibility
        point.setFillColor(sf::Color::Red);
        point.setPosition((projectilePosition.x / pixelsPerMeter + x) * pixelsPerMeter, (projectilePosition.y / pixelsPerMeter + y) * pixelsPerMeter);

        trajectoryPoints.push_back(point);
    }
}

void Level1::togglePause() {
    isPaused = !isPaused;
}

void Level1::handleCollisions() {
    for (b2Contact* contact = world.GetContactList(); contact; contact = contact->GetNext()) {
        if (contact->IsTouching()) {
            b2Body* bodyA = contact->GetFixtureA()->GetBody();
            b2Body* bodyB = contact->GetFixtureB()->GetBody();

            // Iterate through all enemies
            for (auto& enemy : enemies) {
                if ((bodyA == enemy.body || bodyB == enemy.body) && enemy.isAlive) {
                    // Get the velocity and mass of the other body
                    b2Body* otherBody = (bodyA == enemy.body) ? bodyB : bodyA;
                    float mass = otherBody->GetMass();
                    b2Vec2 velocity = otherBody->GetLinearVelocity();
                    float speed = velocity.Length();

                    // If speed and mass exceed the threshold, kill the enemy
                    const float speedThreshold = 0.2f;
                    const float massThreshold = 1.0f;

                    if (speed > speedThreshold || mass > massThreshold) {
                        enemy.isAlive = false;
                        world.DestroyBody(enemy.body);  // Destroy the enemy body
                        std::cout << "Enemy killed by collision!" << std::endl;
                    }
                }
            }
        }
    }
}

void Level1::initEnemies(const std::vector<sf::Vector2f>& positions) {
    // Load the enemy texture once for all enemies
    if (!enemyTexture.loadFromFile("resources/kenney physics assets/PNG/Aliens/alienGreen_round.png")) {
        std::cerr << "Failed to load enemy texture!" << std::endl;
        return;
    }

    // Loop through the provided positions to create the enemies
    for (const auto& pos : positions) {
        Enemy enemy;

        // Set texture and scaling for the enemy sprite
        enemy.sprite.setTexture(enemyTexture);
        enemy.sprite.setScale(1.0f, 1.0f);  // Adjust scaling if needed
        enemy.sprite.setOrigin(enemy.sprite.getGlobalBounds().width / 2.0f, enemy.sprite.getGlobalBounds().height / 2.0f);

        // Set initial position for each enemy
        float xPosition = pos.x;
        float yPosition = pos.y;
        enemy.sprite.setPosition(xPosition, yPosition);

        // Define the enemy body in Box2D
        b2BodyDef enemyBodyDef;
        enemyBodyDef.type = b2_dynamicBody;
        enemyBodyDef.position.Set(xPosition / pixelsPerMeter, yPosition / pixelsPerMeter);
        enemy.body = world.CreateBody(&enemyBodyDef);

        // Define the shape and properties
        b2PolygonShape enemyShape;
        enemyShape.SetAsBox((enemy.sprite.getGlobalBounds().width / 2.0f) / pixelsPerMeter,
            (enemy.sprite.getGlobalBounds().height / 2.0f) / pixelsPerMeter);

        b2FixtureDef enemyFixtureDef;
        enemyFixtureDef.shape = &enemyShape;
        enemyFixtureDef.density = 1.0f;
        enemyFixtureDef.friction = 0.5f;
        enemy.body->CreateFixture(&enemyFixtureDef);

        // Set user data for collision detection
        enemy.body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);

        // Mark the enemy as alive initially
        enemy.isAlive = true;

        // Add the enemy to the list of enemies
        enemies.push_back(enemy);
    }
}

void Level1::checkEnemiesAlive() {
    bool allDead = true;

    for (const auto& enemy : enemies) {
        if (enemy.isAlive) {
            allDead = false;
            break;  // Exit early if we find an enemy that is still alive
        }
    }

    if (allDead) {
        isWin = true;  // Trigger win condition
    }
}
