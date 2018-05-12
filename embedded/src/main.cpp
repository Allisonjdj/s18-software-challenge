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
unsigned long temp_data_start_time = 0;
unsigned long accel_data_start_time = 0;
static float start_time = 0;

// pod states and communication buffer
static PodMachine xPod;
static uint8_t m_buffer[32];
static uint8_t recentCommand = 0;
static bool isValid = true;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                   Data Gen                                                         */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// braking temperature curve
void brake_t(float elapsedTime) {
    double dt = elapsedTime - prev_time;
    if (xPod.state == PodMachine::ST_ACCEL) {
        brake_temp += dt * 0.3f / (brake_temp + 20) * brake_temp;
    } else if (xPod.state == PodMachine::ST_BRAKE) {
        brake_temp += dt * 1.5 * (brake_temp + 50) / brake_temp * fabs(xPod.a) / 25.0;
    } else {
        if (brake_temp >= 10) {
            brake_temp -= 1.5 * dt * (brake_temp - 10) / (brake_temp + 10);
        }
    }
}

// propulsion temperature curve
void propl_t(float elapsedTime) {
    double dt = elapsedTime - prev_time;
    if (xPod.state == PodMachine::ST_ACCEL) {
        propl_temp += dt * 2.7 / (propl_temp + 20) * propl_temp * fabs(xPod.a + 4.5) / 20.0;
    } else {
        if (propl_temp >= 10) {
            propl_temp -= 1.5 * dt * (propl_temp - 10) / (propl_temp + 10);
        }
    }
}

// chip temperature curve
void chip_t(float currentTime) {
    float t = currentTime - start_time;
    chip_temp = 10 + log(t + 1) * 8;
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
    if (xPod.s <= 30000) {
        Wire.write(m_buffer, 32);
    }
}

// receive commands
void receiveEvents(int numBytes) {
    // elapsed time since start
    float elapsedTime = (millis() / 1000) - start_time;

    // read the command received buffer
    uint8_t buff[numBytes];
    Wire.readBytes(buff, (size_t) numBytes);

    // create a packet for the tube server
    uint8_t tubeBuff[12];
    tubeBuff[0] = P_START;
    tubeBuff[2] = xPod.state;
    tubeBuff[11] = P_END;

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
                tubeBuff[1] = STATE_REJECT; // rejected by state machine
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

    isValid = tubeBuff[1] != STATE_REJECT && tubeBuff[1] != INVALID_PACKET && tubeBuff[1] != INVALID_COMMAND;

    recentCommand = buff[1];

    pack_float_value(xPod.v, m_buffer, 3);
    pack_float_value(xPod.s, m_buffer, 7);

    //tubeWrite(tubeBuff, 12);
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

    start_time = millis() / 1000.0;          // timer for data (curves)
    temp_data_start_time = millis();         // timer for temp packets gen per second
    accel_data_start_time = millis();        // timer for accel packets gen per second

    // communication with pod tube server
    Serial.begin(9600);

    delay(1000);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                  Curve Gen                                                         */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static float accel1 = 0;
static float accel2 = 0;
static float accel3 = 0 ;
static float temp1 = 0;
static float temp2 = 0;
static float temp3 = 0;

unsigned long data_timer = millis();

void loop() {
    // stop everything when pod crashes
    if (xPod.s > 30000) {
        Serial.println("Connection Lost - Pod Crased :(");
        //uint8_t tubeBuff[3] = {P_START, POD_CRASHED, P_END};
        //tubeWrite(tubeBuff, 3);

        while (1) while (1);
    }

    unsigned long currTime = millis();
    float currTimeSeconds = currTime / 1000.0;
    float elapsed_time = currTimeSeconds - start_time;

    // sample temperature sensors (100 ms)
    if (currTime - temp_data_start_time >= 100) {
        temp1 = temp_rand(brake_temp, TEMP_1, elapsed_time);
        temp2 = temp_rand(propl_temp, TEMP_2, elapsed_time);
        temp3 = temp_rand(chip_temp, TEMP_3, elapsed_time);

        brake_t(elapsed_time);
        propl_t(elapsed_time);
        chip_t(elapsed_time);
        prev_time = elapsed_time;

        // reset timer
        temp_data_start_time = currTime;
    }

    if (xPod.state != PodMachine::ST_IDLE) {
        // sample acceleration sensors (20 ms)
        if (currTime - accel_data_start_time >= 20) {
            float imu_val_raw = xPod.a;

            accel1 = imu_rand(imu_val_raw, IMU_1, elapsed_time);
            accel2 = imu_rand(imu_val_raw, IMU_2, elapsed_time);
            accel3 = imu_rand(imu_val_raw, IMU_3, elapsed_time);

            // update speed and position
            xPod.tick(elapsed_time);

            // reset timer
            accel_data_start_time = currTime;
        }
    }

    // create a sensor packet to send to the user
    m_buffer[0] = P_START;
    m_buffer[1] = 0;
    m_buffer[2] = xPod.state;
    pack_float_value(elapsed_time, m_buffer, 3);
    pack_float_value(accel1, m_buffer, 7);
    pack_float_value(accel2, m_buffer, 11);
    pack_float_value(accel3, m_buffer, 15);
    pack_float_value(temp1, m_buffer, 19);
    pack_float_value(temp2, m_buffer, 23);
    pack_float_value(temp3, m_buffer, 27);
    m_buffer[31] = P_END;

    // test
    if (currTime - data_timer >= 1000) {
        Serial.print("Recent Command: ");
        Serial.print(recentCommand);
        Serial.print("    Is Valid?: ");
        Serial.print(isValid);
        Serial.print("    State: ");
        Serial.print(xPod.state);
        Serial.print("    Velocity: ");
        Serial.print(xPod.v);
        Serial.print("    Distance: ");
        Serial.println(xPod.s);

        data_timer = currTime;
    }
}
