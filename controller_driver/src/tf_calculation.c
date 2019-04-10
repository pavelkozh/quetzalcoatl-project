
#include <tf_calculation.h>


#define dT  0.001

void tfOutCalculation ( TFConf_t* p_tf  )
{

	if(p_tf->a == 0){
		p_tf->a = p_tf->T/(p_tf->T+dT);
		p_tf->b = p_tf->k*dT/( p_tf->T+dT);
	}
	p_tf->output =  p_tf->prev_output*p_tf->a + p_tf->input*p_tf->b;
	p_tf->prev_output = p_tf->output;
}
