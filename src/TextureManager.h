#pragma once
#include <unordered_map>
#include "SFML/Graphics.hpp"

#include <queue>
#include <mutex>

class ThreadPool;

class TextureManager
{
public:
	typedef std::string String;
	typedef std::vector<sf::Texture*> TextureList;
	typedef std::unordered_map<String, TextureList> HashTable;

	// thread pool new stuff
	struct LoadedTexture {
		sf::Texture* texture;
		String assetName;
		int index;
	};

	
public:
	static TextureManager* getInstance();
	void loadFromAssetList(); //loading of all assets needed for startup
	sf::Texture* getFromTextureMap(const String assetName, int frameIndex);
	int getNumFrames(const String assetName);

	sf::Texture* getStreamTextureFromList(const int index);
	int getNumLoadedStreamTextures() const;

	// thread pool new stuff
	LoadedTexture popReadyTexture();

	// p3 related stuff
	void loadGameAssetsAsync(const std::string& assetListFile);  
	float getLoadingProgress() const; 
	int getTotalAssetsToLoad() const;  
	int getLoadedAssetsCount() const;

private:
	TextureManager();
	TextureManager& operator=(TextureManager const&) {};  // assignment operator is private
	static TextureManager* sharedInstance;

	HashTable textureMap;
	TextureList baseTextureList;
	TextureList streamTextureList;

	const std::string STREAMING_PATH = "Media/Streaming/";
	int streamingAssetCount = 0;

	void countStreamingAssets();
	void instantiateAsTexture(String path, String assetName, bool isStreaming);

	// thread pool new stuff
	std::queue<LoadedTexture> readyQueue;  
	std::mutex queueMutex;  
	ThreadPool* threadPool;  

	// p3 related stuff
	int totalAssetsToLoad = 0; 
	int loadedAssetsCount = 0;
	mutable std::mutex progressMutex;  


};