# ifndef _SGD_H
# define _SGD_H


# define N_PARAM_T unsigned long
// variable type which stores the number of the parameters (which is to be 
// learned) of the SGD model (e.g. total dimensions of w & b in a linear svm 
// classifier)
# define N_CPARAM_T unsigned long    
// variable type which stores the number of the const parameters in the object
// function (which remain constant during learning) of the SGD model (e.g. the
// regularization term coefficient lambda in a linear svm classifier)
# define COMP_T double
// type of the value to be computed (parameters, training samples, etc)
# define SUPV_T unsigned int
// type of the supervising information (classes, labels, etc)
# define N_DAT_T unsigned long
// type of the number of the data set
# define DAT_DIM_T unsigned int
// type of the dimension of the data

typedef void (*F_UPDATE)(COMP_T*, N_PARAM_T, COMP_T*, DAT_DIM_T, SUPV_T);
// function pointer to which updates the ith parameter according to the input 
// data sample of dimension d

class SGD {
public:
    

    SGD(N_PARAM_T _n_param, N_CPARAM_T _n_cparam,
        char         _verbosity = 1,
        COMP_T       _eta0      = 0,
        unsigned int _n_epoch   = 5);
    ~SGD();



    void train(COMP_T* dat, DAT_DIM_T d, N_DAT_T n, SUPV_T* y);
    // Train the parameters "param" with ALL the "n" input data of dimension "d"
    // and the corresponding supervising data. The order of the data samples are
    // automatically shuffled before each epoch. Every parameter is applied with
    // update rule once for each input data sample.
    void train_epoch(COMP_T* dat, DAT_DIM_T d,
                     N_DAT_T* dat_idx, N_DAT_T n, SUPV_T* y);
    // Train the parameters "param" with "n" data samples in "dat" whose indexes
    // are specified by "dat_idx". The training precedure follows exactly the 
    // order of indexes in "dat_idx". The data pass only once (1-epoch). Every 
    // parameter is applied with update rule once for each input data sample. 
    // "y" should be the WHOLE supervising data (same length as "dat"), not just
    // length of "n".
    void train_epoch(COMP_T* dat, DAT_DIM_T d,
                     N_DAT_T* dat_idx, N_DAT_T n, SUPV_T* y,
                     N_PARAM_T* par_idx, N_PARAM_T m);
    // Train a part of parameters in "param" specified by "par_idx" of length 
    // "m" which is an array of parameters' indexes. The training samples are a 
    // part of the whole data set "dat", specified by "dat_idx" similarly.
    void train_one(COMP_T* dat_i, DAT_DIM_T d, SUPV_T y);
    // update EVERY parameter once with one input data 
    // (Sub-program of SGD::train)
    void train_one(COMP_T* dat_i, DAT_DIM_T d, SUPV_T y,
                   N_PARAM_T* par_idx, N_PARAM_T m);
    // update some of the parameters specified by "par_idx" with one input data
    COMP_T compute_loss(COMP_T* dat, DAT_DIM_T d, N_DAT_T n, SUPV_T* y);
    // compute the empirical loss given the current parameters and data
    static void rand_data_index(N_DAT_T* index, N_DAT_T n);
    // generate the array of length n which stores the indexes (0 ~ n-1) of the 
    // data set in random order
private:
    char         verbosity;
    N_PARAM_T    n_param;	    // number of parameters
    N_CPARAM_T   n_cparam;	    // number of constant parameters
    COMP_T*      param;		    // array of parameters
    COMP_T*      cparam;	    // array of constant parameters
    COMP_T       eta0;          // the initial learning rate
    unsigned int n_epoch;       // number of epoches
    F_UPDATE*    update_rule;   // the array of pointers to functions, each of 
                                // which are to update one parameter of param 
                                // respectly with a single input training data

    // temporary parameters during learning
    N_DAT_T      t;             // current pass (number of data samples passed)
    COMP_T       eta;           // current learning rate

    void set_update_rule();
    COMP_T compute_learning_rate();
};



# include "SGD_implementation.h"

# endif
