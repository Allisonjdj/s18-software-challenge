#include "UART.hh"

#include <chrono>

using namespace std::chrono;

static microseconds start;
static int state = 0;

void UART::begin(int) {
    start = duration_cast<microseconds>(system_clock::now().time_since_epoch());
}

int UART::getchar() {
    microseconds now = duration_cast<microseconds>(system_clock::now().time_since_epoch());
    microseconds um = now - start;

    double t = um.count() / 1000.0 / 1000.0;

    int ret = -1;

    if (state == 0 && t > 0.2) {
        ret = 0x01;
        state = 1;
    } else if (state == 1 && t > 0.3) {
        ret = 0x02;
        state = 2;
    } else if (state == 2 && t > 0.4) {
        ret = 0x01;
        state = 3;
    } else if (state == 3 && t > 1) {
        ret = 0x03;
        state = 4;
    } else if (state == 4 && t > 90) {
        ret = 0x04;
        state = 5;
    }

    return ret;
}
