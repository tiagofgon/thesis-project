// =======================================================
// File TestGen.C
//
// Testing the class GaussGen.
// =======================================================
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Rand.h>
#include <CpuTimer.h>
#include <GaussGen.h>

typedef struct hymc
   {
   int      nEvents;
   int      signDefect;
   double   maxError;
   double   QF_Avg;
   double   QF_Max;
   } Stats;

// Initialization and ressource managment
// -------------------------------------- 
void   Init_Job();       
void   Input_Data();
void   Alloc_Vectors();
void   Close_Job();      
void   Free_Vectors();
void   Build_Matrix(double **d, int SZ, int of);

// Functions for this application
// ------------------------------
void   Analyze_Data(Stats *P);
void   Print_Report(Stats *P);

// Global internal variables 
// -------------------------
double **D;
double *M, **MV, **MV2;
Rand   R(777);

// Global variables read in file "cgauss.dat"
// ------------------------------------------
int   N, nTh, nSamples, nSteps;
int   I, J;
float  delta, pb, cut;
float  dA, dB, dC;


int main(int argc, char **argv)
    {
    int n, i, j, count, size, sample;
    double scr;
    CpuTimer T;
    Stats S;

    Init_Job();
    size = 3*N;
    Build_Matrix(D, size, 0);
    GaussGen *GG = new GaussGen(D, M, size, nTh, delta, pb);
    
    for(i=0; i<size; i++)
       for(j=i; j<size; j++) 
	   {
	   MV[i][j] += 0.0;
	   MV2[i][j] += 0.0;
	   }

    T.Start();
    for(sample=1; sample<=nSamples; sample++)
        {
        // ---------------------------
	GG->Request_Vector(nSteps);  // run nSteps MC steps 
	GG->Wait_For_Request();
        // ---------------------------
	for(i=0; i<size; i++)
           for(j=i; j<size; j++) 
	       {
	       scr = M[i]*M[j];
	       MV[i][j] += scr;
	       MV2[i][j] += scr * scr;
	       }

    	if( sample%100 == 0 ) printf("\n sample %d done", sample);
	    }	// end of samples loop
     T.Stop();
     
     //tAvg /= (nSamples*nSteps);
     Analyze_Data(&S);
	
     printf("\n\n I=%d  J=%d : Computed = %g   Exact = %g", 
             I, J, MV[I][J], D[I][J]);
     GG->Print_Report();
     Print_Report(&S);
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
    }


void Close_Job()
    {
    Free_Vectors();
    }


