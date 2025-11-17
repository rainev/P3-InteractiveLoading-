#pragma once
#include "IETThread.h"
#include <atomic>

/// Thread responsible for streaming a single texture asset.
class StreamingThread : public IETThread
{
public:
    StreamingThread(int index);
    ~StreamingThread();

    bool isFinished() const;

protected:
    void run() override;

private:
    int textureIndex = 0;
    std::atomic<bool> finished = false;
};
