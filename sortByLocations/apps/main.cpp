////////////////////////////////     Forditas, futtatas:     /////////////////////////////////////////////////
// Makefile parancsok:
//  make -B sort_cpu
//  make -B sort_gpu
//DEBUG:
//  make -B dsort_cpu
//  make -B dsort_gpu



// ------GPU---------
/*
    salloc -pgpu2 --nodelist=neumann srun --pty --preserve-env /bin/bash -l
    module load gpu/cuda/11.0rc
    module load nvhpc/20.9
    
    nvc++ -I/home/shared/software/cuda/hpc_sdk/Linux_x86_64/20.9/compilers/include-stdpar         vector_copy_gpu.cpp -std=c++11 -O3 -o gpu_test -stdpar
    ./gpu_test

        // GPU Summary:
        nvprof --print-gpu-summary 
*/

// ------CPU---------
// icpc vector_copy.cpp -std=c++11 -ltbb -qopenmp-simd -O3 -xHOST
////////////////////////////////////////////////////////////////////////////////////////////////////////////



#include "sortByLocationsApp.hpp"
#include "LocChangeHandlingApp.hpp"
#include "../include/printers.h"

#include <iomanip>


int main(void){
    std::cout<<std::boolalpha;

    std::ofstream file("times/locChanges/___times_locChHandel_UPDATE_1000000.txt");
    LocChangeHandlingApp app;
    //SortByLocationsApp app;
    LocChangeHandlingApp::Times times;
    
    times = app.run();
    //app.run();
    
    //printer::to_file(app.get_range(), file, "range = ");
    std::vector<int> fullUpdateTime = times.getFullUpdateTime();
    printer::to_file(times.times_refreshLocPtrs, file, "times_refreshLocPtrs = ");
    printer::to_file(times.times_refreshAgents, file, "times_refreshAgents = ");
    printer::to_file(times.times_refreshLocations, file, "times_refreshLocations = ");
    printer::to_file(fullUpdateTime, file, "\n\ntimes_fullUpdate = ");
    printer::to_file(times.times_sortAgain, file, "times_sortAgain = ");

    file.close();
    return 0;
}  
