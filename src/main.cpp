// Include the required Wire library for I2C
#include <Arduino.h>
#include <Wire.h>

#include "addresses.h"
#include "data_provider.h"
#include "statehandler.h"

// data gen
static float brake_temp = 10;
static float propl_temp = 10;
static float chip_temp = 10;
static double prev_time = 0;

// time sensitive
long loop_start_time = 0;
static float start_time = 0;

// pod states and communication buffer
static bool podCrashed = false;
static PodMachine xPod;
static uint8_t m_buffer[32];


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                   Data Gen                                                         */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// braking temperature curve
void brake_t(float elapsed_time) {
    double dt = elapsed_time - prev_time;
    if (xPod.state == PodMachine::ST_ACCEL) {
        brake_temp += dt * 0.2f / (brake_temp + 5) * brake_temp;
    } else if (xPod.state == PodMachine::ST_BRAKE) {
        brake_temp += dt * 2;
    } else {
        if (brake_temp >= 10) {
            brake_temp -= 0.5 * dt * (brake_temp + 50) / brake_temp;
        }
    }
}

// propulsion temperature curve
void propl_t(float elapsed_time) {
    double dt = elapsed_time - prev_time;
    if (xPod.state == PodMachine::ST_ACCEL) {
        propl_temp += dt / (propl_temp + 5) * propl_temp;
    } else {
        if (propl_temp >= 10) {
            propl_temp -= 0.5 * dt * (propl_temp + 30) / propl_temp;
        }
    }
}

// chip temperature curve
void chip_t(float elapsedTime) {
    chip_temp = 10 + static_cast<float>(log(elapsedTime + 1)) * 8;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                               Communication                                                        */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// writes binary packet to Serial
void tubeWrite(uint8_t *buff, size_t size) {
    // write multiple times cuz sometimes packets do not get accepted
    for (int i = 0; i < 10; ++i) {
        Serial.write(buff, size);
    }
}

// send data
void requestEvents() {
    Wire.write(m_buffer, 32);
}

// receive commands
void receiveEvents(int numBytes) {
    // elapsed time since start
    float elapsedTime = (millis() / 1000) - start_time;

    // read the command received buffer
    uint8_t buff[numBytes];
    Wire.readBytes(buff, (size_t) numBytes);

    // create a packet for the tube server
    uint8_t tubeBuff[3];
    tubeBuff[0] = P_START;
    tubeBuff[2] = P_END;

    if (numBytes >= 3 && buff[0] == P_START && buff[2] == P_END) {
        tubeBuff[1] = buff[1];

        PodMachine::States beforeState = xPod.state;

        switch (tubeBuff[1]) {
            case IDLE:
                // switch state to IDLE (they are not allowed to switch to IDLE state manually)
                tubeBuff[1] = STATE_REJECT; // rejected by state machine
                break;
            case READY:
                // switch state to READY
                xPod.ready(elapsedTime);
                break;
            case ACCEL:
                // switch state to ACCEL
                xPod.start(elapsedTime);
                break;
            case COAST:
                // switch state to COAST
                xPod.coast(elapsedTime);
                break;
            case BRAKE:
                // switch state to BRAKE
                xPod.brake(elapsedTime);
                break;
            case STOP:
                // switch state to STOP (they are not allowed to switch to STOP state manually)
                tubeBuff[1] = STATE_REJECT;
                break;
            default:
                // illegal command
                tubeBuff[1] = INVALID_COMMAND;
                break;
        }

        // state did not change
        if (xPod.state == beforeState) {
            tubeBuff[1] = STATE_REJECT;
        }
    } else {
        tubeBuff[1] = INVALID_PACKET;
    }

    tubeWrite(tubeBuff, 3);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                              Project Setup                                                         */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void setup() {
    // Start the I2C Bus as Slave address
    Wire.begin(SLAVE_ADDRESS);

    // Attach a function to trigger when something is requested.
    Wire.onRequest(requestEvents);
    // attach a function to trigger when something is received
    Wire.onReceive(receiveEvents);

    start_time = millis() / 1000.0;     // timer for data (curves)
    loop_start_time = millis();         // timer for packets per second

    // communication with pod tube server
    Serial.begin(9600);

    delay(1000);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                  Curve Gen                                                         */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void loop() {
    // calculate the data 30 times per second
    if (millis() - loop_start_time >= (1000 / 30)) {
        float curr_time = millis() / 1000.0;
        float elapsed_time = curr_time - start_time;

        double imu_val_raw = xPod.a;

        // create a sensor packet to send to the user
        m_buffer[0] = P_START;
        m_buffer[1] = 0;
        m_buffer[2] = 0;
        pack_float_value(elapsed_time, m_buffer, 3);
        pack_float_value(imu_rand(imu_val_raw, IMU_1, elapsed_time), m_buffer, 7);
        pack_float_value(imu_rand(imu_val_raw, IMU_2, elapsed_time), m_buffer, 11);
        pack_float_value(imu_rand(imu_val_raw, IMU_3, elapsed_time), m_buffer, 15);
        pack_float_value(temp_rand(brake_temp, TEMP_1, elapsed_time), m_buffer, 19);
        pack_float_value(temp_rand(propl_temp, TEMP_2, elapsed_time), m_buffer, 23);
        pack_float_value(temp_rand(chip_temp, TEMP_3, elapsed_time), m_buffer, 27);
        m_buffer[31] = P_END;

        xPod.tick(elapsed_time);
        brake_t(elapsed_time);
        propl_t(elapsed_time);
        chip_t(elapsed_time);
        prev_time = elapsed_time;
    }

    // stop everything when pod crashes
    if (podCrashed) {
        uint8_t tubeBuff[3] = {P_START, POD_CRASHED, P_END};
        tubeWrite(tubeBuff, 3);

        while (1) while (1);
    }
}
