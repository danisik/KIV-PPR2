#include <thread>
#include <functional>
#include <cstdlib>
#include <iostream>

#pragma once
/// <summary>
/// Class representing watchdog thread.
/// </summary>
class CustomWatchdog
{
    public:
        CustomWatchdog(int p_interval);

        void start();
        void join();
        void reset();
        void stop();

    private:
        // Max time for run.
        int interval;

        // Current running time.
        int timer;

        // Thread running or not.
        bool process_running;

        // Thread.
        std::thread thread;

        void loop();
};
