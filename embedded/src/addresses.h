#ifndef S18_SOFTWARE_CHALLENGE_ADDRESSES_H
#define S18_SOFTWARE_CHALLENGE_ADDRESSES_H

// podX command addresses
#define IDLE                0x00
#define READY               0x01
#define ACCEL               0x02
#define COAST               0x03
#define BRAKE               0x04
#define STOP                0x05

// server state communication
#define STATE_REJECT        0x07
#define INVALID_COMMAND     0x99
#define INVALID_PACKET      0x15

// packet addresses
#define P_START             0x56
#define P_END               0x23

// sensor data addresses
#define IMU_1               0xff51
#define IMU_2               0xff52
#define IMU_3               0xff53
#define TEMP_1              0xff71
#define TEMP_2              0xff72
#define TEMP_3              0xff73

// slave address
#define SLAVE_ADDRESS 0x5A

#endif //S18_SOFTWARE_CHALLENGE_ADDRESSES_H
