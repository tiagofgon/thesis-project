// File RealRange.h
// ----------------
#include <tbb/tbb_stddef.h>

class RealRange
   {
   private:
   double Beg;
   double End;
   double granularity;

   public:
   RealRange( double b, double e, int g)
      {
      Beg = b;
      End = e;
      if(b>e)
         {
         Beg = e;
         End = b;
         }
      granularity = (e-b)/g;
      }

   RealRange(RealRange& R, tbb::split)
      {
      double middle = (R.End+R.Beg)/2;
      End = middle;
      Beg = R.Beg;
      granularity = R.granularity;
      R.Beg = middle;
      }

   bool empty() const
      { return (End <= Beg); }
     
   bool is_divisible() const
      { return (End >(Beg+granularity)); }

   double begin() const
      { return Beg; }
   
   double end() const
      { return End; }
   };
