#include "scene.h"
#include "app.h"

#include <QPainter>

_ffp::_ffp(float tt, float td) {
    t = tt;
    d = td;
}

Scene::Scene(App *parent) :
    QWidget(parent) {}

void Scene::paintEvent(QPaintEvent *) {
    QPainter p(this);

    p.setPen(Qt::black);
    p.drawRect(0, 0, 779, 399);

    std::size_t size = speeds.size();
    if (size > 10000) {
        size = 10000;
    }
    float max_t = speeds[size - 1].t;
    float min_t = speeds[0].t;
    float max_v = 0;
    float min_v = std::numeric_limits<float>::max();

    for (std::size_t i = 0; i < size; ++i) {
        float s = speeds[i].d;
        if (s < min_v) {
            min_v = s;
        } else if (s > max_v) {
            max_v = s;
        }
    }

    if (min_v > 0) {
        min_v = 0;
    }
    if (max_v < 70) {
        max_v = 70;
    }

    float trange = max_t - min_t;
    float vrange = max_v - min_v;

    float trangei = 1.0f / trange;
    float vrangei = 1.0f / vrange;

    float w = 779 - 20;
    float h = 399 - 20;

    float ts_p = speeds[0].t - min_t;
    ts_p *= trangei;
    float vs_p = speeds[0].d - min_v;
    vs_p *= vrangei;
    ts_p *= w;
    vs_p *= h;
    ts_p += 10;
    vs_p += 10;
    vs_p = h - vs_p;

    p.setPen(Qt::blue);
    for (std::size_t i = 1; i < size; ++i) {
        float ts = speeds[i].t - min_t;
        ts *= trangei;
        float vs = speeds[i].d - min_v;
        vs *= vrangei;
        ts *= w;
        vs *= h;
        ts += 10;
        vs += 10;
        vs = h - vs + 20;
        p.drawLine(QLineF(ts_p, vs_p, ts, vs));
        ts_p = ts;
        vs_p = vs;
    }
}

void Scene::speed(float t, float s) {
    speeds.emplace_back(t, s);

    if (speeds.size() > 10000) {
        speeds.pop_front();
    }

    update();
}
