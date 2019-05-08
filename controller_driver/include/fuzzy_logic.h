#ifndef FUZZY_LOGIC_H
#define FUZZY_LOGIC_H

#include <common.h>

typedef struct {
/*
*
*/
	float		ln;
/*
*
*/
	float		mn;
/*
*
*/
	float		sn;
/*
*
*/
	float		no;
/*
*
*/
	float		sp;
/*
*
*/
	float		mp;
/*
*
*/
	float		lp;
} FuzzyficationVal;

typedef struct {
/*
* @brief minimal value
*/
	float				min_val;
/*
* @brief maxmum value
*/
	float				max_val;
/*
*
*/
	float		ln;
/*
*
*/
	float		mn;
/*
*
*/
	float		sn;
/*
*
*/
	float		no;
/*
*
*/
	float		sp;
/*
*
*/
	float		mp;
/*
*
*/
	float		lp;
/*
* @brief step for term value.
* @note: Read only
*/
	float				step;
/*
* @brief fazzyfication output value.
* @note: Read only
*/
	FuzzyficationVal	output_val;

} FuzzyVar;

typedef struct {
	float A;
	float op1;
	float B;
	float op2;
	float C;
	float then1;
	float then2;
} rule;


/*
*	@brief evenly splits the interval into 6 segments
*/
void fuzzyVarInit(FuzzyVar * fv);

/*
*	@brief fuzzyfication
*/
void fuzzyfication(FuzzyVar *fv, float input_val);

#endif /* FUZZY_LOGIC_H */