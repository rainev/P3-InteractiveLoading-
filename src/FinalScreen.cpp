#include "FinalScreen.h"
#include "TextureManager.h"
#include "BaseRunner.h"
#include <iostream>

FinalScreen::FinalScreen() : AGameObject("FinalScreen")
{
    this->titleText = nullptr;
}

FinalScreen::~FinalScreen()
{
    for (auto sprite : this->assetSprites) {
        delete sprite;
    }
    this->assetSprites.clear();

    if (this->titleText != nullptr) {
        delete this->titleText->getFont();
        delete this->titleText;
    }
}

void FinalScreen::initialize()
{
    std::cout << "[FinalScreen] Initializing..." << std::endl;

    // Title
    sf::Font* font = new sf::Font();
    font->loadFromFile("Media/Sansation.ttf");

    this->titleText = new sf::Text();
    this->titleText->setFont(*font);
    this->titleText->setString("Loaded Assets - Game Scene");
    this->titleText->setCharacterSize(36);
    this->titleText->setFillColor(sf::Color::White);
    this->titleText->setPosition(20.0f, 20.0f);

    // Load and display all assets
    this->loadAllAssets();
    this->arrangeSpritesInGrid();

    std::cout << "[FinalScreen] Loaded " << this->assetSprites.size() << " assets for display" << std::endl;
}

void FinalScreen::loadAllAssets()
{
    // Get total number of loaded streaming textures
    int totalTextures = TextureManager::getInstance()->getNumLoadedStreamTextures();

    if (totalTextures == 0) {
        std::cout << "[FinalScreen] No textures found!" << std::endl;
        return;
    }

    std::cout << "[FinalScreen] Loading " << totalTextures << " textures..." << std::endl;

    // Create sprites for each texture
    for (int i = 0; i < totalTextures; i++) {
        sf::Texture* texture = TextureManager::getInstance()->getStreamTextureFromList(i);

        if (texture != nullptr) {
            sf::Sprite* sprite = new sf::Sprite();
            sprite->setTexture(*texture);

            // Scale sprite to fit within SPRITE_SIZE
            sf::Vector2u textureSize = texture->getSize();
            float scaleX = SPRITE_SIZE / textureSize.x;
            float scaleY = SPRITE_SIZE / textureSize.y;
            float scale = std::min(scaleX, scaleY);  // Use smaller scale to maintain aspect ratio

            sprite->setScale(scale, scale);

            this->assetSprites.push_back(sprite);
        }
    }
}

void FinalScreen::arrangeSpritesInGrid()
{
    const float START_X = 50.0f;
    const float START_Y = 100.0f;  // Below the title

    int currentRow = 0;
    int currentCol = 0;

    for (sf::Sprite* sprite : this->assetSprites) {
        // Calculate position
        float x = START_X + (currentCol * (SPRITE_SIZE + MARGIN));
        float y = START_Y + (currentRow * (SPRITE_SIZE + MARGIN));

        sprite->setPosition(x, y);

        // Move to next column
        currentCol++;

        // Wrap to next row if needed
        if (currentCol >= SPRITES_PER_ROW) {
            currentCol = 0;
            currentRow++;
        }
    }
}

void FinalScreen::processInput(sf::Event event)
{
    // Nothing for now
}

void FinalScreen::update(sf::Time deltaTime)
{
    // Nothing for now - static display
}

void FinalScreen::draw(sf::RenderWindow* targetWindow)
{
    // Draw title
    if (this->titleText != nullptr) {
        targetWindow->draw(*this->titleText);
    }

    // Draw all sprites
    for (sf::Sprite* sprite : this->assetSprites) {
        targetWindow->draw(*sprite);
    }
}