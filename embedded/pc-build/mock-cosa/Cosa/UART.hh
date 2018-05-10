#ifndef ARDUINO_CHALLENGE_UART_H
#define ARDUINO_CHALLENGE_UART_H

class UART {
public:
    void begin(int baud_rate);

    int getchar();
};

static UART uart;

#endif //ARDUINO_CHALLENGE_UART_H
