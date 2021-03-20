#include "myutility.hpp"
#include <vector>
#include <iterator>

struct param{
	static const int agentN = 10; 		// number of agents
	static const int locN = 3; 			// number of locations
	//static const int locChanges = 4;	// number of agents who change the location
};


std::vector<int> generateLocationPtrs(int locN, const std::vector<std::pair<int,int>>& agents_location){
	std::vector<int> locationPtrs(locN+1);
	locationPtrs[0] = 0;
	std::vector<int> cardinalityOfLocations(param::locN);
	#pragma omp parallel for
	for(int i = 0; i < param::locN; i++){
		cardinalityOfLocations[i] = std::count_if(agents_location.begin(), agents_location.end(), [i](std::pair<int,int> p){ return p.second == i; });
	}
	#pragma omp parallel for
	for(int i = 0; i < param::locN; i++){
		locationPtrs[i+1] = std::accumulate(cardinalityOfLocations.begin(), cardinalityOfLocations.begin()+i+1, 0);
	}
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