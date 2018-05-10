#ifndef ARDUINO_CHALLENGE_TRACE_H
#define ARDUINO_CHALLENGE_TRACE_H

class UART;

class Trace {
public:
    void begin(UART *uart);

    Trace &operator<<(char *str);
};

static Trace trace;

#endif //ARDUINO_CHALLENGE_TRACE_H
