#include "Watchdog.h"

Watchdog::Watchdog(int p_interval)
{
    interval = p_interval;
    timer = 0;
    process_running = true;
}

void Watchdog::start()
{
    thread = std::thread(&Watchdog::loop, this);
}

void Watchdog::join()
{
    stop();
    thread.join();
}

void Watchdog::stop()
{
    process_running = false;
}

void Watchdog::reset()
{    
    timer = 0;
}

void Watchdog::loop()
{
    while (process_running)
    {
        timer++;

        if (timer >= interval)
        {
            process_running = false;
            std::cout << "No activity was registered in " << interval << " seconds. Aborting calculation." << std::endl;
            std::abort;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}