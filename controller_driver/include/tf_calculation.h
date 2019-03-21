#ifndef INCLUDE_TF_CALCULATION_H_
#define INCLUDE_TF_CALCULATION_H_

#include <common.h>

/*
 * @brief      Transfer function structure.
 *             Parameters and state of difference equation.
 *             W(s) = k / ( T*s + 1 )
 */
struct TFConf {
  /* gain value (parameter)      */
  float   k;

  /* time constant [s] ( parameter ) */
  float   T;

  /* transfer function input    */
  float input;

  /* transfer function output    */
  float output;

  /* transfer function previous output   */
  float prev_output;

  /*a=T/(T+dt)*/
  float a;
  /*k*dt/(T+dt)*/
  float b;
};

typedef struct TFConf TFConf_t;


//extern TFConf_t tfconf;



/*
 * @brief    Function calculates difference equation
 * @params   Pointer to structure(transfer function state)
 */
void tfOutCalculation ( TFConf_t * );


#endif /* INCLUDE_TF_CALCULATION_H_ */
