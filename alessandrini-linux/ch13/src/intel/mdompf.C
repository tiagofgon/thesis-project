// **************************************
// Copyright (c) 2013 Victor Alessandrini
// All rights reserved.
// Code taken from a book in preparation,
// "Multicore Application Programming"
//  This is source file md.C
// =====================================

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <DMonitor.h>
#include <CpuTimer.h>
#include <omp.h>

// Auxiliary functions declarations
// --------------------------------
void   InitJob();       
void   CloseJob();      
void   ComputeTrajectory();

// Global variables in this module 
// -------------------------------
double Ekin, Epot;        // kinetic and potential energies
DMonitor EK;              // to compute kinetic energy mean values
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
   int n, j, count;
   double engy, scr;

   omp_set_num_threads(nTh);
   for(count=0; count < nSteps; ++count) 
      {
      // calculation of the acceleration 
      //#pragma omp for
      //for(n=0; n<SZ; n++) a[n] = 0.0;

      #pragma omp parallel for 
      for(n=0; n<SZ; n++)
	 {
         a[n] = 0.0;
	 for(j=0; j<SZ; j++) a[n] += D[n][j] * q[j];
	 }
		   
      // integration of the equations of motion
      #pragma omp parallel for  
      for( n=0; n<SZ; n++) 
	 {
	 p[n] -= (delta * a[n]);
	 q[n] += (delta * p[n]);
	 }
      }
      
   // compute again acceleration, needed for energies
   // ------------------------------------------------
   #pragma omp parallel for 
   for(n=0; n<SZ; n++)
      {
      a[n] = 0.0;
      for(j=0; j<SZ; j++) a[n] += D[n][j] * q[j];
      }

    // Compute kinetic energy
    // ----------------------
    engy = 0.0;
    #pragma omp parallel for
    for(n=0; n<SZ; n++) 
       {
       // This step shifts momenta by 0.5*delta 
       scr = p[n] - 0.5 * a[n] * delta; 
       engy += 0.5*scr*scr;
       }
    Ekin = engy
;
    // Compute potential energy 
    // ------------------------
    engy = 0.0;
    #pragma omp parallel for
    for(n=0; n<SZ; n++) engy += 0.5*q[n]*a[n];
    Epot = engy;
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

    std::cout << "\n Number of particles: " << SZ;
    std::cout << "\n Time step          : " << delta;
    std::cout << "\n Trajectory length  : " << nSteps;
    std::cout << "\n Number of samples  : " << nSamples;
    std::cout << "\n D matrix parameters are: "
              << "\n dA = " << dA << "   dB = " << dB << "   dC = " << dC
              << std::endl; 

    
    TR.Start();
    for(sample=1; sample<=nSamples; ++sample)
        {
	ComputeTrajectory();
        std::cout << "\n Ekin = " << Ekin << "       Etot = " 
                  << (Ekin+Epot) << std::endl;
        EK.AccumData(Ekin);
	}
     TR.Stop();
     EK.Reset();
     std::cout << "\nAverage kinetic energy " << EK.Average()
               << "\nVariance:              " << EK.Variance();
     TR.Report();
     CloseJob();
     }
	
// **************************************************************	
		
		
