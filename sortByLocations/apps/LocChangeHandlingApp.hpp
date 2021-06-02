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
#include <map>
#include <iterator>
#include <random>
#include <fstream>
#include <cmath>

using namespace sorting;
using namespace printer;


class LocChangeHandlingApp : public SortByLocTesterApp{
    using loc_change_t = std::pair<int, std::pair<int,int>>;  // [ agent, (from_loc, to_loc) ]
    using time_unit_t = std::chrono::milliseconds;
public:
    struct Times{
        std::vector<int> times_sortAgain;
        std::vector<int> times_refreshLocPtrs;
        std::vector<int> times_refreshAgents;
        std::vector<int> times_refreshLocations;
        std::vector<int> getFullUpdateTime(){
            std::vector<int> times(times_refreshLocPtrs.size());
            for(int i = 0; i<times.size(); i++){
                times[i] = times_refreshLocPtrs[i] + times_refreshAgents[i] + times_refreshLocations[i];
            }
            return times;
        }
    };
private:
    int _locChangeN;
    std::vector<loc_change_t> _locChanges; 

    std::vector<int> _agents_sbA; // sbA = sorted by _agents
    std::vector<int> _locations_sbA;

    std::vector<int> _agents;
    std::vector<int> _locations;
    std::vector<int> _locPtrs; 

    Times _times;

    // helper vectors: 
    //  TODO   (maybe rather in separate classes (hmm maybe NOT))

public:
    LocChangeHandlingApp(){
        __range = SortByLocTesterApp::genRange();
        __It = 1;                                // number of measurement iteratons for statistics
        __agentN =  1000000;  //1<<20; //1<<26;     // 2^18 - fast, 2^20 ~= 1 million // number of values   (number of _agents in the COVID simulator)  
        __locN = __agentN / 3;               // number of distinct _locations (number of _locations in the COVID simulator)
        _locChangeN = 3; //__agentN / 3;
        
        _agents_sbA = std::vector<int>(__agentN); // sbA = sorted by _agents
        _locations_sbA = std::vector<int>(__agentN);
        init_vectors(_agents_sbA, _locations_sbA);

        _locChanges = genLocChanges(_locations_sbA); 

        _agents = std::vector<int>(__agentN);
        _locations = std::vector<int>(__agentN);
        _locPtrs = std::vector<int>(__locN+1);
        std::copy(std::execution::par, _agents_sbA.begin(), _agents_sbA.end(), _agents.begin());
        std::copy(std::execution::par, _locations_sbA.begin(), _locations_sbA.end(), _locations.begin());

        // sort
        sort_STD_PAIR(_agents, _locations);
        generateKeyPtrs(_locations, _locPtrs);
    }

    std::vector<loc_change_t> genLocChanges(std::vector<int> locations_sortedByAgents){
        
        std::vector<loc_change_t> locChanges;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution distrb_agent(0, __agentN-1);
        std::uniform_int_distribution distrb_loc(0, __locN-1);
        for(int i = 0; i < _locChangeN; i++){
            std::pair<int, std::pair<int,int>> tmpLocChange;
            do
                tmpLocChange.first = distrb_agent(gen);
            while( std::find_if(std::execution::par, locChanges.begin(), locChanges.end(), [&](loc_change_t lch){ return tmpLocChange.first == lch.first; }) != locChanges.end());
            tmpLocChange.second.first = locations_sortedByAgents[tmpLocChange.first];
            do
                tmpLocChange.second.second = distrb_loc(gen);
            while( tmpLocChange.second.second == locations_sortedByAgents[tmpLocChange.first]
                    || std::find_if(std::execution::par, locChanges.begin(), locChanges.end(), [&](loc_change_t lch){ return tmpLocChange.first == lch.first; }) != locChanges.end());
            locChanges.push_back(tmpLocChange);
        }

        sortLocChanges(locChanges);

        return locChanges;
    }

    void sortLocChanges(std::vector<loc_change_t> &locChanges){
        // update_agents ::  movingAgents_toInds  wants it:
        // 1. toInd  2. agent
        std::sort(std::execution::par, locChanges.begin(), locChanges.end(), [](loc_change_t lch1, loc_change_t lch2){
            if(lch1.second.second != lch2.second.second)
                return lch1.second.second < lch2.second.second;
            return lch1.first < lch2.first;
        });
    }


