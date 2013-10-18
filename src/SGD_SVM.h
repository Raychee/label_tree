# ifndef _SGD_SVM_H
# define _SGD_SVM_H

# include <iostream>
# include <fstream>
# include "my_typedefs.h"
# include "SGD.h"


class SGD_SVM : public SGD<COMP_T, SUPV_T, DAT_DIM_T, N_DAT_T> {
// 2-class linear SVM
// n samples, d dimensions
// parameters: w, b, lambda
// obj    = 0.5 * lambda * w' * w + sum(i=1:n)(loss_i)
// loss_i = max{0, 1 - y_i * (w' * x_i + b)}
// upate rules:
// w = (1 - lambda * eta) * w + eta * y_i * x_i     (if loss_i > 0)
// w = (1 - lambda * eta) * w                       (if loss_i = 0)
// b = b + eta * y_i                                (if loss_i > 0)
// b = b                                            (if loss_i = 0)
public:
    SGD_SVM(DAT_DIM_T    _d               = 0,
            COMP_T       _lambda          = 0.01,
            char         _verbosity       = 1,
            unsigned int _n_epoch         = 5,
            COMP_T       _eta0_1st_try    = 0.1,
            COMP_T       _eta0_try_factor = 3,
            COMP_T       _eta0            = 0);
    SGD_SVM(SGD_SVM& some);
    ~SGD_SVM();

    COMP_T   regul_coef() const { return lambda; }
    SGD_SVM& regul_coef(COMP_T _lambda) { lambda = _lambda; return *this; }
    
    virtual COMP_T   compute_obj(COMP_T* dat, N_DAT_T n, SUPV_T*y);
    virtual SGD_SVM& ostream_this(std::ostream& out);
    virtual SGD_SVM& ostream_param(std::ostream& out);

private:
    COMP_T*   w;        // weight vector of the linear svm classifier
                        // (length of "dim")
    COMP_T    b;        // bias
    COMP_T    lambda;   // regularizing term's coefficient

    virtual void     rand_data_index(N_DAT_T* index, SUPV_T* y, N_DAT_T n);
    virtual SGD_SVM& train_one(COMP_T* dat, N_DAT_T i, N_DAT_T n, SUPV_T* y);
    virtual SUPV_T   test_one(COMP_T* dat_i) const;
    virtual COMP_T   compute_learning_rate() {
        return eta0 / (1 + lambda * eta0 * t);
    }
    COMP_T           compute_margin(COMP_T* dat_i) const;
    COMP_T           compute_loss(COMP_T* dat_i, SUPV_T y) const;
    // compute the loss for one input data

    virtual SGD_SVM* get_temp_dup() {
        return new SGD_SVM(*this);
    }
};

# endif
