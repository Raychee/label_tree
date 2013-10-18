# include <iomanip>
# include "SGD_SVM.h"

SGD_SVM::SGD_SVM(DAT_DIM_T    _d,
                 COMP_T       _lambda,
                 char         _verbosity,
                 unsigned int _n_epoch,
                 COMP_T       _eta0_1st_try,
                 COMP_T       _eta0_try_factor,
                 COMP_T       _eta0)
        :SGD<COMP_T, SUPV_T, DAT_DIM_T, N_DAT_T>(_d,
                                                 _verbosity,
                                                 _eta0,
                                                 _n_epoch,
                                                 _eta0_1st_try,
                                                 _eta0_try_factor),
         b(0),
         lambda(_lambda) {
    if (!_d) w = NULL;
    else {
        w = new COMP_T[_d];
        std::memset(w, 0, sizeof(COMP_T) * _d);
    }
}

SGD_SVM::SGD_SVM(SGD_SVM& some)
                :SGD<COMP_T, SUPV_T, DAT_DIM_T, N_DAT_T>(some),
                 b(some.b),
                 lambda(some.lambda) {
    if (some.w) {
        w = new COMP_T[dim];
        std::memcpy(w, some.w, sizeof(COMP_T) * dim);
    }
    else w = NULL;
}

SGD_SVM::~SGD_SVM() {
    delete[] w;
}

void SGD_SVM::rand_data_index(N_DAT_T* index, SUPV_T* y, N_DAT_T n) {
// WARNING: current implementation can generate random integers only less than 
// maximum value of type "int" because of the restrictions of rand() from C 
// standard library.
    SUPV_T s_pos_dat = 0, s_neg_dat = 0;
    COMP_T p_pos_dat = 0, p_neg_dat = 0;
    COMP_T st_pos_dat, st_neg_dat;
    std::forward_list<N_DAT_T> candidate_pos_dat, candidate_neg_dat;
    for (N_DAT_T i = 0; i < n; ++i) {
        if (y[i] > 0) {
            ++s_pos_dat;
            candidate_pos_dat.push_front(i);
        }
        else {
            ++s_neg_dat;
            candidate_neg_dat.push_front(i);
        }
    }
    if (s_pos_dat > s_neg_dat) {
        st_pos_dat = 1;
        st_neg_dat = (COMP_T)s_neg_dat / s_pos_dat;
    }
    else {
        st_neg_dat = 1;
        st_pos_dat = (COMP_T)s_pos_dat / s_neg_dat;
    }
    N_DAT_T count = 0;
    while (count < n) {
        p_pos_dat += st_pos_dat;
        if (p_pos_dat >= 1) {
            if (!candidate_pos_dat.empty()) {
                N_DAT_T rand_i = std::rand() % s_pos_dat;
                typename std::forward_list<N_DAT_T>::iterator it0 = 
                    candidate_pos_dat.before_begin();
                typename std::forward_list<N_DAT_T>::iterator it1 = 
                    candidate_pos_dat.begin();
                for (N_DAT_T i = 0; i < rand_i; ++i) {
                    ++it0;
                    ++it1;
                }
                index[count++] = *it1;
                candidate_pos_dat.erase_after(it0);
                --s_pos_dat;
            }
            --p_pos_dat;
        }
        p_neg_dat += st_neg_dat;
        if (p_neg_dat >= 1) {
            if (!candidate_neg_dat.empty()) {
                N_DAT_T rand_i = std::rand() % s_neg_dat;
                typename std::forward_list<N_DAT_T>::iterator it0 = 
                    candidate_neg_dat.before_begin();
                typename std::forward_list<N_DAT_T>::iterator it1 = 
                    candidate_neg_dat.begin();
                for (N_DAT_T i = 0; i < rand_i; ++i) {
                    ++it0;
                    ++it1;
                }
                index[count++] = *it1;
                candidate_neg_dat.erase_after(it0);
                --s_neg_dat;
            }
            --p_neg_dat;
        }
    }
}

SGD_SVM& SGD_SVM::train_one(COMP_T* dat, N_DAT_T i, N_DAT_T n, SUPV_T* y) {
    COMP_T* dat_i = dat + dim * i;
    SUPV_T  y_i   = y[i];
    COMP_T loss = compute_loss(dat_i, y_i);
    if (loss > 0) {
        for (DAT_DIM_T i = 0; i < dim; ++i) {     // update w
            w[i] = (1 - lambda * eta) * w[i] + eta * y_i * dat_i[i];
            // w = (1 - lambda * eta) * w + eta * y_i * x_i
        }
        b += eta * y_i;                           // update b
    }
    else {
        for (DAT_DIM_T i = 0; i < dim; ++i) {     // update w
            w[i] *= 1 - lambda * eta;
            // w = (1 - lambda * eta) * w
        }                                       // update b: do nothing
    }
    return *this;
}

inline SUPV_T SGD_SVM::test_one(COMP_T* dat_i) const {
    COMP_T score = compute_margin(dat_i);
    return score > 0 ? 1 : -1;
}

COMP_T SGD_SVM::compute_obj(COMP_T* dat, N_DAT_T n, SUPV_T* y) {
    COMP_T  loss   = 0;
    COMP_T  reg    = 0;
    COMP_T* dat_i  = dat;
    for (N_DAT_T i = 0; i < n; ++i) {       // sum(i=1:n)(loss_i)
        loss += compute_loss(dat_i, y[i]);
        dat_i += dim;
    }
    for (DAT_DIM_T i = 0; i < dim; ++i) {     // w' * w
        reg += w[i] * w[i];
    }
    reg *= 0.5 * lambda;                    // 0.5 * lambda * w' * w
    return reg + loss;
}

COMP_T SGD_SVM::compute_margin(COMP_T* dat_i) const {
    COMP_T marg = 0;
    for (DAT_DIM_T i = 0; i < dim; ++i) {     // w' * x_i
        marg += dat_i[i] * w[i];
    }
    marg += b;                              // w' * x_i + b
    return marg;
}

inline COMP_T SGD_SVM::compute_loss(COMP_T* dat_i, SUPV_T y) const {
    COMP_T loss = 1 - y * (compute_margin(dat_i));
    // 1 - y_i * (w' * x_i + b)
    return loss < 0 ? 0 : loss;
}

SGD_SVM& SGD_SVM::ostream_this(std::ostream& out) {
    out << "w = ";
    for (DAT_DIM_T i = 0; i < dim; ++i) {
        out << std::setw(16) << w[i];
    }
    out << "\nb = " << std::setw(16) << b;
    return *this;
}

SGD_SVM& SGD_SVM::ostream_param(std::ostream& out) {
    for (DAT_DIM_T i = 0; i < dim; ++i) {
        out << w[i] << " ";
    }
    out << b;
    return *this;
}