//#include "include/statistics.h"
#include "include/sort_by_key_functions.h"

#include <iostream>
#include <vector>
#include <random>
#include <fstream>

using namespace std;

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


namespace PARAM{
    const int It = 5;                // number of measurement iteratons for statistics
    const long int N = 10; //1<<26;    // number of values (number of agents in the COVID simulator)
    const long int keyN =  3;         // number of keys (number of locations in the COVID simulator)
};


//============================   to_file   ======================================
template<typename T>
void to_file(const std::vector<T>& v, ofstream &f){
    f<<"[ ";
    for(int i = 0; i<v.size(); i++){
        f<<v[i];
        if(i != v.size()-1) f<<", ";
    }
    f<<"]\n\n";
}

//============================ init, print ======================================

void init_vectors(std::vector<int>& inputkeys, std::vector<int>& inputdata){
  	std::random_device rd;
    std::mt19937 gen(rd());
  	std::normal_distribution<> distrib(500,200);
    
  	std::generate(inputdata.begin(), inputdata.end(), [&distrib,&gen](){ return std::max(0,(int)std::round(distrib(gen))); });
  	std::generate(inputkeys.begin(), inputkeys.end(), [&distrib,&gen](){ return gen()%PARAM::keyN; });
}

void PRINT_vector(const std::vector<int>& inputkeys){
    for(int key : inputkeys){
        std::cout << key << ",\t";
    }
    std::cout<<"\n--------------------\n";
}


int main(void) { //-------------------------------------------------------- m a i n ------------------------------------------------------------------------------------------
    std::cout<<"arraysize: "<<PARAM::N<<std::endl;
    // Init
    ofstream data("times.txt");
  	std::vector<int> inputdata(PARAM::N);
  	std::vector<int> inputkeys(PARAM::N);
   
    init_vectors(inputkeys, inputdata);
    std::cout<<"--------------------"<<std::endl;
    //PRINT_vector(inputkeys);
    //PRINT_vector(inputdata);
    //std::cout<<"___________________"<<std::endl;
    

    ///////////// SORTs + time measure ////////////////////
    
    std::cout<<"std PAIR ------------------------------------------\n";
    std::vector<float> times_pair;
    for(int i = 0; i<PARAM::It; i++){
        init_vectors(inputkeys, inputdata);
        float time = sort_by_key_STD_PAIR(inputkeys, inputdata);
        times_pair.push_back(time);
    }
    data<<"times_pair = ";
    to_file(times_pair, data);
    
    
    std::cout<<"Helper INDICES vector -----------------------------\n";
    std::vector<float> times_indices;
    for(int i = 0; i<PARAM::It; i++){
        init_vectors(inputkeys, inputdata);
        float time = sort_by_key_HELPER_INDICES_VECTOR(inputkeys, inputdata);
        times_indices.push_back(time);
    }
    data<<"times_indices = ";
    to_file(times_indices, data);
    
   
    // CUSTOM - pairedvectoriterator
    //...
    
    
    std::cout<<"BOOST-TUPLE Iterator ------------------------------\n";
    std::vector<float> times_boost;
    for(int i = 0; i<PARAM::It; i++){
        init_vectors(inputkeys, inputdata);
        float time = sort_by_key_BOOSTTUPLEIT(inputkeys, inputdata);
        times_boost.push_back(time);
    }
    data<<"times_boost = ";
    to_file(times_boost, data);
    
  
  
    data.close();
  	return 0;
}


    
