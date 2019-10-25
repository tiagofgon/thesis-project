// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// Code taken from a book in preparation,
// "Multicore Application Programming"
//  This is source file mdth.C
// =====================================

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <SPool.h>
#include <Barrier.h>
#include <SpBarrier.h>
#include <Reduction.h>
#include <DMonitor.h>
#include <CpuTimer.h>
#define SP_BARRIER

// Auxiliary functions declarations
// --------------------------------
void   InitJob();       
void   CloseJob();      
void   ComputeTrajectory();

// Global variables in this module 
// -------------------------------
double Ek, Et;              // kinetic and total energies
SPool *TP;                  // thread pool
Reduction<double> EK, EP;   // to accumulate energies
DMonitor KEGY;              // to compute kinetic energy mv and vars
int SZ;

#ifdef SP_BARRIER
SpBarrier  *B;                // spin barrier
#else
Barrier  *B;                  // barrier
#endif

// Global variables accessed in "mdaux.C" module
// ---------------------------------------------
int    N;                // number of particles
int    nTh;              // number of threads
int    nSteps;           // number of time steps in trajectory 
int    nSamples;         // number of successive trajectories
double delta;            // time step
float  dA, dB, dC;       // D matrix parameters
double  **D;             // correlation matrix
double  *q, *p, *a;      // position, momenta, acceleration

// -------------------------------------------------
// Computation of the molecular dynamics trajectory
// nSteps time steps, plus energy computations at
// the end
// ------------------------------------------------
void ComputeTrajectory(void *ptr)
   {
   int n, j, count, rank;
   double engy, scr;
   int nl, nh;

   rank = TP->GetRank();
   nl = 0;
   nh = SZ;
   TP->ThreadRange(nl, nh);

   for(count=0; count < nSteps; ++count) 
      {
      // calculation of the acceleration
      for(n=nl; n<nh; n++)
	 {
	 a[n] = 0.0;
	 for(j=0; j<SZ; j++) a[n] += D[n][j] * q[j];
	 }
      B->Wait();  
		   
      // integration of the equations of motion
      for( n=nl; n<nh; n++) 
	 {
	 p[n] -= (delta * a[n]);
	 q[n] += (delta * p[n]);
	 }
      B->Wait();
      }
      
   // compute again accelleration, needed for energies
   // ------------------------------------------------
   for(n=nl; n<nh; n++)
      {
      a[n] = 0.0;
      //#pragma ivdep
      for(j=0; j<SZ; j++) a[n] += D[n][j] * q[j];
      }

    // Compute kinetic energy
    // ----------------------
    engy = 0.0;
    for(n=nl; n<nh; n++) 
       {
       // This step shifts momenta by 0.5*delta 
       scr = p[n] - 0.5 * a[n] * delta; 
       engy += 0.5*scr*scr;
       }
    EK.Accumulate(engy);
;
    // Compute potential energy 
    // ------------------------
    engy = 0.0;
    for(n=nl; n<nh; n++) engy += 0.5*q[n]*a[n];
    EP.Accumulate(engy);
    }


// The main() function 
// -------------------
int main(int argc, char **argv)
    {
    int sample;
    CpuTimer TR;

    InitJob();
    if(argc==2) nTh = atoi(argv[1]);
    SZ = 3*N;
    delta = 0.02;
    TP = new SPool(nTh);

    #ifdef SP_BARRIER
    B  = new SpBarrier(nTh);
    #else
    B  = new Barrier(nTh);
    #endif

    std::cout << "\n Number of particles: " << SZ;
    std::cout << "\n Number of threads  : " << nTh;
    std::cout << "\n Time step          : " << delta;
    std::cout << "\n Trajectory length  : " << nSteps;
    std::cout << "\n Number of samples  : " << nSamples;
   
    TR.Start();
    for(sample=1; sample<=nSamples; ++sample)
        {
	TP->Dispatch(ComputeTrajectory, NULL);
        TP->WaitForIdle();
        std::cout << "\n Ekin = " << EK.Data() << "       Etot = " 
                  << (EK.Data()+EP.Data()) << std::endl;
        KEGY.AccumData(EK.Data());
        EK.Reset();
        EP.Reset();
	}
     TR.Stop();
     KEGY.Reset();
     std::cout << "\n Ekin mean value = " << KEGY.Average() 
               << "\n Ekin variance   = " << KEGY.Variance() << std::endl; 
     TR.Report();
     CloseJob();
     delete TP;
     delete B;
     }
		
/***************************************************************/
		
		
