#include "Cosa/RTT.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"

#include "stdint.h"

#define P_START 0x56
#define P_END 0x23

void setup() {
    RTT::begin();
    Watchdog::begin();
    
    uart.begin(115200);
}

uint8_t num = 0;

void loop() {
    if (++num >= 256)
      num = 0;

    uint8_t pkt[3] = {P_START, num, P_END};

    uart.write(pkt, 3);

    Watchdog:::delay(5);
}
