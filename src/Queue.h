#include <mutex>
#include <condition_variable>
#include <queue>

struct BUFFER_OBJECT
{
    double* buffer{};
    size_t readCount = 0;
};

#pragma once
class Queue {

    private:
        std::condition_variable cond;
        std::mutex mutex;
        std::queue<BUFFER_OBJECT> queue;
        bool shutdown = false;

    public:
        Queue();
        void Push(const BUFFER_OBJECT& item);
        void RequestShutdown();
        bool Pop(BUFFER_OBJECT& item);
        void RestartShutdown();
};

