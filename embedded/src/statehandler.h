#ifndef S18_SOFTWARE_CHALLENGE_STATEHANDLER_H
#define S18_SOFTWARE_CHALLENGE_STATEHANDLER_H

#include <wlib/fsm/StateMachine.h>
#include "genfunc.h"

using namespace wlp;

#ifndef ASSERT_TRUE
#define ASSERT_TRUE
#endif

class PodData : public wlp::EventData {
public:
    double time{};
};

class PodMachine : public wlp::StateMachine {
public:
    double start_time{};
    double a;
    double v;
    double s;
    double last_t;

    PodMachine() :
            StateMachine(States::ST_MAX_STATES, States::ST_IDLE),
            state(ST_IDLE),
            a(0),
            v(0),
            s(0),
            last_t(0) {}

    void ready(double start_time) {
        PodData data;
        data.time = start_time;
        BEGIN_TRANSITION_MAP
                        TRANSITION_MAP_ENTRY(ST_READY)          // ST_IDLE
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_READY
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_ACCEL
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_COAST
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_BRAKE
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_STOPPED
        END_TRANSITION_MAP(&data, PodData)
    }

    void start(double start_time) {
        PodData data;
        data.time = start_time;
        BEGIN_TRANSITION_MAP
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_IDLE
                        TRANSITION_MAP_ENTRY(ST_ACCEL)          // ST_READY
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_ACCEL
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_COAST
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_BRAKE
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_STOPPED
        END_TRANSITION_MAP(&data, PodData)
    }

    void coast(double start_time) {
        PodData data;
        data.time = start_time;
        BEGIN_TRANSITION_MAP
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_IDLE
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_READY
                        TRANSITION_MAP_ENTRY(ST_COAST)          // ST_ACCEL
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_COAST
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_BRAKE
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_STOPPED
        END_TRANSITION_MAP(&data, PodData)
    }

    void brake(double start_time) {
        PodData data;
        data.time = start_time;
        BEGIN_TRANSITION_MAP
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_IDLE
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_READY
                        TRANSITION_MAP_ENTRY(ST_BRAKE)          // ST_ACCEL
                        TRANSITION_MAP_ENTRY(ST_BRAKE)          // ST_COAST
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_BRAKE
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_STOPPED
        END_TRANSITION_MAP(&data, PodData)
    }

    void stop() {
        PodData data;
        data.time = 0;
        BEGIN_TRANSITION_MAP
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_IDLE
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_READY
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_ACCEL
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_COAST
                        TRANSITION_MAP_ENTRY(ST_STOPPED)        // ST_BRAKE
                        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_STOPPED
        END_TRANSITION_MAP(&data, PodData)
    }

    void noop(double) {}

    void compile(double t) {
        if (last_t > t) {
            last_t = t;
        }
        double dt = t - last_t;
        v += a * dt;
        s += v * dt;
        last_t = t;
        if (v <= 0.035 && state == ST_BRAKE) {
            stop();
        }
    }

    void s_accel(double time) {
        double t = time - start_time;
        a = p_accel(t);
        compile(t);
    }

    void s_brake(double time) {
        double t = time - start_time;
        a = p_brake(t);
        compile(t);
    }

    void tick(double time) {
        if (state == ST_BRAKE) { s_brake(time); }
        else if (state == ST_ACCEL) { s_accel(time); }
        else { noop(time); }
    }

    enum States {
        ST_IDLE = 0,
        ST_READY,
        ST_ACCEL,
        ST_COAST,
        ST_BRAKE,
        ST_STOPPED,
        ST_MAX_STATES
    };

    States state;

private:
    phase_accel p_accel;
    phase_brake p_brake;

    STATE_DECLARE(PodMachine, Idle, PodData);

    STATE_DECLARE(PodMachine, Ready, PodData);

    STATE_DECLARE(PodMachine, Accel, PodData);

    STATE_DECLARE(PodMachine, Coast, PodData);

    STATE_DECLARE(PodMachine, Brake, PodData);

    STATE_DECLARE(PodMachine, Stopped, PodData);

BEGIN_STATE_MAP
                        STATE_MAP_ENTRY(&Idle)
                        STATE_MAP_ENTRY(&Ready)
                        STATE_MAP_ENTRY(&Accel)
                        STATE_MAP_ENTRY(&Coast)
                        STATE_MAP_ENTRY(&Brake)
                        STATE_MAP_ENTRY(&Stopped)
    END_STATE_MAP
};

STATE_DEFINE(PodMachine, Idle, PodData) {
    state = ST_IDLE;
    a = 0;
    v = 0;
    s = 0;
}

STATE_DEFINE(PodMachine, Ready, PodData) {
    state = ST_READY;
    a = 0;
    v = 0;
    s = 0;
}

STATE_DEFINE(PodMachine, Accel, PodData) {
    state = ST_ACCEL;
    start_time = data->time;
    a = 0;
    v = 0;
    s = 0;
}

STATE_DEFINE(PodMachine, Coast, PodData) {
    state = ST_COAST;
}

STATE_DEFINE(PodMachine, Brake, PodData) {
    state = ST_BRAKE;
    start_time = data->time;
}

STATE_DEFINE(PodMachine, Stopped, PodData) {
    state = ST_STOPPED;
    a = 0;
    v = 0;
}

#endif //S18_SOFTWARE_CHALLENGE_STATEHANDLER_H
