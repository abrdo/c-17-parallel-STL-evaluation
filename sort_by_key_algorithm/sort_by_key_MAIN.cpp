//#include "include/statistics.h"
#include "include/printers.h"
#include "include/sort_by_key_functions.h"

#include <iostream>
#include <vector>
#include <random>
#include <fstream>

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
    const long int dataN = 10; //1<<26;     // number of values   (number of agents in the COVID simulator)
    const long int keyN =  3;               // number of distinct keys (number of locations in the COVID simulator)
}



void init_vectors(std::vector<int>& inputdata, std::vector<int>& inputkeys){
  	std::random_device rd;
    std::mt19937 gen(rd());
  	std::normal_distribution<> distrib(500,200);
    
  	std::generate(inputdata.begin(), inputdata.end(), [&distrib,&gen](){ return std::max(0,(int)std::round(distrib(gen))); });
  	std::generate(inputkeys.begin(), inputkeys.end(), [&distrib,&gen](){ return gen()%param::keyN; });
}


std::vector<int> calculateKeysPtrs(const std::vector<int>& inputkeys){
	std::vector<int> keyPtrs(param::keyN+1, 0);
	std::for_each(
		std::execution::par,
		inputkeys.begin(),
		inputkeys.end(),
		[&keyPtrs](int key){ 
			std::for_each(std::execution::par, keyPtrs.begin()+key+1, keyPtrs.end(), [](int &keyPtr){ keyPtr++; });
		}
	);
	return keyPtrs;
}


//-------------------------------------------------------- m a i n ------------------------------------------------------------------------------------------
int main(void) { 
    std::cout<<"arraysize: "<<param::dataN<<std::endl;
    // Init
    std::ofstream timesFile("times.txt");
    std::vector<int> inputdata(param::dataN);
    std::vector<int> inputkeys(param::dataN);
    std::vector<int> keyPtrs(param::keyN+1);

    init_vectors(inputkeys, inputdata);
    std::cout<<"--------------------\n";
    //PRINT_vector(inputkeys);
    //PRINT_vector(inputdata);
    //std::cout<<"___________________"<<std::endl;


    ///////////// SORTs + time measure ////////////////////

    std::cout<<"std PAIR ------------------------------------------\n";
    std::vector<float> times_pair;
    for(int i = 0; i<param::It; i++){
        init_vectors(inputkeys, inputdata);
        keyPtrs = calculateKeysPtrs(inputkeys);
        float time = sort_by_key_STD_PAIR(inputdata, inputkeys);
        times_pair.push_back(time);
    }
    timesFile<<"times_pair = ";
    to_file(times_pair, timesFile);


    std::cout<<"Helper INDICES vector -----------------------------\n";
    std::vector<float> times_indices;
    for(int i = 0; i<param::It; i++){
        init_vectors(inputkeys, inputdata);
        keyPtrs = calculateKeysPtrs(inputkeys);
        float time = sort_by_key_HELPER_INDICES_VECTOR(inputdata, inputkeys);
        times_indices.push_back(time);
    }
    timesFile<<"times_indices = ";
    to_file(times_indices, timesFile);


    // CUSTOM - pairedvectoriterator
    //...


    std::cout<<"BOOST-TUPLE Iterator ------------------------------\n";
    std::vector<float> times_boost;
    for(int i = 0; i<param::It; i++){
        init_vectors(inputkeys, inputdata);
        keyPtrs = calculateKeysPtrs(inputkeys);
        float time = sort_by_key_BOOSTTUPLEIT(inputdata, inputkeys);
        times_boost.push_back(time);
    }
    timesFile<<"times_boost = ";
    to_file(times_boost, timesFile);

    

    std::cout<<"calculateKeyPtrs time measurement ------------------------------\n";
    std::vector<float> times_calcKeyPtrs;
    for(int i = 0; i<10; i++){
        init_vectors(inputkeys, inputdata);
        auto t1 = std::chrono::high_resolution_clock::now();
        keyPtrs = calculateKeysPtrs(inputkeys);
        auto t2 = std::chrono::high_resolution_clock::now();
        int time = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
        times_calcKeyPtrs.push_back(time);
    }
    timesFile<<"calculate keyPtrs = ";
    to_file(times_calcKeyPtrs, timesFile);



    timesFile.close();
    return 0;
}


    
