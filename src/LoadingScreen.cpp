#include "LoadingScreen.h"
#include "GameBox.h"
#include "MiniGame.h"
#include "BaseRunner.h"
#include <iostream>

#include "PingPong.h"
#include "SnakeGame.h"
#include "TicTacToeGame.h"

LoadingScreen::LoadingScreen() : AGameObject("LoadingScreen")
{
    this->currentProgress = 0.0f;

    // initial state
    this->currentGame = nullptr;
    this->selectedGameIndex = -1;
    this->currentState = SELECTING;
    this->zoomView = nullptr;
    this->zoomProgress = 0.0f;
    this->backButton = nullptr;
    this->backButtonText = nullptr;
    this->backButtonHovered = false;

    this->backgroundSprite = nullptr;
    this->backgroundTexture = nullptr;
}

LoadingScreen::~LoadingScreen()
{
    delete this->loadingText->getFont();
    delete this->loadingText;
    delete this->progressBarBackground;
    delete this->progressBarFill;

    delete this->backgroundSprite;
    delete this->backgroundTexture;


    for (GameBox* box : this->gameBoxes) {
        delete box;
    }
    this->gameBoxes.clear();

    this->cleanupCurrentGame();

    if (this->backButton != nullptr) {
        delete this->backButtonText->getFont();
        delete this->backButtonText;
        delete this->backButton;
    }

    if (this->zoomView != nullptr) {
        delete this->zoomView;
    }
}

void LoadingScreen::initialize()
{
    std::cout << "[LoadingScreen] Initializing..." << std::endl;

    // Setup BG
    this->backgroundTexture = new sf::Texture();
    if (this->backgroundTexture->loadFromFile("Media/Textures/pacmanBG.jpg")) {
        this->backgroundSprite = new sf::Sprite();
        this->backgroundSprite->setTexture(*this->backgroundTexture);

        // Scale to fit screen
        sf::Vector2u texSize = this->backgroundTexture->getSize();
        float scaleX = (float)BaseRunner::WINDOW_WIDTH / texSize.x;
        float scaleY = (float)BaseRunner::WINDOW_HEIGHT / texSize.y;
        this->backgroundSprite->setScale(scaleX, scaleY);

        std::cout << "[LoadingScreen] Background loaded successfully!" << std::endl;
    }
    else {
        std::cout << "[LoadingScreen] WARNING: Could not load background" << std::endl;
        this->backgroundSprite = nullptr; //default if no bg found
    }

    // Setup loading text
    sf::Font* font = new sf::Font();
    font->loadFromFile("Media/Sansation.ttf");

    this->loadingText = new sf::Text();
    this->loadingText->setFont(*font);
    this->loadingText->setString("Loading Assets... 0%");
    this->loadingText->setCharacterSize(36);
    this->loadingText->setFillColor(sf::Color::White);
    this->loadingText->setPosition(50.0f, 30.0f);

    // Setup progress bar background
    this->progressBarBackground = new sf::RectangleShape(sf::Vector2f(BAR_WIDTH, BAR_HEIGHT));
    this->progressBarBackground->setFillColor(sf::Color(50, 50, 50));
    this->progressBarBackground->setPosition(50.0f, 90.0f);

    // Setup progress bar fill
    this->progressBarFill = new sf::RectangleShape(sf::Vector2f(0, BAR_HEIGHT));
    this->progressBarFill->setFillColor(sf::Color(255, 255, 255));
    this->progressBarFill->setPosition(50.0f, 90.0f);

    // Create game boxes
    this->createGameBoxes();

    // Create back button
    this->createBackButton();

    // Initialize zoom view
    this->zoomView = new sf::View(sf::FloatRect(0, 0, BaseRunner::WINDOW_WIDTH, BaseRunner::WINDOW_HEIGHT));
}

void LoadingScreen::createGameBoxes()
{
    float totalWidth = (BOX_SIZE * 2) + (BOX_SPACING * 2);
    float startX = (BaseRunner::WINDOW_WIDTH - totalWidth) / 2.0f;
    float startY = BaseRunner::WINDOW_HEIGHT / 2.0f - BOX_SIZE / 2.0f + 50.0f;

    // make game boxes
    // number is not relevant thats only display
    GameBox* pongBox = new GameBox("PONG", "0", startX, startY, BOX_SIZE);
    this->gameBoxes.push_back(pongBox);

    GameBox* snakeBox = new GameBox("SNAKE", "1", startX + BOX_SIZE + BOX_SPACING, startY, BOX_SIZE);
    this->gameBoxes.push_back(snakeBox);

    std::cout << "LOADINGSCREEN CREATED " << this->gameBoxes.size() << " game boxes" << std::endl;
}

