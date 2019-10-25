// File HeatUtility.h
// ******************
// ---------------------------------------------------------------
// This function prints a N dimensional vector of complex numbers 
// of type T to stdout. It prints first the N real parts, and
// then the N imaginary parts. It prints 5 T items per line
// ---------------------------------------------------------------
template<typename T>
void PrintCVector(std::complex<T> *V, int N)
   {
   // Offset 0, size N
   int k;
   std::cout << "\n Real part: " << std::endl;
   for(k=0; k<N; ++k)
      {
      std::cout << V[k].real() << "    ";
      if( (k+1)%5 == 0) std::cout << std::endl;
      }
   std::cout << "\n Imaginary part: " << std::endl;
   for(k=0; k<N; k++)
      {
      std::cout << V[k].imag() << "    ";
      if( (k+1)%5 == 0) std::cout << std::endl;
      }
   std::cout << std::endl;
   }


// -------------------------------------------------------
// This function prints a N dimensional vector of elements
// of type T to stdout. It prints 5 T elements per line
// -------------------------------------------------------
template<typename T>
void PrintRVector(T *V, int N)
   {
   // Offset 0, size N
   int k;
   std::cout << "\n Values of vector: " << std::endl;
   for(k=0; k<N; k++)
      {
      std::cout << V[k] << "    ";
      if( (k+1)%5 == 0) std::cout << std::endl;
      }
   std::cout << std::endl;
   }


template<typename T>
void PrintStatus(std::complex<T> *V, int N, int M, float t1, float t2)
   {
   // --------------------------------------------------------------
   // V is a complex vector of size N*M. Each line of length N packs
   // N elements of type complex<T>.
   // This routine selects the line M/2, and prints the start, middle 
   // and end values corresponding to the real part (time t1) and 
   // the imaginary part (time t2)
   // --------------------------------------------------------------
   int pos = N*M/2;                 // this is start of line M/2
   std::cout << "\n Current time : " << t1 ;
   std::cout << "\n End real values " << V[pos].real() << "     " 
             << V[pos+N/2].real() << "     " << V[pos+N-1].real();
   std::cout << std::endl;
   
   std::cout << "\n Current time : " << t2 ;
   std::cout << "\n End real values " << V[pos].imag() << "     " 
             << V[pos+N/2].imag() << "     " << V[pos+N-1].imag();
   std::cout << std::endl;
   }
 

// ----------------------------------------------------------
// This function will initialize the initial condition array. 
// D is a vector of length N*M. This routine initializes N*M 
// values starting at offset 0, using the function fct(x, y).
//
// The value ranges are: 
// na <= x <= nb   mapped to [0, N-1]
// ma <= y <= mb   mapped to [0, M-1]
// ---------------------------------------------------------------
template<typename T>
void RInitialCondition(T *D, int N, int M, T(*fct)(T, T),
                      T na, T nb, T ma, T mb)
   {
   int m, n, kb;
   T x, An, Bn;
   T y, Am, Bm;
   
   An = (nb-na)/(N-1);
   Bn = na;
   Am = (mb-ma)/(M-1);
   Bm = ma;
   for(m=0; m<N; ++m)
      {
      y = Am * m + Bm;
      kb = m*N;
      for(n=0; n<N; ++n)
         {
         x = An * n + Bn;
         D[kb+n] = fct(y, x);
         }
      }
   }

// ------------------------------------------------
// This function initializes a complex initial
// condition array, fct1 and fct2 are the functions
// for the real and imaginary parts
// ------------------------------------------------
template<typename T>
void CInitialCondition(std::complex<T> *D, int N, int M, T(*fct1)(T, T), 
                       T(*fct2)(T, T), T na, T nb, T ma, T mb)
   {
   int k;
   T *Dd= new T[N*M];

   RInitialCondition(Dd, N, M, fct1, na, nb, ma, mb);
   for(k=0; k<(N*M); k++) 
       {
       //D[k].real() = Dd[k]; 
       std::complex<T> c(Dd[k], 0.0);
       D[k] = c;
       }
   RInitialCondition(Dd, N, M, fct2, na, nb, ma, mb);
   for(k=0; k<(N*M); k++) 
       {
       //D[k].imag() = Dd[k];
       std::complex<T> c(0.0, Dd[k]);
       D[k] += c;
       }
   delete [] Dd; 
   }
