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

    // design stuff
    std::vector<sf::RectangleShape*> scanlines;
    sf::RectangleShape* vignette;
    sf::RectangleShape* staticNoise;
    sf::Text* titleText;
    sf::Text* instructionText;

    float scanlineAlpha;
    float staticTimer;
    float glowPulse;

    // actions & layour
    const float SCROLL_SPEED = 200.0f;
    const int SPRITES_PER_ROW = 10;
    const int SPRITE_SIZE = 80;
    const int SPRITE_PADDING = 10;

    // space and transparency
    const int SCANLINE_SPACING = 4;
    const float SCANLINE_ALPHA = 30.0f;  

    // layout spacing
    const float TOP_UI_HEIGHT = 80.0f;      
    const float BOTTOM_UI_HEIGHT = 70.0f;
    const float CONTENT_TOP_MARGIN = 90.0f;  

    void layoutSprites();
    void createScanlines();
    void createVignette();
    void createRetroUI();
};