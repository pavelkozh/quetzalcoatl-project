
#ifndef FUZZY_LOGIC_H
#define FUZZY_LOGIC_H

#include <common.h>

typedef struct {
/*
*
*/
	double		ln;
/*
*
*/
	double		mn;
/*
*
*/
	double		sn;
/*
*
*/
	double		no;
/*
*
*/
	double		sp;
/*
*
*/
	double		mp;
/*
*
*/
	double		lp;
} FuzzyficationVal;

typedef struct {
/*
* @brief minimal value
*/
	double				min_val;
/*
* @brief maxmum value
*/
	double				max_val;
/*
*
*/
	double		ln;
/*
*
*/
	double		mn;
/*
*
*/
	double		sn;
/*
*
*/
	double		no;
/*
*
*/
	double		sp;
/*
*
*/
	double		mp;
/*
*
*/
	double		lp;
/*
* @brief step for term value.
* @note: Read only
*/
	double				step;
/*
* @brief fazzyfication output value.
* @note: Read only
*/
	FuzzyficationVal	output_val;

} FuzzyVar;

typedef struct {
	double A;
	double op1;
	double B;
	double op2;
	double C;
	double then1;
	double then2;
} rule;


/*
*	@brief evenly splits the interval into 6 segments
*/
void fuzzyVarInit(FuzzyVar * fv);

/*
*	@brief fuzzyfication
*/
void fuzzyfication(FuzzyVar *fv, double input_val);

#endif /* FUZZY_LOGIC_H */