void Build_Matrix(double **d, int sz, int offs)
     {
     int     n, i, j, iM, jM;
     int     NN, rest;
     double  x, v, w, scratch;
     double  nx, ny, nz;
     double *rx, *ry, *rz;
     double **AA;
     Rand Ra(1234);

     NN = sz/3;
     rest = sz%3;

     if(rest)
        {
        std::cout << "\n Incorrect matrixx size. Aborting" << std::endl;
        exit(0);
        }
     
     if(offs == 1) AA = d;
     else
        {
        // allocate offset 1 AA
        // --------------------
        AA = (double **)malloc( (sz+1)*sizeof(double *) );
        AA[1] = (double *)malloc ( (sz*sz+1)*sizeof(double) );
        for(int n=2; n<=sz; n++) AA[n] = AA[n-1] + sz;
        }
      
     // ---------------------------------------------
     // Here, we use 2a=1 units. Place randomly the
     // particles in a box of size 6, and do first 
     //  diagonal submatrix of AA 
     //  -------------------------------------------

     rx = (double *)malloc((NN+1)*sizeof(double));
     ry = (double *)malloc((NN+1)*sizeof(double));
     rz = (double *)malloc((NN+1)*sizeof(double));

     rz[0] = 0.0; 
     for(n=1; n<=NN; n++)
        {
	rx[n] = 2.0 * dC * Ra.draw() - dC;
	ry[n] = 2.0 * dC * Ra.draw() - dC;
	rz[n] = 2.0 * dB * Ra.draw() + (dA-dB) + rz[n-1];
	}
	 
     for(i=1; i<=sz; i++)
	for(j=1; j<=sz; j++) AA[i][j] = 0.0; 

     for(i=1; i<=sz; i++) AA[i][i] = 1.0;

     // -----------------------------------------
     // Do off diagonal submatrices, use symmetry
     // -----------------------------------------

     for(i=1; i<=NN; i++)
         {
	 for(j=(i+1); j<=NN; j++)
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

             AA[iM][jM] = v + (w * nx * nx);
             AA[jM][iM] = AA[iM][jM];
             AA[iM+1][jM+1] = v + (w * ny * ny);
             AA[jM+1][iM+1] = AA[iM+1][jM+1];
             AA[iM+2][jM+2] = v + (w * nz * nz);
             AA[jM+2][iM+2] = AA[iM+2][jM+2];

             scratch = (w * nx * ny);

             AA[iM][jM+1] = scratch;
             AA[iM+1][jM] = scratch;
             AA[jM+1][iM] = scratch;
             AA[jM][iM+1] = scratch;

             scratch = (w * nx * nz);

             AA[iM][jM+2] = scratch;
             AA[iM+2][jM] = scratch;
             AA[jM+2][iM] = scratch;
             AA[jM][iM+2] = scratch;

             scratch =  (w * ny * nz);

             AA[iM+1][jM+2] = scratch;
             AA[iM+2][jM+1] = scratch;
             AA[jM+2][iM+1] = scratch;
             AA[jM+1][iM+2] = scratch;
             }
         }

     // -----------------------------------------------
     // Convert and copy matrix if external offset is 0,
     // and also release AA.
     // -----------------------------------------------
     if(offs==0)
         {
         int fullsize = sz*sz;
         for(n=0; n<fullsize; ++n) d[0][n] = AA[1][n+1];
         free(AA[1]);
         free(AA);
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
    size = 3*N;

    M   = (double *)malloc( size*sizeof(double) );
    D = (double **)malloc( size*sizeof(double *) );
    D[0] = (double *)malloc ( (size*size)*sizeof(double) );
    for(n=1; n<size; n++) D[n] = D[n-1] + size;
    
    MV = (double **)malloc( size*sizeof(double *) );
    MV[0] = (double *)malloc ( (size*size)*sizeof(double) );
    for(n=1; n<size; n++) MV[n] = MV[n-1] + size;

    MV2 = (double **)malloc( size*sizeof(double *) );
    MV2[0] = (double *)malloc ( (size*size)*sizeof(double) );
    for(n=1; n<size; n++) MV2[n] = MV2[n-1] + size;
    }
	
void Free_Vectors()
    {
    free(M);
    free( D[0] );
    free(D);
    free( MV[0] );
    free(MV);
    free( MV2[0] );
    free(MV2);
    }


void Analyze_Data(Stats *pS)
    {
    int i, j, size;
    double scr, exact;
    int sgn_defect, n_events;
    double sigma2, qf, qfM;

    size = 3*N;
    for(i=0; i<size; i++)    /* do statistics */
       for(j=i; j<size; j++) 
	   {
	   MV[i][j] /= nSamples;  /* holds averages */
	   MV2[i][j] /= nSamples;
	   scr = MV[i][j];
	   MV2[i][j] -= scr*scr;   /* holds sigma2 */
	   }

    sigma2 = 0.0;    /* compute max sigma squared */
    for(i=0; i<size; i++)
       for(j=i; j<size; j++) 
	   {
	   scr = MV2[i][j];
	   if( scr > sigma2 ) sigma2 = scr;
	   }
    sigma2 /= (nSamples-1);
    
    sgn_defect = 0;      /* compare signs */
    n_events  = 0;
    for(i=0; i<size; i++)
       for(j=i; j<size; j++) 
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
    for(i=0; i<size; i++)
       for(j=i; j<size; j++)
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
    printf("\n Cut value         : %g", cut);
    printf("\n Sign events       : %d", pS->nEvents);
    printf("\n Sign errors       : %d", pS->signDefect);
    printf("\n Max Error         : %g", pS->maxError);
    printf("\n Average QFactor   : %g", pS->QF_Avg);
    printf("\n Worst QFactor     : %g", pS->QF_Max);
    }

/***************************************************************/
		
		
