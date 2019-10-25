// ************************************************************
// File Fft.h
// ----------
// This file contains an number of template utilities and FFT
// routines for 1D, 2D and any dimension fft computation.
// ************************************************************
#ifndef MYFFTCODE_H
#define MYFFTCODE_H

#include <complex>
#include <iostream>
#include <stdlib.h>
#include <math.h>

// ------------------------------------------
// Utility swap function used in FFT routines
// ------------------------------------------
template<typename T>
void ftpswap( T& a, T& b)
   {
   T temp;
   temp = a;
   a = b;
   b = temp;
   }

// ------------------------------------------------------
// This function receives an integer n, cheks if n=2**p
// and returns p. If n is not a power of 2, it returns 0
// THIS ROUTINE WORKS
// ------------------------------------------------------
int Ptwo(int n)
   {
   int count = 1;
   if(n<2)return 0;
   n /= 2;
   while(!(n%2))
      {
      count++;
      n /= 2;
      }
   if(n==1) return count;
   else return 0;
   }

// ------------------------------------------------------
// Transpose matrices. It operates on a matrix allocated
// in the standard way, with zero offset. Notice that this
// function receives an offset 0 vector of size width*height
// ---------------------------------------------------------
template<typename T>
void Transpose(T* mat, int width, int height)
   {
   int n;  // column index
   int m;  // row index
   for (int m = 0; m < height - 1; ++m) 
      {
      for (int n = m + 1; n < width; ++n) 
         {
         T buf = mat[m * width + n];
         mat[m * width + n] = mat[n * width + m];
         mat[n * width + m] = buf;
         }
      }
   }



// *************************************************************
// My version of the 1D FFT routine. The template function fft1 
// is a one dimensional fft using the complex<T> C++ class.
// Offset 0 complex vectors are assumed.
//
// This function follows the same conventions as four1 un NRC,
// except that we are assuming offset zero vectors. In this case,
// d[0, 1, ..., N-1] is a complex array with the data. On return,
// this array holds the FFT of the data.
//
// If isign=1, we have the direct FFT. If isign=-1, we have
// the inverse FFT
//
// sz HAS TO BE a power of 2. This is checked by the routine.
//
// THIS ROUTINE WORKS. GIVES SAME RESULTS AS NRC ROUTINE.
// ************************************************************

template<typename T>
void fft1( std::complex<T> d[], int sz, int isign)
   {
   int m, j;
   int nL, nLh, iSize, nChunks;
   
   double theta;
   std::complex<T> W, Wk, temp;

   // ----------------------------------------
   // Check that incoming size is power of two
   // ----------------------------------------
   m = Ptwo(sz);
   if(m==0)
      {
      std::cout << "\n Error in fft1 : incorrect data size" 
                << std::endl;
      exit(0);
      }

   // ------------------------------------------------------------
   // The first part of the routine is to sort the incoming data
   // array in bit reversed order. Offset zero vectors are assumed.
   // The algorithm below will establish bit reversed order fot
   // integers in [0, sz-1].
   // ------------------------------------------------------------

   j = 0;
   for(int i=0; i<sz; i++)
      {
      if(j>i) ftpswap(d[j], d[i]);     // swap if appropriate

      // Build the bit reversed number for the next i value
      // -------------------------------------------------- 
      m = sz>>1;      // n = N/2, shifts one bit to the right
      while(m >=1 && j>=m)
         {
         j -= m;
         m >>= 1;
         }
      j += m;
      }


   // -----------------------------------------------------
   // Now we have the Danielson-Lanczos part of the routine
   // -----------------------------------------------------

   for(nL=2; nL<=sz; nL*=2)
      {
      // ------------------------------------------------------------
      // This is an outer loop that runs on iteration stages. The
      // integer nL is the "size" of successive Fourier transforms
      // on a subset of data : 2, 4, 8, 16, ..., sz.
      // ------------------------------------------------------------
      nLh = nL/2;
      nChunks = sz/nL;
      theta = 6.28318530717959/(isign*nL);
      //W.real() = cos(theta);
      //W.imag() = sin(theta);
      std::complex<T> c(cos(theta), sin(theta));
      W = c;
      iSize = 0;
      for(int n=0; n<nChunks; n++)
         {
         Wk = 1.0;
         for(int k=iSize; k<(nLh+iSize); k++)
             {
             temp = Wk * d[k+nLh];
             d[k+nLh] = d[k] - temp;
             d[k] += temp;
             Wk *= W;
             }
         iSize +=nL;
         }
      } 
   }



// ***********************************************
// My version of the 2D FFT routine.
// Offset zero matrices are assumed. This function
// receives an offset vector vector of dimension
// szx*szy, where the matrix lines are stored one
// after the other.
// ***********************************************

template<typename T>
void fft2( std::complex<T> *d, int szx, int szy, int isign)
   {
   int p, m;

   // ******************************************
   // Check that incoming sizes are power of two
   // ******************************************
   p = Ptwo(szx);
   if(p==0)
      {
      std::cout << "\n Error in fft1 : incorrect sx data size" 
                << std::endl;
      exit(0);
      }
   p = Ptwo(szx);
   if(p==0)
      {
      std::cout << "\n Error in fft1 : incorrect sy data size" 
                << std::endl;
      exit(0);
      }

   // Perform 1D FFT in x dimension
   // ------------------------------
   for(m=0; m<szy; ++m) fft1(&d[m*szx], szx, isign);

   // Transpose
   // ---------
   Transpose(d, szx, szy);
   
   // Perform again 1D FFT in new x (old y) dimension
   // ----------------------------------------------
   for(m=0; m<szx; ++m) fft1(&d[m*szy], szy, isign);

   // Transpose
   // ---------
   Transpose(d, szy, szx);
   }


// ***********************************************
// My version of HALF the 2D FFT routine, used for
// pipelining.
// Offset zero matrices are assumed. This function
// receives an offset vector vector of dimension
// szx*szy, where the matrix lines are stored one
// after the other.
// ***********************************************

template<typename T>
void fft2h( std::complex<T> *d, int szx, int szy, int isign)
   {
   int p, m;

   // ******************************************
   // Check that incoming sizes are power of two
   // ******************************************
   p = Ptwo(szx);
   if(p==0)
      {
      std::cout << "\n Error in fft1 : incorrect sx data size" 
                << std::endl;
      exit(0);
      }
   p = Ptwo(szx);
   if(p==0)
      {
      std::cout << "\n Error in fft1 : incorrect sy data size" 
                << std::endl;
      exit(0);
      }

   // Perform 1D FFT in x dimension
   // ------------------------------
   for(m=0; m<szy; ++m) fft1(&d[m*szx], szx, isign);

   // Transpose
   // ---------
   Transpose(d, szx, szy);
   }


#endif
