#ifndef S18_SOFTWARE_CHALLENGE_ADDRESSES_H
#define S18_SOFTWARE_CHALLENGE_ADDRESSES_H

// podX command addresses
#define IDLE                0x01
#define READY               0x02
#define ACCEL               0x03
#define COAST               0x04
#define BRAKE               0x05
#define STOP                0x06

// server state communication
#define STATE_REJECT        0x07
#define INVALID_COMMAND     0x99
#define INVALID_PACKET      0x15
#define POD_CRASHED         0x16

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

// data gen
#define IMU_ERR_PERC 7.0
#define TEMP_ERR_VAL 2.0

#endif //S18_SOFTWARE_CHALLENGE_ADDRESSES_H
