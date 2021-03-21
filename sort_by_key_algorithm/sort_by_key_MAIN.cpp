//#include "include/statistics.h"
#include "include/sort_by_key_functions.h"

#include <iostream>
#include <vector>
#include <random>
#include <fstream>

using namespace std;

struct param{
    static const int It = 5;  // number of measurement iteratons
    static const long int N = 1<<26;  // array size
    
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

void init_vectors(std::vector<int>* inputkeys, std::vector<int>* inputdata){
  	std::mt19937 gen(0);
  	std::normal_distribution<> d(500,200);
    
  	std::generate(inputdata->begin(), inputdata->end(),[&d,&gen](){return std::max(0,(int)std::round(d(gen)));});
  	std::generate(inputkeys->begin(), inputkeys->end(),[&d,&gen](){return gen()%20;});
}

void PRINT_vector(const std::vector<int>& inputkeys){
    for(int key : inputkeys){
        std::cout << key << ",\t";
    }
    std::cout<<std::endl<<"--------------------"<<std::endl;
}


int main(void) { //-------------------------------------------------------- m a i n ------------------------------------------------------------------------------------------
    std::cout<<"arraysize: "<<param::N<<std::endl;
    // Init
    ofstream data("times.txt");
  	std::vector<int> inputdata(param::N);
  	std::vector<int> inputkeys(param::N);
   
    init_vectors(&inputkeys, &inputdata);
    std::cout<<"--------------------"<<std::endl;
    //PRINT_vector(inputkeys);
    //PRINT_vector(inputdata);
    //std::cout<<"___________________"<<std::endl;
    

    ///////////// SORTs + time measure ////////////////////
    
    std::cout<<"std PAIR ------------------------------------------\n";
    std::vector<float> times_pair;
    for(int i = 0; i<param::It; i++){
        init_vectors(&inputkeys, &inputdata);
        float time = sort_by_key_STD_PAIR(inputkeys, inputdata);
        times_pair.push_back(time);
    }
    data<<"times_pair = ";
    to_file(times_pair, data);
    
    
    std::cout<<"Helper INDICES vector -----------------------------\n";
    std::vector<float> times_indices;
    for(int i = 0; i<param::It; i++){
        init_vectors(&inputkeys, &inputdata);
        float time = sort_by_key_HELPER_INDICES_VECTOR(inputkeys, inputdata);
        times_indices.push_back(time);
    }
    data<<"times_indices = ";
    to_file(times_indices, data);
    
   
    // CUSTOM - pairedvectoriterator
    //...
    
    
    std::cout<<"BOOST-TUPLE Iterator ------------------------------\n";
    std::vector<float> times_boost;
    for(int i = 0; i<param::It; i++){
        init_vectors(&inputkeys, &inputdata);
        float time = sort_by_key_BOOSTTUPLEIT(inputkeys, inputdata);
        times_boost.push_back(time);
    }
    data<<"times_boost = ";
    to_file(times_boost, data);
    
  
  
    data.close();
  	return 0;
}


    
