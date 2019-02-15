#include <tf_calculation.h>


/* time quantum (s). DT value depends on timer period. */
float DT = 0.001;

/* temporary variable for time constant storage */
float tau = 0.0;


/*
 * @brief    Function calculates difference equation
 * @params   Pointer to structure(transfer function state)
 */
void tfOutCalculation ( TFConf_t* p_tf  )
{
  tau = DT / ( p_tf->T + DT );
  p_tf->output = tau * ( p_tf->k * p_tf->input +  p_tf->prev_output );
  p_tf->prev_output = p_tf->output;
    //return p_tf;
}
