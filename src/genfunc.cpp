#include "genfunc.h"

#include <math.h>

struct sigmoid {
    constexpr sigmoid(
        double L,
        double k,
        double b,
        double p
    );

    double operator()(double t) const;

private:
    double m_L;
    double m_k;
    double m_b;
    double m_p;
};

struct gamma_t {
    constexpr gamma_t(
        double L,
        double k,
        double b
    );

    double operator()(double t) const;

private:
    double m_L;
    double m_k;
    double m_b;
};

constexpr sigmoid::sigmoid(
    double L,
    double k,
    double b,
    double p) :
    m_L(L),
    m_k(k),
    m_b(b),
    m_p(p) {}

double sigmoid::operator()(double t) const {
    return m_L / (1 + exp(-m_k * (t - m_b))) - m_p;
}

constexpr gamma_t::gamma_t(
    double L,
    double k,
    double b) :
    m_L(L),
    m_k(k),
    m_b(b) {}

double gamma_t::operator()(double t) const {
    return m_k * m_L * exp(m_k * (t - m_b)) / pow(exp(m_k * (t - m_b)) + 1, 2);
}

namespace det_ {
    static constexpr sigmoid std_sigmoid(95, 2.3, 3.5, 0.3);

    static constexpr gamma_t std_gamma(95, 2.3, 3.5);

    static double brake_curve_disengage(double t) {
        return -116.350804153 * pow(t * 0.1, 4) * exp(-t * 0.5);
    }

    static double brake_curve(double t) {
        return -129.555 * pow(t * 0.1, 1.1) * exp(-t * 0.14285714285);
    }
}

// tube 2000m

// accelerate 7 seconds to reach 95 m/s travel 332.5m
// coast for 10 seconds to reach 90 m/s travel 925m
// brake for 5 seconds to reach 0 m/s travel 225m

double phase_idle::operator()(double) {
    return 0;
}

double phase_accel::operator()(double t) {
    return det_::std_gamma(t);
}

double phase_coast::operator()(double t) {
    return det_::brake_curve_disengage(t);
}

double phase_brake::operator()(double t) {
    return det_::brake_curve(t);
}

double phase_stopped::operator()(double) {
    return 0;
}
