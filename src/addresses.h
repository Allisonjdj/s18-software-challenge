#ifndef S18_SOFTWARE_CHALLENGE_ADDRESSES_H
#define S18_SOFTWARE_CHALLENGE_ADDRESSES_H

// command addresses
#define INIT    0x01
#define DEINIT  0x02
#define START   0x03
#define BRAKE   0x04
#define STATE_ID        0xff00

// packet addresses
#define P_START         0x56
#define P_END           0x23

// sensor data addresses
#define IMU_1           0xff51
#define IMU_2           0xff52
#define IMU_3           0xff53
#define TEMP_1          0xff71
#define TEMP_2          0xff72
#define TEMP_3          0xff73
#define TEMP_4          0xff74

// server communication
#define INVALID_PACKET 0x05
#define INVALID_COMMAND 0x05
#define POD_CRASHED 0x06

// slave address
#define SLAVE_ADDRESS 0x5A

#endif //S18_SOFTWARE_CHALLENGE_ADDRESSES_H