    ///////////////////////////////////////  Test case init  //////////////////////////////////////////////////////////////////
    void initTestCase(){
        _agents_sbA    = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        _locations_sbA = {0, 0, 0, 1, 1, 1, 2, 2, 2, 0};
        //int lchs[3][2] = {{1, 2}, {4, 0}, {8, 0}};
        
        _agents_sbA    = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        _locations_sbA = {0, 0, 0, 1, 0, 2, 0, 1, 1, 0};
        //int lchs[3][2] = {{9, 2}, {2, 2}, {1, 1}};

        _agents_sbA    = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        _locations_sbA = {2, 2, 0, 1, 2, 0, 2, 2, 1, 0};
        //int lchs[3][2] = {{7, 0}, {6, 1}, {8, 2}};  soution was: --> first update locPtrs

        _agents_sbA    = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        _locations_sbA = {1, 2, 2, 2, 0, 1, 1, 0, 0, 0};
        //int lchs[3][2] = {{3, 1}, {9, 1}, {0, 2}};  --> just good again

        std::vector<int> agents    = {13,      4,       9,       6,       1,       15,      17,      19,      8,       5,       11,      12,      0,       14,      2,       18,      20,      16,      7,       3,       10};
        std::vector<int> locations = {0,       0,       0,       0,       1,       1,       2,       2,       3,       3,       3,       3,       3,       3,       4,       5,       5,       5,       5,       5,       6 };
        // int lchs[3][2] = {{6, 3}, {9, 3}, {17, 6}};

        agents    = {16,      8,       1,       3,       18,      15,      0,       20,      12,      6,       2,       11,      7,       17,      9,       14,      4,       19,      10,      13,      5};
        locations = {0,       0,       1,       1,       1,       1,       2,       3,       3,       3,       3,       4,       4,       4,       5,       5,       5,       5,       6,       6,       6 };
        // int lchs[3][2] = {{17, 0}, {11, 3}, {14, 6}};

        agents =        {5,      15,     2,      3,      7,      8,      12,     13,     4,      0,      11,     9,      17,     16,     10,     14,     6,      1};
        locations =     {0,      0,      1,      1,      1,      1,      1,      1,      2,      2,      2,      4,      4,      4,      4,      5,      5,      5};
        int lchs[3][2] = {{0, 1}, {17, 2}, {9, 5}}; // {agentID, toInd}

        _locChanges = initLocChanges(lchs); // {agentID, toInd}

        // sorted by agents
        sort_STD_PAIR(locations, agents); 
        _agents_sbA = agents;
        _locations_sbA = locations;
        

        _agents = std::vector<int>(__agentN);
        _locations = std::vector<int>(__agentN);
        _locPtrs = std::vector<int>(__locN+1);
        std::copy(std::execution::par, _agents_sbA.begin(), _agents_sbA.end(), _agents.begin());
        std::copy(std::execution::par, _locations_sbA.begin(), _locations_sbA.end(), _locations.begin());

        // sort
        sort_STD_PAIR(_agents, _locations);
        generateKeyPtrs(_locations, _locPtrs);

        // _agentN, locN, _locChangeN
        __agentN = _agents.size();
        std::vector<int> unique_Count;
        std::unique_copy(_locations.begin(), _locations.end(), std::back_inserter(unique_Count));
        __locN = unique_Count.size();
        _locChangeN = _locChanges.size();
    }

    std::vector<loc_change_t> initLocChanges(int (&lchs)[3][2]){
        std::vector<loc_change_t> locChanges;
        for(int i = 0; i < 3; i++){
            int ind = std::distance(_agents_sbA.begin(), std::lower_bound(_agents_sbA.begin(), _agents_sbA.end(), lchs[i][0]));
            loc_change_t lch;
            lch.first = lchs[i][0];
            lch.second.first = _locations_sbA[ind];
            lch.second.second = lchs[i][1];
            
            locChanges.push_back(lch);
        }

        sortLocChanges(locChanges);
        
        return locChanges;
    }

    ///////////////////////////////////////  END Test case init  //////////////////////////////////////////////////////////////////
    
