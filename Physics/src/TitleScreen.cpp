#include "TitleScreen.h"
#include "LevelSelection.h"
#include "Instructions.h"

TitleScreen::TitleScreen(SceneManager& Manager) : MSceneManager(Manager)
{
}

void TitleScreen::init()
{
	MFont.loadFromFile("resources/sugar bread/Sugar Bread.otf");

	MBackgroundTexture.loadFromFile("resources/kenney physics assets/PNG/Backgrounds/colored_land.png");
	MBackgroundSprite.setTexture(MBackgroundTexture);

	MTitleText.setFont(MFont);
	MTitleText.setFillColor(sf::Color::Black);
	MTitleText.setString("2D Physics Game");
	MTitleText.setCharacterSize(80);
	const sf::FloatRect LTitleBounds = MTitleText.getLocalBounds();
	MTitleText.setOrigin(LTitleBounds.width / 2.0f, LTitleBounds.height / 2.0f);
	MTitleText.setPosition(800, 150);

	MPlayButton.setSize(sf::Vector2f(300, 75));
	MPlayButton.setFillColor(sf::Color::Transparent);
	MPlayButton.setOutlineColor(sf::Color::Black);
	MPlayButton.setOutlineThickness(2.0f);

	MPlayText.setFont(MFont);
	MPlayText.setString("PLAY");
	MPlayText.setCharacterSize(40);
	MPlayText.setFillColor(sf::Color::Black);

	MInstructionsButton.setSize(sf::Vector2f(300, 75));
	MInstructionsButton.setFillColor(sf::Color::Transparent);
	MInstructionsButton.setOutlineColor(sf::Color::Black);
	MInstructionsButton.setOutlineThickness(2.0f);

	MInstructionsText.setFont(MFont);
	MInstructionsText.setString("INSTRUCTIONS");
	MInstructionsText.setCharacterSize(40);
	MInstructionsText.setFillColor(sf::Color::Black);

	MQuitButton.setSize(sf::Vector2f(300, 75));
	MQuitButton.setFillColor(sf::Color::Transparent);
	MQuitButton.setOutlineColor(sf::Color::Black);
	MQuitButton.setOutlineThickness(2.0f);

	MQuitText.setFont(MFont);
	MQuitText.setString("QUIT");
	MQuitText.setCharacterSize(40);
	MQuitText.setFillColor(sf::Color::Black);

	// Update positions for the first time
	updateButtonPositions(sf::Vector2u(1600, 900)); // Default window size
}

void TitleScreen::handleInput(sf::RenderWindow& Window, sf::Event& Event)
{
	if (Event.type == sf::Event::MouseButtonReleased && Event.mouseButton.button == sf::Mouse::Left)
	{
		const sf::Vector2i LMousePos = sf::Mouse::getPosition(Window);

		if (MPlayButton.getGlobalBounds().contains(static_cast<float>(LMousePos.x), static_cast<float>(LMousePos.y)))
		{
			MSceneManager.setScene(std::make_shared<LevelSelection>(MSceneManager));
		}
		else if (MInstructionsButton.getGlobalBounds().contains(static_cast<float>(LMousePos.x),
		                                                        static_cast<float>(LMousePos.y)))
		{
			MSceneManager.setScene(std::make_shared<Instructions>(MSceneManager));
		}
		else if (MQuitButton.getGlobalBounds().contains(static_cast<float>(LMousePos.x),
		                                                static_cast<float>(LMousePos.y)))
		{
			Window.close();
		}
	}
}

void TitleScreen::update(float DeltaTime)
{
	// No updates needed for title screen
}

void TitleScreen::draw(sf::RenderWindow& Window)
{
	// Draw background first
	Window.draw(MBackgroundSprite);

	// Draw title and buttons
	Window.draw(MTitleText);
	Window.draw(MPlayButton);
	Window.draw(MInstructionsButton);
	Window.draw(MQuitButton);

	// Draw button text
	Window.draw(MPlayText);
	Window.draw(MInstructionsText);
	Window.draw(MQuitText);
}

