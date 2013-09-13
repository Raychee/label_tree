# ifndef _SGD_IMPLEMENTATION_H
# define _SGD_IMPLEMENTATION_H

# if ALG == OVR

// In OVR case, 
// loss = 0.5 * lambda * 

inline COMP_T compute_loss(COMP_T* dat, DAT_DIM_T d, N_DAT_T n, SUPV_T* y) {
    COMP_T loss = 0;

}

inline COMP_T SGD::compute_learning_rate() {
    return eta0 / (1 + cparam[0] * eta0 * t);
}

inline void update_wi(COMP_T* param, N_PARAM_T i,
                      COMP_T* dat_i, DAT_DIM_T d, SUPV_T y) {
    param[0] = 0;
}
inline void update_b(COMP_T* param, N_PARAM_T i,
                     COMP_T* dat_i, DAT_DIM_T d, SUPV_T y) {
    
}

void SGD::set_update_rule() {
    for (N_PARAM_T i = 0; i < n_param - 1; ++i) {
        update_rule[i] = update_wi;
    }
    update_rule[n_param - 1] = update_b;
}

# else
/************                Custom implementation                 ************/

/************        The loss function definition goes here        ************/
inline COMP_T compute_loss(COMP_T* dat, DAT_DIM_T d, N_DAT_T n, SUPV_T* y) {
    
}

/************   Your rule of setting the learning rate goes here   ************/
inline COMP_T compute_learning_rate() {

}

/************ Your update functions for every parameter goes here  ************/
void some_update_func(COMP_T* param, N_PARAM_T i,
                      COMP_T* dat_i, DAT_DIM_T d, SUPV_T y) {

}

/************             Your update rule goes here               ************/
void SGD::set_update_rule() {

}

# endif

# endif
