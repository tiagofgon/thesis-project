// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
/********************************************************
 * ThPool2.C -- 
 * Second example for Thread pool library
 */

#include <stdio.h>
#include <NPool.h>
#include <Timer.h>
#include <RandInt.h>
#include <SafeCout.h>
#include <string>

using namespace std;

string S[20]={  "STRING 0",
		"STRING 1",
                "STRING 2",
                "STRING 3",
                "STRING 4",
                "STRING 5",
                "STRING 6",
                "STRING 7",
                "STRING 8",
                "STRING 9",
		"STRING 10",
                "STRING 11",
                "STRING 12",
                "STRING 13",
                "STRING 14",
                "STRING 15",
	        "STRING 16",
                "STRING 17",
                "STRING 18",
                "STRING 19"};
	
RandInt r(1000);
SafeCout SC;

class MyTask : public Task
   {
   private:
    string s;
    ostringstream os;     // for stdout output
    Timer T;

   public:
    MyTask(const string& STR) : s(STR) {}
    void ExecuteTask()
       { 
       os << s << " START";
       SC.Flush(os);	
       // --------------------
       T.Wait(r.draw()+200);
       // --------------------
       os << s << " START";
       SC.Flush(os);	
       }
    };

int main(int argc, char **argv)
   {
   int n, nc, Nth, status;
   int gid1, gid2, gid3;

   if(argc==2) Nth = atoi(argv[1]);
   else Nth = 2;
   nc = 40;
   NPool GP(Nth);
   std::cout << "\n *** Second test of basic NPool features\n " 
             << std::endl;

   // Post six independent one task job and wait
   // for the termination of all of them
   // --------------------------------------
   std::cout << "\nMain: posting six requests and waiting for idle\n"
             << std::endl;
   for (n=0; n<6; n++) 
      {
      Task *T = new MyTask(S[n]);
      status = GP.SubmitJob(T);
      if(!status)
	 {
         printf("PostTask returned %d\n", status);
	 exit(1);
	 }
      }
   GP.WaitForIdle();
   
   // ---------------------
   // Post a three task job
   // ---------------------
   TaskGroup *TG = new TaskGroup();
   std::cout << "\nMain: posting a three task job\n"
             << std::endl;
   for (n=10; n<13; n++) 
      {
      Task *T = new MyTask(S[n]);
      TG->Attach(T);
      }
   gid1 = GP.SubmitJob(TG);
    
   // ---------------------------
   // Post another three task job
   // ---------------------------
   TG->Clear();
   std::cout << "\nMain: posting another three task job\n"
             << std::endl;
   for (n=14; n<17; n++) 
      {
      Task *T = new MyTask(S[n]);
      TG->Attach(T);
      }
   gid2 = GP.SubmitJob(TG);

   // -------------------------------
   // Post yet another two task job 
   // as a group
   // -------------------------------
   TG->Clear();
   std::cout << "\nMain: posting another two task job\n"
             << std::endl;
   for (n=17; n<19; n++) 
      {
      Task *T = new MyTask(S[n]);
      TG->Attach(T);
      }
   gid3 = GP.SubmitJob(TG);

   // Wait for the three groups return
   // --------------------------------
   GP.WaitForJob(gid3);
   std::cout << "\nmain: third group returned\n" << std::endl;
   GP.WaitForJob(gid2);
   std::cout << "\nmain: second group returned\n" << std::endl;
   GP.WaitForJob(gid1);
   std::cout << "\nmain: first group returned\n" << std::endl;
   return 0;
   }  
