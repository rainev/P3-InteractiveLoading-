#include "LoadingScreen.h"
#include "BaseRunner.h"
#include <iostream>

LoadingScreen::LoadingScreen() : AGameObject("LoadingScreen")
{
    this->currentProgress = 0.0f;
}

LoadingScreen::~LoadingScreen()
{
    delete this->loadingText->getFont();
    delete this->loadingText;
    delete this->progressBarBackground;
    delete this->progressBarFill;
}

void LoadingScreen::initialize()
{
    std::cout << "[LoadingScreen] Initializing..." << std::endl;

    // Setup loading text
    sf::Font* font = new sf::Font();
    font->loadFromFile("Media/Sansation.ttf");

    this->loadingText = new sf::Text();
    this->loadingText->setFont(*font);
    this->loadingText->setString("Loading Assets...");
    this->loadingText->setCharacterSize(48);
    this->loadingText->setFillColor(sf::Color::White);

    // Center text
    sf::FloatRect textBounds = this->loadingText->getLocalBounds();
    this->loadingText->setPosition(
        (BaseRunner::WINDOW_WIDTH - textBounds.width) / 2.0f,
        BaseRunner::WINDOW_HEIGHT / 2.0f - 100.0f
    );

    // Setup progress bar background
    this->progressBarBackground = new sf::RectangleShape(sf::Vector2f(BAR_WIDTH, BAR_HEIGHT));
    this->progressBarBackground->setFillColor(sf::Color(50, 50, 50));
    this->progressBarBackground->setPosition(
        (BaseRunner::WINDOW_WIDTH - BAR_WIDTH) / 2.0f,
        BaseRunner::WINDOW_HEIGHT / 2.0f
    );

    // Setup progress bar fill
    this->progressBarFill = new sf::RectangleShape(sf::Vector2f(0, BAR_HEIGHT));
    this->progressBarFill->setFillColor(sf::Color(0, 200, 0));
    this->progressBarFill->setPosition(
        (BaseRunner::WINDOW_WIDTH - BAR_WIDTH) / 2.0f,
        BaseRunner::WINDOW_HEIGHT / 2.0f
    );
}

void LoadingScreen::processInput(sf::Event event)
{
    // Nothing for now
}

void LoadingScreen::update(sf::Time deltaTime)
{
    // Progress is updated externally via updateProgress()
}

void LoadingScreen::draw(sf::RenderWindow* targetWindow)
{
    targetWindow->draw(*this->progressBarBackground);
    targetWindow->draw(*this->progressBarFill);
    targetWindow->draw(*this->loadingText);
}

void LoadingScreen::updateProgress(float progress)
{
    this->currentProgress = progress;

    // Update progress bar fill width
    float fillWidth = BAR_WIDTH * progress;
    this->progressBarFill->setSize(sf::Vector2f(fillWidth, BAR_HEIGHT));

    // Update text
    int percentage = (int)(progress * 100.0f);
    this->loadingText->setString("Loading Assets... " + std::to_string(percentage) + "%");

    // Re-center text
    sf::FloatRect textBounds = this->loadingText->getLocalBounds();
    this->loadingText->setPosition(
        (BaseRunner::WINDOW_WIDTH - textBounds.width) / 2.0f,
        BaseRunner::WINDOW_HEIGHT / 2.0f - 100.0f
    );
}