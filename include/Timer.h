//
// Created by roundedglint585 on 3/21/21.
//

#ifndef PROJECT_TIMER_H
#define PROJECT_TIMER_H
#include <chrono>
#include "../include/Logger.h"

namespace timer {
    using Time = std::chrono::time_point<std::chrono::high_resolution_clock>;

    class Timer {
    public:
        Timer() = default;

        void start();

        template<typename T>
        float stop();

    private:
        bool isStarted = false;
        Time time;
    };
}

template<typename T>
float timer::Timer::stop() {
    if(isStarted){
        isStarted = false;
        return std::chrono::duration_cast<T>(std::chrono::high_resolution_clock::now() - time).count();
    }else{
        logger::WARNING("Timer not started");
        return 0.f;
    }
}

#endif //PROJECT_TIMER_H
