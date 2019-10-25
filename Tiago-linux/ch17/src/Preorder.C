/*
    Copyright 2005-2014 Intel Corporation.  All Rights Reserved.

    This file is part of Threading Building Blocks.

    Threading Building Blocks is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.

    Threading Building Blocks is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Threading Building Blocks; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
// ----------------------------------------------------------------------------
// This file is a modofocation of the original parallel_preorder.cpp file in 
// the TBB "parallel_do" example, adapted to the NPool environment. Minor 
// modifications have been made in the Cell operation, which are nor directly 
// relevant to the synchronization pattern exhibited by this example.
// ---------------------------------------------------------------------------

// File Preorder.C
// -----------------------------------------------------------------
// This is parallel_preoder TBB example, proposed by TBB
// to illustrate the parallel_do algorithm. It is an
// excellent example for the parallelization of a strongly
// irregular problem, using tasks.
//
// Command line: preorder [num_threads] [num_nodes] [num_traversals]
//
// Default: num_traversals = 2
//          num_nodes      = 2000
//          num_threads    = 4
// ----------------------------------------------------------------

#include <iostream>
#include <stdio.h>
#include <NPool.h>
#include <math.h>
#include <CpuTimer.h>
#include <TGraph.h>
#include <SafeCout.h>
#include <SafeCounter.h>

using namespace std;

// Global variables
// ----------------
int  nTh;
Graph G;
NPool *NP;
SafeCout    SC;
SafeCounter Count;

class UpdateCell : public Task
   { 
   private:
     Cell *C;
     std::ostringstream os;

   public:
    UpdateCell(Cell *c) : Task(), C(c) {}

    void ExecuteTask()
       {
       C->update();
       //os << "Updated cell:  " <<  C->rank;
       //SC.Flush(os);

       // Restore reference count in preparation for subsequent
       // traversal;
       C->ref_count = ArityOfOp[C->op];

       // Access a successor and decrease its reference count.
       // If ref_count reaches 0, spawn an UpdateCell task
       // ----------------------------------------------------
       for(size_t k=0; k<C->successor.size(); ++k)
          {
          Cell *successor = C->successor[k];
          if( 0 == --(successor->ref_count) )
             {
             UpdateCell *T = new UpdateCell(successor);
             NP->SpawnTask(T, false);
             }
          }
       }
   };   


// Auxiliary function that does the job
// ------------------------------------
void ParallelPreorderTraversal(std::vector<Cell*>& root_set)
   {
   int jobid;
   std::vector<Cell*>::iterator pos;

   // First, create a TaskGroup for the update of the
   // root cells
   // ----------------------------------------------
   TaskGroup *TG = new TaskGroup();
   for(pos=root_set.begin(); pos!=root_set.end(); pos++)
      {
      Cell *ptr = *pos;
      UpdateCell *T = new UpdateCell(ptr);
      TG->Attach(T);
      }

   // Sublit job and wait for it
   // --------------------------
   jobid = NP->SubmitJob(TG);
   NP->WaitForJob(jobid);
   }

// The main function
// -----------------

int main(int argc, char **argv)
   {
   int n, jobid;
   CpuTimer T;
   int nTh, nNodes, nSwaps;
   int target;
   size_t root_set_size = 0;

   // Get command line input
   // ----------------------
   nTh = 4;
   nNodes = 2000;
   nSwaps = 2;
   if(argc==2) nTh = atoi(argv[1]);
   if(argc==3)
      { 
      nTh = atoi(argv[1]);
      nNodes = atoi(argv[2]);
      }
   if(argc==4)
      { 
      nTh = atoi(argv[1]);
      nNodes = atoi(argv[2]);
      nSwaps = atoi(argv[3]);
      }

   NP = new NPool(nTh);

   // Setup the acyclic graph
   // -----------------------
   G.create_random_dag(nNodes);
   std::vector<Cell*> root_set;
   G.get_root_set(root_set);
   root_set_size = root_set.size();
   std::cout << "\n Size of set of root nodes " << root_set_size
             << std::endl;

   std::cout << "\n Configuration :"
             << "\n -------------- "
             << "\n Number of threads " << nTh
             << "\n Number of nodes   " << nNodes
             << "\n Traversals        " << nSwaps << std::endl;
 
   // Do the traversal 
   // ----------------
   T.Start();
   for(unsigned int trial=0; trial<nSwaps; ++trial)
      {
      G.reset_counter();
      ParallelPreorderTraversal(root_set);
      }
   T.Stop();
   G.print(100);
   T.Report();
   }

