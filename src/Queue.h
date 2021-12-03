#include <mutex>
#include <condition_variable>
#include <queue>

/// <summary>
/// Struct representing data buffer with read count.
/// </summary>
struct BUFFER_OBJECT
{
    double* buffer{};
    size_t read_count = 0;
};

// Number representing how many threads are created.
static constexpr const unsigned int THREADS_COUNT = 10;

// Max blocks to be available in queue.
static constexpr const unsigned int MEMORY_BLOCKS_ALLOWED = 4 * THREADS_COUNT;

#pragma once
/// <summary>
/// Class representing blocking queue for SMP.
/// </summary>
class CustomQueue {

    private:
        // Condition variable.
        std::condition_variable cond;
        std::condition_variable cond_push;
        // Mutex.
        std::mutex mutex;
        // Queue of data buffers.
        std::queue<BUFFER_OBJECT> queue;
        // Variable representing file reading done.
        bool shutdown = false;

    public:
        CustomQueue();
        void push(const BUFFER_OBJECT& item);
        void request_shutdown();
        bool pop(BUFFER_OBJECT& item);
        void restart_shutdown();
};

