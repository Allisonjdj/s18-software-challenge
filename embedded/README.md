# Arduino Challenge
### Instructions
DO NOT allow people to see source code. Code should be compiled and loaded
onto an Arduino that is given to people.

### What people get to know
The pod is in the tube and has states
- IDLE
- READY
- ACCEL
- BRAKE
- STOPPED

which occur in that order.

The pod has 4 temperature sensors and 3 IMUs, with 16-bit IDs
```c++
uint16_t temp_ids[4] = {0xff71, 0xff72, 0xff73, 0xff74};
uint16_t imu_ids[4]  = {0xff51, 0xff52, 0xff53};
```

IMUs all measure the pod's acceleration in `m/s/s`, the temperature sensors measure
- T1 and T2: the Left and Right EC Brake temperatures in Celsius
- T3: the motor temperature in Celsius
- T4: the embedded chip temperature in Celsius

The pod also emits its state as a heartbeart using ID `0xff00`

The data is transmitted over UART using a packet
```
[8][16][32][32][8]
```
In order shown above, the bytes are
- Start byte `0x56`
- Packet origin ID, 2 bytes, i.e. the Sensor ID or the heartbeat packet
- Packet data, which is `uint32_t` for State packet and `float32` otherwise
- Time in seconds, as a `float32`
- End byte `0x23`

Commands can be given to the Pod as a single byte, these are
- INIT `0x01`, transition from IDLE->READY
- DEINIT `0x02`, transition from READY->IDLE
- START `0x03`, transition from READY->ACCEL and the motor starts running
- BRAKE `0x04`, transition from ACCEL->BRAKE, motor shuts down and brakes actuate

The pod will automatically switch to STOPPED when its velocity
reaches zero during braking.

The tube is 2000 metres long. The pod's max speed is about 95 m/s. The pod
requires more than 200 m of lead distance to brake from max speed.
