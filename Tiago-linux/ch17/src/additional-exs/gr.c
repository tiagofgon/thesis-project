/*
 * File gr.c
 * Gaussian random number generator. Uses simple
 * uniform deviate in [0,1] 
 */

#define PI  3.1415926535
#define IMUL   314159269
#define IADD   453806245
#define MASK   2147483647
#define SCALE  0.4656612873e-9

#include <math.h>
#include "gr.h"

/* Simple generator. Produces uniform deviates 
 * in [0,1] */

double RandomR (int *seed)
   {
   *seed = (*seed * IMUL + IADD) & MASK;
   return (*seed * SCALE);
   }

double  ransave;
int     flag, seed;

void Rand_Init(int s)
   {
   seed = s;
   ransave = 0.;
   flag = 0;
   }

double Grand()
   {
   double x1, x2, scratch;
   
   if(flag)
      {
      flag = 0;
      return ransave;
      }
   else
      {
      x1 = RandomR(&seed);
      x2 = RandomR(&seed);
      scratch = sqrt(-2*log(x1));
      x1 = cos(2 * PI * x2);
      x2 = sin(2 * PI * x2);
      ransave = scratch * x2;
      flag = 1;
      return(scratch * x1);
      }
   }
     
/*--------------------------------------------------------------*/
