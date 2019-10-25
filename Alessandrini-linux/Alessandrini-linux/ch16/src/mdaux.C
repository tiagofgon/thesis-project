// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
//  This is source file mdaux.C
//
// Auxiliary functions used in the molecular dynamics code
// =======================================================

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Rand.h>
#include <iostream>

// Auxiliary functions declarations
// --------------------------------
void   Build_Matrix(double **d, int sz, int offset);
double Grand();

// Variables used locally by Grand()
// ---------------------------------
double ransave;
int    flag;
Rand   RG(999);

extern float  dA, dB, dC;
extern int N, nTh, NTk, nSteps, nSamples, Gr;
extern double delta;
extern double **D;
extern double *q, *p, *a;
extern double *qs;

//**********************************
//     Auxiliary functions
//**********************************

void InitJob()
    {
    // read data from file "md.dat"
    // ----------------------------
    FILE *fp;
    char buffer[128];

    if( !(fp = fopen("mdd.dat", "r") ))
	   {
	   printf("\n Input error in opening file\n");
	   exit(0);
	   }
	
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &N) ;
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &nTh);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &NTk);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &nSteps);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &nSamples);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &Gr);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%g %g %g", &dA, &dB, &dC );
    fgets(buffer, 80, fp);
    sscanf(buffer, "%g", &delta );
    fclose(fp);

    // perform memory allocations
    // --------------------------
    int SZ = 3*N;
    // matrix
    D = (double **)malloc( SZ*sizeof(double *) );
    D[0] = (double *)malloc ( (SZ*SZ)*sizeof(double) );
    for(int n=1; n<SZ; n++) D[n] = D[n-1] + SZ;
    // vectors
    q   = (double *)malloc( SZ*sizeof(double) );
    p   = (double *)malloc( SZ*sizeof(double) );
    a   = (double *)malloc( SZ*sizeof(double) );
    qs   = (double *)malloc( SZ*sizeof(double) );

    // Construct D matrix
    // ------------------
    Build_Matrix(D, SZ, 0);

    // Define the initial state
    // ------------------------
    for(int n=0; n<SZ; ++n) 
       {
       q[n] = Grand();
       p[n] = Grand();
       }
    }

void CloseJob()
    {
    // release all memory allocations
    // ------------------------------
    free( D[0] );
    free(D);
    free(a);
    free(p);
    free(q);
    free(qs);
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
        std::cout << "\n Incorrect size. Aborting" << std::endl;
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


double Grand()
   {
   double x1, x2, scratch;
   const double pi = 3.1415926535;
   
   if(flag)
      {
      flag = 0;
      return ransave;
      }
   else
      {
      x1 = RG.draw();
      x2 = RG.draw();
      scratch = sqrt(-2*log(x1));
      x1 = cos(2 * pi * x2);
      x2 = sin(2 * pi * x2);
      ransave = scratch * x2;
      flag = 1;
      return(scratch * x1);
      }
   }

/***************************************************************/
		
		
