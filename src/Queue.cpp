#include "Queue.h"

/// <summary>
/// Constructor.
/// </summary>
Queue::Queue()
{
}

/// <summary>
/// Push data buffer to queue and notify one thread that data block is free to use.
/// </summary>
/// <param name="item">Data buffer.</param>
void Queue::push(const BUFFER_OBJECT& item)
{
    {
        std::unique_lock<std::mutex> lock(mutex);
        queue.push(item);
    }
    cond.notify_one();
}

/// <summary>
/// Request shutdown for all threads, if file reading ends.
/// </summary>
void Queue::request_shutdown() 
{
    {
        std::unique_lock<std::mutex> lock(mutex);
        shutdown = true;
    }
    cond.notify_all();
}

/// <summary>
/// Pop last data buffer.
/// </summary>
/// <param name="item">Reference to data buffer</param>
/// <returns>True if getting data buffer from queue was successful, false if not.</returns>
bool Queue::pop(BUFFER_OBJECT& item) 
{
    std::unique_lock<std::mutex> lock(mutex);
    while (true) 
    {
        if (queue.empty())
        {
            if (shutdown) 
            {
                return false;
            }
        }
        else 
        {
            break;
        }

        // If queue is empty and shutdown was not yet called, then wait.
        cond.wait(lock);
    }

    // Get data buffer from queue.
    item = std::move(queue.front());

    // Remove it from queue.
    queue.pop();
    return true;
}

/// <summary>
/// Restart variable representing shutdown.
/// </summary>
void Queue::restart_shutdown()
{
    shutdown = false;
}