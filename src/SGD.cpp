# include <cstdlib>
# include <ctime>
# include <forward_list>
# include "SGD.h"

using std::rand;
using std::srand;
using std::time;
using std::forward_list;
using std::next;

SGD::SGD(N_PARAM_T _n_param, N_CPARAM_T _n_cparam, char _verbosity,
         COMP_T _eta0, unsigned int _n_epoch):
         verbosity(_verbosity),
         n_param(_n_param),
         n_cparam(_n_cparam),
         eta0(_eta0),
         n_epoch(_n_epoch),
         t(0) {
    param = new COMP_T[_n_param];
    cparam = new COMP_T[_n_cparam];
    update_rule = new F_UPDATE[_n_param];
    set_update_rule();
}

SGD::~SGD() {
    delete[] param;
    delete[] cparam;
    delete[] update_rule;
}

void SGD::train(COMP_T* dat, DAT_DIM_T d, N_DAT_T n, SUPV_T* y) {
    N_DAT_T* rand_dat_idx = new N_DAT_T[n];   // buffer for random indexes
    while (t < n_epoch * n) {
        if (t % n == 0) {   // during the start of each epoch, 
                            // re-shuffle the data
            rand_data_index(rand_dat_idx, n);
        }
        train_epoch(dat, d, rand_dat_idx, n, y);
    }
}

void SGD::train_epoch(COMP_T* dat, DAT_DIM_T d, 
                      N_DAT_T* dat_idx, N_DAT_T n, SUPV_T* y) {
    for (N_DAT_T i = 0; i < n; ++i, ++t) {
        N_DAT_T ind = dat_idx[i];
        COMP_T* dat_i = dat + d * ind;
        eta = compute_learning_rate();
        train_one(dat_i, d, y[ind]);
    }
}

void SGD::train_epoch(COMP_T* dat, DAT_DIM_T d,
                      N_DAT_T* dat_idx, N_DAT_T n, SUPV_T* y,
                      N_PARAM_T* par_idx, N_PARAM_T m) {
    for (N_DAT_T i = 0; i < n; ++i, ++t) {
        N_DAT_T ind = dat_idx[i];
        COMP_T* dat_i = dat + d * ind;
        eta = compute_learning_rate();
        train_one(dat_i, d, y[ind], par_idx, m);
    }
}

void SGD::train_one(COMP_T* dat_i, DAT_DIM_T d, SUPV_T y) {
    for (N_PARAM_T i = 0; i < n_param; ++i) {
        update_rule[i](param, i, dat_i, d, y);
    }
}

void SGD::train_one(COMP_T* dat_i, DAT_DIM_T d, SUPV_T y,
                    N_PARAM_T* par_idx, N_PARAM_T m) {
    for (N_PARAM_T i = 0; i < m; ++i) {
        update_rule[par_idx[i]](param, par_idx[i], dat_i, d, y);
    }
}

void SGD::rand_data_index(N_DAT_T* index, N_DAT_T n) {
// WARNING: current implementation can generate random integers only less than 
// maximum value of type "int" because of the restrictions of rand() from C 
// standard library.
    srand(time(NULL));
    forward_list<N_DAT_T> candicate;
    for (N_DAT_T i = 0; i < n; ++i) {
        candicate.push_front(i);
    }
    for (N_DAT_T i = 0; i < n; ++i) {
        N_DAT_T rand_i = rand() % ( n - i );
        forward_list<N_DAT_T>::iterator it0 = candicate.before_begin();
        forward_list<N_DAT_T>::iterator it1 = candicate.begin();
        next(it0, rand_i);
        next(it1, rand_i);
        index[i] = *it1;
        candicate.erase_after(it0);
    }
}
