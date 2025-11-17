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

//=================================================================================
//*******************   THREAD POOL RELATED STUFF START ***************************
//=================================================================================

void TextureManager::loadBatchAsync(int startIndex, int count)
{
	for (int i = 0; i < count; i++)
	{
		int index = startIndex + i;
		if (index >= this->streamingAssetCount)
			break;

		// Add the thread/task to thread pool
		this->threadPool->enqueueTask([this, index]() {
			this->loadSingleStreamAssetSync(index);
			});
	}
}

void TextureManager::loadSingleStreamAssetSync(int index)
{
	int fileNum = 0;

	for (const auto& entry : std::filesystem::directory_iterator(STREAMING_PATH)) {
		if (index == fileNum)
		{
			// Simulate loading of very large file
			IETThread::sleep(100);

			String path = entry.path().string();

			std::vector<String> pathTokens = StringUtils::split(path, '/');
			if (pathTokens.empty()) {
				pathTokens = StringUtils::split(path, '\\');
			}

			String filename = pathTokens[pathTokens.size() - 1];
			std::vector<String> nameTokens = StringUtils::split(filename, '.');
			String assetName = nameTokens[0];

			sf::Texture* texture = new sf::Texture();
			texture->loadFromFile(path);

			// Add thrd to ready q
			{
				std::lock_guard<std::mutex> lock(this->queueMutex);

				this->textureMap[assetName].push_back(texture);
				this->streamTextureList.push_back(texture);

				LoadedTexture loaded;
				loaded.texture = texture;
				loaded.assetName = assetName;
				loaded.index = index;

				this->readyQueue.push(loaded);
			}

			std::cout << "[TextureManager] Loaded streaming texture: " << assetName
				<< " (Queue: " << this->readyQueue.size() << ")" << std::endl;
			break;
		}
		fileNum++;
	}
}

void TextureManager::loadSingleStreamAsset(int index)
{
	this->loadSingleStreamAssetSync(index);
}

bool TextureManager::hasReadyTexture()
{
	std::lock_guard<std::mutex> lock(this->queueMutex);
	return !this->readyQueue.empty();
}

TextureManager::LoadedTexture TextureManager::popReadyTexture()
{
	std::lock_guard<std::mutex> lock(this->queueMutex);
	LoadedTexture loaded = this->readyQueue.front();
	this->readyQueue.pop();
	return loaded;
}

int TextureManager::getReadyQueueSize() const
{
	return this->readyQueue.size();
}

//=================================================================================
//*********************   THREAD POOL RELATED STUFF END ***************************
//=================================================================================

//void TextureManager::loadSingleStreamAsset(int index)
//{
//	int fileNum = 0;
//	
//	for (const auto& entry : std::filesystem::directory_iterator(STREAMING_PATH)) {
//		if(index == fileNum)
//		{
//			//simulate loading of very large file
//			IETThread::sleep(100);
//
//			String path = entry.path().string();
//
//			std::vector<String> pathTokens = StringUtils::split(path, '/');
//			if (pathTokens.empty()) {
//				pathTokens = StringUtils::split(path, '\\');
//			}
//			
//			String filename = pathTokens[pathTokens.size() - 1];
//			std::vector<String> nameTokens = StringUtils::split(filename, '.');
//			String assetName = nameTokens[0];
//	
//			this->instantiateAsTexture(path, assetName, true);
//
//			std::cout << "[TextureManager] Loaded streaming texture: " << assetName << std::endl;
//			break;
//		}
//
//		fileNum++;
//	}
//}

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
