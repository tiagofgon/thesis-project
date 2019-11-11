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

// File Preorder.C
// -----------------------------------------------------------------
// This is parallel_preoder TBB example, proposed by TBB
// to illustrate the parallel_do algorithm. It is an
// excellent example for the parallelization of a strongly
// irregular problem, using tasks.
//
// Command line: preorder [num_threads] [num_nodes] [num_traversals]
//
// Default: num_traversals = 10
//          num_nodes      = 2000
//          num_threads    = 4
// ----------------------------------------------------------------------------

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <CpuTimer.h>
#include <SafeCounter.h>
#include <VGraph.h>

using namespace std;

// Global variables
// ----------------
int  nTh;
Graph G;
SafeCounter SC;
int count_updates;

// ----------------------------------------------------------------------
// This function performs a recursive update of cells in the graph.
// It starts by updating the cell passed as argument, and decreases the
// reference count of all it successor cells. If the reference count of 
// a successor cell reaches 0, this function calls itself recursively to
// update it.
// ---------------------------------------------------------------------
void  UpdateCell(Cell *C)
   {
   C->update();
   count_updates = SC.Next();

   // Restore reference count in preparation for subsequent
   // traversal;
   int arity = ArityOfOp[C->op];

   #pragma omp atomic write
   C->ref_count = arity;

   // Access a successor and decrease its reference count.
   // If ref_count reaches 0, launch an UpdateCell task
   // ----------------------------------------------------
   for(size_t k=0; k<C->successor.size(); ++k)
       {
       Cell *successor = C->successor[k];
       #pragma omp atomic update           // atomic update
       --(successor->ref_count);

       if( 0 == (successor->ref_count) )
          {
          #pragma omp task
             { UpdateCell(successor); }
          }
       }
   }   

// --------------------------------------------------------
// Auxiliary function that does the graph traversal job. It
// updates the vector of root cells passed as argument. Non
// root cells are updated recursively
// --------------------------------------------------------
void ParallelPreorderTraversal(std::vector<Cell*>& root_set)
   {
   int jobid;
   std::vector<Cell*>::iterator pos;

   // First, create a TaskGroup for the update of the
   // root cells
   // ----------------------------------------------
   #pragma omp taskgroup
      {
      for(pos=root_set.begin(); pos!=root_set.end(); pos++)
         {
         Cell *ptr = *pos;
         #pragma omp task 
             { UpdateCell(ptr);}
         }
      }
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
   nTh = 2;
   nNodes = 1000;
   nSwaps = 5;
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
   omp_set_num_threads(nTh);
   T.Start();
   for(unsigned int trial=0; trial<nSwaps; ++trial)
      {
      G.reset_counter();
      #pragma omp parallel 
         {
         #pragma omp single
            { ParallelPreorderTraversal(root_set); }
         }
      }
   T.Stop();
   G.print(100);
   std::cout << "\n Number of updates = " << (SC.Next()-1) << std::endl;
   T.Report();
   }

