// File TaskRange.h 
// ----------------
// 
// Definition of the utility function
// Taskrange()
// -------------------------------------------

#ifndef TASK_RANGE
#define TASK_RANGE

void TaskRange(int& Beg, int& End, int rank, int Nt)
   {
   int n, beg, end;
   int size, D, R;

   size = End-Beg;
   D = (size/Nt);
   R = size%Nt;

   end = Beg;
   for(n=1; n<=rank; n++)
      {
      beg = end;
      end = beg+D;
      if(R)
         {
         end++;
         R--;
         }
      }
   Beg = beg;
   End = end;
   }
   

#endif
