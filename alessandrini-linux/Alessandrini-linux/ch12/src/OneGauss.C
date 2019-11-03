// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
/* OneGauss.C
 *
 * This example generates a lerge number of samples of
 * correlated Gaussian vectors, using the parallel GaussVec
 * routine, and checks that they have the required properties.
 *
 * One producer GaussVec object is used.
 * =======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GaussVec.h>
#include <Rand.h>
#include <CpuTimer.h>
#include <iostream>
#include <iomanip>

/* Initialization and ressource management
   -------------------------------------*/ 
void   Init_Job();       
void   Input_Data();
void   Alloc_Vectors();
void   Close_Job();      
void   Free_Vectors();
void   Initial_State();
void   Build_Matrix(double **d, int sz, int offs);

/* Functions for this application
   ------------------------------*/
void   Analyze_Data();
void   Report_Results(int i, int j, int d);

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

    Init_Job();
    size = 3*N;
    Build_Matrix(D, size, 0);
    
    T.Start();
    for(i=0; i<size; i++)
       for(j=0; j<size; j++) 
	   {
	   MV[i][j] += 0.0;
	   MV2[i][j] += 0.0;
	   }

    for(sample=1; sample<=nSamples; sample++)
        {
        GV->Request_Vector(nSteps);
        GV->Wait_For_Request();
	   
	for(i=0; i<size; i++)
           for(j=0; j<size; j++) 
	       {
	       scr = M[i]*M[j];
	       MV[i][j] += scr;
	       MV2[i][j] += scr * scr;
	       }
    	if( sample%100 == 0 ) printf("\n sample %d done", sample);
	}	/* end of samples loop */
     T.Stop();
     
     Analyze_Data();
     GV->Print_Report();
     std::cout << std::endl;
     Report_Results(I, J, 2);
	
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
    D[0] = (double *)malloc ( (9*N*N)*sizeof(double) );
    for(n=1; n<3*N; n++) D[n] = D[n-1] + 3*N;
    
    MV = (double **)malloc( size*sizeof(double *) );
    MV[0] = (double *)malloc ( (9*N*N)*sizeof(double) );
    for(n=1; n<3*N; n++) MV[n] = MV[n-1] + 3*N;

    MV2 = (double **)malloc( size*sizeof(double *) );
    MV2[0] = (double *)malloc ( (9*N*N)*sizeof(double) );
    for(n=1; n<3*N; n++) MV2[n] = MV2[n-1] + 3*N;
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

void Analyze_Data()
    {
    int i, j, size;
    double scr, exact;
    double sigma2;

    size = 3*N;
    for(i=0; i<size; i++)    // do statistics 
       for(j=0; j<size; j++) 
	   {
	   MV[i][j] /= nSamples;  // holds averages 
	   MV2[i][j] /= nSamples;
	   scr = MV[i][j];
	   MV2[i][j] -= scr*scr;   // holds sigma2 
	   }

    sigma2 = 0.0;    // compute max sigma squared 
    for(i=0; i<size; i++)
       for(j=0; j<size; j++) 
	   {
	   scr = MV2[i][j];
	   if( scr > sigma2 ) sigma2 = scr;
	   }
    sigma2 /= (nSamples-1);
    }

// ----------------------------------------------------------
// This function reports the target (requested) values of the 
// correlations : D[k][l], D[k+d][l+d], ..., D[k+9*d][l+9*d]
// as well as the values measured from the collection of vector
// samples. The indices k,l are read from theinput file
// -----------------------------------------------------------
void Report_Results(int k, int l, int d)
   {
   int I = k;
   int J = l;

   std::cout << "\n Indices       Exact        Computed  " << std::endl;
   for(int count = 0; count<10; count++)
      {
      std::cout <<std:: setw(2) << I <<"   " << std::setw(2) << J 
                << "        " <<  D[I][J] << "       " 
                << MV[I][J]  << std::endl;
      I += d;
      J += d;
      }
   }


/***************************************************************/
		
		
