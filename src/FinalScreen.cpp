#include "FinalScreen.h"
#include "TextureManager.h"
#include "BaseRunner.h"
#include <iostream>
#include <cstdlib>
#include <cmath>

FinalScreen::FinalScreen() : AGameObject("FinalScreen")
{
    this->scanlineAlpha = SCANLINE_ALPHA;
    this->staticTimer = 0.0f;
    this->glowPulse = 0.0f;
}

FinalScreen::~FinalScreen()
{
    for (auto sprite : this->assetSprites) {
        delete sprite;
    }
    this->assetSprites.clear();

    for (sf::RectangleShape* scanline : this->scanlines) {
        delete scanline;
    }
    this->scanlines.clear();

    delete this->vignette;
    delete this->staticNoise;
    delete this->titleText->getFont();
    delete this->titleText;
    delete this->instructionText;
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
            float scale = std::min(scaleX, scaleY);
            sprite->setScale(scale, scale);

            this->assetSprites.push_back(sprite);
        }
    }

    // spritelayout
    this->layoutSprites();

    // fx
    this->createScanlines();
    this->createVignette();
    this->createRetroUI();
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
        float y = row * (SPRITE_SIZE + SPRITE_PADDING) + SPRITE_PADDING + CONTENT_TOP_MARGIN;

        this->assetSprites[i]->setPosition(x, y);

        col++;
        if (col >= SPRITES_PER_ROW) {
            col = 0;
            row++;
        }
    }
}

void FinalScreen::createScanlines()
{
    // glitchy looking stuff
    for (int y = 0; y < BaseRunner::WINDOW_HEIGHT; y += SCANLINE_SPACING) {
        sf::RectangleShape* scanline = new sf::RectangleShape(
            sf::Vector2f(BaseRunner::WINDOW_WIDTH, 1.0f)
        );
        scanline->setPosition(0, y);
        scanline->setFillColor(sf::Color(0, 0, 0, (int)SCANLINE_ALPHA));
        this->scanlines.push_back(scanline);
    }
}

void FinalScreen::createVignette()
{
    this->vignette = new sf::RectangleShape(
        sf::Vector2f(BaseRunner::WINDOW_WIDTH, BaseRunner::WINDOW_HEIGHT)
    );
    this->vignette->setPosition(0, 0);
    this->vignette->setFillColor(sf::Color(0, 0, 0, 0));

    // noise effect
    this->staticNoise = new sf::RectangleShape(
        sf::Vector2f(BaseRunner::WINDOW_WIDTH, BaseRunner::WINDOW_HEIGHT)
    );
    this->staticNoise->setPosition(0, 0);
    this->staticNoise->setFillColor(sf::Color(255, 255, 255, 5)); 
}

void FinalScreen::createRetroUI()
{
    // Load retro font
    sf::Font* font = new sf::Font();
    font->loadFromFile("Media/Sansation.ttf");

    // Title text
    this->titleText = new sf::Text();
    this->titleText->setFont(*font);
    this->titleText->setString("=== ASSET GALLERY ===");
    this->titleText->setCharacterSize(42);
    this->titleText->setFillColor(sf::Color(0, 255, 0));
    this->titleText->setOutlineColor(sf::Color(0, 100, 0));
    this->titleText->setOutlineThickness(2.0f);

    sf::FloatRect titleBounds = this->titleText->getLocalBounds();
    this->titleText->setPosition(
        (BaseRunner::WINDOW_WIDTH - titleBounds.width) / 2.0f,
        20.0f
    );

    // Instruction text sa bottom bar
    this->instructionText = new sf::Text();
    this->instructionText->setFont(*font);

    this->instructionText->setString("[ ALL ASSETS LOADED ]");
    this->instructionText->setCharacterSize(20);
    this->instructionText->setFillColor(sf::Color(100, 255, 100));

    sf::FloatRect instrBounds = this->instructionText->getLocalBounds();
    this->instructionText->setPosition(
        (BaseRunner::WINDOW_WIDTH - instrBounds.width) / 2.0f,
        BaseRunner::WINDOW_HEIGHT - 60.0f
    );
}

void FinalScreen::processInput(sf::Event event)
{
   // when inimplement scrolling
}

void FinalScreen::update(sf::Time deltaTime)
{
    // Title text glow pulse
    int greenValue = 200 + (int)(std::sin(this->glowPulse * 2.0f) * 55.0f);
    this->titleText->setFillColor(sf::Color(0, greenValue, 0));
}

void FinalScreen::draw(sf::RenderWindow* targetWindow)
{
    for (sf::Sprite* sprite : this->assetSprites) {
        sprite->setColor(sf::Color(255, 255, 255, 255));
        targetWindow->draw(*sprite);
    }

    // Displays borders top & bttm
    sf::RectangleShape borderTop(sf::Vector2f(BaseRunner::WINDOW_WIDTH, 2));
    borderTop.setPosition(0, TOP_UI_HEIGHT - 5);
    borderTop.setFillColor(sf::Color(0, 255, 0));
    targetWindow->draw(borderTop);

    sf::RectangleShape borderBottom(sf::Vector2f(BaseRunner::WINDOW_WIDTH, 2));
    borderBottom.setPosition(0, BaseRunner::WINDOW_HEIGHT - BOTTOM_UI_HEIGHT);
    borderBottom.setFillColor(sf::Color(0, 255, 0));
    targetWindow->draw(borderBottom);

    // displays text
    targetWindow->draw(*this->titleText);
    targetWindow->draw(*this->instructionText);
}