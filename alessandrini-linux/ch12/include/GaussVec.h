// File GaussGen.h
// ---------------
// Interface class for a correlated gaussian generator
// ---------------------------------------------------
#include<SPool.h>
#include<Barrier.h>
#include<Rand.h>

class GaussVec
   {
   private:
    int   size;
    int   mcSteps;
    int   accept, randSeed; 
    int   tMin, tMax, tAvg, tlength;
    long  accepted, rejected;
    int   nTh;
    int   nbrSamples;

    int   result_flag;
    double ransave;

    double  **D;       // external matrix
    double   *V;       // external vector, return value
    double   *read_buffer;

    double  *p, *q, *a;
    double  *qS, *pS;
    double  *Ei, *Ef;
    double  delta, pb;

    Barrier *B;
    SPool   *TP;
    Rand    *R;

    double Grand(); 
    void   err_exit(const char *C);
    void   Initial_State();
    void   Alloc_Vectors();
    void   Free_Vectors();
    int    Next_Trajectory_Length();
    void   Next_Gaussians();
    int    Test_Acceptance(double dE);
    double Partial_Energy(int n1, int n2, int fg);

   public:
    GaussVec(double **d, double *v, int vSize, int nThreads, 
             double dt, double prob, long seed);
    ~GaussVec();
    void Print_Report();
    void Reset();

    void   MCTask();
    void Request_Vector(int steps);
    void Request_Vector(double *v, int steps);
    void Wait_For_Request();
   };
