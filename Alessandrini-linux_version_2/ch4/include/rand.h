/* ------------------------------------------
 * File rand.h
 * Two versions of a simple generator, that 
 * produces uniform deviates in [0,1]. 
 * ------------------------------------------*/

#define PI     3.1415926535
#define IMUL   314159269
#define IADD   453806245
#define MASK   2147483647
#define SCALE  0.4656612873e-9

/* ------------------------------------------- 
 * In this function, the integer "seed" is a 
 * static variable preserved between calls. 
 * ------------------------------------------*/

double Rand()
   {
   static int seed = 999;             // internal state variable
   seed = (seed * IMUL + IADD) & MASK;
   return (seed * SCALE);
   }

/* ------------------------------------------- 
 * In this function, the integer "seed" passed
 * as argument to the generator. Preserving 
 * state is the responsability of the client
 * code. Tread safe version. 
 * ------------------------------------------*/

double RandS(int& seed)
   {
   seed = (seed * IMUL + IADD) & MASK;
   return (seed * SCALE);
   }




