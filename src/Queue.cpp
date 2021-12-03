#include "Queue.h"

/// <summary>
/// Constructor.
/// </summary>
CustomQueue::CustomQueue()
{
}

/// <summary>
/// Push data buffer to CustomQueue and notify one thread that data block is free to use.
/// </summary>
/// <param name="item">Data buffer.</param>
void CustomQueue::push(const BUFFER_OBJECT& item)
{
    {
        std::unique_lock<std::mutex> lock(mutex);

        // If buffers count in queue is same as allowed blocks, then wait.
        if (queue.size() >= MEMORY_BLOCKS_ALLOWED)
        {
            cond_push.wait(lock);
        }

        queue.push(item);
    }
    cond.notify_one();
}

/// <summary>
/// Request shutdown for all threads, if file reading ends.
/// </summary>
void CustomQueue::request_shutdown() 
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
/// <returns>True if getting data buffer from CustomQueue was successful, false if not.</returns>
bool CustomQueue::pop(BUFFER_OBJECT& item) 
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

        // If CustomQueue is empty and shutdown was not yet called, then wait.
        cond.wait(lock);
    }

    // Get data buffer from CustomQueue.
    item = std::move(queue.front());

    // Allow queue to enqueue another data block.
    if (queue.size() == MEMORY_BLOCKS_ALLOWED)
    {
        cond_push.notify_one();
    }

    // Remove it from CustomQueue.
    queue.pop();
    return true;
}

/// <summary>
/// Restart variable representing shutdown.
/// </summary>
void CustomQueue::restart_shutdown()
{
    shutdown = false;
}