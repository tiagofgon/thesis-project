// Hybrid MPI-Threads application
// ==============================
//
// Statistical analysis of the correlated Gaussian generator GaussGen
//
// GaussGen is an intrinsic multithreaded routine with an internal
// propietary SPool thread pool. It is called by a client thread, that
// does not get involved in the computation, does other things and when
// the time comes waits for the returned vector
//
// We have in this code N MPI processes. The master (N=0) receives
// correlated Gaussian vectors samples from the (N-1 slaves), and 
// performs a global statistical analysis.
//
// This is a produce-consumer parallel pattern, with a lot of producers
// and one consumer. The point is that producers are asked to perform
// an amount of work to produce samples, easily processed by the
// consumer.
//
// Each slave runs a GaussGen object. This example shows how easy it 
// is to overlap computations and communications: each slave process 
// submits a new vector request to its propietary GaussGen and, before 
// waiting for the result, sends to the master the outcome of the 
// previous request.
//
// The basic MPI message passing routines used here are:
// ----------------------------------------------------
//
// MPI_Send(ovector,         // memory buffer holding the message
//          vsize,           // number of data items in buffer
//          MPI_DOUBLE,      // type of data item in buffer
//          0,               // rank of receiving MPI process (0 is master)
//          MSG,             // an integer "tag" that labels the message
//          MPI_COMM_WORLD); // "communicator" 
//
// The message tag is normally used by the reveiving MPI processes to
// distinguish between different messages coming from the same source.
// This is not needed in this application, and we use a unique tag for
// everybody.
//
// MPI_Status stat;
// ...
// MPI_Recv(vdata,          // buffer
//          vsize,          // number of data items
//          MPI_DOUBLE,     // type of data item
//          sl ,            // rank of source process
//          MSG,            // tag
//          MPI_COMM_WORLD, // communicator
//          &stat);         // further information stored here
//
// This function catches the messages coming from the source "sl". But
// it can also catch messages coming from any source, using the ANY_SOURCE
// identifier instead of the source rank. In all cases, this function
// stores source information in the "stat" data item. A process that has
// used ANY_SOURCE and wants further source information can get it from
// the return value of "stat".
// 
// MpiGauss.C : Any number of slaves
// ====================================================================

#define MSG   1         // message tag

#include <math.h>
#include <mpi.h>
#include <GaussVec.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <Rand.h>
#include <CpuTimer.h>

// Initialization and ressource managment
// --------------------------------------
void   Init_Job();       
void   Input_Data();
void   Alloc_MasterVectors();
void   Free_MasterVectors();
void   Close_Job();      
void   Alloc_SlaveVectors();
void   Free_SlaveVectors();
void   Initial_State();
void   SwitchVectors();
void   Build_Matrix(double **d, int sz, int offs);

// Auxiliary functions for post-treatment
// --------------------------------------
void   Analyze_Data(int nSl);
void   Report_Results(int n, int m, int d);

// Global internal variables 
// -------------------------
double    **D;               // all
double    **MV, **MV2;       // master
CpuTimer  T;                 // master

GaussVec  *GV;               // slaves
Rand      *R;                // slaves

// Global variables read in file "cgauss.dat"
// -----------------------------------------
int   N, nTh, nSamples, nSteps;
int   I, J, mem_affinity;
float  delta, pb, cut;
float  dA, dB, dC;


// -----------------------------------------------------------------
// Data buffers used for message passing:
// --------------------------------------
// vdata is a vector where the master process receives correlated
// Gaussian vectors from the slaves.
//
// ivector and ovector are vectors owned by each slave, where the 
// vectors they compute are stored. There are two vector buffers per 
// slave in order to overlap computations and communications in the
// way to be shown below: while ivector receives the return of the
// parallel (multithreaded) generator, ovector is involved in
// the transfer of the preceeding result to the master
// ------------------------------------------------------------------
double *vdata;         // owned by master
double *ivector;       // one per slave
double *ovector;       // one per slave 

