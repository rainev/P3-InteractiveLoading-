#pragma once
#include <SFML/Graphics.hpp>

class MiniGame
{
public:
    virtual ~MiniGame() {}

    virtual void initialize() = 0;
    virtual void processInput(sf::Event event) = 0;
    virtual void update(sf::Time deltaTime) = 0;
    virtual void draw(sf::RenderWindow* targetWindow) = 0;
    virtual void reset() = 0;

    virtual const char* getName() const = 0;
};