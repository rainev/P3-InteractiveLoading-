#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class GameBox
{
public:
    GameBox(const std::string& gameName, const std::string& emoji,
        float x, float y, float size);
    ~GameBox();

    void draw(sf::RenderWindow* targetWindow);
    bool isMouseOver(sf::Vector2f mousePos) const;
    void setHovered(bool hovered);

    std::string getGameName() const { return this->gameName; }
    sf::Vector2f getPosition() const { return this->box->getPosition(); }
    sf::Vector2f getSize() const { return this->box->getSize(); }

private:
    std::string gameName;
    sf::RectangleShape* box;
    sf::Text* nameText;
    sf::Text* emojiText;

    bool isHovered;

    const sf::Color NORMAL_COLOR = sf::Color(70, 70, 90);
    const sf::Color HOVER_COLOR = sf::Color(100, 100, 150);
};