#include "AssetLoader.hpp"
#include <chrono>
#include <thread>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

void loaderThreadFunc(
    LoadedAssets* data,
    std::vector<AssetInfo> assets,
    int delayPerAssetMs
) {
    data->totalCount = assets.size();

    for (const auto& asset : assets) {
        std::this_thread::sleep_for(std::chrono::milliseconds(delayPerAssetMs));

        if (asset.type == AssetType::Texture) {
            auto tex = std::make_shared<sf::Texture>();
            if (tex->loadFromFile(asset.path)) {
                std::lock_guard<std::mutex> lock(data->mtx);
                data->textures.push_back(tex);
            }
        } else if (asset.type == AssetType::Sound) {
            auto buf = std::make_shared<sf::SoundBuffer>();
            if (buf->loadFromFile(asset.path)) {
                std::lock_guard<std::mutex> lock(data->mtx);
                data->sounds.push_back(buf);
            }
        }

        data->loadedCount.fetch_add(1);
    }

    data->done.store(true);
}

static bool hasExtension(const fs::path& p,
                         const std::vector<std::string>& exts)
{
    auto ext = p.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    for (const auto& e : exts) {
        if (ext == e) return true;
    }
    return false;
}

std::vector<AssetInfo> discoverAssets(
    const std::string& texturesRoot,
    const std::string& audioRoot
) {
    std::vector<AssetInfo> assets;

    // Discover textures (png, jpg, jpeg)
    std::vector<std::string> texExts = {".png", ".jpg", ".jpeg", ".bmp"};
    if (fs::exists(texturesRoot)) {
        for (const auto& entry : fs::recursive_directory_iterator(texturesRoot)) {
            if (!entry.is_regular_file()) continue;
            if (!hasExtension(entry.path(), texExts)) continue;

            assets.push_back(AssetInfo{
                entry.path().string(),
                AssetType::Texture
            });
        }
    }

    // Discover audio (wav, ogg, flac, mp3)
    std::vector<std::string> audioExts = {".wav", ".ogg", ".flac", ".mp3"};
    if (fs::exists(audioRoot)) {
        for (const auto& entry : fs::recursive_directory_iterator(audioRoot)) {
            if (!entry.is_regular_file()) continue;
            if (!hasExtension(entry.path(), audioExts)) continue;

            assets.push_back(AssetInfo{
                entry.path().string(),
                AssetType::Sound
            });
        }
    }

    return assets;
}