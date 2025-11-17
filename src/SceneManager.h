#pragma once
#include <string>

class SceneManager
{
public:
    typedef std::string String;

    enum SceneType {
        LOADING_SCENE,
        GAME_SCENE
    };

    static SceneManager* getInstance();

    void setCurrentScene(SceneType scene);
    SceneType getCurrentScene() const;
    bool isLoadingComplete() const;
    void setLoadingComplete(bool complete);

private:
    SceneManager();
    SceneManager(SceneManager const&) {};
    SceneManager& operator=(SceneManager const&) {};
    static SceneManager* sharedInstance;

    SceneType currentScene;
    bool loadingComplete;
};