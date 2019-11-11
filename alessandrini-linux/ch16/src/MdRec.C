// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File MdRec.C
// ---------------
//
// Version of MdSch.C, but using task recycling
// to reduce memory allocations. The acceleration
// and time step tasks are recycled.
// =======================================

#include <stdlib.h>
#include <iostream>
#include <IntRange.h>
#include <DMonitor.h>
#include <tbb/tick_count.h>
#include "tbb/parallel_reduce.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_stddef.h"

using namespace tbb;

// Auxiliary functions declarations
// --------------------------------
void   InitJob();       
void   CloseJob();      
void   ComputeTrajectory();

// Global variables 
// ----------------
int      SZ;                // size of vectors
DMonitor M;

// Global variables accessed in module "mdaux.C"
// --------------------------------------------
int   N;                 // number of particles
int   nTh;               // number of threads
int   NTk;               // number of tasks
int   nSteps;            // number of time steps in trajectory 
int   nSamples;          // number of successive trajectories
double delta;            // time step
float dA, dB, dC;        // parameters for D
double **D;              // correlation matrix
double *q, *p, *a;       // position, momenta, acceleration
double *qs;              // position bis
int    Gr;                // granularity;


class Accelerations : public task
   {
   private:
    int rank;
    int beg, end;
    empty_task* successor;    // new, needed for recycling

   public:
   // Constructor
   Accelerations(int n, empty_task *t) : rank(n), successor(t) 
      {
      int SZ = 3*N;
      int step = SZ/NTk;
      beg = rank*step;
      end = (rank+1)*step;
      if(rank==(NTk-1)) end = SZ ;
      }

   // compute acceleration
   // --------------------
   task *execute()
      {
      for(size_t n=beg; n!=end; ++n)
          {
          a[n] = 0.0;
          for(size_t j=0; j<SZ; ++j) a[n] += D[n][j] * q[j];
          }
      successor->decrement_ref_count();  // recycling protocol
      recycle_as_child_of( *parent() );
      return NULL;
      }
   };


class TimeStep : public task
   {
   private:
    int rank;
    int beg, end;
    empty_task* successor;   // new, needed for recycling

   public:
   // Constructor
   TimeStep(int n, empty_task *t) : rank(n), successor(t) 
      {
      int SZ=3*N;
      int step = SZ/NTk;
      beg = rank*step;
      end = (rank+1)*step;
      if(rank==(NTk-1)) end = SZ;
      }

   // compute trajectory
   // ------------------
   task *execute()
      {
      for(size_t n=beg; n!=end; ++n)
	 {
	 p[n]  -= (delta * a[n]);
	 q[n] += (delta * p[n]);
	 }
      successor->decrement_ref_count();   // recycling protocol
      recycle_as_child_of( *parent() );
      return NULL;
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

void RunTrajectory()
   {
   Accelerations*    x[NTk];
   TimeStep*         y[NTk];
   empty_task* dummy;
   empty_task* e1, *e2;
   

   // Allocate tasks
   // --------------
   dummy = new( task::allocate_root() ) empty_task();
   e1    = new( task::allocate_root() ) empty_task();
   e2    = new( task::allocate_root() ) empty_task();

   for(int n=0; n<NTk; ++n)
       x[n] = new( e1->allocate_child() ) Accelerations(n, dummy);
   
   for(int n=0; n<NTk; ++n)
       y[n] = new( e2->allocate_child() ) TimeStep(n, dummy);

   // HERE STARTS THE RECURRENT PART
   // ------------------------------
   for(int k=0; k<nSteps; ++k)
      {
      // Set reference counts of successor tasks
      // ---------------------------------------   
      dummy->set_ref_count(NTk+1);
      e1->set_ref_count(NTk+1);

      // Spawn NTk Accelerations, and wait for them to complete
      // -------------------------------------------------------
      for(int n=0; n<NTk; ++n) task::spawn(*x[n]);
      dummy->wait_for_all();

      dummy->set_ref_count(NTk+1);
      e2->set_ref_count(NTk+1);
      
      // Spawn NTk TimeSteps, and wait for them to complete
      // --------------------------------------------------
      for(int n=0; n<NTk; ++n) task::spawn(*y[n]);
      dummy->wait_for_all();
      }

   // destroy tasks
   // --------------
   task::destroy(*dummy);
   task::destroy(*e1);
   task::destroy(*e2);
   for(int n=0; n<NTk; ++n) 
      {
      task::destroy(*x[n]);
      task::destroy(*y[n]);
      }
   }
		   
// The main() function 
// -------------------
int main(int argc, char **argv)
    {
    int sample, count;

    InitJob();
    if(argc==2) nTh = atoi(argv[1]);
    SZ = 3*N;
    delta = 0.02;
    Gr = 4;

    std::cout << "\n Number of particles: " << SZ;
    std::cout << "\n Time step          : " << delta;
    std::cout << "\n Trajectory length  : " << nSteps;
    std::cout << "\n Number of samples  : " << nSamples;
    std::cout << "\n dA: " << dA << "  dB: " << dB 
              << "  dC: " << dC << std::endl;

    // Initialization of TBB scheduler
    // -------------------------------
    task_scheduler_init init(nTh);

    tick_count t0 = tick_count::now();
    for(sample=0; sample<nSamples; ++sample)
        {
        RunTrajectory();
        Energies EG;    
        IntRange R(0, SZ, SZ/Gr);
        parallel_reduce(R, EG);
        std::cout << "\n Ekin = " << EG.Ekin << "       Etot = " 
                  << EG.Etot << std::endl;
        M.AccumData(EG.Ekin);
    	}
    tick_count t1 = tick_count::now();
    M.Reset();
    std::cout << "\nAverage kinetic energy " << M.Average()
              << "\nVariance:              " << M.Variance();
    std::cout << "\n Wall time = " << (t1-t0).seconds() << " seconds"
              << std::endl;
    CloseJob();
    }
		
/***************************************************************/
		
		
