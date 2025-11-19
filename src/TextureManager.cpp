#include <fstream>
#include <iostream>
#include <filesystem>
#include "TextureManager.h"
#include "StringUtils.h"
#include "IETThread.h"
#include "ThreadPool.h"

//a singleton class
TextureManager* TextureManager::sharedInstance = NULL;

TextureManager* TextureManager::getInstance() {
	if (sharedInstance == NULL) {
		//initialize
		sharedInstance = new TextureManager();
	}

	return sharedInstance;
}

TextureManager::TextureManager()
{
	this->countStreamingAssets();
	// thread pool stuff
	this->threadPool = new ThreadPool(4);
}

void TextureManager::loadFromAssetList()
{
	std::cout << "[TextureManager] Reading from asset list" << std::endl;
	std::ifstream stream("Media/assets.txt");
	String path;

	while(std::getline(stream, path))
	{
		std::vector<String> tokens = StringUtils::split(path, '/');
		String assetName = StringUtils::split(tokens[tokens.size() - 1], '.')[0];
		this->instantiateAsTexture(path, assetName, false);
		std::cout << "[TextureManager] Loaded texture: " << assetName << std::endl;
	}
}

sf::Texture* TextureManager::getFromTextureMap(const String assetName, int frameIndex)
{
	if (!this->textureMap[assetName].empty()) {
		return this->textureMap[assetName][frameIndex];
	}
	else {
		std::cout << "[TextureManager] No texture found for " << assetName << std::endl;
		return NULL;
	}
}

int TextureManager::getNumFrames(const String assetName)
{
	if (!this->textureMap[assetName].empty()) {
		return this->textureMap[assetName].size();
	}
	else {
		std::cout << "[TextureManager] No texture found for " << assetName << std::endl;
		return 0;
	}
}

sf::Texture* TextureManager::getStreamTextureFromList(const int index)
{
	return this->streamTextureList[index];
}

int TextureManager::getNumLoadedStreamTextures() const
{
	return this->streamTextureList.size();
}

void TextureManager::countStreamingAssets()
{
	this->streamingAssetCount = 0;
	for (const auto& entry : std::filesystem::directory_iterator(STREAMING_PATH)) {
		this->streamingAssetCount++;
	}
	std::cout << "[TextureManager] Number of streaming assets: " << this->streamingAssetCount << std::endl;
}

void TextureManager::instantiateAsTexture(String path, String assetName, bool isStreaming)
{
	sf::Texture* texture = new sf::Texture();
	texture->loadFromFile(path);
	this->textureMap[assetName].push_back(texture);

	if(isStreaming)
	{
		this->streamTextureList.push_back(texture);
	}
	else
	{
		this->baseTextureList.push_back(texture);
	}
	
}

// P3 loading screen related stuff
void TextureManager::loadGameAssetsAsync(const std::string& directoryPath)
{
	std::cout << "[TextureManager] Starting async load from directory: " << directoryPath << std::endl;

	// asset count
	this->totalAssetsToLoad = 0;
	for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
		if (entry.is_regular_file()) {
			this->totalAssetsToLoad++;
		}
	}

	std::cout << "[TextureManager] Total assets to load: " << this->totalAssetsToLoad << std::endl;

	this->loadedAssetsCount = 0;

	// loading
	int index = 0;
	for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
		if (entry.is_regular_file()) {
			String path = entry.path().string();

			// Enqueue each asset load task to thread pool
			this->threadPool->enqueueTask([this, path, index]() {

				// "Mention how much time delay you added for loading each asset."
				IETThread::sleep(0.01);  

				// asset name
				std::vector<String> tokens = StringUtils::split(path, '/');
				if (tokens.empty()) {
					tokens = StringUtils::split(path, '\\');
				}
				String filename = tokens[tokens.size() - 1];
				String assetName = StringUtils::split(filename, '.')[0];

				// Load texture
				sf::Texture* texture = new sf::Texture();
				if (texture->loadFromFile(path)) {

					// store them
					{
						std::lock_guard<std::mutex> lock(this->queueMutex);
						this->textureMap[assetName].push_back(texture);
						this->streamTextureList.push_back(texture);
					}

					// progress update
					{
						std::lock_guard<std::mutex> lock(this->progressMutex);
						this->loadedAssetsCount++;
					}

					std::cout << "[TextureManager] Loaded: " << assetName
						<< " (" << this->loadedAssetsCount << "/"
						<< this->totalAssetsToLoad << ")" << std::endl;
				}
				else {
					std::cout << "[TextureManager] Failed to load: " << path << std::endl;
				}
				});

			index++;
		}
	}
}

float TextureManager::getLoadingProgress() const
{
	std::lock_guard<std::mutex> lock(this->progressMutex);

	if (this->totalAssetsToLoad == 0)
		return 0.0f;

	return (float)this->loadedAssetsCount / (float)this->totalAssetsToLoad;
}

int TextureManager::getTotalAssetsToLoad() const
{
	return this->totalAssetsToLoad;
}

int TextureManager::getLoadedAssetsCount() const
{
	std::lock_guard<std::mutex> lock(this->progressMutex);
	return this->loadedAssetsCount;
}