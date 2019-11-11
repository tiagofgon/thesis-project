// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File GaussVec.C
// ---------------
// Definition of GaussVec class
//
// Using offset 0 vectors and matrices
// -----------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Rand.h>
#include <GaussVec.h>

void TaskFct(void *P)
   {
   GaussVec *G = (GaussVec *)P;
   G->MCTask();
   }

/* ------------------------------------------------------
 * Parallel thread job functions for Monte Carlo evolution
 * At each barrier, we indicate the nature of the context:
 * P->P means a barrier between two parallel sections
 * P->S means a transition from parallel to sequential
 * S->P means a transition from sequential to parallel.
 * -----------------------------------------------------*/

void GaussVec::MCTask()
    {
    int count, step;
    int n, j, rank, nl, nh, status;
    double scr, dE;

    rank = TP->GetRank();
    nl = 0;
    nh = size;
    TP->ThreadRange(nl, nh);

    // *************************
    // We incoporate a loop over
    // MonteCarlo steps
    // *************************

    for(step=0; step<mcSteps; step++)
       {
       if(rank==1)
          {
          Next_Gaussians();     // fill the write buffer
          tlength = Next_Trajectory_Length();
          }
       status = B->Wait();
	
       // Langevin step. Compute acceleration
         
       for(n=nl; n<nh; n++)
          {
          a[n] = 0.0;
          for(j=0; j<size; j++) a[n] += D[n][j] * q[j];
	  }
	
       // Refresh momenta, and store configuration
	
       scr = 0.5*delta;
       for(n=nl; n<nh; n++)
          p[n] = read_buffer[n] + scr * a[n];
    
       for(n=nl; n<nh; n++)
          {
          pS[n] = p[n];
          qS[n] = q[n];
          }
    
       // Compute initial energy
	
       Ei[rank] = Partial_Energy(nl, nh, 0);
	
       // In all cases, code arrives here with tlength
       // initialized. Do until a configuration is accepted 
       
       accept = 1;	
       do
          {	
          // Restore configuration, in case previous MC step
	  // was rejected
	    
	  if(accept == 0)
	      {	   
              for(n=nl; n<nh; n++)
                 {
                 p[n] = pS[n];
                 q[n] = qS[n];
                 }
	      }
      
          status = B->Wait();
          //if(status>0) err_exit("barrier error");

          // MD trajectory. The trajectory length has already
          // been computed 
	
          for(count=0; count < tlength; count++) 
	      {
              for(n=nl; n<nh; n++)
	         {
	         a[n] = 0.0;
	         for(j=0; j<size; j++) a[n] += D[n][j] * q[j];
	         }
	    
	      status = B->Wait();
	      //if(status>0) err_exit("barrier error");
		   
              for( n=nl; n<nh; n++) 
	         {
	         p[n] -= (delta * a[n]);
	         q[n] += (delta * p[n]);
		 }

	     status = B->Wait();
	     //if(status>0) err_exit("barrier error");
	     }

          // Compute final energy.
	     
	  Ef[rank] = Partial_Energy(nl, nh, 1);
	 
	  status = B->Wait();
	  //if(status>0) err_exit("barrier error");
          if(rank==1)
	      {
	      dE = 0.0;
	      for(n=1; n<=nTh; n++) dE += ( Ei[n]-Ef[n] );
	      accept = Test_Acceptance(dE);
	      if(accept)
	          {
		  tAvg += tlength;
		  if (tlength < tMin) tMin = tlength;
		  if (tlength > tMax) tMax = tlength;
		  }
	      tlength = Next_Trajectory_Length();
	      }

          status = B->Wait();
	  //if(status>0) err_exit("barrier error");
	  }while(accept==0);
       }
    
    for(n=nl; n<nh; n++)
       {
       V[n] = 0.0;
       for(int m=0; m<size; m++) V[n] += D[n][m] * q[m];
       }
    }


// Constructor and destructor
// --------------------------

GaussVec::GaussVec(double **d, double *v, int vSize, int nThreads, 
                   double dt, double prob, long seed)
   {
   D    = d;
   V    = v;
   size = vSize;
   nTh  = nThreads;
   delta = dt;
   pb = prob;
   result_flag = 0;
   nbrSamples = 0;

   accepted = rejected = 0;
   tMin = 50000;
   tMax = 0;
   tAvg = 0;

   B  = new Barrier(nTh);
   TP = new SPool(nTh);
   R  = new Rand(seed);
   Alloc_Vectors();

   // Initial state
   // -------------
   for(int n=0; n<size; n++) q[n] = Grand();
   }
   
GaussVec::~GaussVec()
    {
    delete B;
    delete TP;
    delete R;
    Free_Vectors();
    }

