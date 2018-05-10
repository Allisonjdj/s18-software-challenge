#ifndef S18_SOFTWARE_CHALLENGE_DATA_PROVIDER_H
#define S18_SOFTWARE_CHALLENGE_DATA_PROVIDER_H

#include <stdint.h>
#include <stdlib.h>

#include <wlib/stl/Bitset.h>
#include "SimplexNoise.h"
#include "addresses.h"

static_assert(sizeof(float) == 4, "");
static_assert(sizeof(uint32_t) == 4, "");

void pack_float_value(float val, uint8_t *buffer, uint8_t pos) {
    auto *fPtr = reinterpret_cast<float *>(buffer + pos);
    *fPtr = val;
}

double imu_rand(float a, uint8_t id, float time) {
    double maxv = a * IMU_ERR_PERC / 100.0;
    if (maxv < 1) {
        maxv = 1;
    }
    float noise = SimplexNoise::noise(time, id / 10.0f, rand() / 10000.0f) * maxv;
    return a + noise;
}

double temp_rand(float t, uint8_t id, float time) {
    double noise = SimplexNoise::noise(static_cast<float>(time), id / 10.0f, rand() / 10000.0f) * TEMP_ERR_VAL;
    return t + noise;
}

#endif //S18_SOFTWARE_CHALLENGE_DATA_PROVIDER_H
