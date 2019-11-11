/* =======================================================
 * Copyright(c) 2002-2003 Victor Alessandrini
 * All rights reserved.
 * Code from book "Application Programming with
 * Posix Threads"
 * ------------------------------------------
 * This is source file GAUSS.C
 * Uses a HMC algorithm for the generation of correlated 
 * Gaussian random vectors 
 *
 * Same as cgauss.c, but uses C++ verion of barrier and 
 * ThPeer
 * =======================================================*/
#define PI  3.1415926535
#define IMUL   314159269
#define IADD   453806245
#define MASK   2147483647
#define SCALE  0.4656612873e-9

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gr.h>
#include <Barrier.h>
#include <NPool.h>
#include <CpuTimer.h>

/* Simple generator. Produces uniform deviates 
 * in [0,1] */

double RandomR (int *seed)
   {
   *seed = (*seed * IMUL + IADD) & MASK;
   return (*seed * SCALE);
   }

double  ransave;
int     flag, seed;

void Rand_Init(int s)
   {
   seed = s;
   ransave = 0.;
   flag = 0;
   }

double Grand()
   {
   double x1, x2, scratch;
   
   if(flag)
      {
      flag = 0;
      return ransave;
      }
   else
      {
      x1 = RandomR(&seed);
      x2 = RandomR(&seed);
      scratch = sqrt(-2*log(x1));
      x1 = cos(2 * PI * x2);
      x2 = sin(2 * PI * x2);
      ransave = scratch * x2;
      flag = 1;
      return(scratch * x1);
      }
   }
     
typedef struct hymc
   {
   int      nEvents;
   int      signDefect;
   double   maxError;
   double   QF_Avg;
   double   QF_Max;
   } Stats;

/* Initialization and ressource managment
   -------------------------------------*/ 
void   Init_Job();       
void   Input_Data();
void   Alloc_Vectors();
void   Close_Job();      
void   Free_Vectors();
void   Initial_State();
void   Build_Matrix();
void   Switch_Buffers();

/* Internal generator functions
   ----------------------------*/
void   HMC_Run(double *V, int nIter);
void   th_Init(void *P);
void   th_MC(void *P);
void   th_Config(void *P);
void   Next_Gaussians();   
int    Next_Trajectory_Length();
void   Distribute_Indices(int size);
int    Test_Acceptance(double x);
double Partial_Energy(int nl, int nh, int flag);
void   err_exit(char *p);

/* Functions for this application
   ------------------------------*/
void   Analyze_Data(Stats *P);
void   Print_Report(Stats *P);
void   Timer_Report();

/* Global internal variables 
   -------------------------*/
int   accept, randSeed; 
int   tMin, tMax, tAvg, tlength;
long  accepted, rejected;
int   *NL, *NH;

double *array1, *array2;
double *write_buffer, *read_buffer;

double **D;
double *p, *q, *a;
double *qS, *pS;
double *M, **MV, **MV2;
double Ei[9], Ef[9];

Barrier  *barrier;   
ThPeer   *TP;

clock_t start_time, end_time;
struct tms tms_S, tms_E;

/* Global variables read in file "cgauss.dat"
   ----------------------------------------*/
int   N, nTh, nSamples, nSteps;
int   I, J, mem_affinity;
float  delta, pb, cut;
float  dA, dB, dC;


/* ----------------------------------------------
 * This first thread function initializes vectors
 * to enforce memory affinity
 * ----------------------------------------------*/

void th_Init(void *P)
   {
   int n, m, nl, nh;
   int rank, size;

   rank = TP->GetRank();
   nl = NL[rank];
   nh = NH[rank];
   size = 3*N;

   for(n=nl; n<=nh; n++)
      {
      q[n] = 0.0;
      p[n] = 0.0;
      a[n] = 0.0;
      qS[n] = 0.0;
      pS[n] = 0.0;
      M[n] = 0.0;
      for(m=1; m<=size; m++) 
          {
          D[n][m] = 0.0;
          MV[n][m] = 0.0;
          MV2[n][m] = 0.0;
          } 
      }
   }

/* ------------------------------------------------------
 * Parallel thread job functions for Monte Carlo evolution
 * At each barrier, we indicate the nature of the context:
 * P->P means a barrier between two parallel sections
 * P->S means a transition from parallel to sequential
 * S->P means a transition from sequential to parallel.
 * -----------------------------------------------------*/

