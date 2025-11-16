#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <atomic>
#include <mutex>
#include <memory>
#include <string>
#include <vector>

enum class AssetType {
    Texture,
    Sound
};

struct AssetInfo {
    std::string path;
    AssetType type;
};

struct LoadedAssets {
    std::vector<std::shared_ptr<sf::Texture>> textures;
    std::vector<std::shared_ptr<sf::SoundBuffer>> sounds;

    std::mutex mtx;
    std::atomic<size_t> loadedCount{0};
    size_t totalCount = 0;
    std::atomic<bool> done{false};
};

void loaderThreadFunc(
    LoadedAssets* data,
    std::vector<AssetInfo> assets,
    int delayPerAssetMs
);

std::vector<AssetInfo> discoverAssets(
    const std::string& texturesRoot,
    const std::string& audioRoot
);