    Times run() { //////////////////////////////////////////////////    RUN    ////////////////////////////////////////////////////////////////////
        std::cout<<"agentN: "<<__agentN<<std::endl;
        for(int aN : {10}){
            for(int k = 0; k < __It; k++){

                //// initTestCase(); // else default: rand gen test

                std::cout << "\n////////////// INITIALIZED //////////////\n";
                //// PRINT_all();
                //// std::cout<<"\n";
                

                update_locations_sbA(); // is not in full time measure
                update_agents();
                update_locPtrs();
                update_locations();


                
                // save UPDATE METHOD results
                std::vector<int> _agentsU = _agents;
                std::vector<int> _locationsU = _locations;
                std::vector<int> _locPtrsU = _locPtrs; 

                std::cout << "\n////////////// UPDATED //////////////\n";
                //// PRINT_all();


                // ... vs sort again
                std::copy(std::execution::par, _agents_sbA.begin(), _agents_sbA.end(), _agents.begin());
                std::copy(std::execution::par, _locations_sbA.begin(), _locations_sbA.end(), _locations.begin());
                float time_sort = sort_STD_PAIR(_agents, _locations);
                float time_gen_locPtrs = generateKeyPtrs(_locations, _locPtrs);
                int time_sortAgain = time_sort + time_gen_locPtrs;
                _times.times_sortAgain.push_back(time_sortAgain);
                std::cout << "\n////////////// SORTED ///////////////\n";
                //// PRINT_all();

                // validate UPDATE METHOD
                bool eq_agents    = _agentsU == _agents;
                bool eq_locations = _locationsU == _locations;
                bool eq_locPtrs   = _locPtrsU == _locPtrs;
                std::cout << "\neq_agents: \t" << eq_agents << std::endl;
                std::cout <<   "eq_locations: \t" << eq_locations << std::endl;
                std::cout <<   "eq_locPtrs: \t" << eq_locPtrs << std::endl;

            }
        }
        return _times;
    }


