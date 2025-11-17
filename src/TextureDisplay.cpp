#include "TextureDisplay.h"
#include <iostream>
#include "TextureManager.h"
#include "BaseRunner.h"
#include "GameObjectManager.h"
#include "IconObject.h"
#include "StreamingThread.h"

TextureDisplay::TextureDisplay(): AGameObject("TextureDisplay")
{
	
}

TextureDisplay::~TextureDisplay()
{
	if (currentStreamThread != nullptr)
	{
		delete currentStreamThread;
		currentStreamThread = nullptr;
	}
}


void TextureDisplay::initialize()
{
	
}

void TextureDisplay::processInput(sf::Event event)
{
	
}

//=============================
// THREAD POOL update method
//=============================
void TextureDisplay::update(sf::Time deltaTime)
{
    this->ticks += BaseRunner::TIME_PER_FRAME.asMilliseconds();

    // Schedule batch loading at intervals
    if (this->ticks >= this->STREAMING_LOAD_DELAY)
    {
        this->ticks = 0.0f;

        int currentIndex = this->iconList.size();
        int batchSize = 10;

		// 480 is ung total streaming assets
        if (currentIndex < 480) 
        {
            std::cout << "[TextureDisplay] Scheduling batch load starting at index "
                << currentIndex << std::endl;
            TextureManager::getInstance()->loadBatchAsync(currentIndex, batchSize);
        }
    }

    // this spawns icons 
    while (TextureManager::getInstance()->hasReadyTexture())
    {
        TextureManager::LoadedTexture loaded = TextureManager::getInstance()->popReadyTexture();
        this->spawnObject();

        std::cout << "[TextureDisplay] Spawned icon. Ready queue size: "
            << TextureManager::getInstance()->getReadyQueueSize() << std::endl;
    }
}
//void TextureDisplay::update(sf::Time deltaTime)
//{
//    ticks += BaseRunner::TIME_PER_FRAME.asMilliseconds();
//
//    if (streamingType == SINGLE_STREAM)
//    {
//        // Start a load if delay passed and no active worker
//        if (ticks >= STREAMING_LOAD_DELAY && currentStreamThread == nullptr)
//        {
//            ticks = 0.0f;
//
//            int currentIndex = iconList.size();
//            const int totalAssets = 480;
//
//            if (currentIndex < totalAssets)
//            {
//                currentStreamThread = new StreamingThread(currentIndex);
//                currentStreamThread->start();
//            }
//        }
//
//        // Check for completion
//        if (currentStreamThread != nullptr && currentStreamThread->isFinished())
//        {
//            spawnObject();
//            delete currentStreamThread;
//            currentStreamThread = nullptr;
//        }
//    }
//}

void TextureDisplay::spawnObject()
{
	String objectName = "Icon_" + to_string(this->iconList.size());
	IconObject* iconObj = new IconObject(objectName, this->iconList.size());
	this->iconList.push_back(iconObj);

	//set position
	int IMG_WIDTH = 68; int IMG_HEIGHT = 68;
	float x = this->columnGrid * IMG_WIDTH;
	float y = this->rowGrid * IMG_HEIGHT;
	iconObj->setPosition(x, y);

	std::cout << "Set position: " << x << " " << y << std::endl;

	this->columnGrid++;
	if(this->columnGrid == this->MAX_COLUMN)
	{
		this->columnGrid = 0;
		this->rowGrid++;
	}
	GameObjectManager::getInstance()->addObject(iconObj);
}
