// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// Code taken from a book in preparation,
// "Multicore Application Programming"
//  This is source file mdth.C
// =====================================

#include <stdlib.h>
#include <iostream>
#include <omp.h>
#include <ThreadRangeOmp.h>
#include <Cpu_Timer.h>
#include <DMonitor.h>

// Auxiliary functions declarations
// --------------------------------
void   InitJob();       
void   CloseJob();      
void   ComputeTrajectory();

// Global variables in this module 
// -------------------------------
double Ek, Et;        // kinetic and potential energies
DMonitor M;              // to compute kinetic energy mean values
int SZ;

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

// -------------------------------------------------
// Computation of the molecular dynamics trajectory
// nSteps time steps, plus energy computations at
// the end
// ------------------------------------------------
void ComputeTrajectory()
   {
   int n, j, count, rank;
   double ekin, epot, scr;
   int nl, nh;

   nl = 0;
   nh = SZ;
   ThreadRangeOmp(nl, nh);

   for(count=0; count < nSteps; ++count) 
      {
      // calculation of the acceleration
      for(n=nl; n<nh; n++)
	 {
	 a[n] = 0.0;
	 for(j=0; j<SZ; j++) a[n] += D[n][j] * q[j];
	 }
      
      #pragma omp barrier
		   
      // integration of the equations of motion
      for( n=nl; n<nh; n++) 
	 {
	 p[n] -= (delta * a[n]);
	 q[n] += (delta * p[n]);
	 }
      #pragma omp barrier
      }
   
   // compute again acceleration, needed for energies
   // -----------------------------------------------
   for(n=nl; n<nh; n++)
      {
      a[n] = 0.0;
      for(j=0; j<SZ; j++) a[n] += D[n][j] * q[j];
      }

   // Compute kinetic energy
   // ----------------------
   ekin = 0.0;
   for(n=nl; n<nh; n++) 
      {
      // This step shifts momenta by 0.5*delta 
      scr = p[n] - 0.5 * a[n] * delta; 
      ekin += 0.5*scr*scr;
      }
      
   // Compute potential energy 
   // ------------------------
   for(n=nl; n<nh; n++) epot += 0.5*q[n]*a[n];

   // Cumulate partial energies
   // -------------------------
   #pragma omp critical
      {
      Ek += ekin;
      Et += (ekin+epot);
      }
   }

// The main() function 
// -------------------
int main(int argc, char **argv)
    {
    int sample;
    Cpu_Timer TR;

    InitJob();

    if(argc==2) nTh = atoi(argv[1]);
    SZ = 3*N;
    delta = 0.02;

    std::cout << "\n Number of particles: " << SZ;
    std::cout << "\n Number of threads  : " << nTh;
    std::cout << "\n Time step          : " << delta;
    std::cout << "\n Trajectory length  : " << nSteps;
    std::cout << "\n Number of samples  : " << nSamples;
   
    TR.Start();
    omp_set_num_threads(nTh);
    for(sample=1; sample<=nSamples; ++sample)
        {
        Ek = 0.0;
        Et = 0.0;

        #pragma omp parallel 
            {
            ComputeTrajectory();
            }

        // Print kinetic and total energies
        // --------------------------------
        std::cout << "\n Ekin = " << Ek << "       Etot = " 
                  << Et << std::endl;

        // Accumulate kinetic energy values
        // --------------------------------
        M.AccumData(Ek);
	}
     TR.Stop();

     M.Reset();
     std::cout << "\n Kinetic energy average   = " <<  M.Average()
             << "\n Kinetic energy variance  = " <<  M.Variance();

     TR.Report();
     CloseJob();
     }
		
/***************************************************************/
		
		
