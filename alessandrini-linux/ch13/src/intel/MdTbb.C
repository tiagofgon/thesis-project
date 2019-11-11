// **************************************
// Copyright (c) 2012 Victor Alessandrini
// All rights reserved.
// Code taken from a book in preparation,
// "Multicore Application Programming"
//  This is source file md.C
// =====================================

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <IntRange.h>
#include <DMonitor.h>
#include <tbb/tick_count.h>
#include "tbb/parallel_for.h"
#include "tbb/parallel_reduce.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_stddef.h"

using namespace tbb;

// Auxiliary functions declarations
// --------------------------------
void   InitJob();       
void   CloseJob();      
void   ComputeTrajectory();
void   Swap(double *a, double *b);

// Global variables in this module 
// -------------------------------
DMonitor M;
int     SZ;
double  *qs;               // position bis
int     Gr;                // granularity;

// Global variables accessed in "mdaux.C" module
// ---------------------------------------------
int    N;                // number of particles
int    nTh;              // number of threads
int    nSteps;           // number of time steps in trajectory 
int    nSamples;         // number of successive trajectories
float  delta;            // time step
float  dA, dB, dC;       // D matrix parameters
double  **D;             // correlation matrix
double  *q, *p, *a;      // position, momenta, acceleration

class Accelerations
   {
   public:

   // Constructor
   Accelerations() { }

   // compute acceleration
   // --------------------
   void operator() (const IntRange& rg) const
      {
      for(size_t n=rg.begin(); n!=rg.end(); ++n)
          {
          a[n] = 0.0;
          for(size_t j=0; j<SZ; ++j) a[n] += D[n][j] * q[j];
          }
      }
   };


class TimeStep
   {
   public:

   // Constructor
   TimeStep() {}

   // compute one trajectory step
   // ---------------------------
   void operator() (const IntRange& rg) const
      {
      for(size_t n=rg.begin(); n!=rg.end(); ++n)
	 {
	 p[n]  -= (delta * a[n]);
	 q[n] += (delta * p[n]);
	 }
      }
   };


class Energies
   {
   public:
   double Ekin;
   double Etot;

   // Constructor
   // -----------
   Energies() : Ekin(0.0), Etot(0.0) {}

   // Split constructor
   // -----------------
   Energies(Energies& E, split toto) : Ekin(0.0), Etot(0.0) {}

   // Compute energies
   // ----------------
   void operator() (const IntRange& rg)
      {
      double ekin, epot, scr;

      // compute again the acceleration
      for(size_t n=rg.begin(); n!=rg.end(); ++n)
            {
            a[n] = 0.0;
            for(size_t j=0; j<SZ; ++j) a[n] += D[n][j] * q[j];
	    }

      // Compute kinetic energy, and cumulate
      // ------------------------------------
      ekin = 0.0;
      for(size_t n=rg.begin(); n!=rg.end(); ++n)
         {
         // This step shifts momenta by 0.5*delta 
         scr = p[n] - 0.5 * a[n] * delta; 
         ekin += 0.5*scr*scr;
         }
      Ekin += ekin;

      // Compute potential energy, and
      // cumulate total energy 
      // -----------------------------
      epot = 0.0;
      for(size_t n=rg.begin(); n!=rg.end(); ++n) 
         epot += 0.5*q[n]*a[n];
      Etot += (ekin+epot);
      }

   void join(const Energies& EG) 
      {
      Ekin += EG.Ekin;
      Etot += EG.Etot;
      }

   };

		   
// The main() function 
// -------------------
int main(int argc, char **argv)
    {
    int sample, count;

    InitJob();
    if(argc==2) nTh = atoi(argv[1]);
    SZ = 3*N;
    delta = 0.02;
    Gr = nTh;

    std::cout << "\n Number of particles: " << SZ;
    std::cout << "\n Time step          : " << delta;
    std::cout << "\n Trajectory length  : " << nSteps;
    std::cout << "\n Number of samples  : " << nSamples;
    std::cout << "\n dA: " << dA << "  dB: " << dB 
              << "  dC: " << dC << std::endl;

    // Initialization of TBB scheduler
    // -------------------------------
    task_scheduler_init init(nTh);

    Accelerations AC;
    TimeStep TS;
    IntRange R(0, SZ, SZ/Gr);

    tbb::tick_count t0 = tbb::tick_count::now();
    for(sample=0; sample<nSamples; ++sample)
        {
        // Let the system evolve nSteps
        // ----------------------------
        for(count=0; count<nSteps; ++count)
            {
            parallel_for(R, AC);
            parallel_for(R, TS);
            }
        Energies EG;
        parallel_reduce(R, EG);
        std::cout << "\n Ekin = " << EG.Ekin << "       Etot = " 
                  << EG.Etot << std::endl;
        M.AccumData(EG.Ekin);
	}
     tbb::tick_count t1 = tbb::tick_count::now();
     M.Reset();
     std::cout << "\nAverage kinetic energy " << M.Average()
               << "\nVariance:              " << M.Variance();
     std::cout << "\n Wall time = " << (t1-t0).seconds() << " seconds"
               << std::endl;
     CloseJob();
     }
		
void Swap(double *a, double *b)
   {
   double *temp = a;
   a = b;
   b = temp;
   }

/***************************************************************/
		
		
