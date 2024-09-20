#include "Instructions.h"
#include "TitleScreen.h"

Instructions::Instructions(SceneManager& Manager) : MSceneManager(Manager)
{
}

void Instructions::init()
{
	MFont.loadFromFile("resources/sugar bread/Sugar Bread.otf");

	MBackgroundTexture.loadFromFile("resources/kenney physics assets/PNG/Backgrounds/blue_land.png");
	MBackgroundSprite.setTexture(MBackgroundTexture);

	MTitleText.setFont(MFont);
	MTitleText.setFillColor(sf::Color::Black);
	MTitleText.setString("INSTRUCTIONS");
	MTitleText.setCharacterSize(80);
	const sf::FloatRect LTitleBounds = MTitleText.getLocalBounds();
	MTitleText.setOrigin(LTitleBounds.width / 2.0f, LTitleBounds.height / 2.0f);
	MTitleText.setPosition(800, 150);

	MInstructionsText.setFont(MFont);
	MInstructionsText.setFillColor(sf::Color::Black);
	MInstructionsText.setString("- Fling projectiles to destroy enemies\n- Use the mouse to aim and shoot");
	MInstructionsText.setCharacterSize(50);
	const sf::FloatRect LInstructionsTextBounds = MInstructionsText.getLocalBounds();
	MInstructionsText.setOrigin(LInstructionsTextBounds.width / 2.0f, LInstructionsTextBounds.height / 2.0f);
	MInstructionsText.setPosition(800, 350);

	MBackButton.setSize(sf::Vector2f(300, 75));
	MBackButton.setFillColor(sf::Color::Transparent);
	MBackButton.setOutlineColor(sf::Color::Black);
	MBackButton.setOutlineThickness(2.0f);
	MBackButton.setPosition(650, 675);

	MBackText.setFont(MFont);
	MBackText.setFillColor(sf::Color::Black);
	MBackText.setString("BACK");
	MBackText.setCharacterSize(50);
	const sf::FloatRect LBackTextBounds = MBackText.getLocalBounds();
	MBackText.setOrigin(LBackTextBounds.width / 2.0f, LBackTextBounds.height / 2.0f + LBackTextBounds.top / 2.0f);
	// Adjust for top offset
	MBackText.setPosition(MBackButton.getPosition().x + MBackButton.getSize().x / 2.0f,
	                      MBackButton.getPosition().y + MBackButton.getSize().y / 2.0f);
}

void Instructions::handleInput(sf::RenderWindow& Window, sf::Event& Event)
{
	if (Event.type == sf::Event::MouseButtonReleased && Event.mouseButton.button == sf::Mouse::Left)
	{
		const sf::Vector2i LMousePos = sf::Mouse::getPosition(Window);

		if (MBackButton.getGlobalBounds().contains(static_cast<float>(LMousePos.x), static_cast<float>(LMousePos.y)))
		{
			MSceneManager.setScene(std::make_shared<TitleScreen>(MSceneManager));
		}
	}
}

void Instructions::update(float DeltaTime)
{
}

void Instructions::draw(sf::RenderWindow& Window)
{
	// Draw the background first
	Window.draw(MBackgroundSprite);

	Window.draw(MTitleText);
	Window.draw(MInstructionsText);
	Window.draw(MBackButton);
	Window.draw(MBackText);
}

void Instructions::updateButtonPositions(const sf::Vector2u& WindowSize)
{
	const float LScaleX = static_cast<float>(WindowSize.x) / 1600.0f;
	const float LScaleY = static_cast<float>(WindowSize.y) / 900.0f;

	// Scale and position the background
	MBackgroundSprite.setScale(
		static_cast<float>(WindowSize.x) / MBackgroundTexture.getSize().x,
		static_cast<float>(WindowSize.y) / MBackgroundTexture.getSize().y
	);

	// Update title size and position
	MTitleText.setCharacterSize(static_cast<unsigned int>(80 * LScaleY));
	const sf::FloatRect LTitleBounds = MTitleText.getLocalBounds();
	MTitleText.setOrigin(LTitleBounds.width / 2.0f, LTitleBounds.height / 2.0f);
	MTitleText.setPosition(WindowSize.x / 2.0f, 150 * LScaleY);

	// Update instruction text size and position
	MInstructionsText.setCharacterSize(static_cast<unsigned int>(50 * LScaleY));
	const sf::FloatRect LInstructionsTextBounds = MInstructionsText.getLocalBounds();
	MInstructionsText.setOrigin(LInstructionsTextBounds.width / 2.0f, LInstructionsTextBounds.height / 2.0f);
	MInstructionsText.setPosition(WindowSize.x / 2.0f, 350 * LScaleY);

	// Update back button size and position
	MBackButton.setSize(sf::Vector2f(300 * LScaleX, 75 * LScaleY));
	MBackButton.setPosition((WindowSize.x / 2.0f) - (MBackButton.getSize().x / 2.0f), 675 * LScaleY);

	// Update back button text size and position
	MBackText.setCharacterSize(static_cast<unsigned int>(50 * LScaleY));
	const sf::FloatRect LBackTextBounds = MBackText.getLocalBounds();
	MBackText.setOrigin(LBackTextBounds.width / 2.0f, LBackTextBounds.height / 2.0f + LBackTextBounds.top);
	// Adjust for top offset
	MBackText.setPosition(MBackButton.getPosition().x + MBackButton.getSize().x / 2.0f,
	                      MBackButton.getPosition().y + MBackButton.getSize().y / 2.0f);
}
