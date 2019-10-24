/* =======================================================
 * Copyright(c) 2002-2003 Victor Alessandrini
 * All rights reserved.
 * Code from book "Application Programming with
 * Posix Threads"
 * ------------------------------------------
 * This is source file GAUSS1.C
 * This the /proj/hymc/bench/gauss.C code, using the
 * vath library.
 *
 * This is Gauss2.C.
 * ----------------
 *
 * We supress the call to Distribute_Indices, using the
 * ThreadRange built in function. Arrays NL, NH are no
 * longer needed.
 *
 * This is Gauss3.C
 * ----------------
 * Modified NextGaussians(). SwitchBuffers no longer needed.
 * =======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GaussVec.h>
#include <Rand.h>
#include <CpuTimer.h>
#include <iostream>

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

/* Functions for this application
   ------------------------------*/
void   Analyze_Data(Stats *P);
void   Analysis_Report(Stats *P);

/* Global internal variables 
   -------------------------*/
double    **D;
double    *M, **MV, **MV2;
CpuTimer  T;
GaussVec  *GV;
Rand      R(789);

/* Global variables read in file "cgauss.dat"
   ----------------------------------------*/
int   N, nTh, nSamples, nSteps;
int   I, J, mem_affinity;
float  delta, pb, cut;
float  dA, dB, dC;


/* -------------------
 * The main() function 
 * ------------------*/

int main(int argc, char **argv)
    {
    int n, i, j, count, size, sample;
    double scr;
    Stats S;

    Init_Job();
    Build_Matrix();
    
    T.Start();
    size = 3*N;
    for(i=1; i<=size; i++)
       for(j=i; j<=size; j++) 
	   {
	   MV[i][j] += 0.0;
	   MV2[i][j] += 0.0;
	   }

    for(sample=1; sample<=nSamples; sample++)
        {
        GV->Request_Vector(nSteps);
        GV->Wait_For_Request();
	   
	for(i=1; i<=size; i++)
           for(j=i; j<=size; j++) 
	       {
	       scr = M[i]*M[j];
	       MV[i][j] += scr;
	       MV2[i][j] += scr * scr;
	       }
    	if( sample%100 == 0 ) printf("\n sample %d done", sample);
	}	/* end of samples loop */
     T.Stop();
     
     Analyze_Data(&S);
	
     printf("\n\n I=%d  J=%d : Computed = %g   Exact = %g", 
             I, J, MV[I][J], D[I][J]);

     GV->Print_Report();
     Analysis_Report(&S);
     printf("\n\n Timing for one generator with %d threads", nTh);
     T.Report();
     Close_Job();
     }
		
/***********************************
 *     Auxiliary functions
 **********************************/

void Init_Job()
    {
    Input_Data();
    Alloc_Vectors();
    GV = new GaussVec(D, M, 3*N, nTh, delta, pb, 777);
    }

void Close_Job()
    {
    delete GV;
    Free_Vectors();
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
	    rx[n] = 2.0 * dC * R.draw() - dC;
	    ry[n] = 2.0 * dC * R.draw() - dC;
	    rz[n] = 2.0 * dB * R.draw() + (dA-dB) + rz[n-1];
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

    M   = (double *)malloc( size*sizeof(double) );
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
    free(M);
    free( D[1] );
    free(D);
    free( MV[1] );
    free(MV);
    free( MV2[1] );
    free(MV2);
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


void Analysis_Report(Stats *pS)  
    {
    printf("\n Cut value         : %g", cut);
    printf("\n Sign events       : %d", pS->nEvents);
    printf("\n Sign errors       : %d", pS->signDefect);
    printf("\n Max Error         : %g", pS->maxError);
    printf("\n Average QFactor   : %g", pS->QF_Avg);
    printf("\n Worst QFactor     : %g", pS->QF_Max);
    }

/***************************************************************/
		
		
