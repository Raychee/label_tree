# ifndef _SGD_SVM_H
# define _SGD_SVM_H

# include <iostream>
# include <fstream>
# include "SGD.h"

typedef double        COMP_T;
// type of the value to be computed (parameters, training samples, etc)
typedef int           SUPV_T;
// type of the supervising information (classes, labels, etc)
typedef unsigned long N_DAT_T;
// type of the number of the data set
typedef unsigned int  DAT_DIM_T;
// type of the dimension of the data

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
    SGD_SVM(DAT_DIM_T    _d,
            COMP_T       _lambda          = 0.2,
            char         _verbosity       = 1,
            unsigned int _n_epoch         = 5,
            COMP_T       _eta0_1st_try    = 0.5,
            COMP_T       _eta0_try_factor = 2,
            COMP_T       _eta0            = 0);
    SGD_SVM(SGD_SVM& some);
    ~SGD_SVM();

    virtual SGD_SVM& train_one(COMP_T* dat, N_DAT_T i,
                               DAT_DIM_T d, N_DAT_T n, SUPV_T y);
    virtual SUPV_T   test_one(COMP_T* dat_i, DAT_DIM_T d) const;
    virtual COMP_T   compute_obj(COMP_T* dat, DAT_DIM_T d, N_DAT_T n, SUPV_T*y);
    virtual SGD_SVM& ostream_this(std::ostream& out);
    virtual SGD_SVM& ostream_param(std::ostream& out);

private:
    COMP_T*   w;        // weight vector of the linear svm classifier
                        // (length of "d")
    COMP_T    b;        // bias
    DAT_DIM_T d;        // dimension of the feature space
    COMP_T    lambda;   // regularizing term's coefficient

    virtual SGD_SVM* get_temp_dup() {
        return new SGD_SVM(*this);
    }
    virtual COMP_T compute_learning_rate() {
        return eta0 / (1 + lambda * eta0 * t);
    }

    COMP_T compute_margin(COMP_T* dat_i, DAT_DIM_T d) const;
    COMP_T compute_loss(COMP_T* dat_i, DAT_DIM_T d, SUPV_T y) const;
    // compute the loss for one input data

    // DEBUG
};

# endif
