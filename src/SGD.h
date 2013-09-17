# ifndef _SGD_H
# define _SGD_H

# include <iostream>
# include <cstdlib>
# include <cstring>
# include <ctime>
# include <forward_list>


template <typename _COMP_T,
          typename _SUPV_T,
          typename _DAT_DIM_T,
          typename _N_DAT_T>
class SGD {
// General Stochastic Gradient Descent solver model
// _COMP_T:    type of the value to be computed (parameters, data samples, etc)
// _SUPV_T:    type of the supervising information (classes, labels, etc)
// _DAT_DIM_T: type of the dimension of the data
// _N_DAT_T:   type of the number of the data set
public:
    SGD(char         _verbosity       = 1,
        _COMP_T      _eta0            = 0,
        unsigned int _n_epoch         = 5,
        _COMP_T      _eta0_1st_try    = 0.5,
        _COMP_T      _eta0_try_factor = 2);
    // SGD(const SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>& some);   // No need to explictly declare copy-constructor;
                               // default is ok
    virtual ~SGD(){};

    SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>& 
    train(_COMP_T* dat, _DAT_DIM_T d, _N_DAT_T n, _SUPV_T* y);
    // Train the parameters "param" with ALL the "n" input data of dimension "d"
    // and the corresponding supervising data. The order of the data samples are
    // automatically shuffled before each epoch. Every parameter is applied with
    // update rule once for each input data sample.
    SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>& 
    train_epoch(_COMP_T* dat, _DAT_DIM_T d, _N_DAT_T n,
                _N_DAT_T* dat_idx, _N_DAT_T m, _SUPV_T* y);
    // Train the parameters "param" with "m" data samples in "dat" whose indexes
    // are specified by "dat_idx". The training precedure follows exactly the 
    // order of indexes in "dat_idx". The data pass only once (1-epoch). Every 
    // parameter is applied with update rule once for each input data sample. 
    // "y" should be the WHOLE supervising data ("n", same length as "dat"), not
    // just length of "m".
    SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>& 
    determine_eta0(_COMP_T* dat, _DAT_DIM_T d, _N_DAT_T n, _SUPV_T* y,
                   _N_DAT_T* dat_idx = NULL, _N_DAT_T m = 0);
    // Determine the initial learning rate eta0 automatically according to the 
    // given data set. You can specify a part of the data set using an index 
    // array "dat_idx" of length "m" to improve efficiency. Once index array is
    // provided, it should be shuffled before it is passed as the argument.
    static void rand_data_index(_N_DAT_T* index, _N_DAT_T n);
    // generate the array of length n which stores the indexes (0 ~ n-1) of the 
    // data set in random order

    virtual SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>& 
    train_one(_COMP_T* dat, _N_DAT_T i,
              _DAT_DIM_T d, _N_DAT_T n, _SUPV_T y) = 0;
    // update EVERY parameter once with one input data 
    // (Sub-program of SGD::train_epoch)
    virtual _SUPV_T test_one(_COMP_T* dat_i, _DAT_DIM_T d) const = 0;
    // test one data sample
    virtual _COMP_T compute_obj(_COMP_T* dat, _DAT_DIM_T d, _N_DAT_T n,
                                _SUPV_T* y) const = 0;
    // compute the object function (e.g. sum of empirical losses plus a 
    // regularizing term) given the current parameters and data
    virtual SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>& 
    output_stream(std::ostream& out) = 0;
    // output the SGD solver to a standard ostream

    SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>& 
    verbose(char _verbosity) { verbosity = _verbosity; return *this; }
    SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>& 
    init_learning_rate(_COMP_T _eta0) { eta0 = _eta0; return *this; }
    SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>& 
    num_of_epoches(unsigned int _n_epoch) { n_epoch = _n_epoch; return *this; }
    SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>& 
    try_learning_rate(_COMP_T _eta0_1st_try, _COMP_T _eta0_try_factor) {
        eta0_1st_try    = _eta0_1st_try;
        eta0_try_factor = _eta0_try_factor;
        return *this;
    }
    char         verbose()                  const { return verbosity; }
    _COMP_T      init_learning_rate()       const { return eta0; }
    unsigned int num_of_epoches()           const { return n_epoch; }
    _COMP_T      learning_rate_try_1st()    const { return eta0_1st_try; }
    _COMP_T      learning_rate_try_factor() const { return eta0_try_factor; }

protected:
    _COMP_T  eta0;              // the initial learning rate

    // temporary parameters during learning
    _N_DAT_T t;                  // current pass (number of data samples passed)
    _COMP_T  eta;                // current learning rate

