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
    static constexpr gamma_t std_gamma(340, 0.37, 16.7);

    static double brake_curve_disengage(double t) {
        return -116.350804153 * pow(t * 0.1, 4) * exp(-t * 0.5);
    }

    static double brake_curve(double t) {
        return -83 * pow(t * 0.27027, 4.3) * exp(-t * 0.52631578947);
    }
}

// tube 2000m

// accelerate 25 seconds to reach 340 m/s
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