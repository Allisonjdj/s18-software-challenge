#include "RTT.hh"


#include <chrono>

void RTT::begin() {}

double RTT::millis() {
    using namespace std::chrono;
    microseconds us = duration_cast<microseconds>(system_clock::now().time_since_epoch());
    return us.count() / 1000.0;
}