void th_MC(void *P)
    {
    int size, count;
    int n, j, rank, nl, nh, status;
    double scr, dE;

    //rank = *(int *)P;
    rank = TP->GetRank();
    nl = NL[rank];
    nh = NH[rank];
    size = 3*N;
	
    /* Langevin step. Compute acceleration*/
         
    for(n=nl; n<=nh; n++)
       {
       a[n] = 0.0;
       for(j=1; j<=size; j++) a[n] += D[n][j] * q[j];
	   }
	
	/* Refresh momenta, and store configuration */
	
    scr = 0.5*delta;
    #pragma cdir nodep	
    for(n=nl; n<=nh; n++)
       p[n] = read_buffer[n] + scr * a[n];
    
	#pragma cdir nodep	
    for(n=nl; n<=nh; n++)
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
       /* Restore configuration, in case previous MC step
	    * was rejected */
	    
	   if(accept == 0)
	      {	   
          for(n=nl; n<=nh; n++)
             {
             p[n] = pS[n];
             q[n] = qS[n];
             }
		  }
       /*------------< P->P >---------------*/
        status = barrier->Wait();
	    if(status>0) err_exit("barrier error");

       /* MD trajectory. The trajectory length has already
		* been computed */
	
 	   for(count=0; count < tlength; count++) 
	      {
          for(n=nl; n<=nh; n++)
	         {
	         a[n] = 0.0;
	         for(j=1; j<=size; j++) a[n] += D[n][j] * q[j];
	         }
	     /*------------< P->P >---------------*/
	      status = barrier->Wait();
	      if(status>0) err_exit("barrier error");
		   
          for( n=nl; n<=nh; n++) 
	         {
	         p[n] -= (delta * a[n]);
	         q[n] += (delta * p[n]);
		     }

	     /*------------< P->P >---------------*/
	     status = barrier->Wait();
	     if(status>0) err_exit("barrier error");
	     }

     /* Compute final energy. */
	     
	 Ef[rank] = Partial_Energy(nl, nh, 1);
	 
	 /*------------< P->S >---------------*/
	 status = barrier->Wait();
	 if(status>0) err_exit("barrier error");
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
	 /*-------------< S->P >---------------*/
      status = barrier->Wait();
	  if(status>0) err_exit("barrier error");
	 }while(accept==0);
     }

/*---------------------------------------------------
 * Thread job function for matrix multiplication. This 
 * function receives as argument the (double *) where
 * the result must be returned. Therefore, it must get
 * its intrinsic rank from the thread pool utility.
 * --------------------------------------------------*/

void th_Config(void *P)
    {
    int n, m, rank, size, nl, nh;
    double *V;
    
    rank = TP->GetRank();
    V = (double *)P;
    size = 3*N;
    nl = NL[rank];
    nh = NH[rank];
	
    for(n=nl; n<=nh; n++)
       {
       V[n] = 0.0;
       for(m=1; m<=size; m++) V[n] += D[n][m] * q[m];
       }
    }
	   
/* --------------------------------------------------
 * This function just iterates HMC runs to produce a 
 * new configuration, returned in V. There are nIter
 * Langevin steps follwed bt accepted MD steps.
 * -------------------------------------------------*/

void HMC_Run(double *V, int nIter)
   {
   int count;
   
   Next_Gaussians();     /* fill the write buffer */
   Switch_Buffers();
   tlength = Next_Trajectory_Length();

   for(count=1; count<=nIter; count++)
       {    
       TP->Dispatch(th_MC, NULL);
       Next_Gaussians();
       TP->WaitForIdle();
       Switch_Buffers();
       }
  
   TP->Dispatch(th_Config, (void *)V);
   TP->WaitForIdle();
   }

/* -------------------
 * The main() function 
 * ------------------*/

int main(int argc, char **argv)
    {
    int n, i, j, count, size, sample;
    double scr;
    Stats S;

    Init_Job();
    if(mem_affinity)
      {    
      TP->Dispatch(th_Init, NULL);
      TP->WaitForIdle();
      }
    Build_Matrix();
    Initial_State();
    
    start_time = times(&tms_S);
    size = 3*N;
    for(i=1; i<=size; i++)
       for(j=i; j<=size; j++) 
	   {
	   MV[i][j] += 0.0;
	   MV2[i][j] += 0.0;
	   }

    for(sample=1; sample<=nSamples; sample++)
        {
	HMC_Run(M, nSteps);  /* run nSteps steps */
	   
	for(i=1; i<=size; i++)
           for(j=i; j<=size; j++) 
	       {
	       scr = M[i]*M[j];
	       MV[i][j] += scr;
	       MV2[i][j] += scr * scr;
	       }

    	if( sample%100 == 0 ) printf("\n sample %d done", sample);
	    }	/* end of samples loop */
     end_time = times(&tms_E) - start_time;
     
     tAvg /= (nSamples*nSteps);
     Analyze_Data(&S);
	
     printf("\n\n I=%d  J=%d : Computed = %g   Exact = %g", 
             I, J, MV[I][J], D[I][J]);
     Print_Report(&S);
     Close_Job();
     }
		
