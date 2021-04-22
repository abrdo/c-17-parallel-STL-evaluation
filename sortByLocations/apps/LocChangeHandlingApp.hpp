# pragma once

#include "SortByLocTesterApp.hpp"
#include "../include/printers.h"
#include "../include/sorting.h"


#ifndef GPU
// for CPU:
#include <pstl/algorithm>
#include <pstl/numeric>
#include <pstl/execution>
#else
// for GPU:
#include <algorithm>
#include <numeric>
#include <execution>
#endif

#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <cmath>

using namespace sorting;
using namespace printer;


class LocChangeHandlingApp : public SortByLocTesterApp{
    using loc_change_t = std::pair<int, std::pair<int,int>>;  // [ agent, (from_loc, to_loc) ] 
    int _locChangeN;
public:
    LocChangeHandlingApp(){
        __range = SortByLocTesterApp::genRange();
        __It = 5;                                // number of measurement iteratons for statistics
        __agentN =  900; //1<<20; //1<<26;     // 2^18 - fast, 2^20 ~= 1 million // number of values   (number of agents in the COVID simulator)  
        __locN =  __agentN / 3;               // number of distinct locations (number of locations in the COVID simulator)
        _locChangeN = __agentN / 3;
    }

    std::vector<loc_change_t> genLocChanges(std::vector<int> locations_sortedByAgents){
        std::vector<loc_change_t> output;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution distrb_agent(0, __agentN-1);
        std::uniform_int_distribution distrb_loc(0, __locN-1);
        for(int i = 0; i < _locChangeN; i++){
            std::pair<int, std::pair<int,int>> tmpLocChange;
            do
                tmpLocChange.first = distrb_agent(gen);
            while( std::find_if(std::execution::par, output.begin(), output.end(), [&](loc_change_t lch){ return tmpLocChange.first == lch.first; }) != output.end());
            tmpLocChange.second.first = locations_sortedByAgents[tmpLocChange.first];
            do
                tmpLocChange.second.second = distrb_loc(gen);
            while( tmpLocChange.second.second == locations_sortedByAgents[tmpLocChange.first]
                    || std::find_if(std::execution::par, output.begin(), output.end(), [&](loc_change_t lch){ return tmpLocChange.first == lch.first; }) != output.end());
            output.push_back(tmpLocChange);
        }
        return output;
    }

    void run(std::vector<int> &times_manualUpdate, std::vector<int> &times_sortAgain){ 
        for(int aN : {100000}){
            for(int k = 0; k < 10; k++){
                __agentN =  aN;   
                __locN =  __agentN / 3;  
                _locChangeN = __agentN / 3;
                std::cout<<"agentN: "<<__agentN<<std::endl;
                
                // init
                std::vector<int> agents_sbA(__agentN); // sbA = sorted by agents
                std::vector<int> locations_sbA(__agentN);
                init_vectors(agents_sbA, locations_sbA);

                std::vector<int> agents(__agentN);
                std::vector<int> locations(__agentN);
                std::vector<int> locPtrs(__locN+1);
                std::copy(std::execution::par, agents_sbA.begin(), agents_sbA.end(), agents.begin());
                std::copy(std::execution::par, locations_sbA.begin(), locations_sbA.end(), locations.begin());
                // sort
                sort_STD_PAIR(agents, locations);
                generateKeyPtrs(locations, locPtrs);
                // loc change
                std::vector<loc_change_t> locChanges = genLocChanges(locations_sbA); 

                ///////// update ////////// 
                // agents_sbA
                // ---
                // locations_sbA
                std::for_each(std::execution::par, locChanges.begin(), locChanges.end(), [&](loc_change_t lch){
                    //std::cout << (locations_sbA[lch.first] == lch.second.first); // check  
                    locations_sbA[lch.first] = lch.second.second;
                });

                
                auto t_begin = std::chrono::high_resolution_clock::now();
                // locPtrs
                std::for_each(std::execution::seq, locChanges.begin(), locChanges.end(), [&locPtrs](loc_change_t lch){
                    if(lch.second.first < lch.second.second)
                        for(int k = lch.second.first + 1; k < lch.second.second; k++)
                            locPtrs[k]++;
                    else
                        for(int k = lch.second.second + 1; k < lch.second.first; k++)
                            locPtrs[k]--;
                });

                // agents
                std::for_each(std::execution::seq, locChanges.begin(), locChanges.end(), [&agents, &locPtrs](loc_change_t lch){
                    auto it_originalAgentInd = std::lower_bound(agents.begin()+locPtrs[lch.second.first], agents.begin()+locPtrs[lch.second.first+1], lch.first);
                    auto it_newAgentInd = std::lower_bound(agents.begin()+locPtrs[lch.second.second], agents.begin()+locPtrs[lch.second.second+1], lch.first);
                    if(lch.second.first < lch.second.second)
                        for( auto it_currAgent = it_originalAgentInd; it_currAgent != it_newAgentInd; it_currAgent++)
                            *it_currAgent = *(it_currAgent+1);
                    else
                        for( auto it_currAgent = it_originalAgentInd; it_currAgent != it_newAgentInd; it_currAgent--)
                            *it_currAgent = *(it_currAgent-1);
                    *it_newAgentInd = lch.first; 
                });

                // locations
                for(int i = 0; i < __locN; i++)
                    std::fill(std::execution::par, locations.begin()+locPtrs[i+1], locations.begin()+locPtrs[i+1], i);   

                auto t_end = std::chrono::high_resolution_clock::now();

                // ... vs sort again
                std::copy(std::execution::par, agents_sbA.begin(), agents_sbA.end(), agents.begin());
                std::copy(std::execution::par, locations_sbA.begin(), locations_sbA.end(), locations.begin());
                // sort
                float time_sort = sort_STD_PAIR(agents, locations);
                float time_gen_locPtrs = generateKeyPtrs(locations, locPtrs);


                // time measuring
                int time_manualUpdate = std::chrono::duration_cast<std::chrono::nanoseconds>(t_end-t_begin).count();
                int time_sortAgain = time_sort + time_gen_locPtrs;
                times_manualUpdate.push_back(time_manualUpdate);
                times_sortAgain.push_back(time_sortAgain);
            }
        }

    }
};

    
