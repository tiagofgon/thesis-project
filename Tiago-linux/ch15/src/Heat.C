// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Heat.C
// 
// Heat conduction problem in 2D. Following the time evolution
// of an initial heat distribution concentrated at the center of
// a rectangular plate.
//
// The system evolution is trivially solved in Fourier space. The
// computational problem is transforming back to real space at
// successive time intervals, to watch the heat diffusion.
// .
// Uses my 2D FFt routine based on successive 1D FFT plus matrix 
// transposition.
//
// *******************************************************
#include <Fft.h>
#include <CpuTimer.h>
#include <InputList.h>
#include <iostream>
#include <HeatUtility.h>

void InitJob();
void CloseJob();

// This function maps the FFT of the initial condition, stored in 
// FF to the FFT of the solution at time t
// --------------------------------------------------------------
void Damp(std::complex<float>* FF, int MM, int NN, float t);

// ------------------------------------------------------------
// Function to be used to pass initial condition.The initial
// heat distribution is concentrated at the center of the plate
// x = y =0.  Change here the function FCT1 definition for
// other initial heat configurations. This function is defined
// in the included file  
// ------------------------------------------------------------
float FCT1(float x, float y)   // real part 
   {
   float retval;
   retval = exp(-x*x-y*y);
   return retval;
   }
   
float FCT2(float x, float y)   // imaginary part, equal to O
   {
   float retval = 0.0;
   return retval;
   }
   
// Global variables
// ----------------
std::complex<float> *D;      // holds FT of initial condition
std::complex<float> *F1;     // working buffers, hold damped FT for time t1
std::complex<float> *F2;     // holds damped FT for time t2
float   *damp;               // precomputed array of damping factors
int     N, M;                // sizes of 2D array
int     nB;                  // number of working buffers F[]
int     steps;               // number of time steps
float   deltaT;              // time step

// -----------------
// The main function
// -----------------
int main(int argc, char **argv)
   {
   int Ntot;
   float a = 2.0;
   int k, count;
   float t1, t2;
   std::complex<float> _Im(0.0, 1.0);   // imaginary unit
   CpuTimer TR;

   InitJob();
   Ntot = N*M;
   deltaT = 3000;
   F1  = new std::complex<float>[Ntot+1];

   // Fills the matrix D with initial condition data. FCT1 is the real part, 
   // and FCT2 the imaginary part (equal to 0). These two functions are
   // defined in HeatUtility.h
   // ---------------------------------------------------------------
   CInitialCondition(D, N, M, FCT1, FCT2, -a, a, -a, a);
   PrintStatus(D, N, M, 0.0, 0.0);

   fft2<float>(D, N, M, 1);        // Compute FFT of initial condition
                                   
   TR.Start();
   t2 = 0.0;
   for(count=1; count<steps; count++)
      {
      t1 = t2 + deltaT;
      t2 = t1 + deltaT;

      for(k=0; k<Ntot; ++k)  // Copy D to F1 and F2
         { 
         F1[k] = D[k];    // will be FT at time t1
         F2[k] = D[k];    // will be FT at time t2
         }

      // damp F1 to time t1 and F2 to time t2
      // ------------------------------------
      Damp(F1, M, N, t1);
      Damp(F2, M, N, t2);

      // Construct F1 + i * F2, and insert time information. 
      // --------------------------------------------------
      for(k=0; k<Ntot; ++k) 
          F1[k] += _Im * F2[k];
      std::complex<float> c(t1, t2);
      F1[Ntot] = c;
      
      fft2h(F1, M, N, -1);               // first FFT half
      fft2h(F1, M, N, -1);               // second FFT half
      for(k=0; k<Ntot; ++k) F1[k] /= (N*M);
      PrintStatus(F1, N, M, F1[Ntot].real(), F1[Ntot].imag());
      }
   TR.Stop();
   TR.Report();
   CloseJob();
   }


// ********************
// Auxiliary functions
// ********************
void InitJob()
   {
   int Ntot;
   float x1;

   // -------------- input ----------------- 
   InputList IL;
   IL.RegisterData("N", &N, NI, 1);
   IL.RegisterData("M", &M, NI, 1);
   IL.RegisterData("steps", &steps, NI, 1);
   IL.RegisterData("nB", &nB, NI, 1);
   IL.RegisterData("deltaT", &deltaT, NF, 1);
   IL.ReadData("Heat.dat");
   IL.PrintData();

   // --------- memory allocation ----------
   Ntot = N*M;
   D   = new std::complex<float>[Ntot];
   F2  = new std::complex<float>[Ntot];
   damp = new float[N];       
  
   // --------- compute damp array -------------- 
   for(int k=0; k<N; k++)
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

void Damp(std::complex<float>* FF, int MM, int NN, float t)
   {
   int k1, k2, k;
   float df1, df2;

   for(k1=0; k1<MM; ++k1) 
         {
         df1 = exp(damp[k1]*t);         // damping factor
         for(k2=0; k2<NN; ++k2)
            {
            df2 = exp(damp[k2]*t);      // damping factor
            k = NN*k1 + k2;
            FF[k] *= (df1*df2);     // damp Fourier components
            }
         }
    }
