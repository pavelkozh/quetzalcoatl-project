
#ifndef FUZZY_LOGIC_H
#define FUZZY_LOGIC_H

#include <common.h>

typedef struct {
/*
*
*/
    double      ln;
/*
*
*/
    double      mn;
/*
*
*/
    double      sn;
/*
*
*/
    double      no;
/*
*
*/
    double      sp;
/*
*
*/
    double      mp;
/*
*
*/
    double      lp;
} FuzzyficationVal;

typedef struct {
/*
* @brief minimal value
*/
    double              min_val;
/*
* @brief maxmum value
*/
    double              max_val;
/*
*@brief fuzzy value
*/
    double      ln;
/*
*
*/
    double      mn;
/*
*
*/
    // double      sn;
/*
*
*/
    double      no;
/*
*
*/
    // double      sp;
/*
*
*/
    double      mp;
/*
*
*/
    double      lp;
/*
* @brief step for term value.
* @note: Read only
*/
    double              step;
/*
* @brief fazzyfication output value.
* @note: Read only
*/
    FuzzyficationVal    output_val;

} FuzzyVar;


/*
* Vs - Vspeed
* dVs - dVspeed
* Cp - ClutchPosition
* Bp - BreakPosition
* Cs - ClutchSpeed
* Bs - BreakSpeed
*/
typedef struct {
    double Vs;
    double op1;
    double dVs;
    double op2;
    double Cp;
    double op3;
    double Bp;
    double Cs;
    double Bs;
} rule;

static rule rules[100];
static uint8_t rule_cnt = 0;

static FuzzyVar VSpeed_err = {
        .min_val = -5.0,
        .max_val = 5.0
};

static FuzzyVar dVSpeed_err = {
        .min_val = -0.3,
        .max_val = 0.3
};

static FuzzyVar Clutch_pos = {
        .min_val = 0.0,
        .max_val = 82500.0
};

static FuzzyVar Break_pos = {
        .min_val = 0,
        .max_val = 30000.0
};
static FuzzyVar Clutch_speed = {
        .min_val = -100.0,
        .max_val = 100.0
};
static FuzzyVar Break_speed = {
        .min_val = -100.0,
        .max_val = 100.0
};
/*
*@brief Init fuction
*/
void fuzzylogicInit();
/*
*   @brief evenly splits the interval into 6 segments
*/
void fuzzyVarInit(FuzzyVar * fv);

/*
*   @brief fuzzyfication
*/
void fuzzyfication(FuzzyVar *fv, double input_val);

/*
*   @brief add rulle for fuzzy regulator
*/
void addRule(double Vs,  double dVs, double Cs, double Bs);

/*
*   @brief calculate fuzzy regulator
*/
void calculateFLReg(double _VSpeed_err, double _dVSpeed_err, double *res_buff);


#endif /* FUZZY_LOGIC_H */
