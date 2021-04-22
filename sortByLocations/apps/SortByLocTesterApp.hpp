# pragma once

#include "../include/printers.h"
#include "../include/sorting.h"

#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <cmath>

class SortByLocTesterApp{
protected:
    std::ofstream timesFile;
    std::vector<int> __range = {10000, 20000, 50000, 100000, 200000, 500000, 1000000, 2000000, 5000000};
    int __It = 5;                                // number of measurement iteratons for statistics
    int __agentN = 1<<20; //1<<26;     // 2^18 - fast, 2^20 ~= 1 million // number of values   (number of agents in the COVID simulator)  
    int __locN =  __agentN / 3;               // number of distinct locations (number of locations in the COVID simulator)

    void init_vectors(std::vector<int>& agents, std::vector<int>& locations){
        for(int i =  0; i<agents.size(); i++)
            agents[i] = i;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution distrib(0, __locN-1);
        std::generate(locations.begin(), locations.end(), [&](){ return distrib(gen); });
    }

    static std::vector<int> genRange(){
        std::vector<int> range = {10000, 20000, 50000, 100000, 200000, 500000, 1000000, 2000000, 5000000};
        return range;  
    }

public:
    std::vector<int> get_range(){ return __range; }



};