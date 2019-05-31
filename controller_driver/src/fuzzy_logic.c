#include <fuzzy_logic.h>



void fuzzylogicInit(){

    fuzzyVarInit(&VSpeed_err);
    fuzzyVarInit(&dVSpeed_err);
    fuzzyVarInit(&Clutch_pos);
    fuzzyVarInit(&Break_pos);
    fuzzyVarInit(&Clutch_speed);
    fuzzyVarInit(&Break_speed);


    // dVSpeed_err = p - Скорость падает ?????????
    // VSpeed_err = p - Заданная скорость выше текущей 
    //Clutch_speed = n - отпускать педаль
// addRule(VSpeed_err.no, dVSpeed_err.ln, Clutch_speed.lp, Break_speed.no);
// addRule(VSpeed_err.no, dVSpeed_err.mn, Clutch_speed.lp, Break_speed.no);
// addRule(VSpeed_err.no, dVSpeed_err.no, Clutch_speed.no, Break_speed.no);
// addRule(VSpeed_err.no, dVSpeed_err.mp, Clutch_speed.mn, Break_speed.no);
// addRule(VSpeed_err.no, dVSpeed_err.lp, Clutch_speed.ln, Break_speed.no);

// addRule(VSpeed_err.mn, dVSpeed_err.ln, Clutch_speed.lp, Break_speed.no);
// addRule(VSpeed_err.mn, dVSpeed_err.mn, Clutch_speed.lp, Break_speed.no);
// addRule(VSpeed_err.mn, dVSpeed_err.no, Clutch_speed.mp, Break_speed.no);
// addRule(VSpeed_err.mn, dVSpeed_err.mp, Clutch_speed.no, Break_speed.no);
// addRule(VSpeed_err.mn, dVSpeed_err.lp, Clutch_speed.ln, Break_speed.no);

// addRule(VSpeed_err.ln, dVSpeed_err.ln, Clutch_speed.lp, Break_speed.no);
// addRule(VSpeed_err.ln, dVSpeed_err.mn, Clutch_speed.lp, Break_speed.no);
// addRule(VSpeed_err.ln, dVSpeed_err.no, Clutch_speed.mp, Break_speed.no);
// addRule(VSpeed_err.ln, dVSpeed_err.mp, Clutch_speed.no, Break_speed.no);
// addRule(VSpeed_err.ln, dVSpeed_err.lp, Clutch_speed.mn, Break_speed.no);

// addRule(VSpeed_err.mp, dVSpeed_err.ln, Clutch_speed.mp, Break_speed.no);
// addRule(VSpeed_err.mp, dVSpeed_err.mn, Clutch_speed.no, Break_speed.no);
// addRule(VSpeed_err.mp, dVSpeed_err.no, Clutch_speed.mn, Break_speed.no);
// addRule(VSpeed_err.mp, dVSpeed_err.mp, Clutch_speed.mn, Break_speed.no);
// addRule(VSpeed_err.mp, dVSpeed_err.lp, Clutch_speed.ln, Break_speed.no);

// addRule(VSpeed_err.lp, dVSpeed_err.ln, Clutch_speed.lp, Break_speed.no);
// addRule(VSpeed_err.lp, dVSpeed_err.mn, Clutch_speed.lp, Break_speed.no);
// addRule(VSpeed_err.lp, dVSpeed_err.no, Clutch_speed.mn, Break_speed.no);
// addRule(VSpeed_err.lp, dVSpeed_err.mp, Clutch_speed.mn, Break_speed.no);
// addRule(VSpeed_err.lp, dVSpeed_err.lp, Clutch_speed.ln, Break_speed.no);


addRule(VSpeed_err.no, dVSpeed_err.ln, Clutch_speed.no, Break_speed.no);
addRule(VSpeed_err.no, dVSpeed_err.mn, Clutch_speed.no, Break_speed.no);
addRule(VSpeed_err.no, dVSpeed_err.no, Clutch_speed.no, Break_speed.no);
addRule(VSpeed_err.no, dVSpeed_err.mp, Clutch_speed.no, Break_speed.no);
addRule(VSpeed_err.no, dVSpeed_err.lp, Clutch_speed.no, Break_speed.no);

addRule(VSpeed_err.mn, dVSpeed_err.ln, Clutch_speed.mp, Break_speed.no);
addRule(VSpeed_err.mn, dVSpeed_err.mn, Clutch_speed.mp, Break_speed.no);
addRule(VSpeed_err.mn, dVSpeed_err.no, Clutch_speed.mp, Break_speed.no);
addRule(VSpeed_err.mn, dVSpeed_err.mp, Clutch_speed.mp, Break_speed.no);
addRule(VSpeed_err.mn, dVSpeed_err.lp, Clutch_speed.mp, Break_speed.no);

addRule(VSpeed_err.ln, dVSpeed_err.ln, Clutch_speed.lp, Break_speed.no);
addRule(VSpeed_err.ln, dVSpeed_err.mn, Clutch_speed.lp, Break_speed.no);
addRule(VSpeed_err.ln, dVSpeed_err.no, Clutch_speed.lp, Break_speed.no);
addRule(VSpeed_err.ln, dVSpeed_err.mp, Clutch_speed.lp, Break_speed.no);
addRule(VSpeed_err.ln, dVSpeed_err.lp, Clutch_speed.lp, Break_speed.no);

addRule(VSpeed_err.mp, dVSpeed_err.ln, Clutch_speed.mn, Break_speed.no);
addRule(VSpeed_err.mp, dVSpeed_err.mn, Clutch_speed.mn, Break_speed.no);
addRule(VSpeed_err.mp, dVSpeed_err.no, Clutch_speed.mn, Break_speed.no);
addRule(VSpeed_err.mp, dVSpeed_err.mp, Clutch_speed.mn, Break_speed.no);
addRule(VSpeed_err.mp, dVSpeed_err.lp, Clutch_speed.mn, Break_speed.no);

addRule(VSpeed_err.lp, dVSpeed_err.ln, Clutch_speed.ln, Break_speed.no);
addRule(VSpeed_err.lp, dVSpeed_err.mn, Clutch_speed.ln, Break_speed.no);
addRule(VSpeed_err.lp, dVSpeed_err.no, Clutch_speed.ln, Break_speed.no);
addRule(VSpeed_err.lp, dVSpeed_err.mp, Clutch_speed.ln, Break_speed.no);
addRule(VSpeed_err.lp, dVSpeed_err.lp, Clutch_speed.ln, Break_speed.no);

}

