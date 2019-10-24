//******************************************************
// File GaussGen.h
//
// Implementation of gaussuan random generator, using
// Box-Muller algorithm. It requires a uniform generator
// in [0,1], which is the function RandomR given below.
//
// The gaussian random generator is implemented as a
// C++ class.
// *****************************************************

#define _PI 3.1415026535

#include <math.h>
#include <Rand.h>

class GaussGen
    {
    private:
    int    flag;
    double ransave;
    Rand   *R;

    public:
    GaussGen() 
       { 
       flag=0; 
       ransave=0.0;
       R = new Rand(999);
       }

    GaussGen(int seed) 
       { 
       flag=0; 
       ransave=0.0;
       R = new Rand(seed);
       }

    ~GaussGen()
       { delete R; } 

    double Draw()
       {
       double x1, x2, scratch;
   
       if(flag)
          {
          flag = 0;
          return ransave;
          }
       else
          {
          x1 = R->draw();
          x2 = R->draw();
          scratch = sqrt(-2*log(x1));
          x1 = cos(2 * _PI * x2);
          x2 = sin(2 * _PI * x2);
          ransave = scratch * x2;
          flag = 1;
          return(scratch * x1);
          }
       }
    };

//////////////////////////////////////////////////////////