void LoadingScreen::createBackButton()
{
    this->backButton = new sf::RectangleShape(sf::Vector2f(150.0f, 50.0f));
    this->backButton->setPosition(50.0f, BaseRunner::WINDOW_HEIGHT - 100.0f);
    this->backButton->setFillColor(sf::Color(100, 50, 50));
    this->backButton->setOutlineThickness(2.0f);
    this->backButton->setOutlineColor(sf::Color::White);

    sf::Font* font = new sf::Font();
    font->loadFromFile("Media/Sansation.ttf");

    this->backButtonText = new sf::Text();
    this->backButtonText->setFont(*font);
    this->backButtonText->setString("< Back");
    this->backButtonText->setCharacterSize(24);
    this->backButtonText->setFillColor(sf::Color::White);

    sf::FloatRect textBounds = this->backButtonText->getLocalBounds();
    this->backButtonText->setPosition(
        50.0f + (150.0f - textBounds.width) / 2.0f,
        BaseRunner::WINDOW_HEIGHT - 100.0f + (50.0f - textBounds.height) / 2.0f - 5.0f
    );
}

void LoadingScreen::processInput(sf::Event event)
{
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x),
            static_cast<float>(event.mouseMove.y));
        this->handleMouseMove(mousePos);
    }
    else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x),
                static_cast<float>(event.mouseButton.y));
            this->handleMouseClick(mousePos);
        }
    }

    // Pass input to current game if playing
    if (this->currentState == PLAYING && this->currentGame != nullptr) {
        this->currentGame->processInput(event);
    }
}

void LoadingScreen::handleMouseMove(sf::Vector2f mousePos)
{
    if (this->currentState == SELECTING) {
        // Check hover on game boxes
        for (GameBox* box : this->gameBoxes) {
            box->setHovered(box->isMouseOver(mousePos));
        }
    }
    else if (this->currentState == PLAYING) {
        // Check hover on back button
        bool wasHovered = this->backButtonHovered;
        this->backButtonHovered = this->backButton->getGlobalBounds().contains(mousePos);

        if (this->backButtonHovered != wasHovered) {
            if (this->backButtonHovered) {
                this->backButton->setFillColor(sf::Color(150, 70, 70));
            }
            else {
                this->backButton->setFillColor(sf::Color(100, 50, 50));
            }
        }
    }
}

void LoadingScreen::handleMouseClick(sf::Vector2f mousePos)
{
    if (this->currentState == SELECTING) {
        // Check if any game box was clicked
        for (int i = 0; i < this->gameBoxes.size(); i++) {
            if (this->gameBoxes[i]->isMouseOver(mousePos)) {
                std::cout << "[LoadingScreen] Game selected: " << this->gameBoxes[i]->getGameName() << std::endl;
                this->startZoomToGame(i);
                break;
            }
        }
    }
    else if (this->currentState == PLAYING) {
        // Check if back button was clicked
        if (this->backButton->getGlobalBounds().contains(mousePos)) {
            std::cout << "[LoadingScreen] Back button clicked" << std::endl;
            this->startZoomOut();
        }
    }
}

void LoadingScreen::startZoomToGame(int gameIndex)
{
    this->selectedGameIndex = gameIndex;
    this->currentState = ZOOMING_IN;
    this->zoomProgress = 0.0f;

    // Get target position (center of clicked box)
    GameBox* selectedBox = this->gameBoxes[gameIndex];
    sf::Vector2f boxPos = selectedBox->getPosition();
    sf::Vector2f boxSize = selectedBox->getSize();
    this->zoomTarget = sf::Vector2f(
        boxPos.x + boxSize.x / 2.0f,
        boxPos.y + boxSize.y / 2.0f
    );

    std::cout << "[LoadingScreen] Starting zoom to: " << this->zoomTarget.x << ", " << this->zoomTarget.y << std::endl;
}

void LoadingScreen::startZoomOut()
{
    this->currentState = ZOOMING_OUT;
    this->zoomProgress = 0.0f;

    // Reset game
    if (this->currentGame != nullptr) {
        this->currentGame->reset();
    }
}

void LoadingScreen::update(sf::Time deltaTime)
{
    if (this->currentState == ZOOMING_IN || this->currentState == ZOOMING_OUT) {
        this->updateZoom(deltaTime);
    }

    // Update current game if playing
    if (this->currentState == PLAYING && this->currentGame != nullptr) {
        this->currentGame->update(deltaTime);
    }
}