/*
*   @brief evenly splits the interval into 6 segments
*/
void fuzzyVarInit(FuzzyVar * fv){
        fv->step = (fv->max_val - fv->min_val)/4.0;
        fv->ln = fv->min_val;
        fv->mn = fv->step   + fv->min_val;
        // fv->sn = fv->step*2 + fv->min_val;
        fv->no = fv->step*2 + fv->min_val;
        // fv->sp = fv->step*4 + fv->min_val;
        fv->mp = fv->step*3 + fv->min_val;
        fv->lp = fv->max_val;
        
        // fv->output_val.ln = 0.0;
        // fv->output_val.mn = 0.0;
        // fv->output_val.sn = 0.0;
        // fv->output_val.no = 0.0;
        // fv->output_val.sp = 0.0;
        // fv->output_val.mp = 0.0;
        // fv->output_val.lp = 0.0;
};

double fuzzyficationTerm(FuzzyVar *fv,double val){
    double res = 0.0;
    if(val <= 0.0) 
        res = (1.0/fv->step) * val+1.0;
    else
        res = -(1.0/fv->step) * val+1.0;
    if (res < 0) res = 0.0;
    return res;
}

// void fuzzyfication(FuzzyVar * fv, double input_val){

//     fv->output_val.ln = 0.0;
//     fv->output_val.mn = 0.0;
//     fv->output_val.sn = 0.0;
//     fv->output_val.no = 0.0;
//     fv->output_val.sp = 0.0;
//     fv->output_val.mp = 0.0;
//     fv->output_val.lp = 0.0;

//     if(input_val <= fv->min_val) 
//         fv->output_val.ln = 1.0;
//     else
//         if(input_val >= fv->max_val)
//             fv->output_val.lp = 1.0;
//         else{
//     fv->output_val.ln = fuzzyficationTerm(fv,input_val-fv->ln);
//     fv->output_val.mn = fuzzyficationTerm(fv,input_val-fv->mn);
//     fv->output_val.sn = fuzzyficationTerm(fv,input_val-fv->sn);
//     fv->output_val.no = fuzzyficationTerm(fv,input_val-fv->no);
//     fv->output_val.sp = fuzzyficationTerm(fv,input_val-fv->sp);
//     fv->output_val.mp = fuzzyficationTerm(fv,input_val-fv->mp);
//     fv->output_val.lp = fuzzyficationTerm(fv,input_val-fv->lp);
//     }
// }

void addRule(double Vs,  double dVs,  double Cs, double Bs){
    rules[rule_cnt].Vs = Vs;
    rules[rule_cnt].dVs = dVs;
    // rules[rule_cnt].Cp = Cp;
    // rules[rule_cnt].Bp = Bp;
    rules[rule_cnt].Cs = Cs;
    rules[rule_cnt].Bs = Bs;
    rule_cnt++;
}


void calculateFLReg(double _VSpeed_err, double _dVSpeed_err, double *res_buff){
    double sum_ch1=0,sum_ch2=0,sum_al=0;
    // fuzzyfication(&VSpeed_err,_VSpeed_err);
    // fuzzyfication(&dVSpeed_err,_dVSpeed_err);
    // fuzzyfication(&Clutch_pos,_Clutch_pos);
    // fuzzyfication(&Break_pos,_Break_pos);

    for(uint8_t i=0; i<rule_cnt; i++){
        double alpha = 0;
        double  Vs=0,dVs=0,Cp=0,Bp=0;

        if( ((rules[i].Vs==VSpeed_err.ln) && (_VSpeed_err<=VSpeed_err.ln)) || ((rules[i].Vs==VSpeed_err.lp) && (_VSpeed_err>=VSpeed_err.lp))) {
            Vs = 1;
        }else{
            Vs = fuzzyficationTerm(&VSpeed_err,_VSpeed_err - rules[i].Vs);
        }
        alpha = Vs;

        if( ((rules[i].dVs==dVSpeed_err.ln) && (_dVSpeed_err<=dVSpeed_err.ln)) || ((rules[i].dVs==dVSpeed_err.lp) && (_dVSpeed_err>=dVSpeed_err.lp))) {
            dVs = 1;
        }else{
            dVs = fuzzyficationTerm(&dVSpeed_err,_dVSpeed_err - rules[i].dVs);
        }


        if(dVs < alpha) alpha = dVs;
        // Cp = fuzzyficationTerm(&Clutch_pos,_Clutch_pos - rules[i].Cp);
        // if(Cp < alpha) alpha = Cp;
        // Bp = fuzzyficationTerm(&Break_pos,_Break_pos - rules[i].Bp);
        // if(Bp < alpha) alpha = Bp;
        sum_ch1 += alpha * rules[i].Cs;
        sum_ch2 += alpha * rules[i].Bs;
        sum_al += alpha;
    }
    res_buff[0] = sum_ch1/sum_al;
    res_buff[1] = sum_ch2/sum_al;
}


