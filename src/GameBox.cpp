#include "GameBox.h"
#include <iostream>

GameBox::GameBox(const std::string& gameName, const std::string& emoji,
    float x, float y, float size)
    : gameName(gameName), isHovered(false)
{
    // Create box
    this->box = new sf::RectangleShape(sf::Vector2f(size, size));
    this->box->setPosition(x, y);
    this->box->setFillColor(NORMAL_COLOR);
    this->box->setOutlineThickness(3.0f);
    this->box->setOutlineColor(sf::Color::White);

    // Load font
    sf::Font* font = new sf::Font();
    font->loadFromFile("Media/Sansation.ttf");

    // Create emoji text
    this->emojiText = new sf::Text();
    this->emojiText->setFont(*font);
    this->emojiText->setString(emoji);
    this->emojiText->setCharacterSize(60);
    this->emojiText->setFillColor(sf::Color::White);

    // Center emoji
    sf::FloatRect emojiBounds = this->emojiText->getLocalBounds();
    this->emojiText->setPosition(
        x + (size - emojiBounds.width) / 2.0f,
        y + size / 3.0f
    );

    // Create name text
    this->nameText = new sf::Text();
    this->nameText->setFont(*font);
    this->nameText->setString(gameName);
    this->nameText->setCharacterSize(24);
    this->nameText->setFillColor(sf::Color::White);

    // Center name
    sf::FloatRect nameBounds = this->nameText->getLocalBounds();
    this->nameText->setPosition(
        x + (size - nameBounds.width) / 2.0f,
        y + size * 0.7f
    );
}

GameBox::~GameBox()
{
    delete this->nameText->getFont();
    delete this->nameText;
    delete this->emojiText;
    delete this->box;
}

void GameBox::draw(sf::RenderWindow* targetWindow)
{
    targetWindow->draw(*this->box);
    targetWindow->draw(*this->emojiText);
    targetWindow->draw(*this->nameText);
}

bool GameBox::isMouseOver(sf::Vector2f mousePos) const
{
    return this->box->getGlobalBounds().contains(mousePos);
}

void GameBox::setHovered(bool hovered)
{
    this->isHovered = hovered;

    if (hovered) {
        this->box->setFillColor(HOVER_COLOR);
        this->box->setOutlineColor(sf::Color::Yellow);
    }
    else {
        this->box->setFillColor(NORMAL_COLOR);
        this->box->setOutlineColor(sf::Color::White);
    }
}