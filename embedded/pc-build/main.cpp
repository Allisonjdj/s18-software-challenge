#include "loop.h"

#include <chrono>
#include <thread>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

int main(int argc, char *argv[]) {
    using namespace std::this_thread;
    using namespace std::chrono;

    setup();

    while (true) {
        loop();
        sleep_for(nanoseconds(10));
    }
}

#pragma clang diagnostic pop