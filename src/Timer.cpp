//
// Created by roundedglint585 on 3/21/21.
//

#include "../include/Timer.h"

void timer::Timer::start() {
    if(!isStarted){
        isStarted = true;
        time = std::chrono::high_resolution_clock::now();
    }
}


