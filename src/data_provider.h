#ifndef S18_SOFTWARE_CHALLENGE_DATA_PROVIDER_H
#define S18_SOFTWARE_CHALLENGE_DATA_PROVIDER_H

#include <stdint.h>
#include <wlib/stl/Bitset.h>
#include "SimplexNoise.h"

static_assert(sizeof(float32_t) == 4, "");
static_assert(sizeof(uint32_t) == 4, "");

void make_packet_server(uint32_t messageId, float32_t start_time, float32_t curr_time, uint8_t *dat) {
    float32_t t = curr_time - start_time;

    dat[0] = P_START;
    *reinterpret_cast<uint32_t *>(dat + 1) = messageId;
    *reinterpret_cast<float32_t *>(dat + 1 + 4) = t;
    dat[9] = P_END;
}

void make_pack_int(uint32_t val, float32_t start_time, float32_t curr_time, uint8_t *dat, uint8_t id) {
    float32_t t = curr_time - start_time;

    dat[0] = P_START;
    *(dat + 1) = id;
    *(dat + 1 + 1) = val;
    *reinterpret_cast<float32_t *>(dat + 1 + 1 + 4) = t;
    dat[11] = P_END;
}

void make_pack_float(float32_t val, float32_t start_time, float32_t curr_time, uint8_t *dat, uint8_t id) {
    float32_t t = curr_time - start_time;

    dat[0] = P_START;
    *(dat + 1) = id;
    *reinterpret_cast<float *>(dat + 1 + 1) = val;
    *reinterpret_cast<float *>(dat + 1 + 2 + 2) = t;
    dat[11] = P_END;
}

#define IMU_ERR_PERC 7.0

double imu_rand(float32_t a, uint8_t id, float32_t time) {
    double maxv = a * IMU_ERR_PERC / 100.0;
    if (maxv < 1) {
        maxv = 1;
    }
    double noise = SimplexNoise::noise(static_cast<float>(time), id / 10.0f, rand() / 10000.0f) * maxv;
    return a + noise;
}

#define TEMP_ERR_VAL 2.0

double temp_rand(float32_t t, uint8_t id, float32_t time) {
    double noise = SimplexNoise::noise(static_cast<float>(time), id / 10.0f, rand() / 10000.0f) * TEMP_ERR_VAL;
    return t + noise;
}


#endif //S18_SOFTWARE_CHALLENGE_DATA_PROVIDER_H
