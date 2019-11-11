// File IntRange.h
// ----------------
#include <tbb/tbb_stddef.h>

class IntRange
   {
   private:
   int Beg;
   int End;
   int granularity;

   public:
   IntRange( int b, int e, int g)
      {
      Beg = b;
      End = e;
      if(b>e)
         {
         Beg = e;
         End = b;
         }
      granularity = (End-Beg)/g;
      }

   IntRange(IntRange& R, tbb::split)
      {
      int middle = (R.End+R.Beg)/2;
      End = middle;
      Beg = R.Beg;
      granularity = R.granularity;
      R.Beg = middle;
      }

   bool empty() const
      { return (End == Beg); }
     
   bool is_divisible() const
      { return (End >(Beg+granularity+1)); }

   int begin() const
      { return Beg; }
   
   int end() const
      { return End; }
   };