    void update_locations_sbA(){
        std::for_each(std::execution::par, _locChanges.begin(), _locChanges.end(), [&](loc_change_t lch){
            //std::cout << (_locations_sbA[lch.first] == lch.second.first); // check  
            _locations_sbA[lch.first] = lch.second.second;
        });
    }

        
    void update_locPtrs(){ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<int> locInds(__locN);
        std::iota(locInds.begin(), locInds.end(), 0);

        auto t_locPtrs_begin = std::chrono::high_resolution_clock::now();
        std::for_each(locInds.begin(), locInds.end(), [&](int i){
            _locPtrs[i] -= std::count_if(std::execution::par, _locChanges.begin(), _locChanges.end(), [&](loc_change_t lch){
                return lch.second.first < i;
            });
            _locPtrs[i] += std::count_if(std::execution::par, _locChanges.begin(), _locChanges.end(), [&](loc_change_t lch){
                return lch.second.second < i;
            });
        });
        auto t_locPtrs_end = std::chrono::high_resolution_clock::now();

        int time_refreshLocPtrs = std::chrono::duration_cast<time_unit_t>( t_locPtrs_end - t_locPtrs_begin ).count();
        _times.times_refreshLocPtrs.push_back(time_refreshLocPtrs);
    }


void update_agents(){ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // HELPER arrays
        std::vector<std::pair<int,int>> staticAgents_inds(__agentN - _locChangeN);
        std::vector<std::pair<int,int>> movingAgents_fromInds(_locChangeN);
        std::vector<std::pair<int,int>> movingAgents_toInds(_locChangeN);


        ////////////// for DEBUG purpuse ////////////////
        std::fill(staticAgents_inds.begin(), staticAgents_inds.end(), std::make_pair<int,int>(-1,-1));
        std::fill(movingAgents_fromInds.begin(), movingAgents_fromInds.end(), std::make_pair<int,int>(-1,-1));
        std::fill(movingAgents_toInds.begin(), movingAgents_toInds.end(), std::make_pair<int,int>(-1,-1));

        // ----------------------- START time measuring -------------------------------------
        auto t_agents2_begin = std::chrono::high_resolution_clock::now();

        // init movingAgents_fromInds, movingAgents
        std::vector<int> movingAgents(_locChangeN);
        std::vector<int> changeInds(_locChangeN);
        std::iota(changeInds.begin(), changeInds.end(), 0);
        std::for_each(std::execution::par, changeInds.begin(), changeInds.end(), [this, &movingAgents_fromInds, &movingAgents](int i){
            auto ptr = std::lower_bound(_agents.begin() + _locPtrs[_locChanges[i].second.first], _agents.begin() + _locPtrs[_locChanges[i].second.first + 1], _locChanges[i].first);  // must exist accurately
            
            ////std::for_each(_agents.begin() + _locPtrs[_locChanges[i].second.first], _agents.begin() + _locPtrs[_locChanges[i].second.first + 1], [](int i){ std::cout << i << " ";});
            ////std::cout <<"\n to find: \t"<< _locChanges[i].first << "\n";
            ////std::cout <<"it points to: \t" <<*ptr<<std::endl;

            int fromInd = std::distance(_agents.begin(), ptr);
            movingAgents_fromInds[i] = std::make_pair(_locChanges[i].first, fromInd);
            movingAgents[i] = _locChanges[i].first;
        });
        ////PRINT_vector(movingAgents_fromInds, "first", "Moving agents:    ");
        ////PRINT_vector(movingAgents_fromInds, "second", "mvAgs_fromInds:  ");
        
        // TODO: ? is it needed indead? (isnt enough to go throw by indexes in parallel loops?)
        // init agents_oldInds
        std::vector<std::pair<int,int>> agents_oldInds(__agentN);
        for(int i = 0; i < __agentN; i++){
            agents_oldInds[i] = std::make_pair(_agents[i], i);
        }

        // (DELETION) staticAgents_inds = agents_oldInds - "moving Agents"  
        std::for_each(std::execution::par, agents_oldInds.begin(), agents_oldInds.end(), [this, &movingAgents, &movingAgents_fromInds, &staticAgents_inds](std::pair<int,int> agent_oldInd){
            if(std::binary_search(movingAgents.begin(), movingAgents.end(), agent_oldInd.first))
                return;
            int shiftLeft = std::count_if(movingAgents_fromInds.begin(), movingAgents_fromInds.end(), [&](std::pair<int, int> mvAgent_fromInd){
                return mvAgent_fromInd.second < agent_oldInd.second; // shouldnt be equal because than it is filterd out above
            });
            std::pair<int,int> newStatAgent_ind;
            newStatAgent_ind.first  = agent_oldInd.first;
            newStatAgent_ind.second = agent_oldInd.second - shiftLeft;
            staticAgents_inds[newStatAgent_ind.second] = newStatAgent_ind; // TODO ERROR
            //////std::cout << agent_oldInd.first << std::endl;
            //////std::cout << agent_oldInd.second - shiftLeft << std::endl;
            //////std::cout << std::endl;
        });
        ////PRINT_vector(staticAgents_inds, "first" , "statAg:      ");


        // create locPtrs for staticAgents_inds 
        std::vector<int> locPtrs_stat(__locN + 1);
        std::vector<int> locs(__locN);
        std::iota(locs.begin(), locs.end(), 0);
        std::vector<int> locPtrs_shifts(__locN + 1, 0);
        std::for_each(std::execution::par, locs.begin(), locs.end(), [this, &locPtrs_shifts](int loc){
            locPtrs_shifts[loc + 1] = std::count_if(std::execution::par, _locChanges.begin(), _locChanges.end(), [&loc](loc_change_t lch){
                return lch.second.first == loc;
            }); 
        });
        std::inclusive_scan(std::execution::par, locPtrs_shifts.begin(), locPtrs_shifts.end(), locPtrs_shifts.begin());
        std::transform(std::execution::par, _locPtrs.begin(), _locPtrs.end(), locPtrs_shifts.begin(), locPtrs_stat.begin(), [](int lPtr, int shift){
            return lPtr - shift;  // shouldn't throw segfault
        });
        ////PRINT_vector(locPtrs_stat, "locPtrs_stat:  ");

        // movingAgents_toInds  (not intent)   !!! : locChanges must be sorted by toInds!
        std::for_each(std::execution::par, changeInds.begin(), changeInds.end(), [this, &movingAgents_toInds, &staticAgents_inds, &locPtrs_stat](int i){
            auto ptr = std::lower_bound(staticAgents_inds.begin() + locPtrs_stat[_locChanges[i].second.second], staticAgents_inds.begin() + locPtrs_stat[_locChanges[i].second.second + 1], _locChanges[i].first, [&](std::pair<int,int> agent_ind, int agent){
                return agent_ind.first < agent;
            });  // shouldn't exist accuratelly
            int toInd = std::distance(staticAgents_inds.begin(), ptr);
            movingAgents_toInds[i] = std::make_pair(_locChanges[i].first, toInd + i);   // locChanges is sorted by 1. toLocation 2. agnetID   =>   movingAgents_toInds is sorted by 1. inds 2. agents
                                                                                        // +i is because of the "self shift"
        });     
        ////PRINT_vector(movingAgents_toInds, "first",  "mvAg:         ");
        ////PRINT_vector(movingAgents_toInds, "second", "TO ind:  ");

        // (INSERTION) Insert staticAgents into _agents
        std::function<int(int, int)> calcShift = [&calcShift, &movingAgents_toInds](int beginInd, int toInd){ // shouldn't throw segfault
            int shift = std::count_if(movingAgents_toInds.begin(), movingAgents_toInds.end(), [&](std::pair<int,int> agent_ind){
                return beginInd <= agent_ind.second  && agent_ind.second <= toInd;
            });
            if(shift != 0)
                shift = shift + calcShift(toInd+1, toInd + shift);
            return shift;
        };
        std::for_each(std::execution::seq /*TODO*/, staticAgents_inds.begin(), staticAgents_inds.end(), [&](std::pair<int,int> agent_ind){
            int shiftRigth = calcShift(0, agent_ind.second);
            _agents[agent_ind.second + shiftRigth] = agent_ind.first;
        });

        // Insert movingAgents into _agents
        std::for_each(std::execution::par, movingAgents_toInds.begin(), movingAgents_toInds.end(), [&](std::pair<int,int> agent_ind){
            _agents[agent_ind.second] = agent_ind.first;
        });

        auto t_agents2_end = std::chrono::high_resolution_clock::now();

        int time_refreshAgents2 = std::chrono::duration_cast<time_unit_t>( t_agents2_end - t_agents2_begin ).count();
        _times.times_refreshAgents.push_back(time_refreshAgents2);
    }

        
    void update_locations(){ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<int> locInds(__locN);
        std::iota(locInds.begin(), locInds.end(), 0);
        
        auto t_locations_begin = std::chrono::high_resolution_clock::now();
        std::for_each(locInds.begin(), locInds.end(), [this](int i){
            std::fill(std::execution::par, _locations.begin()+_locPtrs[i], _locations.begin()+_locPtrs[i+1], i);  
        }); 
        auto t_locations_end = std::chrono::high_resolution_clock::now();

        int time_refreshLocations = std::chrono::duration_cast<time_unit_t>( t_locations_end - t_locations_begin ).count();
        _times.times_refreshLocations.push_back(time_refreshLocations);
    }


