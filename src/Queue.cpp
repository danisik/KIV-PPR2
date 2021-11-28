#include "Queue.h"

Queue::Queue()
{
}

void Queue::Push(const BUFFER_OBJECT& item)
{
    {
        std::unique_lock<std::mutex> lock(mutex);
        queue.push(item);
    }
    cond.notify_one();
}

void Queue::RequestShutdown() 
{
    {
        std::unique_lock<std::mutex> lock(mutex);
        shutdown = true;
    }
    cond.notify_all();
}

bool Queue::Pop(BUFFER_OBJECT& item) 
{
    std::unique_lock<std::mutex> lock(mutex);
    for (;;) 
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
        cond.wait(lock);
    }
    item = std::move(queue.front());
    queue.pop();
    return true;
}

void Queue::RestartShutdown()
{
    shutdown = false;
}