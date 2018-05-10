#include "app.h"
#include "loop.h"
#include "scene.h"

#include <QLabel>

enum PodState {
    IDLE,
    READY,
    ACCEL,
    COAST,
    BRAKE,
    STOPPED,
    NUM_STATES
};

static const char *names[] = {
    "Idle",
    "Ready",
    "Accel",
    "Coast",
    "Brake",
    "Stopped"
};

void time_label(QLabel *l, float time) {
    QString str;
    str.sprintf("Time:  %.4f", time);
    l->setText(str);
}

void state_label(QLabel *l, int state) {
    QString str;
    str.sprintf("State: %s", names[state]);
    l->setText(str);
}

void temp_label(QLabel *l, float temp, int id) {
    QString str;
    str.sprintf("Temp %i: %.3f C", id - 0xff70, temp);
    l->setText(str);
}

void imu_label(QLabel *l, float acc, int id) {
    QString str;
    str.sprintf("Accel %i: %.3f m/s/s", id - 0xff50, acc);
    l->setText(str);
}

void vel_label(QLabel *l, float vel) {
    QString str;
    str.sprintf("Speed: %.3f m/s", vel);
    l->setText(str);
}

void pos_label(QLabel *l, float pos) {
    QString str;
    str.sprintf("Distance: %.3f m", pos);
    l->setText(str);
}

void acc_label(QLabel *l, float acc) {
    QString str;
    str.sprintf("Accel: %.3f m/s/s", acc);
    l->setText(str);
}

App::App(int, char **) :
    m_time_label(new QLabel(this)),
    m_state_label(new QLabel(this)),
    m_vel_label(new QLabel(this)),
    m_acc_label(new QLabel(this)),
    m_pos_label(new QLabel(this)),
    s(new Scene(this)),

    m_pod_time(0.0f),
    m_pod_state(static_cast<int>(IDLE)) {

    setFixedSize(800, 600);

    for (int i = 0; i < 3; ++i) {
        m_imu_label[i] = new QLabel(this);
        m_imu_label[i]->setGeometry(10, 50 + i * 20, 150, 20);
        m_temp_label[i] = new QLabel(this);
        m_temp_label[i]->setGeometry(10, 110 + i * 20, 150, 20);
        temp_label(m_temp_label[i], 10.0, i + 0xff71);
        imu_label(m_imu_label[i], 0.0, i + 0xff51);
    }
    m_temp_label[3] = new QLabel(this);
    m_temp_label[3]->setGeometry(10, 170, 150, 20);
    temp_label(m_temp_label[3], 10.0, 0xff74);

    time_label(m_time_label, m_pod_time);
    state_label(m_state_label, m_pod_state);
    m_time_label->setGeometry(10, 10, 150, 20);
    m_state_label->setGeometry(10, 30, 150, 20);

    m_acc_label->setGeometry(160, 30, 150, 20);
    acc_label(m_acc_label, 0);

    m_vel_label->setGeometry(160, 50, 150, 20);
    vel_label(m_vel_label, 0);

    m_pos_label->setGeometry(160, 70, 150, 20);
    pos_label(m_pos_label, 0);


    s->setGeometry(10, 190, 780, 400);

    setup();

    m_timer.start(1, this);
}

App::~App() {
    delete m_time_label;
    delete m_state_label;
    delete m_vel_label;
    delete m_acc_label;
    delete m_pos_label;
    for (int i = 0; i < 3; ++i) {
        delete m_imu_label[i];
        delete m_temp_label[i];
    }
    delete m_temp_label[3];
    delete s;
}

void App::receive_state(float time, int state) {
    m_pod_time = time;
    time_label(m_time_label, time);
    if (state != m_pod_state) {
        m_pod_state = state;
        state_label(m_state_label, state);
    }
}

void App::receive_data(float time, float data, int origin_id) {
    if (0xff70 < origin_id && origin_id <= 0xff74) {
        int i = origin_id - 0xff71;
        temp_label(m_temp_label[i], data, origin_id);
    } else if (0xff50 < origin_id && origin_id <= 0xff53) {
        int i = origin_id - 0xff51;
        imu_label(m_imu_label[i], data, origin_id);
    } else if (origin_id == 5) {
        vel_label(m_vel_label, data);
    } else if (origin_id == 6) {
        acc_label(m_acc_label, data);
    } else if (origin_id == 7) {
        pos_label(m_pos_label, data);
    }

    if (origin_id == 0xff73) {
        s->speed(time, data);
    }
}

void App::timerEvent(QTimerEvent *) {
    loop();
}
