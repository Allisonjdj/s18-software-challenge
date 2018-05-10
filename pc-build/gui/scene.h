#ifndef ARDUINO_CHALLENGE_SCENE_H
#define ARDUINO_CHALLENGE_SCENE_H

#include <QWidget>

#include <deque>

class App;

struct _ffp {
    _ffp(float tt, float td);

    float t;
    float d;
};

class Scene : public QWidget {
    Q_OBJECT

public:
    explicit Scene(App *parent);

    void speed(float t, float s);

protected:
    void paintEvent(QPaintEvent *ev) override;

    std::deque<_ffp> speeds;
};

#endif //ARDUINO_CHALLENGE_SCENE_H
