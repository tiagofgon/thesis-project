// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File HeatAux.C
// ---------------
//
// Auxiliary functions for initialization and resource 
// managment, common to all the versions of the Heat code. 
// ------------------------------------------------------
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <complex>

using namespace std;

// ----------------------------------
// Declaration of auxiliary functions
// ----------------------------------
void InputData();
void PrintData();
void InitJob();
void ExitJob();


extern std::complex<float> *D;  // holds FT of initial condition
extern std::complex<float> *F2; // holds damped FT for time t2
extern float   *damp;           // array of damping factors.

// Data read from data file
// ------------------------
extern int     N, M;        // sizes of 2D array
extern int     nB;          // number of working buffers F[]
extern int     steps;       // number of time steps
extern float   deltaT;      // time step

void InputData()
    {
    FILE *fp;
    char buffer[128];

    if( !(fp = fopen("fheat.dat", "r") ))
	   {
	   cout << "\n Input error" << endl;
	   exit(0);
	   }
	
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &N);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &M);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &steps);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%g", &deltaT);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &nB);
    fclose(fp);
    }

void PrintData()
    {
    cout << "\n Problem size:    N=" << N << ",  M=" << M; 
    cout << "\n Number of steps: " << steps; 
    cout << "\n Time step:       " << deltaT; 
    cout << "\n Number buffers : " << nB << endl;
    }

void InitJob()
   {
   float x1;
   int Ntot = N*M;
   // memory allocation
   // -----------------
   D   = new std::complex<float>[Ntot];
   F2  = new std::complex<float>[Ntot];
   damp = new float[N];       
   
   for(int k=0; k<N; k++)  // Initialize damp[]
      {
      x1 = sin(3.1415926535*k/N);
      damp[k] = -2.0*x1*x1;
      }
   }

void CloseJob()
   {
   delete [] D;
   delete [] F2;
   delete [] damp;
   }
