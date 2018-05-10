#include "Trace.hh"
#include "gui/app.h"

#include <iostream>
#include <cstdint>

void Trace::begin(UART *) {}

Trace &Trace::operator<<(char *str) {
    if (str[0] == 0x56 && str[11] == 0x23) {
        uint16_t id = *reinterpret_cast<uint16_t *>(str + 1);
        float data = *reinterpret_cast<float *>(str + 1 + 2);
        float time = *reinterpret_cast<float *>(str + 1 + 2 + 4);
        if (id == 0xff00) {
            int state = *reinterpret_cast<uint32_t *>(str + 1 + 2);
            g_app->receive_state(time, state);
            //printf("\n%i: (%.2f, %i)\n", id, time, *reinterpret_cast<uint32_t *>(str + 1 + 2));
        } else {
            g_app->receive_data(time, data, id);
            //printf("%i: (%.2f, %.2f)\n", id, time, data);
        }
    }
}