    void PRINT_locChanges(){
        std::cout<<"locChanges: \n";
        for(loc_change_t lch : _locChanges){
            std::cout << lch.first << "\t[ " << lch.second.first << "\t" << lch.second.second << " ]\n";
        }
        std::cout<<"--------------------"<<std::endl;
    }
    void PRINT_all(){
        PRINT_vector(_agents_sbA, "_agents_sbA");
        PRINT_vector(_locations_sbA, "_locations_sbA");
        PRINT_vector(_locPtrs,  "_locPtrs :    ");
        PRINT_vector(_agents,   "_agents :     ");
        PRINT_vector(_locations,"_locations :  ");
        PRINT_locChanges();
    }

};

    


/*

    void update_agents_seq(){ //V1 - MOST CRITICAL //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        auto t_agents_begin = std::chrono::high_resolution_clock::now();
        std::for_each(std::execution::seq, _locChanges.begin(), _locChanges.end(), [this](loc_change_t lch){
            auto it_originalAgentInd = std::lower_bound(_agents.begin()+_locPtrs[lch.second.first], _agents.begin()+_locPtrs[lch.second.first+1], lch.first);
            auto it_newAgentInd = std::lower_bound(_agents.begin()+_locPtrs[lch.second.second], _agents.begin()+_locPtrs[lch.second.second+1], lch.first);
            if(lch.second.first < lch.second.second)
                for( auto it_currAgent = it_originalAgentInd; it_currAgent != it_newAgentInd; it_currAgent++)
                    *it_currAgent = *(it_currAgent+1);
            else
                for( auto it_currAgent = it_originalAgentInd; it_currAgent != it_newAgentInd; it_currAgent--)
                    *it_currAgent = *(it_currAgent-1);
            *it_newAgentInd = lch.first; 
        });
        auto t_agents_end = std::chrono::high_resolution_clock::now();

        int time_refreshAgents = std::chrono::duration_cast<time_unit_t>( t_agents_end - t_agents_begin ).count();
        _times.times_refreshAgents.push_back(time_refreshAgents);
    }

    void update_agents__simultainly_the_shiftLeft_shiftRigth__wrong(){ // V2  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // helper arrays:
        std::map<int, std::pair<int,int>> indChanges;  // [agentID [fromInd, toInd]]  (ind in _agents array)       // TODOOOOOOOOOOOOO fill it. 
        std::vector<std::pair<int,int>> oldInds_agents(__agentN);       // [0, 1, 2, 3 ... __agentN]
        std::vector<int> movedFrom(__agentN, 0);  // there will be 1 from where the agent moved away, else 0   - from where we cut out.
        std::vector<int> movedTo(__agentN, 0);    //    -- there will be 1 to where the agent moved to, else 0       - to where we insert
        std::vector<int> shiftLeft(__agentN, 0);  // the ith element shows us with how many index do we have to shift the ith agent in the _agents array
        std::vector<int> shiftRight(__agentN, 0); //    -- practically: newAgents[i] = oldAgents[i] - shiftLeft[i] + shiftRight[i]
        
        // init oldInds_agents
        for(int i = 0; i < __agentN; i++){
            oldInds_agents[i] = std::make_pair(i, _agents[i]);
        }

        auto t_agents2_begin = std::chrono::high_resolution_clock::now();
        
        // fill movedFrom, movedTo
        std::for_each(std::execution::par, _locChanges.begin(), _locChanges.end(), [this, &movedFrom, &movedTo, &indChanges](loc_change_t lch){
            // locate function
            const std::function<void(loc_change_t&)> locate = [&](loc_change_t &new_ich){
                int newInd = new_ich.second.second;
                while(movedTo[newInd] == 1){
                    loc_change_t *ich_ptr = nullptr;
                    for(loc_change_t ich : indChanges){
                        if(new_ich.second.second == ich.second.second){
                            ich_ptr = &ich; // must exist
                            break;
                        }
                    }
                    if(lch.first < ich_ptr->first){
                        newInd--;
                        new_ich.second.second -= 1;
                        locate(new_ich);
                    }else{
                        loc_change_t ich = *ich_ptr;
                        locate(ich);
                    }
                }                     
                indChanges[new_ich.first] = new_ich.second;
                movedTo[newInd] = 1;
            };
            
            //bool check = std::binary_search(_agents.begin()+_locPtrs[lch.second.first], _agents.begin()+_locPtrs[lch.second.first+1], lch.first);
            //if(check != true){
            //    std::cout<<"false!"; // TODO: debug - talán azért, mert a frissített locptrsben már nincsenek ott a régi from helyeknél az agentek
            //}
            auto it_oldInd = std::lower_bound(_agents.begin()+_locPtrs[lch.second.first], _agents.begin()+_locPtrs[lch.second.first+1], lch.first);
            auto it_newInd = std::lower_bound(_agents.begin()+_locPtrs[lch.second.second], _agents.begin()+_locPtrs[lch.second.second+1], lch.first);
            int oldInd = std::distance(_agents.begin(), it_oldInd);
            int newInd = std::distance(_agents.begin(), it_newInd);
            movedFrom[oldInd] = 1;
            
            std::pair<int,int> tmp = std::make_pair(oldInd, newInd);
            loc_change_t newIndChange = std::make_pair(lch.first, tmp);
            
            // lock begin   TODO
            locate(newIndChange);
            // lock end     TODO
        });


        // calc shiftLeft array
        std::exclusive_scan(std::execution::par, movedFrom.begin(), movedFrom.end(), shiftLeft.begin(), 0);

        // calc shiftRight array
        std::inclusive_scan(std::execution::par, movedTo.begin(), movedTo.end(), shiftRight.begin());


        // update _agents with new inds
        std::vector<int> newInds(oldInds_agents.size());
        std::for_each(std::execution::par, oldInds_agents.begin(), oldInds_agents.end(), [this, &movedFrom, &shiftLeft, &shiftRight](std::pair<int, int> oldInd_agent){
            if(movedFrom[oldInd_agent.first])
                return;
            int newInd = oldInd_agent.first - shiftLeft[oldInd_agent.first] + shiftRight[oldInd_agent.first];
            _agents[newInd] = oldInd_agent.second;
        });
        // insert the moving _agents to their new _locations
        std::for_each(std::execution::par, indChanges.begin(), indChanges.end(), [this, &indChanges](loc_change_t ich){
            _agents[ich.second.second] = ich.first;
        });

        
        auto t_agents2_end = std::chrono::high_resolution_clock::now();

        int time_refreshAgents2 = std::chrono::duration_cast<time_unit_t>( t_agents2_end - t_agents2_begin ).count();
        _times.times_refreshAgents.push_back(time_refreshAgents2);
    }

*/