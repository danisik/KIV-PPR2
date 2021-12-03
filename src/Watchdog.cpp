#include "Watchdog.h"

/// <summary>
/// Constructor.
/// </summary>
/// <param name="p_interval">Interval</param>
CustomWatchdog::CustomWatchdog(int p_interval)
{
    interval = p_interval;
    timer = 0;
    process_running = true;
}

/// <summary>
/// Start CustomWatchdog.
/// </summary>
void CustomWatchdog::start()
{
    thread = std::thread(&CustomWatchdog::loop, this);
}

/// <summary>
/// End CustomWatchdog.
/// </summary>
void CustomWatchdog::join()
{
    stop();
    thread.join();
}

/// <summary>
/// Stop CustomWatchdog.
/// </summary>
void CustomWatchdog::stop()
{
    process_running = false;
}

/// <summary>
/// Reset current timer.
/// </summary>
void CustomWatchdog::reset()
{    
    timer = 0;
}

/// <summary>
/// Thread body.
/// </summary>
void CustomWatchdog::loop()
{
    while (process_running)
    {
        timer++;

        if (timer >= interval)
        {
            process_running = false;
            std::cout << "No activity was registered in " << interval << " seconds. Aborting calculation." << std::endl;
            std::abort();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}