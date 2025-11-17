#pragma once
#include "AGameObject.h"
#include <vector>

class FinalScreen : public AGameObject
{
public:
    FinalScreen();
    ~FinalScreen();

    void initialize() override;
    void processInput(sf::Event event) override;
    void update(sf::Time deltaTime) override;
    void draw(sf::RenderWindow* targetWindow) override;

private:
    std::vector<sf::Sprite*> assetSprites;

    float scrollOffset;
    float targetScrollOffset;
    bool needsScrolling;

    const float SCROLL_SPEED = 200.0f;
    const int SPRITES_PER_ROW = 10;
    const int SPRITE_SIZE = 30;
    const int SPRITE_PADDING = 10;

    void layoutSprites();
};