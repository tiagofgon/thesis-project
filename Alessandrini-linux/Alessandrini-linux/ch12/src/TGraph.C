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
// This file is the original Graph.cpp file in the TBB "parallel_do" example,
// adapted to the NPool environment. Minor modifications have been made in the
// Cell operation, which are nor directly relevant to the synchronization
// pattern exhibited by this example.
// ---------------------------------------------------------------------------

#include <cstdlib>
#include <TGraph.h>
#include <SafeCounter.h>
#include <iostream>
#include <Rand.h>
#include <math.h>

using namespace std;

SafeCounter Counter;
const double PRECISION = 0.0000001;

void Graph::create_random_dag( size_t number_of_nodes ) 
    {
    Rand R(999);

    // Fix the total number of nodes in the graph
    // ------------------------------------------
    my_vertex_set.resize(number_of_nodes);

    // Visit all the nodes in the graph
    // --------------------------------
    for( size_t k=0; k<number_of_nodes; ++k ) 
        {
        // First, we randomly pick an operation for
        // each node
        // ---------------------------------------
        Cell& c = my_vertex_set[k];
        c.rank = k;

        int op = int((rand()>>8)%3u); // random in [0, 2]
        if( op>int(k) ) op = int(k);  // ??

        switch( op ) 
            {
            default:
                c.op = OP_VALUE;
                c.value = R.draw();
                break;
            case 1:
                c.op = OP_SEARCH1;
                break;
            case 2:
                c.op = OP_SEARCH2;
                break;
            }

        // Next, we identify an input cells by 
        // randomly picking already visited cells
        // --------------------------------------
        for( int j=0; j<ArityOfOp[c.op]; ++j ) 
            {
            Cell& input = my_vertex_set[rand()%k];
            c.input[j] = &input;
            }
        }
    }


void Graph::print(int stride) 
    {
    for( size_t k=0; k<my_vertex_set.size(); ++k ) 
        {
        if(k%stride==0)
            std::cout<<"\nCell "<<k<<" : " << my_vertex_set[k].value;
        std::cout<<std::endl;
        }
    }

void Graph::check_update() 
    {
    for( size_t k=0; k<my_vertex_set.size(); ++k ) 
        {
        if(my_vertex_set[k].updated == false)
        std::cout<<"Cell "<<k<<" not updated\n";
        std::cout<<std::endl;
        }
    }

// This function completes the construction of the acyclic
// graph. It does several things

void Graph::get_root_set( vector<Cell*>& root_set ) 
    {
    // First, clear the successor vectors inside each
    // cell
    // ----------------------------------------------
    for( size_t k=0; k<my_vertex_set.size(); ++k ) 
        {
        my_vertex_set[k].successor.clear();
        }

    // Next, clear the root_set vector passed as argument
    // just in case.
    // --------------------------------------------------
    root_set.clear();

    // Visit each graph node
    // ---------------------
    for( size_t k=0; k<my_vertex_set.size(); ++k ) 
        {
        Cell& c = my_vertex_set[k];

        // Fix the reference count of this cell
        // ------------------------------------
        c.ref_count = ArityOfOp[c.op];

        // Add a reference to this cell in the successor
        // vector of the previous cells providing input
        // ---------------------------------------------
        for( int j=0; j<ArityOfOp[c.op]; ++j ) 
            {
            c.input[j]->successor.push_back(&c);
            }

        // If this cell does not take any input, add it
        // to the root_set vactor
        // --------------------------------------------
        if( ArityOfOp[c.op]==0 )
            root_set.push_back(&my_vertex_set[k]);
        }
    }

void Graph::reset_counter()
   {
   Counter.Reset();
   }

void Cell::update() 
    {
    double scratch, error1, error2;
    double target1, target2;
    bool   test1, test2;
    int n = Counter.Next();
    Rand R(999*n);

    switch( op ) 
        {
        case OP_VALUE:
            break;
        case OP_SEARCH1:
            target1 = input[0]->value;
            do
               {
               scratch = R.draw();
               error1 = fabs(scratch-target1);
               }while(error1>PRECISION);
            value = scratch;
            break;
        case OP_SEARCH2:
            target1 = input[0]->value;
            target2 = input[1]->value; 
            do
               {
               scratch = R.draw();
               error1 = fabs(scratch-target1);
               error2 = fabs(scratch-target2);
               test1 = (error1 > PRECISION);
               test2 = (error2 > PRECISION);
               }while(test1 && test2);
            value = scratch;   
            break;
        }
    updated = true;
    }

