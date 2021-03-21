#include "myutility.hpp"
#include <vector>
#include <algorithm>
//#include <execution>


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
	static const int agentN = 10; 		// number of agents
	static const int locN = 3; 			// number of locations
	//static const int locChanges = 4;	// number of agents who change the location
}


std::vector<int> generateLocationPtrs(int locN, const std::vector<std::pair<int,int>>& agents_location){
	std::vector<int> locationPtrs(locN+1, 0);
	std::for_each(
		/*std::execution::par,*/    // itt már lehet hogy nagyobb lesz az overhead ha nincs nagyon sok location
		agents_location.begin(),
		agents_location.end(),
		[&locationPtrs](std::pair<int,int> p){ 
			std::for_each(/*std::execution::par,*/ locationPtrs.begin()+p.second+1, locationPtrs.end(), [](int &locPtr){ locPtr++; });
		}
	);
	return locationPtrs;
}

// =====================================================================   m a i n   =================================================================================================
int main(){
	
	std::vector<std::pair<int, int>> agents_location = generateVector_withIndex_RandomPairValues(param::agentN, 0, param::locN-1); // the indexes are agent index
	std::vector<int> locationPtrs = generateLocationPtrs(param::locN, agents_location); // the ith elemetn says where the ith location starts in the agents_SortedByLocation array,    - the last points to the of the sortedarray 
	PRINT_intVector("locationPtrs", locationPtrs);
	
	//------- Sort by locations ---------	
	PRINT_pairVector("agents_location", agents_location);
	std::sort(agents_location.begin(), agents_location.end(), [](std::pair<int,int> pair1, std::pair<int,int> pair2){ return pair1.second < pair2.second; });
	PRINT_pairVector("agents_location -- sorted by locations", agents_location);
	
	
	//------- Location changes ---------
	agents_location[3].second = 2;
	agents_location[5].second = 1;
	agents_location[8].second = 0;
	agents_location[1].second = 1;
	
	// brutforce: again sort by locations
	

	return 0;
}