/***********************************
 *     Auxiliary functions
 **********************************/

void Init_Job()
    {
    Input_Data();
    Alloc_Vectors();
    Rand_Init(777);
    Distribute_Indices(3*N);

    barrier = new Barrier(nTh);
    TP = new ThPeer(nTh, true);
	
    /*randSeed = 999;*/
	write_buffer = array1;
	read_buffer  = array2;

    accepted = rejected = 0;
    tMin = 50000;
    tMax = 0;
    tAvg = 0;
     }

void Close_Job()
    {
    Timer_Report();
    delete barrier;
    delete TP;
    Free_Vectors();
    }

void Initial_State()
    {
    int n, m, size;

    size = 3*N;
    for(n=1; n<=size; n++) q[n] = Grand();
    }
    	

void Build_Matrix()
     {
     int     n, size, i, j, iM, jM;
     double  x, v, w, scratch;
     double  nx, ny, nz;
     double *rx, *ry, *rz;

     size  = 3*N;

     /* Here, we use 2a=1 units. Place randomly the
      * particles in a box of size 6, and do first 
      * diagonal submatrix of D */

     rx = (double *)malloc((N+1)*sizeof(double));
     ry = (double *)malloc((N+1)*sizeof(double));
     rz = (double *)malloc((N+1)*sizeof(double));

     #ifdef OLD_INIT	 
	 for(n=0; n<=N; n++)
	    {
		rx[n] = 6 * RandomR(&randSeed);
		ry[n] = 6 * RandomR(&randSeed);
		rz[n] = 6 * RandomR(&randSeed);
		}
     #else
     rz[0] = 0.0; 
     for(n=1; n<=N; n++)
        {
	    rx[n] = 2.0 * dC * RandomR(&randSeed) - dC;
	    ry[n] = 2.0 * dC * RandomR(&randSeed) - dC;
	    rz[n] = 2.0 * dB * RandomR(&randSeed) + (dA-dB) + rz[n-1];
	    }
     #endif
	 
     for(i=1; i<=size; i++)
	    {
	    for(j=1; j<=size; j++) D[i][j] = 0.0; 
	    }
     for(i=1; i<=size; i++) D[i][i] = 1.0;

     /*
      * Do off diagonal submatrices, use symmetry
      */

     for(i=1; i<=N; i++)
         {
	     for(j=(i+1); j<=N; j++)
	         {
	         nx = rx[i] - rx[j];
             ny = ry[i] - ry[j];
             nz = rz[i] - rz[j];
             x = sqrt(nx*nx+ny*ny+nz*nz);
             nx /= x;
             ny /= x;
             nz /= x;
             if(x > 1.0)
	            {
	            v = (3.0/(8*x)) * (1.0 + 1.0/(6.0*x*x)); 
                w = (3.0/(8*x)) * (1.0 - 1.0/(2.0*x*x));
                }
             else
                {
	            v = 1.0 - (9.0*x)/16.0;
                w = (3.0*x)/16.0;
                }

             iM = 3*(i-1)+1;
             jM = 3*(j-1)+1;

             D[iM][jM] = v + (w * nx * nx);
             D[jM][iM] = D[iM][jM];
             D[iM+1][jM+1] = v + (w * ny * ny);
             D[jM+1][iM+1] = D[iM+1][jM+1];
             D[iM+2][jM+2] = v + (w * nz * nz);
             D[jM+2][iM+2] = D[iM+2][jM+2];

             scratch = (w * nx * ny);

             D[iM][jM+1] = scratch;
             D[iM+1][jM] = scratch;
             D[jM+1][iM] = scratch;
             D[jM][iM+1] = scratch;

             scratch = (w * nx * nz);

             D[iM][jM+2] = scratch;
             D[iM+2][jM] = scratch;
             D[jM+2][iM] = scratch;
             D[jM][iM+2] = scratch;

             scratch =  (w * ny * nz);

             D[iM+1][jM+2] = scratch;
             D[iM+2][jM+1] = scratch;
             D[jM+2][iM+1] = scratch;
             D[jM+1][iM+2] = scratch;
             }
         }
	 free(rx);
	 free(ry);
	 free(rz);
  }