    virtual SGD* get_temp_dup() = 0;
    // Create and return a temporary duplicate of the current SGD solver. Used 
    // in determin_eta0().
    virtual _COMP_T compute_learning_rate() = 0;
private:
    char         verbosity;
    unsigned int n_epoch;       // number of epoches
    
    _COMP_T eta0_1st_try;       // the first guess of eta0
                                // (if eta0 is not specified by user)
    _COMP_T eta0_try_factor;    // the factor that eta0 multiplies for each try
};

// a wrapper of function output_stream(), operator << overload for convenience
template <typename _COMP_T,
          typename _SUPV_T,
          typename _DAT_DIM_T,
          typename _N_DAT_T>
inline std::ostream& operator<<(std::ostream& out,
                                SGD<_COMP_T,
                                    _SUPV_T,
                                    _DAT_DIM_T,
                                    _N_DAT_T>& sgd) {
    sgd.output_stream(out);
    return out;
}


template <typename _COMP_T,
          typename _SUPV_T,
          typename _DAT_DIM_T,
          typename _N_DAT_T>
SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>::
SGD(char         _verbosity,
    _COMP_T      _eta0,
    unsigned int _n_epoch,
    _COMP_T      _eta0_1st_try,
    _COMP_T      _eta0_try_factor)
   :eta0(_eta0),
    t(0),
    verbosity(_verbosity),
    n_epoch(_n_epoch),
    eta0_1st_try(_eta0_1st_try),
    eta0_try_factor(_eta0_try_factor) {
    std::srand(std::time(NULL));
}

// Copy-constructor of SGD is equivalent to the default
// SGD::SGD(const SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>& some):
//          verbosity(some.verbosity),
//          eta0(some.eta0),
//          n_epoch(some.n_epoch),
//          update_rule(some.update_rule),
//          eta0_1st_try(some.eta0_1st_try),
//          eta0_try_factor(some.eta0_try_factor),
//          t(some.t),
//          eta(some.eta) {
// }

template <typename _COMP_T,
          typename _SUPV_T,
          typename _DAT_DIM_T,
          typename _N_DAT_T>
SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>& 
SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>::
train(_COMP_T* dat, _DAT_DIM_T d, _N_DAT_T n, _SUPV_T* y) {
    _N_DAT_T* rand_dat_idx = new _N_DAT_T[n];   // buffer for random indexes
    if (verbosity >= 1) {
        std::cout << "SGD Training: \n\tData: " << n << " samples, "
                  << d << " feature dimensions.\n\tStopping Criterion: "
                  << n_epoch << " epoches." << std::endl;
    }
    if (!eta0) determine_eta0(dat, d, n, y);
    while (t < n_epoch * n) {
        if (!(t % n)) {                       // during the start of each epoch, 
            if (verbosity >= 1) {
                std::cout << "Shuffling the data set... " << std::flush;
            }
            rand_data_index(rand_dat_idx, n); // re-shuffle the data
            if (verbosity >= 1) {
                std::cout << "Done.\nTraining: Epoch = "
                          << t / n + 1  << " ... ";
                if (verbosity >= 2) std::cout << std::endl;
                else std::cout.flush();
            }
        }
        train_epoch(dat, d, n, rand_dat_idx, n, y);
        if (verbosity == 1) {
            std::cout << "Done." << std::endl;
        }
    }
    if (verbosity >= 1) {
        std::cout << "SGD Training: finished." << std::endl;
    }
    delete[] rand_dat_idx;
    return *this;
}

template <typename _COMP_T,
          typename _SUPV_T,
          typename _DAT_DIM_T,
          typename _N_DAT_T>
SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>& 
SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>::
train_epoch(_COMP_T* dat, _DAT_DIM_T d, _N_DAT_T n,
            _N_DAT_T* dat_idx, _N_DAT_T m, _SUPV_T* y) {
    for (_N_DAT_T i = 0; i < m; ++i, ++t) {
        _N_DAT_T ind = dat_idx[i];
        if (verbosity >= 2) {
            std::cout << "\tSGD training through sample " << ind
                      << " (" << i+1 << "/" << m << ")... " << std::flush;
        }
        eta = compute_learning_rate();
        train_one(dat, ind, d, n, y[ind]);
        if (verbosity >= 2) {
            std::cout << "Done." << std::endl;
        }
    }
    return *this;
}

template <typename _COMP_T,
          typename _SUPV_T,
          typename _DAT_DIM_T,
          typename _N_DAT_T>
void SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>::
rand_data_index(_N_DAT_T* index, _N_DAT_T n) {
// WARNING: current implementation can generate random integers only less than 
// maximum value of type "int" because of the restrictions of rand() from C 
// standard library.
    std::forward_list<_N_DAT_T> candicate;
    for (_N_DAT_T i = 0; i < n; ++i) {
        candicate.push_front(i);
    }
    for (_N_DAT_T i = 0; i < n; ++i) {
        _N_DAT_T rand_i = std::rand() % ( n - i );
        typename std::forward_list<_N_DAT_T>::iterator it0 = 
            candicate.before_begin();
        typename std::forward_list<_N_DAT_T>::iterator it1 = 
            candicate.begin();
        for (_N_DAT_T i = 0; i < rand_i; ++i) {
            ++it0;
            ++it1;
        }
        index[i] = *it1;
        candicate.erase_after(it0);
    }
}

template <typename _COMP_T,
          typename _SUPV_T,
          typename _DAT_DIM_T,
          typename _N_DAT_T>
SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>& 
SGD<_COMP_T, _SUPV_T, _DAT_DIM_T, _N_DAT_T>::
determine_eta0(_COMP_T* dat, _DAT_DIM_T d, _N_DAT_T n, _SUPV_T* y,
               _N_DAT_T* dat_idx, _N_DAT_T m) {
    _COMP_T eta0_try1, eta0_try2, obj_try1, obj_try2;

    if (verbosity >= 1) {
        std::cout << "No eta0 specified. Deciding eta0 automatically... ";
        if (verbosity >= 2) std::cout << std::endl;
        else std::cout.flush();
    }

    bool alloc_dat_idx = false;
    if (!dat_idx) {
        if (verbosity >= 2) {
            std::cout << "\tNo sampling information provided. "
                      << "Shuffling the data set... " << std::flush;
        }
        dat_idx = new _N_DAT_T[n];
        m = n;
        alloc_dat_idx = true;
        rand_data_index(dat_idx, m);
        if (verbosity >= 2) {
            std::cout << "Done." << std::endl;
        }
    }

    eta0_try2 = eta0_1st_try;
    if (verbosity >= 2) {
        std::cout << "\tTrying eta0 = " << eta0_try2 << "... ";
        if (verbosity >= 3) std::cout << std::endl;
        else std::cout.flush();
    }
    SGD* tempSGD = get_temp_dup(); tempSGD->eta = eta0_try2;
    for (_N_DAT_T i = 0; i < m; ++i) {
        _N_DAT_T ind = dat_idx[i];
        if (verbosity >= 3) {
            std::cout << "\t\tSGD training through sample " << ind
                      << " (" << i+1 << "/" << m << ")... " << std::flush;
        }
        tempSGD->train_one(dat + d * ind, d, y[ind]);
        if (verbosity >= 3) {
            std::cout << "Done." << std::endl;
        }
    }
    obj_try2 = tempSGD->compute_obj(dat, d, n, y);
    delete tempSGD;
    if (verbosity >= 3) {
        std::cout << "\teta0 = " << eta0_try2
                  << ", obj = " << obj_try2 << std::endl;
    }
    else if (verbosity >= 2) {
        std::cout << "Done. obj = " << obj_try2 << std::endl;
    }
    do {
        eta0_try1 = eta0_try2;
        obj_try1  = obj_try2;
        eta0_try2 *= eta0_try_factor;
        if (verbosity >= 2) {
            std::cout << "\tTrying eta0 = " << eta0_try2 << "... ";
            if (verbosity >= 3) std::cout << std::endl;
            else std::cout.flush();
        }
        tempSGD = get_temp_dup(); tempSGD->eta = eta0_try2;
        for (_N_DAT_T i = 0; i < m; ++i) {
            _N_DAT_T ind = dat_idx[i];
            if (verbosity >= 3) {
                std::cout << "\t\tSGD training through sample " << ind
                          << " (" << i+1 << "/" << m << ")... " << std::flush;
            }
            tempSGD->train_one(dat + d * ind, d, y[ind]);
            if (verbosity >= 3) {
                std::cout << "Done." << std::endl;
            }
        }
        obj_try2 = tempSGD->compute_obj(dat, d, n, y);
        delete tempSGD;
        if (verbosity >= 3) {
            std::cout << "\teta0 = " << eta0_try2 
                      << ", obj = " << obj_try2 << std::endl;
        }
        else if (verbosity >= 2) {
            std::cout << "Done. obj = " << obj_try2 << std::endl;
        }
    } while (obj_try1 > obj_try2);
    eta0 = eta0_try1;
    if (verbosity >= 2) {
        std::cout << "Setting eta0 = " << eta0 << std::endl;
    }
    else if (verbosity >= 1) {
        std::cout << "Done. eta0 = " << eta0 << std::endl;
    }

    if (alloc_dat_idx) delete[] dat_idx;
}


# endif
