#include <SFML/Graphics.hpp>
#include "SceneManager.h"
#include "TitleScreen.h"

bool isFullscreen = false;

void toggleFullscreen(sf::RenderWindow& window, bool& isFullscreen) {
    if (isFullscreen) {
        window.create(sf::VideoMode(1600, 900), "2D Physics Game", sf::Style::Resize | sf::Style::Close);
    }
    else {
        window.create(sf::VideoMode::getDesktopMode(), "2D Physics Game", sf::Style::Fullscreen);
    }
    isFullscreen = !isFullscreen;
}

void enforceAspectRatio(sf::RenderWindow& window, const float aspectRatio) {
    sf::Vector2u windowSize = window.getSize();  // Get the current window size
    float currentAspectRatio = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);

    // If the current aspect ratio does not match the desired one
    if (currentAspectRatio > aspectRatio) {
        // Too wide: adjust the width
        unsigned int newWidth = static_cast<unsigned int>(windowSize.y * aspectRatio);
        window.setSize(sf::Vector2u(newWidth, windowSize.y));
    }
    else if (currentAspectRatio < aspectRatio) {
        // Too tall: adjust the height
        unsigned int newHeight = static_cast<unsigned int>(windowSize.x / aspectRatio);
        window.setSize(sf::Vector2u(windowSize.x, newHeight));
    }

    // Adjust the view to fit the new window size
    sf::FloatRect visibleArea(0, 0, window.getSize().x, window.getSize().y);
    window.setView(sf::View(visibleArea));
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1600, 900), "2D Physics Game", sf::Style::Resize | sf::Style::Close);

    // Desired aspect ratio (16:9)
    const float aspectRatio = 16.0f / 9.0f;

    SceneManager sceneManager;

    // Start with the title screen
    sceneManager.setScene(std::make_shared<TitleScreen>(sceneManager));

    sf::Clock clock;

    while (window.isOpen()) {
    	sceneManager.getCurrentScene()->updateButtonPositions(window.getSize());  // Update to current window size

        sf::Event event;
        while (window.pollEvent(event)) {


            if (event.type == sf::Event::Closed)
                window.close();

            // Handle fullscreen toggle with F11
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F11) {
                toggleFullscreen(window, isFullscreen);
            }

            // Enforce aspect ratio on resize
            if (event.type == sf::Event::Resized) {
                enforceAspectRatio(window, aspectRatio);

                // Recalculate positions and sizes
                auto currentScene = sceneManager.getCurrentScene();
                if (currentScene) {
                    currentScene->updateButtonPositions(window.getSize());
                }
            }

            // Forward the event to the current scene's input handler
            sceneManager.handleInput(window, event);
        }

        float deltaTime = clock.restart().asSeconds();

        sceneManager.update(deltaTime);

        window.clear();
        sceneManager.draw(window);
        window.display();
    }

    return 0;
}