void Input_Data()
    {
    FILE *fp;
    char buffer[128];

    if( !(fp = fopen("cgauss.dat", "r") ))
	   {
	   printf("\n Input error\n");
	   exit(0);
	   }
	
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &N);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &nTh);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &nSteps);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &nSamples);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &mem_affinity);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d%d", &I, &J);	
    fgets(buffer, 80, fp);
    sscanf(buffer, "%g %g", &delta, &pb);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%g %g %g", &dA, &dB, &dC);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%g", &cut);
    fclose(fp);
    }	
		
void Alloc_Vectors()
    {
    int n, size;
    size = 3*N+1;

    q   = (double *)malloc( size*sizeof(double) );
    p   = (double *)malloc( size*sizeof(double) );
    a   = (double *)malloc( size*sizeof(double) );
    qS  = (double *)malloc( size*sizeof(double) );
    pS  = (double *)malloc( size*sizeof(double) );
    M   = (double *)malloc( size*sizeof(double) );
    NL  = (int *)malloc( (nTh+1)*sizeof(int) );
    NH  = (int *)malloc( (nTh+1)*sizeof(int) );
    array1  = (double *)malloc( size*sizeof(double) );
    array2  = (double *)malloc( size*sizeof(double) );

    D = (double **)malloc( size*sizeof(double *) );
    D[1] = (double *)malloc ( (9*N*N+1)*sizeof(double) );
    for(n=2; n<=3*N; n++) D[n] = D[n-1] + 3*N;
    
    MV = (double **)malloc( size*sizeof(double *) );
    MV[1] = (double *)malloc ( (9*N*N+1)*sizeof(double) );
    for(n=2; n<=3*N; n++) MV[n] = MV[n-1] + 3*N;

    MV2 = (double **)malloc( size*sizeof(double *) );
    MV2[1] = (double *)malloc ( (9*N*N+1)*sizeof(double) );
    for(n=2; n<=3*N; n++) MV2[n] = MV2[n-1] + 3*N;
    }
	
void Free_Vectors()
    {
    free(q);
    free(p);
    free(qS);
    free(M);
    free(a);
    free(pS);
    free(NL);
    free(NH);
    free(array1);
    free(array2);
    free( D[1] );
    free(D);
    free( MV[1] );
    free(MV);
    free( MV2[1] );
    free(MV2);
    }

void Distribute_Indices(int size)
   {
   int n, nBase, nRest, index;

   for(n=1; n<=nTh; n++)
      {
      NL[n] = 0;
      NH[n] = 0;
      }
   nBase = size/nTh;
   nRest = size%nTh;
   index = 0;
   for(n=1; n<=nTh; n++)
      {
      index++;
      NL[n] = index;
      index += (nBase-1);
      if(nRest)
         {
         index++;
         nRest--;
         }
      NH[n] = index;
      }
   NH[nTh] = size;
   }

/*-----------------------------------------------------
 * This function fills the "write" buffer with ordinary
 * Gaussian deviates, used in the Langevin step. It is
 * called only by the main thread
 * ---------------------------------------------------*/

void Next_Gaussians()
    {
    int n, size;
    size = 3*N;
    for(n=1; n<=size; n++) write_buffer[n] = Grand();
    }

/*-----------------------------------------
 * This function switches write_buffer and
 * read_buffer
 * ---------------------------------------*/

 void Switch_Buffers()
    {
    double *v;
	v = write_buffer;
	write_buffer = read_buffer;
	read_buffer = v;
	}

/* ------------------------------------------------
 * The next trajectory length is the number of times 
 * the random number generator is called before it
 * returns a value smaller than "pb".
 * ------------------------------------------------*/

int Next_Trajectory_Length()
    {
    int retval;
    double d;

    retval = 1;
    do
       {
       d = RandomR(&randSeed);
       retval++;
       }while( d>pb );
    return retval;
    }

/* This is the correct Test_Acceptance() function that
 * works in the C++ code */

int Test_Acceptance(double dE)
    {
    int retval;
    double test, scratch;
		 
    retval = 1;
    if( dE>0.0 ) accepted++;
	else
        {
	test = exp(dE);
        scratch = RandomR(&randSeed);
        if(scratch < test) accepted++;
	    else 
            {
            rejected++;
            retval = 0;
            }
        }
    return retval;
    }


