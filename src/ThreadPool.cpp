#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads) : stop(false), activeTasks(0)
{
    for (size_t i = 0; i < numThreads; i++)
    {
        workers.emplace_back([this] { this->workerThread(); });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();

    for (std::thread& worker : workers)
    {
        if (worker.joinable())
            worker.join();
    }
}

void ThreadPool::enqueueTask(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.push(task);
    }
    condition.notify_one();
}

bool ThreadPool::isIdle() const
{
    return tasks.empty() && activeTasks == 0;
}

int ThreadPool::getQueueSize() const
{
    return tasks.size();
}

void ThreadPool::workerThread()
{
    while (true)
    {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stop || !tasks.empty(); });

            if (stop && tasks.empty())
                return;

            task = std::move(tasks.front());
            tasks.pop();
            activeTasks++;
        }

        task();

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            activeTasks--;
        }
    }
}