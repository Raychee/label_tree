# include <iomanip>
# include "SGD_SVM.h"

SGD_SVM::SGD_SVM(DAT_DIM_T    _d,
                 COMP_T       _lambda,
                 char         _verbosity,
                 unsigned int _n_epoch,
                 COMP_T       _eta0_1st_try,
                 COMP_T       _eta0_try_factor,
                 COMP_T       _eta0)
        :SGD<COMP_T, SUPV_T, DAT_DIM_T, N_DAT_T>(_verbosity,
                                                 _eta0,
                                                 _n_epoch,
                                                 _eta0_1st_try,
                                                 _eta0_try_factor),
         b(0),
         d(_d),
         lambda(_lambda) {
    w = new COMP_T[_d];
    std::memset(w, 0, sizeof(COMP_T) * _d);
}

SGD_SVM::SGD_SVM(SGD_SVM& some)
                :SGD<COMP_T, SUPV_T, DAT_DIM_T, N_DAT_T>(some),
                 b(some.b),
                 d(some.d),
                 lambda(some.lambda) {
    w = new COMP_T[d];
    std::memcpy(w, some.w, sizeof(COMP_T) * d);
}

SGD_SVM::~SGD_SVM() {
    delete[] w;
}

SGD_SVM& SGD_SVM::train_one(COMP_T* dat, N_DAT_T i,
                            DAT_DIM_T d, N_DAT_T n, SUPV_T y) {
    COMP_T* dat_i = dat + d * i;
    COMP_T loss = compute_loss(dat_i, d, y);
    if (loss > 0) {
        for (DAT_DIM_T i = 0; i < d; ++i) {     // update w
            w[i] = (1 - lambda * eta) * w[i] + eta * y * dat_i[i];
            // w = (1 - lambda * eta) * w + eta * y_i * x_i
        }
        b += eta * y;                           // update b
    }
    else {
        for (DAT_DIM_T i = 0; i < d; ++i) {     // update w
            w[i] *= 1 - lambda * eta;
            // w = (1 - lambda * eta) * w
        }                                       // update b: do nothing
    }
    return *this;
}

inline SUPV_T SGD_SVM::test_one(COMP_T* dat_i, DAT_DIM_T d) const {
    COMP_T score = compute_margin(dat_i, d);
    return score > 0 ? 1 : -1;
}

COMP_T SGD_SVM::compute_obj(COMP_T* dat, DAT_DIM_T d, N_DAT_T n, SUPV_T* y) {
    COMP_T  loss   = 0;
    COMP_T  reg    = 0;
    COMP_T* dat_i  = dat;
    for (N_DAT_T i = 0; i < n; ++i) {       // sum(i=1:n)(loss_i)
        loss += compute_loss(dat_i, d, y[i]);
        dat_i += d;
    }
    for (DAT_DIM_T i = 0; i < d; ++i) {     // w' * w
        reg += w[i] * w[i];
    }
    reg *= 0.5 * lambda;                    // 0.5 * lambda * w' * w
    return reg + loss;
}

COMP_T SGD_SVM::compute_margin(COMP_T* dat_i, DAT_DIM_T d) const {
    COMP_T marg = 0;
    for (DAT_DIM_T i = 0; i < d; ++i) {     // w' * x_i
        marg += dat_i[i] * w[i];
    }
    marg += b;                              // w' * x_i + b
    return marg;
}

inline COMP_T SGD_SVM::compute_loss(COMP_T* dat_i, DAT_DIM_T d, SUPV_T y) const {
    COMP_T loss = 1 - y * (compute_margin(dat_i, d));
    // 1 - y_i * (w' * x_i + b)
    return loss < 0 ? 0 : loss;
}

SGD_SVM& SGD_SVM::ostream_this(std::ostream& out) {
    out << "w = ";
    for (DAT_DIM_T i = 0; i < d; ++i) {
        out << std::setw(16) << w[i];
    }
    out << "\nb = " << std::setw(16) << b;
    return *this;
}

SGD_SVM& SGD_SVM::ostream_param(std::ostream& out) {
    for (DAT_DIM_T i = 0; i < d; ++i) {
        out << w[i] << " ";
    }
    out << b;
    return *this;
}