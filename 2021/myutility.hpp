#pragma once

#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <utility>
#include <iterator>

////////////////////////////// generators: //////////////////////////////////////////////////////////////

inline std::vector<int> generateVector_withRandomIntValues(int N, int lowerBound, int upperBound){
	std::vector<int> v(N);
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<> distrib(lowerBound, upperBound);
	std::generate(v.begin(), v.end(), [&distrib, &gen](){ return distrib(gen); });
	return v;
}

inline std::vector<std::pair<int,int>> generateVector_withIndex_RandomPairValues(int N, int lowerBound, int upperBound){
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

/////////////////////////// printers /////////////////////////////////////////////////////////
inline void PRINT_intVector(std::string label, std::vector<int> v, int until = -1, int from = -1 ){
	if(from == -1) from = 0;
	if(until == -1) until = v.size();
	std::cout<<label<<":\n";
	std::copy(v.begin()+from, v.begin()+until, std::ostream_iterator<int>(std::cout, "\t"));
	std::cout<<"\n";
}

inline void PRINT_pairVector(std::string label, std::vector<std::pair<int, int>> v, int until = -1, int from = -1 ){
	if(from == -1) from = 0;
	if(until == -1) until = v.size();
	std::cout<<label<<":\n";
	for(int i = from; i<until; i++)
		std::cout<<v[i].first<<"\t";
	std::cout<<"\n";
	for(int i = from; i<until; i++)
		std::cout<<v[i].second<<"\t";
	std::cout<<"\n";
}