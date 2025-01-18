#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <chrono> // для работы с временем и таймерами

class Timer {
public:
    Timer();
    ~Timer();
private:
    std::chrono::time_point<std::chrono::_V2::system_clock> start, end;
};

#include "../src/timer.cpp"

#endif