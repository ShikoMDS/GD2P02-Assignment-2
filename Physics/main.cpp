#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <iostream>

const float SCALE = 50.0f; // Increase scale to make gravity feel stronger

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML + Box2D - Hello World");
    window.setFramerateLimit(60);

    b2Vec2 gravity(0.0f, 9.8f); // Keep standard gravity
    b2World world(gravity);

    // Define the ground body (static)
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, 500.0f / SCALE); // Adjusted for the new scale
    b2Body* groundBody = world.CreateBody(&groundBodyDef);

    // Create the ground shape
    b2PolygonShape groundBox;
    groundBox.SetAsBox(400.0f / SCALE, 10.0f / SCALE);
    groundBody->CreateFixture(&groundBox, 0.0f);

    // Create a shape to represent the Box2D ground in SFML
    sf::RectangleShape groundShape(sf::Vector2f(800.0f, 20.0f)); // Width and height in pixels
    groundShape.setFillColor(sf::Color::Red); // Set a color for visibility
    groundShape.setOrigin(400.0f, 10.0f); // Set origin to the center
    groundShape.setPosition(400.0f, 500.0f); // Position in pixels

    // Define a dynamic body (falling box)
    b2BodyDef dynamicBodyDef;
    dynamicBodyDef.type = b2_dynamicBody;
    dynamicBodyDef.position.Set(400.0f / SCALE, 100.0f / SCALE); // Adjusted for the new scale
    b2Body* dynamicBody = world.CreateBody(&dynamicBodyDef);

    // Create a box shape for the dynamic body
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(25.0f / SCALE, 25.0f / SCALE);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    dynamicBody->CreateFixture(&fixtureDef);

    // Create an SFML shape
    sf::RectangleShape boxShape(sf::Vector2f(50.0f, 50.0f));
    boxShape.setFillColor(sf::Color::Green);
    boxShape.setOrigin(25.0f, 25.0f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Step the Box2D world
        float timeStep = 1.0f / 60.0f;
        int32 velocityIterations = 6;
        int32 positionIterations = 2;
        world.Step(timeStep, velocityIterations, positionIterations);

        // Get the position of the dynamic body
        b2Vec2 position = dynamicBody->GetPosition();
        float angle = dynamicBody->GetAngle();

        // Update the SFML shape
        boxShape.setPosition(position.x * SCALE, position.y * SCALE);
        boxShape.setRotation(angle * 180.0f / b2_pi);

        window.clear();
        window.draw(boxShape);
        window.draw(groundShape);
        window.display();
    }

    return 0;
}
