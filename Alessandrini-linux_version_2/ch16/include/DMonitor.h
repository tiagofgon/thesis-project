// DMonitor.H
// *******************************
#ifndef MONITOR_H
#define MONITOR_H
#include <math.h>

class DMonitor
   {
   private:
 
   double average;
   double variance;
   double accum_avg;
   double accum_var;
   long   counter;

   public:

   DMonitor()
      {
      accum_avg = 0.0;
      accum_var = 0.0;
      counter = 0;
      }
  
   void AccumData(double d) 
      { 
      accum_avg += d; 
      accum_var += d*d;
      counter++; 
      }

   void Reset()
      {
      average = accum_avg/counter; 
      variance = sqrt (accum_var/counter - average*average); 
      accum_var = 0.0;
      accum_avg = 0.0;
      counter = 0;
      }

   double Variance() { return variance; }
   double Average() { return average; }
   };


#endif
