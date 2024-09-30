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

    initEnemy();  // Initialize the enemy
}

void Level1::update(float deltaTime) {
    if (!isPaused) {
        world.Step(deltaTime, 8, 3);  // Update the Box2D world

        // Update the enemy's position
        if (isEnemyAlive) {
            b2Vec2 enemyPos = enemyBody->GetPosition();
            enemySprite.setPosition(enemyPos.x * pixelsPerMeter, enemyPos.y * pixelsPerMeter);
            enemySprite.setRotation(enemyBody->GetAngle() * 180.0f / b2_pi);
        }

        // Handle collisions and check for death
        handleCollisions();

        // Update block and projectile positions and rotations
        b2Vec2 position = blockBody->GetPosition();
        float angle = blockBody->GetAngle();
        blockShape.setPosition(position.x * pixelsPerMeter, position.y * pixelsPerMeter);
        blockShape.setRotation(angle * 180.0f / 3.14159f);

        // Update the projectile
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

    // Draw the enemy if alive
    if (isEnemyAlive) {
        window.draw(enemySprite);
    }

    if (isPaused) {
        // Draw the dark overlay and buttons
        darkOverlay.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
        window.draw(darkOverlay);

        // Adjust button positions based on window size
        float scaleX = static_cast<float>(window.getSize().x) / 1600.0f;
        float scaleY = static_cast<float>(window.getSize().y) / 900.0f;

        resumeButton.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
        resumeButton.setPosition(600 * scaleX, 300 * scaleY);

        restartButton.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
        restartButton.setPosition(600 * scaleX, 400 * scaleY);

        menuButton.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
        menuButton.setPosition(600 * scaleX, 500 * scaleY);

        // Draw buttons
        window.draw(resumeButton);
        window.draw(restartButton);
        window.draw(menuButton);
    }
}

void Level1::updateButtonPositions(const sf::Vector2u& windowSize)
{
	Scene::updateButtonPositions(windowSize);
}

void Level1::handleInput(sf::RenderWindow& window, sf::Event& event) {
    // Handle pausing with the 'P' key without using KeyReleased and avoiding key repeat
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
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

    if (isPaused) {
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
    else {
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
            // Get the two fixtures involved in the collision
            b2Fixture* fixtureA = contact->GetFixtureA();
            b2Fixture* fixtureB = contact->GetFixtureB();
            b2Body* bodyA = fixtureA->GetBody();
            b2Body* bodyB = fixtureB->GetBody();

            // Identify if the enemy is involved
            bool isEnemyInvolved = (bodyA == enemyBody || bodyB == enemyBody);
            b2Body* otherBody = (bodyA == enemyBody) ? bodyB : bodyA;

            if (isEnemyInvolved) {
                // Get the velocity and mass of the other body (projectile or block)
                float mass = otherBody->GetMass();
                b2Vec2 velocity = otherBody->GetLinearVelocity();
                float speed = velocity.Length();

                // Define reasonable thresholds for mass and speed to kill the enemy
                const float speedThreshold = 0.2f;  // Adjust this value based on your game dynamics
                const float massThreshold = 1.0f;   // Adjust this for reasonable mass to kill

                // If the speed or mass is greater than the threshold, the enemy dies
                if (speed > speedThreshold || mass > massThreshold) {
                    isEnemyAlive = false;
                    world.DestroyBody(enemyBody);  // Destroy the enemy body
                    std::cout << "Enemy killed by collision!" << std::endl;
                }
            }
        }
    }
}

void Level1::initEnemy() {
    // Load enemy texture
    if (!enemyTexture.loadFromFile("resources/kenney physics assets/PNG/Aliens/alienGreen_round.png")) {
        // Handle loading error
    }

    // Adjust scaling to match the size you need
    enemySprite.setTexture(enemyTexture);
    enemySprite.setScale(1.0f, 1.0f);  // Adjust scaling if needed
    enemySprite.setOrigin(enemySprite.getGlobalBounds().width / 2.0f, enemySprite.getGlobalBounds().height / 2.0f);

    // Define the enemy body in Box2D
    b2BodyDef enemyBodyDef;
    enemyBodyDef.type = b2_dynamicBody;
    enemyBodyDef.position.Set(1100.0f / pixelsPerMeter, 800.0f / pixelsPerMeter);  // Set position on the slope
    enemyBody = world.CreateBody(&enemyBodyDef);

    // Define the enemy shape, matching the sprite bounds
    b2PolygonShape enemyShape;
    enemyShape.SetAsBox((enemySprite.getGlobalBounds().width / 2.0f) / pixelsPerMeter,
        (enemySprite.getGlobalBounds().height / 2.0f) / pixelsPerMeter);

    b2FixtureDef enemyFixtureDef;
    enemyFixtureDef.shape = &enemyShape;
    enemyFixtureDef.density = 1.0f;
    enemyFixtureDef.friction = 0.5f;
    enemyBody->CreateFixture(&enemyFixtureDef);

    // Set user data for collision detection later
    enemyBody->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}
