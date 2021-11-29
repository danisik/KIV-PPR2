#include <mutex>
#include <condition_variable>
#include <queue>

/// <summary>
/// Struct representing data buffer with read count.
/// </summary>
struct BUFFER_OBJECT
{
    double* buffer{};
    size_t readCount = 0;
};

#pragma once
/// <summary>
/// Class representing blocking queue for SMP.
/// </summary>
class Queue {

    private:
        // Condition variable.
        std::condition_variable cond;
        // Mutex.
        std::mutex mutex;
        // Queue of data buffers.
        std::queue<BUFFER_OBJECT> queue;
        // Variable representing file reading done.
        bool shutdown = false;

    public:
        Queue();
        void Push(const BUFFER_OBJECT& item);
        void RequestShutdown();
        bool Pop(BUFFER_OBJECT& item);
        void RestartShutdown();
};

