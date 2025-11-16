// src/main.cpp
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <thread>
#include <algorithm>
#include <string>
#include <iostream>

#include "PingPong.hpp"
#include "AssetLoader.hpp"

int main() {
    const unsigned int windowWidth  = 1280;
    const unsigned int windowHeight = 720;

    sf::RenderWindow window(
        sf::VideoMode(windowWidth, windowHeight),
        "Interactive Loading - PingPong"
    );
    window.setFramerateLimit(60);

    // --- Interactive mini-game during loading ---
    PingPong game(windowWidth, windowHeight);

    // --- Font (Oswald) ---
    sf::Font font;
    if (!font.loadFromFile("assets/ui/Oswald-VariableFont_wght.ttf")) {
        std::cerr << "Failed to load font: assets/ui/Oswald-VariableFont_wght.ttf\n";
        // We continue anyway to avoid crashing; text just won't show correctly.
    }

    // --- FPS counter (updated every 0.5s) ---
    sf::Clock fpsClock;
    int frameCount = 0;
    float currentFps = 0.f;

    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(18);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setOutlineColor(sf::Color::Black);
    fpsText.setOutlineThickness(2.f);
    fpsText.setLetterSpacing(1.1f);
    fpsText.setPosition(10.f, 10.f);
    fpsText.setString("FPS: 0");

    // --- Loading label ---
    sf::Text loadingText;
    loadingText.setFont(font);
    loadingText.setCharacterSize(22);
    loadingText.setFillColor(sf::Color::White);
    loadingText.setOutlineColor(sf::Color::Black);
    loadingText.setOutlineThickness(2.f);
    loadingText.setLetterSpacing(1.1f);
    loadingText.setString("Loading assets...");
    loadingText.setPosition(windowWidth * 0.2f, windowHeight - 100.f);

    // --- Progress bar ---
    sf::RectangleShape progressBg(sf::Vector2f(windowWidth * 0.6f, 20.f));
    progressBg.setFillColor(sf::Color(60, 60, 60));
    progressBg.setPosition(windowWidth * 0.2f, windowHeight - 60.f);

    sf::RectangleShape progressFill(sf::Vector2f(0.f, 20.f));
    progressFill.setFillColor(sf::Color(0, 200, 80));
    progressFill.setPosition(progressBg.getPosition());

    // --- Asset loader state ---
    LoadedAssets loaded;

    // Dynamically discover all textures and audio files under assets/
    // (CMake copies assets/ into build/, so these are relative to the build dir)
    std::vector<AssetInfo> assetsToLoad =
        discoverAssets("assets/texture", "assets/audio");

    // Fixed delay per asset to make the effect of multithreading visible
    int delayPerAssetMs = 10000;  // mention this in your PPT

    // --- Start loader thread ---
    std::thread loaderThread(
        loaderThreadFunc,
        &loaded,
        assetsToLoad,
        delayPerAssetMs
    );

    // --- Simple app state machine ---
    enum class AppState { Loading, Gallery };
    AppState state = AppState::Loading;

    sf::Clock dtClock;

    while (window.isOpen()) {
        // Events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float dt = dtClock.restart().asSeconds();

        // --- FPS update every 0.5s ---
        frameCount++;
        float elapsed = fpsClock.getElapsedTime().asSeconds();
        if (elapsed >= 0.5f) {
            currentFps = frameCount / elapsed;
            frameCount = 0;
            fpsClock.restart();
            fpsText.setString("FPS: " + std::to_string(static_cast<int>(currentFps)));
        }

        if (state == AppState::Loading) {
            // Run your PingPong mini-game while assets load
            game.update(dt);

            // Compute loading progress
            float progress = 0.f;
            if (loaded.totalCount > 0) {
                progress = static_cast<float>(loaded.loadedCount.load()) /
                           static_cast<float>(loaded.totalCount);
            }
            progress = std::clamp(progress, 0.f, 1.f);

            progressFill.setSize(
                sf::Vector2f(progressBg.getSize().x * progress,
                             progressBg.getSize().y)
            );

            // Optional: scale difficulty with progress (uncomment if you like)
            // game.updateSpeed(300.f + 400.f * progress, 400.f + 200.f * progress);

            if (loaded.done.load()) {
                state = AppState::Gallery;
            }
        }

        // --- Render ---
        window.clear(sf::Color(10, 10, 20));

        if (state == AppState::Loading) {
            // Interactive loading screen = mini-game + UI
            game.render(window);
            window.draw(progressBg);
            window.draw(progressFill);
            window.draw(loadingText);
        } else if (state == AppState::Gallery) {
            // Display loaded textures in a simple grid
            float x = 20.f;
            float y = 60.f;
            float margin = 20.f;
            float maxRowHeight = 0.f;

            {
                std::lock_guard<std::mutex> lock(loaded.mtx);
                for (auto& texPtr : loaded.textures) {
                    sf::Sprite sprite;
                    sprite.setTexture(*texPtr);

                    // Scale down large textures for display
                    float scale = 0.3f;
                    sprite.setScale(scale, scale);

                    sf::FloatRect bounds = sprite.getGlobalBounds();
                    if (x + bounds.width + margin > windowWidth) {
                        x = 20.f;
                        y += maxRowHeight + margin;
                        maxRowHeight = 0.f;
                    }

                    sprite.setPosition(x, y);
                    window.draw(sprite);

                    x += bounds.width + margin;
                    if (bounds.height > maxRowHeight) {
                        maxRowHeight = bounds.height;
                    }
                }
            }

            // "All assets loaded" label
            sf::Text doneText;
            doneText.setFont(font);
            doneText.setCharacterSize(28);
            doneText.setFillColor(sf::Color::White);
            doneText.setOutlineColor(sf::Color::Black);
            doneText.setOutlineThickness(2.f);
            doneText.setLetterSpacing(1.1f);
            doneText.setString("All assets loaded.");
            doneText.setPosition(20.f, 10.f);
            window.draw(doneText);
        }

        // Draw FPS on top in all states
        window.draw(fpsText);

        window.display();
    }

    if (loaderThread.joinable())
        loaderThread.join();

    return 0;
}