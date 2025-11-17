#include "SnakeGame.h"
#include "BaseRunner.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

SnakeGame::SnakeGame()
{
    this->gameState = READY;
    this->score = 0;
    this->moveTimer = 0.0f;
    this->cellShape = nullptr;
    this->foodShape = nullptr;
    this->gridBackground = nullptr;
    this->scoreText = nullptr;
    this->instructionText = nullptr;
    this->gameOverText = nullptr;

    // Seed random for food generation
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

SnakeGame::~SnakeGame()
{
    delete this->cellShape;
    delete this->foodShape;
    delete this->gridBackground;

    if (this->scoreText != nullptr) {
        delete this->scoreText->getFont();
        delete this->scoreText;
    }
    if (this->instructionText != nullptr) {
        delete this->instructionText;
    }
    if (this->gameOverText != nullptr) {
        delete this->gameOverText;
    }
}

void SnakeGame::initialize()
{
    std::cout << "[SnakeGame] Initializing..." << std::endl;

    // Initialize snake in the center
    Position start = { GRID_SIZE / 2, GRID_SIZE / 2 };
    this->snake.clear();
    this->snake.push_back(start);
    this->snake.push_back({ start.x - 1, start.y });
    this->snake.push_back({ start.x - 2, start.y });

    // Initial direction (moving right)
    this->direction = { 1, 0 };
    this->nextDirection = { 1, 0 };

    // Spawn first food
    this->spawnFood();

    // Create shapes
    this->cellShape = new sf::RectangleShape(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
    this->cellShape->setFillColor(sf::Color::Green);

    this->foodShape = new sf::RectangleShape(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
    this->foodShape->setFillColor(sf::Color::Red);

    // Grid background
    this->gridBackground = new sf::RectangleShape(
        sf::Vector2f(GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE)
    );
    this->gridBackground->setPosition(GRID_OFFSET_X, GRID_OFFSET_Y);
    this->gridBackground->setFillColor(sf::Color(30, 30, 30));
    this->gridBackground->setOutlineThickness(3.0f);
    this->gridBackground->setOutlineColor(sf::Color::White);

    // Setup text
    this->setupText();

    this->gameState = READY;
    this->score = 0;
    this->moveTimer = 0.0f;

    std::cout << "[SnakeGame] Initialization complete!" << std::endl;
}

void SnakeGame::setupText()
{
    sf::Font* font = new sf::Font();
    font->loadFromFile("Media/Sansation.ttf");

    // Score text
    this->scoreText = new sf::Text();
    this->scoreText->setFont(*font);
    this->scoreText->setCharacterSize(32);
    this->scoreText->setFillColor(sf::Color::White);
    this->scoreText->setPosition(GRID_OFFSET_X, GRID_OFFSET_Y - 60.0f);
    this->scoreText->setString("Score: 0");

    // Instruction text
    this->instructionText = new sf::Text();
    this->instructionText->setFont(*font);
    this->instructionText->setCharacterSize(24);
    this->instructionText->setFillColor(sf::Color(200, 200, 200));
    this->instructionText->setPosition(
        GRID_OFFSET_X,
        GRID_OFFSET_Y + GRID_SIZE * CELL_SIZE + 20.0f
    );
    this->instructionText->setString("Arrow Keys to Move | Press Any Arrow Key to Start");

    // Game over text
    this->gameOverText = new sf::Text();
    this->gameOverText->setFont(*font);
    this->gameOverText->setCharacterSize(48);
    this->gameOverText->setFillColor(sf::Color::Red);
    this->gameOverText->setString("GAME OVER!");

    sf::FloatRect bounds = this->gameOverText->getLocalBounds();
    this->gameOverText->setPosition(
        GRID_OFFSET_X + (GRID_SIZE * CELL_SIZE - bounds.width) / 2.0f,
        GRID_OFFSET_Y + (GRID_SIZE * CELL_SIZE - bounds.height) / 2.0f - 50.0f
    );
}

void SnakeGame::spawnFood()
{
    Position newFood;
    bool validPosition = false;

    // Keep trying until we find a position not occupied by snake
    while (!validPosition) {
        newFood.x = std::rand() % GRID_SIZE;
        newFood.y = std::rand() % GRID_SIZE;

        validPosition = !this->isFoodOnSnake(newFood);
    }

    this->food = newFood;
    std::cout << "[SnakeGame] Food spawned at: " << food.x << ", " << food.y << std::endl;
}

bool SnakeGame::isFoodOnSnake(const Position& pos)
{
    for (const Position& segment : this->snake) {
        if (segment == pos) {
            return true;
        }
    }
    return false;
}

void SnakeGame::processInput(sf::Event event)
{
    if (event.type == sf::Event::KeyPressed) {

        // Start game if in READY state
        if (this->gameState == READY) {
            if (event.key.code == sf::Keyboard::Up ||
                event.key.code == sf::Keyboard::Down ||
                event.key.code == sf::Keyboard::Left ||
                event.key.code == sf::Keyboard::Right) {
                this->gameState = PLAYING;
                std::cout << "[SnakeGame] Game started!" << std::endl;
            }
        }

        // Restart game if game over
        if (this->gameState == GAME_OVER) {
            if (event.key.code == sf::Keyboard::Space) {
                this->reset();
                this->gameState = PLAYING;
            }
        }

        // Handle direction changes (only if playing)
        if (this->gameState == PLAYING) {
            if (event.key.code == sf::Keyboard::Up && this->direction.y == 0) {
                this->nextDirection = { 0, -1 };
            }
            else if (event.key.code == sf::Keyboard::Down && this->direction.y == 0) {
                this->nextDirection = { 0, 1 };
            }
            else if (event.key.code == sf::Keyboard::Left && this->direction.x == 0) {
                this->nextDirection = { -1, 0 };
            }
            else if (event.key.code == sf::Keyboard::Right && this->direction.x == 0) {
                this->nextDirection = { 1, 0 };
            }
        }
    }
}

void SnakeGame::update(sf::Time deltaTime)
{
    if (this->gameState != PLAYING) {
        return;
    }

    // Update move timer
    this->moveTimer += deltaTime.asSeconds();

    if (this->moveTimer >= MOVE_DELAY) {
        this->moveTimer = 0.0f;

        // Update direction
        this->direction = this->nextDirection;

        // Calculate new head position
        Position newHead = this->snake.front();
        newHead.x += this->direction.x;
        newHead.y += this->direction.y;

        // Check collision with walls
        if (newHead.x < 0 || newHead.x >= GRID_SIZE ||
            newHead.y < 0 || newHead.y >= GRID_SIZE) {
            this->handleGameOver();
            return;
        }

        // Check collision with self
        for (const Position& segment : this->snake) {
            if (segment == newHead) {
                this->handleGameOver();
                return;
            }
        }

        // Add new head
        this->snake.push_front(newHead);

        // Check if food eaten
        if (newHead == this->food) {
            this->score++;
            this->scoreText->setString("Score: " + std::to_string(this->score));
            this->spawnFood();
            std::cout << "[SnakeGame] Food eaten! Score: " << this->score << std::endl;
        }
        else {
            // Remove tail (snake doesn't grow)
            this->snake.pop_back();
        }
    }
}

void SnakeGame::handleGameOver()
{
    this->gameState = GAME_OVER;
    std::cout << "[SnakeGame] Game Over! Final Score: " << this->score << std::endl;

    this->instructionText->setString("Press SPACE to Restart");
}

void SnakeGame::draw(sf::RenderWindow* targetWindow)
{
    // Draw grid background
    targetWindow->draw(*this->gridBackground);

    // Draw snake
    for (const Position& segment : this->snake) {
        this->cellShape->setPosition(
            GRID_OFFSET_X + segment.x * CELL_SIZE + 1,
            GRID_OFFSET_Y + segment.y * CELL_SIZE + 1
        );

        // Make head brighter
        if (segment == this->snake.front()) {
            this->cellShape->setFillColor(sf::Color(100, 255, 100));
        }
        else {
            this->cellShape->setFillColor(sf::Color::Green);
        }

        targetWindow->draw(*this->cellShape);
    }

    // Draw food
    this->foodShape->setPosition(
        GRID_OFFSET_X + this->food.x * CELL_SIZE + 1,
        GRID_OFFSET_Y + this->food.y * CELL_SIZE + 1
    );
    targetWindow->draw(*this->foodShape);

    // Draw UI
    targetWindow->draw(*this->scoreText);
    targetWindow->draw(*this->instructionText);

    // Draw game over text
    if (this->gameState == GAME_OVER) {
        targetWindow->draw(*this->gameOverText);
    }
}

void SnakeGame::reset()
{
    std::cout << "[SnakeGame] Resetting..." << std::endl;
    this->initialize();
}