void GaussVec::Reset()
    {
   result_flag = 0;
   nbrSamples = 0;
   accepted = rejected = 0;
   tMin = 50000;
   tMax = 0;
   tAvg = 0;
   for(int n=0; n<size; n++) q[n] = Grand();
   }

// ------------------------------------------------
// The two basic functions called by client threads
// ------------------------------------------------

void GaussVec::Request_Vector(int nIts)
    {
    mcSteps = nIts;
    nbrSamples++;
    TP->Dispatch(TaskFct, (void *)this);
    }

void GaussVec::Request_Vector(double *v, int nIts)
    {
    V = v;
    mcSteps = nIts;
    nbrSamples++;
    TP->Dispatch(TaskFct, (void *)this);
    }

void GaussVec::Wait_For_Request()
    {
    TP->WaitForIdle();
    }
    
// -------------------
// Auxiliary functions
// -------------------
void GaussVec::Alloc_Vectors()
    {
    q   = new double[size];
    p   = new double[size];
    a   = new double[size];
    pS  = new double[size];
    qS  = new double[size];
    read_buffer =  new double[size];
    Ei  = new double[nTh+1];
    Ef  = new double[nTh+1];
    }
	
void GaussVec::Free_Vectors()
    {
    delete [] q;
    delete [] p;
    delete [] a;
    delete [] qS;
    delete [] pS;
    delete [] read_buffer;
    delete [] Ei;
    delete [] Ef;
    }

void GaussVec::Next_Gaussians()
    {
    int n;
    for(n=0; n<size; n++) read_buffer[n] = Grand();
    }

// A Gaussian random number generator
// ----------------------------------
double GaussVec::Grand()
   {
   double x1, x2, scratch;
   double _PI = 3.1415926535;
   
   if(result_flag)
      {
      result_flag = 0;
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
      result_flag = 1;
      return(scratch * x1);
      }
   }
     

// ------------------------------------------------
// The next trajectory length is the number of times 
// the random number generator is called before it
// returns a value smaller than "pb".
// -------------------------------------------------
int GaussVec::Next_Trajectory_Length()
    {
    int retval;
    double d;

    retval = 1;
    do
       {
       d = R->draw();
       retval++;
       }while( d>pb );
    return retval;
    }

// This is the correct Test_Acceptance() function
// that works in the C++ code
// ----------------------------------------------
int GaussVec::Test_Acceptance(double dE)
    {
    int retval;
    double test, scratch;
		 
    retval = 1;
    if( dE>0.0 ) accepted++;
	else
        {
	test = exp(dE);
        scratch = R->draw();
        if(scratch < test) accepted++;
	    else 
            {
            rejected++;
            retval = 0;
            }
        }
    return retval;
    }


double GaussVec::Partial_Energy(int nl, int nh, int flag)
    {
    int n, j;
    double engy, scr;

    /* We have q at time T, and p at time T-0.5*delta.
     * We need to recompute the acceleration at present
     * time T, to be able to compute p at time T.
     * Since we are recomputing the acceleration, this
     * function call MUST be preceeded by a barrier call,
     * if it is called in the middle of a MD trajectory
     * calculation. We must make sure that everybody has 
     * finished updating its (q, p) values */
    
    if(flag)
        {
	for(n=nl; n<nh; n++)
           {
           scr = 0.0;
           for(j=0; j<size; j++) scr += D[n][j] * q[j];
	   a[n] = scr;
	   }
        }
       
    // No barrier is needed here, since we will not be  
    // updating coordinates and momenta 

    engy = 0.0;
    // Compute potential energy 
    for(n=nl; n<nh; n++) engy += 0.5*q[n]*a[n];
	
    // Compute kinetic energy */
    if(flag)
       {
       for(n=nl; n<nh; n++) 
          {
	  // This step shifts momenta by 0.5*delta 
          scr = p[n] + 0.5 * a[n] * delta; 
          engy += 0.5*scr*scr;
          }
       }
    else
       {
       for(n=nl; n<nh; n++) 
          {
          scr = read_buffer[n]; 
          engy += 0.5*scr*scr;
          }
       }
    return engy;
    }

void GaussVec::Print_Report()
    {
    int avgLength = tAvg/(mcSteps*nbrSamples);

    printf("\n\n N=%d  nTh=%d  nSteps=%d", size, nTh, mcSteps);
    printf("\n delta=%g  probability=%g \n", delta, pb);
	
    printf("\n Accepted =  %d  Rejected = %d", accepted, rejected);
    printf("\n tMin = %d  tMax = %d  tAvg = %d\n", tMin, tMax, avgLength);

    printf("\n Iterations per sample : %d", mcSteps);
    }

void GaussVec::err_exit(const char *ch)
    {
    printf("\n %s \n", ch);
    exit(0);
    }

////////////////////////////////////////////////////////////////
		
	
