#include "FinalScreen.h"
#include "TextureManager.h"
#include "BaseRunner.h"
#include <iostream>

FinalScreen::FinalScreen() : AGameObject("FinalScreen")
{
    this->scrollOffset = 0.0f;
    this->targetScrollOffset = 0.0f;
    this->needsScrolling = false;
}

FinalScreen::~FinalScreen()
{
    for (auto sprite : this->assetSprites) {
        delete sprite;
    }
    this->assetSprites.clear();
}

void FinalScreen::initialize()
{
    std::cout << "[FinalScreen] Initializing..." << std::endl;

    // Load and display all assets
    int numTextures = TextureManager::getInstance()->getNumLoadedStreamTextures();
    std::cout << "[GameScene] Found " << numTextures << " loaded textures" << std::endl;

    // Create sprites for each loaded texture
    for (int i = 0; i < numTextures; i++) {
        sf::Texture* texture = TextureManager::getInstance()->getStreamTextureFromList(i);

        if (texture != nullptr) {
            sf::Sprite* sprite = new sf::Sprite();
            sprite->setTexture(*texture);

            // Scale to fit SPRITE_SIZE
            sf::Vector2u textureSize = texture->getSize();
            float scaleX = (float)SPRITE_SIZE / textureSize.x;
            float scaleY = (float)SPRITE_SIZE / textureSize.y;
            float scale = std::min(scaleX, scaleY); // Maintain aspect ratio
            sprite->setScale(scale, scale);

            this->assetSprites.push_back(sprite);
        }
    }

    this->layoutSprites();
}

void FinalScreen::layoutSprites()
{
    float rowWidth = SPRITES_PER_ROW * SPRITE_SIZE + (SPRITES_PER_ROW + 1) * SPRITE_PADDING;
    float startX = (BaseRunner::WINDOW_WIDTH - rowWidth) / 2.0f;

    if (startX < 0.0f) {
        startX = 0.0f;
    }

    int row = 0;
    int col = 0;

    for (int i = 0; i < this->assetSprites.size(); i++) {
        float x = startX + col * (SPRITE_SIZE + SPRITE_PADDING) + SPRITE_PADDING;
        float y = row * (SPRITE_SIZE + SPRITE_PADDING) + SPRITE_PADDING;

        this->assetSprites[i]->setPosition(x, y);

        col++;
        if (col >= SPRITES_PER_ROW) {
            col = 0;
            row++;
        }
    }

    // Check if we need scrolling
    int totalRows = (this->assetSprites.size() + SPRITES_PER_ROW - 1) / SPRITES_PER_ROW;
    float totalHeight = totalRows * (SPRITE_SIZE + SPRITE_PADDING) + SPRITE_PADDING;

    if (totalHeight > BaseRunner::WINDOW_HEIGHT) {
        this->needsScrolling = true;
        std::cout << "[GameScene] Scrolling enabled. Total height: " << totalHeight << std::endl;
    }
    else {
        std::cout << "[GameScene] All assets fit on screen" << std::endl;
    }
}

void FinalScreen::processInput(sf::Event event)
{
    if (!this->needsScrolling)
        return;

    // Arrow keys or WASD for scrolling
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
            this->targetScrollOffset += 100.0f; // Scroll down
        }
        else if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
            this->targetScrollOffset -= 100.0f; // Scroll up
        }
    }
}

void FinalScreen::update(sf::Time deltaTime)
{
    if (!this->needsScrolling)
        return;

    // Smooth scrolling with lerp
    float diff = this->targetScrollOffset - this->scrollOffset;
    this->scrollOffset += diff * 5.0f * deltaTime.asSeconds();

    // Clamp scroll offset
    int totalRows = (this->assetSprites.size() + SPRITES_PER_ROW - 1) / SPRITES_PER_ROW;
    float totalHeight = totalRows * (SPRITE_SIZE + SPRITE_PADDING) + SPRITE_PADDING;
    float maxScroll = totalHeight - BaseRunner::WINDOW_HEIGHT;

    if (this->targetScrollOffset < 0)
        this->targetScrollOffset = 0;
    if (this->targetScrollOffset > maxScroll)
        this->targetScrollOffset = maxScroll;

    // This one auto scrolls
    /*
    this->targetScrollOffset += SCROLL_SPEED * deltaTime.asSeconds();
    if (this->targetScrollOffset > maxScroll) {
        this->targetScrollOffset = 0; // Loop back to top
    }
    */
}

void FinalScreen::draw(sf::RenderWindow* targetWindow)
{
    for (sf::Sprite* sprite : this->assetSprites) {
        sf::Vector2f originalPos = sprite->getPosition();
        sprite->setPosition(originalPos.x, originalPos.y - this->scrollOffset);
        targetWindow->draw(*sprite);
        sprite->setPosition(originalPos); // Reset position
    }
}