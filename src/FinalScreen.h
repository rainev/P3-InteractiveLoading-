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
    sf::Text* titleText;

    void loadAllAssets();
    void arrangeSpritesInGrid();

    // size margin layout (in order)
    const float SPRITE_SIZE = 100.0f;  
    const float MARGIN = 10.0f;        
    const int SPRITES_PER_ROW = 16;    
};