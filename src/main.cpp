#include <Cosa/RTT.hh>
#include <Cosa/TWI.hh>
#include <Cosa/OutputPin.hh>
#include <Cosa/Watchdog.hh>
#include <Cosa/Trace.hh>
#include <Cosa/UART.hh>
#include <Cosa/Memory.h>

#include <wlib/stl/Bitset.h>

#include "statehandler.h"
#include "addresses.h"
#include "data_provider.h"


// A Planet X Pod Communicator is the pod software that let's the programmers
// communicate with the Planet X Pod. It provides data from different sensors and accepts commands
class XPodComs : public TWI::Slave {
private:
    // Buffer for request and response
    static const uint8_t BUF_MAX = 8;
    uint8_t m_buf[BUF_MAX]{};

public:
    // Construct the XPod slave device
    XPodComs() : TWI::Slave(SLAVE_ADDRESS) {
        write_buf(m_buf, sizeof(m_buf));
        read_buf(m_buf, sizeof(m_buf));
    }

    // Request handler; events from incoming requests
    virtual void on_request(void *buf, size_t size) override;
};

// The TWI XPodComs device
static XPodComs xPodComs;
static PodMachine xPod;
static float32_t start_time = 0;
static float32_t t1_brake = 10;
static float32_t t3_mot = 10;
static float32_t t4_chip = 10;

static double prev_time = 0;


void brake_t(float32_t elapsed_time) {
    double dt = elapsed_time - prev_time;
    if (xPod.state == PodMachine::ST_ACCEL) {
        t1_brake += dt * 0.2f / (t1_brake + 5) * t1_brake;
    } else if (xPod.state == PodMachine::ST_BRAKE) {
        t1_brake += dt * 2;
    } else {
        if (t1_brake >= 10) {
            t1_brake -= 0.5 * dt * (t1_brake + 50) / t1_brake;
        }
    }
}

void mot_t(float32_t elapsed_time) {
    double dt = elapsed_time - prev_time;
    if (xPod.state == PodMachine::ST_ACCEL) {
        t3_mot += dt / (t3_mot + 5) * t3_mot;
    } else {
        if (t3_mot >= 10) {
            t3_mot -= 0.5 * dt * (t3_mot + 30) / t3_mot;
        }
    }
}

void chip_t(float32_t elapsedTime) {
    t4_chip = 10 + static_cast<float32_t>(log(elapsedTime + 1)) * 8;
}

void XPodComs::on_request(void *buf, size_t size) {
    float32_t curr_time = RTT::millis() / 1000.0;
    float32_t elapsed_time = curr_time - start_time;

    uint8_t serPack[10];

    // if pod is crashed, we do not send Crashed packet to both server and programmers
    if (xPod.s >= 2000) {
        make_packet_server(POD_CRASHED, start_time, curr_time, serPack);
        make_pack_int(0, start_time, curr_time, m_buf, POD_CRASHED);

        return;
    }

    if (size > 0) {
        auto useBuf = static_cast<uint8_t* >(buf);
        uint8_t start = *(useBuf);
        uint8_t end = *(useBuf + 1 + 1 + 2 + 2);

        // check if the address is correct
        if (start == P_START && end == P_END) {
            // get a command
            uint32_t cmd = *(useBuf + 1);
            bool correct = true;

            switch (cmd) {
                case INIT:
                    xPod.init(elapsed_time);
                    break;
                case DEINIT:
                    xPod.deinit(elapsed_time);
                    break;
                case START:
                    xPod.start(elapsed_time);
                    break;
                case BRAKE:
                    xPod.brake(elapsed_time);
                    break;
                default:
                    correct = false;
                    break;
            }

            if (!correct) {
                // packet received had invalid command id
                make_packet_server(INVALID_COMMAND, start_time, curr_time, serPack);
            } else {
                // packet received had correct command id, hence gucci
                make_packet_server(cmd, start_time, curr_time, serPack);
            }
        } else {
            // packet start or end was wrong
            make_packet_server(INVALID_PACKET, start_time, curr_time, serPack);
        }

        uart.write(serPack, 10);
    }

    double imu_val_raw = xPod.a;

    // provide sensor data continuously
    memset(m_buf, 0, 12);
    // State
    make_pack_int(xPod.state, start_time, curr_time, m_buf, STATE_ID);
    trace << m_buf;
    // IMU 1
    make_pack_float(imu_rand(imu_val_raw, IMU_1, elapsed_time), start_time, curr_time, m_buf, IMU_1);
    trace << m_buf;
    // IMU 2
    make_pack_float(imu_rand(imu_val_raw, IMU_2, elapsed_time), start_time, curr_time, m_buf, IMU_2);
    trace << m_buf;
    // IMU 3
    make_pack_float(imu_rand(imu_val_raw, IMU_3, elapsed_time), start_time, curr_time, m_buf, IMU_3);
    trace << m_buf;
    // TEMP 1
    make_pack_float(temp_rand(t1_brake, TEMP_1, elapsed_time), start_time, curr_time, m_buf, TEMP_1);
    trace << m_buf;
    // TEMP 2
    make_pack_float(temp_rand(t1_brake, TEMP_2, elapsed_time), start_time, curr_time, m_buf, TEMP_2);
    trace << m_buf;
    // TEMP 3
    make_pack_float(temp_rand(t3_mot, TEMP_3, elapsed_time), start_time, curr_time, m_buf, TEMP_3);
    trace << m_buf;
    // TEMP 4
    make_pack_float(temp_rand(t4_chip, TEMP_4, elapsed_time), start_time, curr_time, m_buf, TEMP_4);
    trace << m_buf;

    // Whoops pod crashed
    if (xPod.s >= 2000) {
        return;
    }

    xPod.tick(elapsed_time);
    brake_t(elapsed_time);
    mot_t(elapsed_time);
    chip_t(elapsed_time);
    prev_time = elapsed_time;
}

void setup() {
    RTT::begin();

    start_time = RTT::millis() / 1000.0;

    // Start trace output stream on the serial port
    uart.begin(9600);
    trace.begin(&uart, PSTR("CosaTWISlave: started"));

    // Check amount of free memory and size of classes
    TRACE(free_memory());
    TRACE(sizeof(xPod));
    TRACE(sizeof(OutputPin));

    // Start the watchdog ticks counter
    Watchdog::begin();

    // Start the TWI echo device
    xPodComs.begin();
}

void loop() {
    Event::service();
}
