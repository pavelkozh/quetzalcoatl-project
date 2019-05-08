#include <fuzzy_logic.h>



rule rules[100];
uint8_t rule_cnt = 0;

FuzzyVar VSpeed_err = {
        .min_val = -12.0,
        .max_val = 12.0
};

FuzzyVar dVSpeed_err = {
        .min_val = -6.0,
        .max_val = 6.0
};

FuzzyVar Clutch_pos = {
        .min_val = 0.0,
        .max_val = 60000.0
};

FuzzyVar Break_pos = {
        .min_val = 0,
        .max_val = 40000.0
};
FuzzyVar Clutch_speed = {
        .min_val = -100.0,
        .max_val = 100.0
};
FuzzyVar Break_speed = {
        .min_val = -100.0,
        .max_val = 100.0
};
void fuzzylogicInit(){
	fuzzyVarInit(&VSpeed_err);
	fuzzyVarInit(&dVSpeed_err);
	fuzzyVarInit(&Clutch_pos);
	fuzzyVarInit(&Break_pos);
	fuzzyVarInit(&Clutch_speed);
	fuzzyVarInit(&Break_speed);

	addRule(VSpeed_err.ln);
}

/*
*	@brief evenly splits the interval into 6 segments
*/
void fuzzyVarInit(FuzzyVar * fv){
		fv->step = (fv->max_val - fv->min_val)/6.0;

		fv->ln = fv->min_val;
		fv->mn = fv->step	+ fv->min_val;
		fv->sn = fv->step*2 + fv->min_val;
		fv->no = fv->step*3 + fv->min_val;
		fv->sp = fv->step*4 + fv->min_val;
		fv->mp = fv->step*5 + fv->min_val;
		fv->lp = fv->max_val;

		fv->output_val.ln = 0.0;
		fv->output_val.mn = 0.0;
		fv->output_val.sn = 0.0;
		fv->output_val.no = 0.0;
		fv->output_val.sp = 0.0;
		fv->output_val.mp = 0.0;
		fv->output_val.lp = 0.0;
};


float fuzzyficationTerm(FuzzyVar *fv,float val){
	float res = 0.0;
	if(val <= 0.0) 
		res = (1.0/fv->step) * val+1.0;
	else
		res = -(1.0/fv->step) * val+1.0;
	if (res < 0) res = 0.0;
	return res;
}

void fuzzyfication(FuzzyVar * fv, float input_val){

	fv->output_val.ln = 0.0;
	fv->output_val.mn = 0.0;
	fv->output_val.sn = 0.0;
	fv->output_val.no = 0.0;
	fv->output_val.sp = 0.0;
	fv->output_val.mp = 0.0;
	fv->output_val.lp = 0.0;

	if(input_val <= fv->min_val) 
		fv->output_val.ln = 1.0;
	else
		if(input_val >= fv->max_val)
			fv->output_val.lp = 1.0;
		else{
	fv->output_val.ln = fuzzyficationTerm(fv,input_val-fv->ln);
	fv->output_val.mn = fuzzyficationTerm(fv,input_val-fv->mn);
	fv->output_val.sn = fuzzyficationTerm(fv,input_val-fv->sn);
	fv->output_val.no = fuzzyficationTerm(fv,input_val-fv->no);
	fv->output_val.sp = fuzzyficationTerm(fv,input_val-fv->sp);
	fv->output_val.mp = fuzzyficationTerm(fv,input_val-fv->mp);
	fv->output_val.lp = fuzzyficationTerm(fv,input_val-fv->lp);
	}
}

void addRule(float A, float op1,float B, float op2, float C, float then1, float then2){
	rules[rule_cnt].A = A;
	rules[rule_cnt].op1 = op1;
	rules[rule_cnt].B = B;
	rules[rule_cnt].op2 = op2;
	rules[rule_cnt].C = C;
	rules[rule_cnt].then1 = then1;
	rules[rule_cnt].then2 = then2;
	rule_cnt++;
}

void calculateFLReg(FuzzyVar A, FuzzyVar B, FuzzyVar C, ){

}