int main(int argc, char **argv)
   {
   MPI_Status stat;     // Used by Mpi_Recv, not needed here
   int myid;            // MPI rank of this process in [0, numprocs-1]
   int numprocs;        // number of MPI processes
   int vsize;           // vector size
   int thread_support;  // to know about the thread support level
   CpuTimer *T;         // only master will measure times

   // MPI Initialization
   // ------------------
   MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &thread_support);
   MPI_Comm_size(MPI_COMM_WORLD, &numprocs);     // get numprocs
   MPI_Comm_rank(MPI_COMM_WORLD, &myid);         // get myid
   int nSlaves = numprocs-1;

   if(argc==2) nSamples = atoi(argv[1]);  // read the number of samples
   else nSamples = 2000;

   // -----------------------------------------------------------
   // Initializaton steps. Master will allocate one communication
   // buffer, where slaves data is collected. This is sufficient in
   // the present case, where master-slave communicatons are blocking
   // Each slave has two vector buffers, to overlap computation and
   // communication.
   //
   // Master does not realy needs to know the correlation matrix D for
   // the computation: only slaves need D. But master also allocates
   // D in order to be able to print the input target correlations 
   // and compare with the computational results.
   // ----------------------------------------------------------
   if(myid==0)
       {
       // ---------------------
       // master initialization
       // ---------------------
       Input_Data();
       vsize = 3*N;
       T = new CpuTimer();
       
       Alloc_MasterVectors();
       Build_Matrix(D, vsize, 0);
       for(int i=0; i<vsize; i++)
          for(int j=0; j<vsize; j++) 
	      {
	      MV[i][j] = 0.0;
	      MV2[i][j] = 0.0;
	      }
       }
   else
       {
       // --------------------
       // slave initialization
       // --------------------
       R = new Rand(999*myid);
       Input_Data();
       vsize = 3*N;
       Alloc_SlaveVectors();
       Build_Matrix(D, vsize, 0);
       GV = new GaussVec(D, ovector, vsize, 2, delta, pb, 999*myid);
       }

   if(myid == 0)
      {
      std::cout << "\n Number of procs = " << numprocs << std::endl;
      if(thread_support >= MPI_THREAD_FUNNELED)
         std::cout << "\n Funneled multithreading supported" << std::endl;
      }
   MPI_Barrier(MPI_COMM_WORLD);

   // ---------------------------------------------
   // End of initialization step. Enter computation
   // ---------------------------------------------

   if(myid == 0)  // master code
      {
      T->Start();
      for(int sample=0; sample<nSamples; sample++) 
	 {
         for(int sl=1; sl <=nSlaves; sl++)
            {
	    MPI_Recv(vdata, vsize, MPI_DOUBLE, sl , MSG, MPI_COMM_WORLD, &stat);

            // Collect mean values and correlations in matrices MV 
            // and MV2 for later analysis
            // ---------------------------------------------------
	    for(int i=0; i<vsize; i++)
               for(int j=0; j<vsize; j++) 
	          {
	          double scr = vdata[i]*vdata[j];
	          MV[i][j] += scr;
	          MV2[i][j] += scr * scr;
	          }
            }

    	 if( sample%100 == 0 )    // report message
            std::cout << "\n sample " << sample << " done" << std::endl;
         }
      T->Stop();
      }    // end of master code
   else
      {   
      // Slave code. Start by filling the ivector buffer with data
      // ---------------------------------------------------------
      GV->Request_Vector(ivector, nSteps);
      GV->Wait_For_Request();
      for(int sample=0; sample<nSamples; sample++) 
         {
         SwitchVectors();  // exchange ivector <--> ovector
         GV->Request_Vector(ivector, nSteps);
        
         // Before waiting, send previous vector to the master
         // --------------------------------------------------
	 MPI_Send(ovector, vsize, MPI_DOUBLE, 0, MSG, MPI_COMM_WORLD);
         GV->Wait_For_Request();
         }
      }  // end of slave code

   MPI_Barrier(MPI_COMM_WORLD);   // a safenet   

   // ------------------------------------------------------------
   // One of the slaves prints some data: report of the generator
   // operation, and one exact target correlation to be compared
   // to the computational result (only slaves know D)
   // ------------------------------------------------------------
   if(myid == 1) GV->Print_Report();
   
   MPI_Barrier(MPI_COMM_WORLD);

   // ----------------------------------------------------------
   // Master performs statistical analysis and reports results on
   // on the generator quality
   // ----------------------------------------------------------
   if(myid == 0) // here, master performs statistical analysys
       {
       Analyze_Data(nSlaves);
       T->Report();
       Report_Results(6, 9, 2);
       Free_MasterVectors();
       }
   else Free_SlaveVectors();

   MPI_Finalize();
   return 0;
   }
	

// Auxiliary functions
// -------------------		 
		 
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

void SwitchVectors()
    {
    double *temp = ovector;
    ovector = ivector;
    ivector = temp;
    }
		
void Alloc_MasterVectors()
    {
    int n, size;
    size = 3*N;

    vdata = (double *)malloc( size*sizeof(double));

    D = (double **)malloc( size*sizeof(double *) );
    D[0] = (double *)malloc ( (9*N*N)*sizeof(double) );
    for(n=1; n<size; n++) D[n] = D[n-1] + size;

    MV = (double **)malloc( size*sizeof(double *) );
    MV[0] = (double *)malloc ( (9*N*N)*sizeof(double) );
    for(n=1; n<size; n++) MV[n] = MV[n-1] + size;

    MV2 = (double **)malloc( size*sizeof(double *) );
    MV2[0] = (double *)malloc ( (9*N*N)*sizeof(double) );
    for(n=1; n<size; n++) MV2[n] = MV2[n-1] + size;
    }


void Alloc_SlaveVectors()
    {
    int n, size;
    size = 3*N;

    ivector = (double *)malloc( size*sizeof(double));
    ovector = (double *)malloc( size*sizeof(double));
    D = (double **)malloc( size*sizeof(double *) );
    D[0] = (double *)malloc ( (9*N*N)*sizeof(double) );
    for(n=1; n<size; n++) D[n] = D[n-1] + size;
    }
	
	
void Free_MasterVectors()
    {
    free( vdata );
    free( D[0] );
    free(D);
    free( MV[0] );
    free(MV);
    free( MV2[0] );
    free(MV2);
    }

void Free_SlaveVectors()
    {
    free( D[0] );
    free(D);
    free(ivector);
    free(ovector);
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


void Analyze_Data(int nSl)
    {
    int i, j, size;
    double scr, exact;
    double sigma2;

    size = 3*N;
    for(i=0; i<size; i++)    // do statistics 
       for(j=0; j<size; j++) 
	   {
	   MV[i][j] /= (nSamples*nSl);  // holds averages 
	   MV2[i][j] /= (nSamples*nSl);
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
    sigma2 /= (nSamples*nSl-1);
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
