#include <thread>
#include <functional>
#include <cstdlib>
#include <iostream>

#pragma once
class Watchdog
{
    public:
        Watchdog(int p_interval);

        void start();
        void join();
        void reset();
        void stop();

    private:
        int interval;
        unsigned int timer;
        bool process_running;
        std::thread thread;

        void loop();
};