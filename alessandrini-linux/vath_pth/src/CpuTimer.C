// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
// ----------------------------------
// Timer reporter class
// --------------------

#include <CpuTimer.h>

 void CpuTimer::Start()
    { 
    start_time = times(&tms_S); 
    }

 void CpuTimer::Stop()
    { 
    end_time = times(&tms_E) - start_time; 
    }

 void CpuTimer::Report()
    {
    long clock_ticks;
    double wtime, utime, stime;

    clock_ticks = sysconf(_SC_CLK_TCK);
    wtime = end_time/(double) clock_ticks;
    utime = (tms_E.tms_utime - tms_S.tms_utime)/(double)clock_ticks;
    stime = (tms_E.tms_stime - tms_S.tms_stime)/(double)clock_ticks;
    std::cout << "\n\n CpuTimer Report : \n\n";
    std::cout << " WALL TIME : " << wtime << "\n";
    std::cout << " USER TIME : " << utime << "\n";
    std::cout << " SYST TIME : " << stime << std::endl;
    }