void Analyze_Data(Stats *pS)
    {
    int i, j, size;
    double scr, exact;
    int sgn_defect, n_events;
    double sigma2, qf, qfM;

    size = 3*N;
    for(i=1; i<=size; i++)    /* do statistics */
       for(j=i; j<=size; j++) 
	   {
	   MV[i][j] /= nSamples;  /* holds averages */
	   MV2[i][j] /= nSamples;
	   scr = MV[i][j];
	   MV2[i][j] -= scr*scr;   /* holds sigma2 */
	   }

    sigma2 = 0.0;    /* compute max sigma squared */
    for(i=1; i<=size; i++)
       for(j=i; j<=size; j++) 
	   {
	   scr = MV2[i][j];
	   if( scr > sigma2 ) sigma2 = scr;
	   }
    sigma2 /= (nSamples-1);
    
    sgn_defect = 0;      /* compare signs */
    n_events  = 0;
    for(i=1; i<=size; i++)
       for(j=i; j<=size; j++) 
	   {
	   scr = MV[i][j];
           exact = D[i][j];
	   if(fabs(scr) > cut && fabs(exact) > cut)
	      {
	      n_events++;
	      if (scr * exact < 0.0) sgn_defect++;
	      }
	   }	  

    qf = 0.0;            /* compute quality factors */
    qfM= 0.0;
    for(i=1; i<=size; i++)
       for(j=i; j<=size; j++)
	    {  
	    scr = fabs(MV[i][j] - D[i][j]);
	    qf += scr;
	    if(scr > qfM) qfM = scr;
	    }
    qf *= 2.0/(size*(size+1));

    pS->nEvents = n_events;
    pS->signDefect = sgn_defect;
    pS->maxError = sqrt(sigma2);
    pS->QF_Avg = qf;
    pS->QF_Max = qfM;
    }


void Print_Report(Stats *pS)  
    {
    printf("\n\n N=%d  nTh=%d  nSteps=%d", N, nTh, nSteps);
    printf("\n delta=%g  probability=%g \n", delta, pb);
	
    printf("\n Accepted =  %d  Rejected = %d", accepted, rejected);
    printf("\n tMin = %d  tMax = %d  tAvg = %d\n", tMin, tMax, tAvg);

    printf("\n Samples : %d  Iterations per sample : %d", 
            nSamples, nSteps);
    printf("\n Cut value         : %g", cut);
    printf("\n Sign events       : %d", pS->nEvents);
    printf("\n Sign errors       : %d", pS->signDefect);
    printf("\n Max Error         : %g", pS->maxError);
    printf("\n Average QFactor   : %g", pS->QF_Avg);
    printf("\n Worst QFactor     : %g", pS->QF_Max);
    }


double Partial_Energy(int nl, int nh, int flag)
    {
    int n, j, size;
    double engy, scr;

	/* We have q at time T, and p at time T-0.5*delta.
	 * We need to recompute the acceleration at present
	 * time T, to be able to compute p at time T.
	 * Since we are recomputing the acceleration, this
	 * function call MUST be preceeded by a barrier call,
	 * if it is called in the middle of a MD trajectory
	 * calculation. We must make sure that everybody has 
	 * finished updating its (q, p) values */
    
	size = 3*N;
	if(flag)
       {
	   for(n=nl; n<=nh; n++)
          {
          scr = 0.0;
          for(j=1; j<=size; j++) scr += D[n][j] * q[j];
	      a[n] = scr;
	      }
       }
       
	/* No barrier is needed here, since we will not be
	 * updating coordinates and momenta */

    engy = 0.0;
	/* Compute potential energy */
    for(n=nl; n<=nh; n++) engy += 0.5*q[n]*a[n];
	
	/* Compute kinetic energy */
    if(flag)
       {
	   for(n=nl; n<=nh; n++) 
          {
		  /* This step shifts momenta by 0.5*delta */
          scr = p[n] + 0.5 * a[n] * delta; 
          engy += 0.5*scr*scr;
          }
       }
    else
       {
	   for(n=nl; n<=nh; n++) 
          {
          scr = read_buffer[n]; 
          engy += 0.5*scr*scr;
          }
       }
	
    return engy;
    }

 void Timer_Report()
   {
   long clock_ticks;
   double wtime, utime, stime;

   clock_ticks = sysconf(_SC_CLK_TCK);
   wtime = end_time/(double) clock_ticks;
   utime = (tms_E.tms_utime - tms_S.tms_utime)/(double)clock_ticks;
   stime = (tms_E.tms_stime - tms_S.tms_stime)/(double)clock_ticks;
   fprintf(stdout, "\n\n Timer Report : \n\n");
   fprintf(stdout, " WALL TIME : %7.2f\n", wtime);
   fprintf(stdout, " USER TIME : %7.2f\n", utime);
   fprintf(stdout, " SYST TIME : %7.2f\n", stime);
   }

void err_exit(char *ch)
    {
    printf("\n %s \n", ch);
    exit(0);
    }
/***************************************************************/
		
		
