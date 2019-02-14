#ifndef INCLUDE_TF_CALCULATION_H_
#define INCLUDE_TF_CALCULATION_H_

#include <common.h>

/*
 * @brief      Transfer function structure
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
};

typedef struct TFConf TFConf_t;

//extern TFConf_t tfconf;



/*
 * @brief
 * @param
 * @return
 */
void tfOutCalculation ( TFConf_t * );


#endif /* INCLUDE_TF_CALCULATION_H_ */
