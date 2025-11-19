#include "FinalScreen.h"
#include "TextureManager.h"
#include "BaseRunner.h"
#include <iostream>
#include <cstdlib>
#include <cmath>

FinalScreen::FinalScreen() : AGameObject("FinalScreen")
{
    this->scrollOffset = 0.0f;
    this->targetScrollOffset = 0.0f;
    this->needsScrolling = false;
    this->scanlineAlpha = SCANLINE_ALPHA;
    this->staticTimer = 0.0f;
    this->glowPulse = 0.0f;

    this->totalContentHeight = 0.0f;
    this->availableHeight = 0.0f;
    this->windowPtr = nullptr;
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

    int totalRows = (this->assetSprites.size() + SPRITES_PER_ROW - 1) / SPRITES_PER_ROW;
    float totalHeight = totalRows * (SPRITE_SIZE + SPRITE_PADDING) + SPRITE_PADDING;

    float availableHeight = BaseRunner::WINDOW_HEIGHT - TOP_UI_HEIGHT - BOTTOM_UI_HEIGHT;

    if (totalHeight > availableHeight) {
        this->needsScrolling = true;
        std::cout << "[GameScene] Scrolling enabled. Total height: " << totalHeight << std::endl;
    }
    else {
        std::cout << "[GameScene] All assets fit on screen" << std::endl;
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

    if (this->needsScrolling) {
        this->instructionText->setString("[ USE W/S OR UP/DOWN TO SCROLL ]");
    }
    else {
        this->instructionText->setString("[ ALL ASSETS LOADED ]");
    }

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
    if (!this->needsScrolling)
        return;

    // Arrow keys or WASD for scrolling
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
            this->targetScrollOffset += 100.0f; 
        }
        else if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
            this->targetScrollOffset -= 100.0f;
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

    float availableHeight = BaseRunner::WINDOW_HEIGHT - TOP_UI_HEIGHT - BOTTOM_UI_HEIGHT;
    float maxScroll = totalHeight - availableHeight;

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

    //// Animate scanline intensity (subtle pulse)
    //this->glowPulse += deltaTime.asSeconds() * 2.0f;
    //this->scanlineAlpha = SCANLINE_ALPHA + (std::sin(this->glowPulse) * 5.0f);

    //for (sf::RectangleShape* scanline : this->scanlines) {
    //    scanline->setFillColor(sf::Color(0, 0, 0, (int)this->scanlineAlpha));
    //}

    //// Static noise flicker
    //this->staticTimer += deltaTime.asSeconds();
    //if (this->staticTimer >= 0.1f) {
    //    this->staticTimer = 0.0f;
    //    int alpha = 3 + (std::rand() % 5);
    //    this->staticNoise->setFillColor(sf::Color(255, 255, 255, alpha));
    //}

    // Title text glow pulse
    int greenValue = 200 + (int)(std::sin(this->glowPulse * 2.0f) * 55.0f);
    this->titleText->setFillColor(sf::Color(0, greenValue, 0));
}

void FinalScreen::draw(sf::RenderWindow* targetWindow)
{
    for (sf::Sprite* sprite : this->assetSprites) {
        sf::Vector2f originalPos = sprite->getPosition();
        float scrolledY = originalPos.y - this->scrollOffset;

        sprite->setPosition(originalPos.x, scrolledY);

        sprite->setColor(sf::Color(255, 255, 255, 255));

        float spriteBottom = scrolledY + SPRITE_SIZE;
        float contentBottom = BaseRunner::WINDOW_HEIGHT - BOTTOM_UI_HEIGHT - 10;

        if (scrolledY < contentBottom && spriteBottom > TOP_UI_HEIGHT) {
            targetWindow->draw(*sprite);
        }

        sprite->setPosition(originalPos);
    }

    //for (sf::RectangleShape* scanline : this->scanlines) {
    //    targetWindow->draw(*scanline);
    //}

    // Draw subtle static noise
    //targetWindow->draw(*this->staticNoise);

    //// Draw vignette edges (darker corners)
    //// Top edge
    //sf::RectangleShape topVignette(sf::Vector2f(BaseRunner::WINDOW_WIDTH, 80));
    //topVignette.setPosition(0, 0);
    //topVignette.setFillColor(sf::Color(0, 0, 0, 120));
    //targetWindow->draw(topVignette);

    //// Bottom edge
    //sf::RectangleShape bottomVignette(sf::Vector2f(BaseRunner::WINDOW_WIDTH, 80));
    //bottomVignette.setPosition(0, BaseRunner::WINDOW_HEIGHT - 80);
    //bottomVignette.setFillColor(sf::Color(0, 0, 0, 120));
    //targetWindow->draw(bottomVignette);

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