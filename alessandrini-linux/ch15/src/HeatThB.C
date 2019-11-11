// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File HeatThB.C
// ***************
// Heat conduction problem in 2D
// Iterating in time the system evolution
// 
// SPool threads, pilelining using a Boolean Lock for
// synchronization.
// .
// Uses my 2D FFt routine based on 1D plus
// matrix transposition.
//
// NOTICE: for safe operation, the number nB of working 
// arrays available must be at least equal to (Nstages+1), 
// that is to say, 3 in this case. Oherwise, two threads 
// may access and overwrite the same working buffer.  
//
// *******************************************************
#include <Fft.h>
#include <BLock.h>
#include <SPool.h>
#include <CpuTimer.h>
#include <iostream>
#include <HeatUtility.h>
#include <InputList.h>

void InitJob();
void CloseJob();
void Damp(std::complex<float>* FF, int MM, int NN, float t);


// ------------------------------------------------
// Function to be used to pass initial condition
// Change here the function definition if one wants
// to change the initial condition
// -----------------------------------------------
float FCT1(float x, float y)
   {
   float retval;
   retval = exp(-x*x-y*y);
   return retval;
   }
   
float FCT2(float x, float y)
   {
   float retval = 0.0;
   return retval;
   }
   

// *************
// The main code
// *************

SPool  TP(2);                // Two threads, ranks 1 and 2.
BLock  BL(false);            // pipeline with boolean lock
std::complex<float> *D;      // holds FT of initial condition
std::complex<float> **F;     // working buffers, hold damped FT for time t1
std::complex<float> *F2;     // holds damped FT for time t2
float   *damp;               // array of damping factors, computed once.
int     N, M;                // sizes of 2D array
int     nB;                  // number of working buffers F[]
int     steps;               // number of time steps
float   deltaT;              // time step
      
// -------------------------------
// These are two functions
// executed by the two stages
// -------------------------------

void FirstStage()
   {
   int k, count, bindex;
   float t1, t2;
   int Ntot = N*M;
   std::complex<float> _Im(0.0, 1.0);
    
   t2 = 0.0;
   for(count=1; count<steps; count++)
      {
      bindex = count%nB;
      std::complex<float> *F1 = F[bindex];
      t1 = t2 + deltaT;
      t2 = t1 + deltaT;

      for(k=0; k<Ntot; ++k)  // copy D to F1 and F2
         { 
         F1[k] = D[k];    // will be FT at time t1
         F2[k] = D[k];
         }

      Damp(F1, M, N, t1);     // -> F1(t1)
      Damp(F2, M, N, t2);     // -> F2(t2)

      // Construct F1 + i*F2, and insert time info 
      // -----------------------------------------
      for(k=0; k<Ntot; ++k) 
          F1[k] += _Im * F2[k];
      std::complex<float> c(t1, t2);
      F1[Ntot] = c;
      
      fft2h(F1, M, N, -1);       //  first half of FFT

      BL.Wait_Until(false, 0);   // synchronize with next stage
      BL.Set_And_Notify(true);
      }
   }


void SecondStage()
   {
   int k;
   int count, bindex;
   int Ntot = N*M;
   
   for(count=1; count<steps; count++)
      {
      bindex = count%nB;
      std::complex<float> *F1 = F[bindex];

      BL.Wait_Until(true, 0);      // synchronize with previous stage
      BL.Set_And_Notify(false);
      fft2h(F1, M, N, -1);               // complete fft computation

      for(k=0; k<Ntot; ++k) F1[k] /= (N*M);
      PrintStatus(F1, N, M, F1[Ntot].real(), F1[Ntot].imag());
      } 
   }

// The task function submitted to the pool
// ---------------------------------------      
void TaskFct(void *P)
   {
   int rank = TP.GetRank();
   if(rank==1) FirstStage();
   if(rank==2) SecondStage();
   }


// -----------------
// The main function
// -----------------
int main(int argc, char **argv)
   {
   float a = 2.0;
   InitJob();
   int Ntot = N*M;
   deltaT = 3000;
   CpuTimer TR;              // launch parallel computation

   std::cout << "\n Problem size is " << N 
             << " x " << M  << std::endl;

   // Initialize initial condition 
   // Fills the matrix D with initial condition data. FCT1 is the real 
   // part, and FCT2 the imaginary part (equal to 0). These two functions 
   // are defined in HeatUtility.h
   // ---------------------------------------------------------------
   CInitialCondition(D, N, M, FCT1, FCT2, -a, a, -a, a);
   PrintStatus(D, N, M, 0.0, 0.0);

   fft2<float>(D, N, M, 1);  // FFT of initial condition

   TR.Start();
   // -----------------------
   TP.Dispatch(TaskFct, NULL);
   TP.WaitForIdle();
   // -----------------------
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
   F   = new std::complex<float>*[nB];
   for(int k=0; k<nB; k++) F[k] = new std::complex<float>[Ntot+1];
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
   for(int k=0; k<nB; k++) delete [] F[k];
   delete [] F;
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
