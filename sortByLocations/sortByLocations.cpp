//#include "include/statistics.h"
#include "include/printers.h"
#include "include/sorting.h"

#include <iostream>
#include <vector>
#include <random>
#include <fstream>

using namespace sort;

/////////// Ford�t�s, futtat�s: ///////////////////

// ------GPU---------
/*
salloc -pgpu2 --nodelist=neumann srun --pty --preserve-env /bin/bash -l
module load gpu/cuda/11.0rc
module load nvhpc/20.9
nvc++ -I/home/shared/software/cuda/hpc_sdk/Linux_x86_64/20.9/compilers/include-stdpar         vector_copy_gpu.cpp -std=c++11 -O3 -o gpu_test -stdpar
./gpu_test
*/
      // GPU Summary:
      //nvprof --print-gpu-summary 


// ------CPU---------
// icpc vector_copy.cpp -std=c++11 -ltbb -qopenmp-simd -O3 -xHOST



namespace param{
    const int It = 5;                       // number of measurement iteratons for statistics
    const long int agentN = 1<<20; //1<<26;     // 2^18 - fast, 2^20 ~= 1 million // number of values   (number of agents in the COVID simulator)  
    const long int locN =  agentN;               // number of distinct locations (number of locations in the COVID simulator)
}

void init_vectors(std::vector<int>& agents, std::vector<int>& locations){
  	std::random_device rd;
    std::mt19937 gen(rd());
  	std::normal_distribution<> distrib(500,200);
    
  	std::generate(agents.begin(), agents.end(), [&distrib,&gen](){ return std::max(0,(int)std::round(distrib(gen))); });
  	std::generate(locations.begin(), locations.end(), [&distrib,&gen](){ return gen()%param::locN; });
}


//-------------------------------------------------------- m a i n ------------------------------------------------------------------------------------------
int main(void) { 
    std::cout<<"agentN: "<<param::agentN<<std::endl;
    // Init
    std::ofstream timesFile("times_agentN-"+to_str(param::agentN)+".txt");
    std::vector<int> agents(param::agentN);
    std::vector<int> locations(param::agentN);
    std::vector<int> locationPtrs(param::locN+1);                 // v2
    std::vector<std::vector<int>> agentsAtLocations(param::locN); // v1

    init_vectors(locations, agents);
    std::cout<<"--------------------\n";
    //PRINT_vector(locations);
    //PRINT_vector(agents);
    //std::cout<<"___________________"<<std::endl;


    ///////////// SORTs + time measure ////////////////////
/*
    std::cout<<"std PAIR ------------------------------------------\n";
    std::vector<float> times_pair;
    for(int i = 0; i<param::It; i++){
        init_vectors(locations, agents);
        //locationPtrs = calculateLocationsPtrs(locations);
        float time = sort_STD_PAIR(agents, locations);
        times_pair.push_back(time);
    }
    timesFile<<"times_pair = ";
    to_file(times_pair, timesFile);


    std::cout<<"Helper INDICES vector -----------------------------\n";
    std::vector<float> times_indices;
    for(int i = 0; i<param::It; i++){
        init_vectors(locations, agents);
        //locationPtrs = calculateLocationsPtrs(locations);
        float time = sort_HELPER_INDICES_VECTOR(agents, locations);
        times_indices.push_back(time);
    }
    timesFile<<"times_indices = ";
    to_file(times_indices, timesFile);


    // CUSTOM - pairedvectoriterator
    //...


    std::cout<<"BOOST-TUPLE Iterator ------------------------------\n";
    std::vector<float> times_boost;
    for(int i = 0; i<param::It; i++){
        init_vectors(locations, agents);
        //locationPtrs = calculateLocationsPtrs(locations);
        float time = sort_BOOSTTUPLEIT(agents, locations);
        times_boost.push_back(time);
    }
    timesFile<<"times_boost = ";
    to_file(times_boost, timesFile);

    ///*
    std::cout<<"calculateLocationPtrs time measurement ------------------------------\n";
    std::vector<float> times_calcLocationPtrs;
    for(int i = 0; i<param::It; i++){
        std::cout<<i<<"\n";
        init_vectors(locations, agents);
        auto t1 = std::chrono::high_resolution_clock::now();
        locationPtrs = calculateLocationsPtrs(locations);
        auto t2 = std::chrono::high_resolution_clock::now();
        int time = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
        times_calcLocationPtrs.push_back(time);
    }
    timesFile<<"calculate_locationPtrs = ";
    to_file(times_calcLocationPtrs, timesFile);
    */



//////////////////////////// vector<vector>       vs      2 plain vector + locPtrs //////////////////////////////////////////////////////////

   std::cout<<"vector<vector> ------------------------------------------\n";
    std::vector<float> times_vectorOfVector;
    for(int i = 0; i<param::It; i++){
        init_vectors(locations, agents);
        float time_sort = sort_intoVectorVector(agents, locations, agentsAtLocations);
        times_vectorOfVector.push_back(time_sort);
    }
    timesFile<<"times_vectorOfVector = ";
    to_file(times_vectorOfVector, timesFile);

    std::cout<<"std PAIR ------------------------------------------\n";
    std::vector<float> times_pair;
    for(int i = 0; i<param::It; i++){
        init_vectors(locations, agents);
        float time_sort = sort_STD_PAIR(agents, locations);
        float time_locPtrs = generateKeyPtrs(locations, locationPtrs);
        times_pair.push_back(time_sort + time_locPtrs);
    }
    timesFile<<"times_pair = ";
    to_file(times_pair, timesFile);






    timesFile.close();
    return 0;
}


    
