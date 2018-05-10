#ifndef ARDUINO_CHALLENGE_APP_H
#define ARDUINO_CHALLENGE_APP_H

#include <QMainWindow>
#include <QBasicTimer>

#include <memory>

class Scene;
class QLabel;
class QVBoxLayout;

class App : public QMainWindow {
Q_OBJECT

public:
    App(int argc, char *argv[]);

    ~App() override;

    void receive_state(float time, int state);

    void receive_data(float time, float data, int origin_id);

private:
    void timerEvent(QTimerEvent *ev) override;

    QLabel *m_time_label;
    QLabel *m_state_label;
    QLabel *m_imu_label[3];
    QLabel *m_temp_label[4];
    QLabel *m_vel_label;
    QLabel *m_acc_label;
    QLabel *m_pos_label;
    Scene *s;

    float m_pod_time;
    int m_pod_state;

    QBasicTimer m_timer;
};

extern App *g_app;

#endif //ARDUINO_CHALLENGE_APP_H
