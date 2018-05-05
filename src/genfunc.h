#ifndef GEN_FUNC_H
#define GEN_FUNC_H

struct gen_func {
    virtual double operator()(double t) = 0;
};

struct phase_idle : public gen_func {
    double operator()(double t) override;
};

struct phase_accel : public gen_func {
    double operator()(double t) override;
};

struct phase_coast : public gen_func {
    double operator()(double t) override;
};

struct phase_brake : public gen_func {
    double operator()(double t) override;
};

struct phase_stopped : public gen_func {
    double operator()(double t) override;
};

#endif
