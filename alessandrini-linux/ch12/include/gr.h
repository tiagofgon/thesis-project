/*
 * gr.h
 * 
 * Header for scalar gaussian random number generator
 * Uses Box-Miller algorithms
 *
 */

extern  void   Rand_Init(int seed);
extern  double RandomR(int *seed);
extern  double Grand();
