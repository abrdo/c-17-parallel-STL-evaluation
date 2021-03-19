#include <algorithm>

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <iterator>
#include <utility>


struct param{
	static const int agentN = 10; 		// number of agents
	static const int locN = 3; 			// number of locations
	static const int locChanges = 4;	// number of agents who change the location
};

std::vector<int> generateVector_withRandomIntValues(int N, int lowerBound, int upperBound){
	std::vector<int> v(N);
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<> distrib(lowerBound, upperBound);
	std::generate(v.begin(), v.end(), [&distrib, &gen](){ return distrib(gen); });
	return v;
}

std::vector<std::pair<int,int>> generateVector_withIndex_RandomPairValues(int N, int lowerBound, int upperBound){
	std::vector<std::pair<int,int>> v(N);
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<> distrib(lowerBound, upperBound);
	
	int tmpSecond;
	std::pair<int,int> tmpPair;
	for(int i = 0; i < N; i++){
		tmpSecond = distrib(gen);
		tmpPair = std::make_pair(i, tmpSecond);
		v[i] = tmpPair;
	}
	return v;
}


void PRINT_intVector(std::vector<int> v, int until = -1, int from = -1 ){
	if(from == -1) from = 0;
	if(until == -1) until = v.size();
	std::copy(v.begin()+from, v.begin()+until, std::ostream_iterator<int>(std::cout, "\t"));
	std::cout<<"\n";
}
void PRINT_pairVector(std::vector<std::pair<int, int>> v, int until = -1, int from = -1 ){
	if(from == -1) from = 0;
	if(until == -1) until = v.size();
	for(int i = from; i<until; i++)
		std::cout<<v[i].first<<"\t";
	std::cout<<"\n";
	for(int i = from; i<until; i++)
		std::cout<<v[i].second<<"\t";
	std::cout<<"\n";
}

// =====================================================================   m a i n   =================================================================================================
int main(){
	
	std::vector<std::pair<int, int>> agents_location = generateVector_withIndex_RandomPairValues(param::agentN, 0, param::locN-1); // the indexes are agent index
	std::vector<int> locationPtrs(param::locN+1); // the ith elemetn says where the ith location starts in the agents_SortedByLocation array,    - the last points to the of the sortedarray 
	//std::vector<int> agents_SortedByLocation(param::agentN);
	
	
	//------- Sort by locations ---------
	locationPtrs[0] = 0;
	#pragma omp parallel for
	for(int i = 0; i < param::locN; i++){
		int counted_ith_locations = std::count_if(agents_location.begin(), agents_location.end(), [i](std::pair<int,int> p){ return p.second == i; });
		locationPtrs[i+1] = locationPtrs[i] + counted_ith_locations;
	}
	PRINT_intVector(locationPtrs);
	
	PRINT_pairVector(agents_location);
	std::sort(agents_location.begin(), agents_location.end(), [](std::pair<int,int> pair1, std::pair<int,int> pair2){ return pair1.second < pair2.second; });
	PRINT_pairVector(agents_location);
	
	
	//------- Location changes ---------
	agents_location[3].second = 2;
	agents_location[5].second = 1;
	agents_location[8].second = 0;
	agents_location[1].second = 1;
	// brutforce: again sort by locations
	
	
	
	

	return 0;
}