void LoadingScreen::updateZoom(sf::Time deltaTime)
{
    this->zoomProgress += ZOOM_SPEED * deltaTime.asSeconds();

    if (this->zoomProgress >= 1.0f) {
        this->zoomProgress = 1.0f;

        if (this->currentState == ZOOMING_IN) {
            // Zoom in complete - start playing
            this->currentState = PLAYING;
            this->createMiniGame(this->selectedGameIndex);
            std::cout << "[LoadingScreen] Zoom in complete, now PLAYING" << std::endl;
        }
        else if (this->currentState == ZOOMING_OUT) {
            // Zoom out complete - back to selecting
            this->currentState = SELECTING;
            this->cleanupCurrentGame();
            std::cout << "[LoadingScreen] Zoom out complete, back to SELECTING" << std::endl;
        }
    }

    // Calculate zoom factor (ease in-out)
    float t = this->zoomProgress;
    float easedT = t * t * (3.0f - 2.0f * t); // Smoothstep

    if (this->currentState == ZOOMING_IN) {
        // Zoom in: scale gets smaller (zooming in effect)
        float zoomScale = 1.0f - (0.9f * easedT); // 1.0 -> 0.1
        float viewWidth = BaseRunner::WINDOW_WIDTH * zoomScale;
        float viewHeight = BaseRunner::WINDOW_HEIGHT * zoomScale;

        this->zoomView->setSize(viewWidth, viewHeight);
        this->zoomView->setCenter(
            this->zoomTarget.x * easedT + (BaseRunner::WINDOW_WIDTH / 2.0f) * (1.0f - easedT),
            this->zoomTarget.y * easedT + (BaseRunner::WINDOW_HEIGHT / 2.0f) * (1.0f - easedT)
        );
    }
    else if (this->currentState == ZOOMING_OUT) {
        // Zoom out: scale gets bigger (zooming out effect)
        float zoomScale = 0.1f + (0.9f * easedT); // 0.1 -> 1.0
        float viewWidth = BaseRunner::WINDOW_WIDTH * zoomScale;
        float viewHeight = BaseRunner::WINDOW_HEIGHT * zoomScale;

        this->zoomView->setSize(viewWidth, viewHeight);
        this->zoomView->setCenter(
            this->zoomTarget.x * (1.0f - easedT) + (BaseRunner::WINDOW_WIDTH / 2.0f) * easedT,
            this->zoomTarget.y * (1.0f - easedT) + (BaseRunner::WINDOW_HEIGHT / 2.0f) * easedT
        );
    }
}

void LoadingScreen::createMiniGame(int gameIndex)
{
    this->cleanupCurrentGame();

    switch (gameIndex) {
    case 0: // Pong
        this->currentGame = new PingPong();
        break;
    case 1: // Snake
        this->currentGame = new SnakeGame();
        break;
    case 2: // Tic-Tac-Toe
        //this->currentGame = new TicTacToeGame();
        break;
    }

    if (this->currentGame != nullptr) {
        this->currentGame->initialize();
        std::cout << "[LoadingScreen] Created mini-game: " << this->currentGame->getName() << std::endl;
    }
}

void LoadingScreen::cleanupCurrentGame()
{
    if (this->currentGame != nullptr) {
        delete this->currentGame;
        this->currentGame = nullptr;
    }
}

void LoadingScreen::draw(sf::RenderWindow* targetWindow)
{
    // zoom
    if (this->currentState == ZOOMING_IN || this->currentState == ZOOMING_OUT) {
        targetWindow->setView(*this->zoomView);
    }
    else {
        targetWindow->setView(targetWindow->getDefaultView());
    }

    // bg
    if (this->backgroundSprite != nullptr) {
        targetWindow->draw(*this->backgroundSprite);
    }
    else {
        // in case no bg img was found
        sf::RectangleShape fallback(sf::Vector2f(BaseRunner::WINDOW_WIDTH, BaseRunner::WINDOW_HEIGHT));
        fallback.setFillColor(sf::Color(20, 20, 40));
        targetWindow->draw(fallback);
    }

    // Draw based on state
    if (this->currentState == SELECTING || this->currentState == ZOOMING_IN || this->currentState == ZOOMING_OUT) {
        // Draw loading UI
        targetWindow->draw(*this->progressBarBackground);
        targetWindow->draw(*this->progressBarFill);
        targetWindow->draw(*this->loadingText);

        // Draw game boxes (fade out during zoom in)
        if (this->currentState != ZOOMING_IN || this->zoomProgress < 0.5f) {
            for (GameBox* box : this->gameBoxes) {
                box->draw(targetWindow);
            }
        }
    }

    if (this->currentState == PLAYING) {
        // Reset to default view for game
        targetWindow->setView(targetWindow->getDefaultView());

        // Draw current mini-game
        if (this->currentGame != nullptr) {
            this->currentGame->draw(targetWindow);
        }

        // Draw back button
        targetWindow->draw(*this->backButton);
        targetWindow->draw(*this->backButtonText);

        // Draw loding progress (in %)
        sf::Vector2f oldPos = this->loadingText->getPosition();
        this->loadingText->setCharacterSize(24);
        this->loadingText->setPosition(BaseRunner::WINDOW_WIDTH - 300.0f, 30.0f);
        targetWindow->draw(*this->loadingText);
        this->loadingText->setCharacterSize(36);
        this->loadingText->setPosition(oldPos);
    }
}

void LoadingScreen::updateProgress(float progress)
{
    this->currentProgress = progress;

    // Update progress bar fill width
    float fillWidth = BAR_WIDTH * progress;
    this->progressBarFill->setSize(sf::Vector2f(fillWidth, BAR_HEIGHT));

    int percentage = (int)(progress * 100.0f);
    this->loadingText->setString("Loading Assets... " + std::to_string(percentage) + "%");
}
