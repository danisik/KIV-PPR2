#include "Watchdog.h"

/// <summary>
/// Constructor.
/// </summary>
/// <param name="p_interval">Interval</param>
Watchdog::Watchdog(int p_interval)
{
    interval = p_interval;
    timer = 0;
    process_running = true;
}

/// <summary>
/// Start watchdog.
/// </summary>
void Watchdog::start()
{
    thread = std::thread(&Watchdog::loop, this);
}

/// <summary>
/// End watchdog.
/// </summary>
void Watchdog::join()
{
    stop();
    thread.join();
}

/// <summary>
/// Stop watchdog.
/// </summary>
void Watchdog::stop()
{
    process_running = false;
}

/// <summary>
/// Reset current timer.
/// </summary>
void Watchdog::reset()
{    
    timer = 0;
}

/// <summary>
/// Thread body.
/// </summary>
void Watchdog::loop()
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