void TitleScreen::updateButtonPositions(const sf::Vector2u& WindowSize)
{
	const float LScaleX = static_cast<float>(WindowSize.x) / 1600.0f;
	const float LScaleY = static_cast<float>(WindowSize.y) / 900.0f;

	// Scale and position the background
	MBackgroundSprite.setScale(
		static_cast<float>(WindowSize.x) / MBackgroundTexture.getSize().x,
		static_cast<float>(WindowSize.y) / MBackgroundTexture.getSize().y
	);

	// Align title
	MTitleText.setCharacterSize(static_cast<unsigned int>(80 * LScaleY)); // Scaling title text
	const sf::FloatRect LTitleBounds = MTitleText.getLocalBounds();
	MTitleText.setOrigin(LTitleBounds.width / 2.0f, LTitleBounds.height / 2.0f);
	MTitleText.setPosition(WindowSize.x / 2.0f, 150 * LScaleY);

	// Set Button sizes
	MPlayButton.setSize(sf::Vector2f(300 * LScaleX, 75 * LScaleY));
	MInstructionsButton.setSize(sf::Vector2f(300 * LScaleX, 75 * LScaleY));
	MQuitButton.setSize(sf::Vector2f(300 * LScaleX, 75 * LScaleY));

	// Position Buttons
	const float LButtonSpacing = 40 * LScaleY;
	MQuitButton.setPosition((WindowSize.x / 2.0f) - (MQuitButton.getSize().x / 2.0f), 675 * LScaleY);
	MInstructionsButton.setPosition((WindowSize.x / 2.0f) - (MInstructionsButton.getSize().x / 2.0f),
	                                MQuitButton.getPosition().y - MInstructionsButton.getSize().y - LButtonSpacing);
	MPlayButton.setPosition((WindowSize.x / 2.0f) - (MPlayButton.getSize().x / 2.0f),
	                        MInstructionsButton.getPosition().y - MPlayButton.getSize().y - LButtonSpacing);

	// Scale text size based on window size
	MPlayText.setCharacterSize(static_cast<unsigned int>(40 * LScaleY));
	MInstructionsText.setCharacterSize(static_cast<unsigned int>(40 * LScaleY));
	MQuitText.setCharacterSize(static_cast<unsigned int>(40 * LScaleY));

	// Center text within buttons
	const sf::FloatRect LPlayTextRect = MPlayText.getLocalBounds();
	MPlayText.setOrigin(LPlayTextRect.left + LPlayTextRect.width / 2.0f,
	                    LPlayTextRect.top + LPlayTextRect.height / 2.0f);
	MPlayText.setPosition(MPlayButton.getPosition().x + MPlayButton.getSize().x / 2.0f,
	                      MPlayButton.getPosition().y + MPlayButton.getSize().y / 2.0f);

	const sf::FloatRect LInstructionsTextRect = MInstructionsText.getLocalBounds();
	MInstructionsText.setOrigin(LInstructionsTextRect.left + LInstructionsTextRect.width / 2.0f,
	                            LInstructionsTextRect.top + LInstructionsTextRect.height / 2.0f);
	MInstructionsText.setPosition(MInstructionsButton.getPosition().x + MInstructionsButton.getSize().x / 2.0f,
	                              MInstructionsButton.getPosition().y + MInstructionsButton.getSize().y / 2.0f);

	const sf::FloatRect LQuitTextRect = MQuitText.getLocalBounds();
	MQuitText.setOrigin(LQuitTextRect.left + LQuitTextRect.width / 2.0f,
	                    LQuitTextRect.top + LQuitTextRect.height / 2.0f);
	MQuitText.setPosition(MQuitButton.getPosition().x + MQuitButton.getSize().x / 2.0f,
	                      MQuitButton.getPosition().y + MQuitButton.getSize().y / 2.0f);
}
