#include "StreamingThread.h"
#include "TextureManager.h"

StreamingThread::StreamingThread(int index) :
    textureIndex(index)
{
}

StreamingThread::~StreamingThread()
{
}

void StreamingThread::run()
{
    // Perform load on separate thread
    TextureManager::getInstance()->loadSingleStreamAsset(textureIndex);
    finished = true;
}

bool StreamingThread::isFinished() const
{
    return finished.load();
}
