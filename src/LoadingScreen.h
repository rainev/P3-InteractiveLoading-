#pragma once
#include "AGameObject.h"
#include <vector>

class GameBox;
class MiniGame;

class LoadingScreen : public AGameObject
{
public:
    LoadingScreen();
    ~LoadingScreen();

    void initialize() override;
    void processInput(sf::Event event) override;
    void update(sf::Time deltaTime) override;
    void draw(sf::RenderWindow* targetWindow) override;

    void updateProgress(float progress); 

private:
    sf::Text* loadingText;
    sf::RectangleShape* progressBarBackground;
    sf::RectangleShape* progressBarFill;
    float currentProgress;

    // Game selection
    std::vector<GameBox*> gameBoxes;
    MiniGame* currentGame;
    int selectedGameIndex;

    // Different states of the gamebox
    enum LoadingState {
        SELECTING,      // Showing game boxes
        ZOOMING_IN,     // zoom in a game
        PLAYING,        // Playing 
        ZOOMING_OUT     // zoom back to menu
    };
    LoadingState currentState;

    // Zoom transition
    sf::View* zoomView;
    float zoomProgress;
    sf::Vector2f zoomTarget;
    const float ZOOM_SPEED = 3.0f;

    // Back button
    sf::RectangleShape* backButton;
    sf::Text* backButtonText;
    bool backButtonHovered;

    // Helper methods
    void createGameBoxes();
    void createBackButton();
    void handleMouseMove(sf::Vector2f mousePos);
    void handleMouseClick(sf::Vector2f mousePos);
    void startZoomToGame(int gameIndex);
    void startZoomOut();
    void updateZoom(sf::Time deltaTime);
    void createMiniGame(int gameIndex);
    void cleanupCurrentGame();

    const float BAR_WIDTH = 600.0f;
    const float BAR_HEIGHT = 40.0f;
    const float BOX_SIZE = 200.0f;
    const float BOX_SPACING = 50.0f;
};