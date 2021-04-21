//#include "include/statistics.h"
#include "include/printers.h"
#include "include/sorting.h"

#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <cmath>

using namespace sorting;
using namespace printer;

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


/** TODO 
 * - generateKeyPtrs() -
 * [v] test if it generates the right vector 
 * [v] time measure compare with std::pair sort
 * [ ] evaluation of running time of the algorithm with different arraysizes     - how does it scale up with 
 */



std::vector<int> genRange(){
    std::vector<int> range = {10000, 20000, 50000, 100000, 200000, 500000, 1000000, 2000000, 5000000};
    return range;
}

namespace param{
    const std::vector<int> range = genRange(); 
    const int It = 5;                                // number of measurement iteratons for statistics
    const long int agentN = 1<<20; //1<<26;     // 2^18 - fast, 2^20 ~= 1 million // number of values   (number of agents in the COVID simulator)  
    const long int locN =  agentN / 3;               // number of distinct locations (number of locations in the COVID simulator)
}

void init_vectors(std::vector<int>& agents, std::vector<int>& locations, int keyN = param::locN){
    for(int i =  0; i<agents.size(); i++)
        agents[i] = i;
  	std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution distrib(0, keyN-1);
    std::generate(locations.begin(), locations.end(), [&](){ return distrib(gen); });
}


//-------------------------------------------------------- m a i n ------------------------------------------------------------------------------------------
int main(void) { 
    std::cout<<"agentN: "<<param::agentN<<std::endl;
    // Init
    std::ofstream timesFile("times/GEN_times_2pow"+to_str(log2(param::agentN))+".txt");
    std::vector<int> agents(param::agentN);
    std::vector<int> locations(param::agentN);
    std::vector<int> locationPtrs(param::locN+1);
    init_vectors(agents, locations);

/*
    std::cout<<"Sortd by agents\n";
    PRINT_vector(agents);
    PRINT_vector(locations);
    PRINT_vector(locationPtrs);
*/

    //For(arraysizes)
    std::cout<<"std PAIR ------------------------------------------\n";
    std::vector<float> times_sort(param::range.size()), times_gen_locPtrs(param::range.size()), sum(param::range.size());
    for(int i = 0; i<param::range.size(); i++){
        std::cout<<i;
        int currSize = param::range[i];
        std::cout<<currSize;

        agents.resize(currSize);
        locations.resize(currSize);
        init_vectors(agents, locations);

        for(int k = 0; k<1; k++){
            
            float time_sort = sort_STD_PAIR(agents, locations);
            float time_gen_locPtrs = generateKeyPtrs(locations, locationPtrs);

            times_sort[i] = time_sort; // overwriting the fist measurement with the second, because teh first is invalid.
            times_gen_locPtrs[i] = time_gen_locPtrs;
            sum[i] = time_sort + time_gen_locPtrs;
        }

    }
    to_file(param::range, timesFile, "range = ");
    to_file(times_sort, timesFile, "times_sort = ");
    to_file(times_gen_locPtrs, timesFile, "times_gen_locPtrs = ");
    to_file(sum, timesFile, "sum = ");
/*
    std::cout<<"Sortd by locations\n";
    PRINT_vector(agents);
    PRINT_vector(locations);
    PRINT_vector(locationPtrs);
*/
    
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

    timesFile.close();
    return 0;
}


    
