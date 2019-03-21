
#include <tf_calculation.h>


#define dT  0.001
//float tau = 0.0;
//TFConf_t tfconf;

void tfOutCalculation ( TFConf_t* p_tf  )
{
//  tau = DT / ( p_tf->T + DT );
//  p_tf->output = tau * ( p_tf->k * p_tf->input +  p_tf->prev_output );
//  p_tf->prev_output = p_tf->output;

	if(p_tf->a == 0){
		p_tf->a = p_tf->T/(p_tf->T+dT);
		p_tf->b = p_tf->k*dT/( p_tf->T+dT);
	}
	p_tf->output =  p_tf->prev_output*p_tf->a + p_tf->input*p_tf->b;
	p_tf->prev_output = p_tf->output;
    //return p_tf;
}
