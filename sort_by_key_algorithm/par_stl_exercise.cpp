#include <pstl/algorithm>
#include <pstl/numeric>
#include <pstl/execution>

#include <iostream>
#include <chrono>
#include <algorithm>
#include <random>
#include <cmath>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/tuple/tuple.hpp>
//#include <boost/tuple/tuple_comparison.hpp>

int main(void) {
    
  	std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
  	std::chrono::duration<double> elapsed_seconds;
  
  	std::mt19937 gen(0);
  	std::normal_distribution<> d(500,200);
  	const int N = 1<<20;
  	std::vector<int> inputdata(N);
  	std::generate(inputdata.begin(), inputdata.end(),[&d,&gen](){return std::max(0,(int)std::round(d(gen)));});
  	std::vector<int> inputkeys(N);
  	std::generate(inputkeys.begin(), inputkeys.end(),[&d,&gen](){return gen()%20;});
  
  
    // To learn:    transfrom, reduce, sort, unique      functions    --> boost könyvtár szépségei :)
    ///////////////////////////////////////////////////////////////
    
  	/*You are given a vector of scores between 0 and 1000 (inputdata)
     
      1.
  	  Copy those values and compute                                            ?? why copy? -- double
  	  the average of scores (~500), and their standard deviation (~200).
  	  Make sure that you return and calculate with double values
  	  instead of ints, otherwise you will get integer overflow problems.
       
  	  Hint: for st. dev. use transform_reduce
  	  where the operators take integers but return doubles
       
      2. 
  	  Calculate the number of scores above 800, 500, 200.
  	  Find the minimum an maximum element. Try using  minmax_element!
       
  	  Hint: use "auto" for the return type
       
      3.
  	  Check if there is an element with value 1234
  	*/
    {
        // 1) avg, standard deviation  --------------------------------------------------------------------------------------------------------
        std::vector<double> dataD(N);
        std::copy(pstl::execution::par_unseq, inputdata.begin(), inputdata.end(), dataD.begin());
        
        // avg ~500  // REDUCE
        double avg = std::reduce(pstl::execution::par, dataD.begin(), dataD.end());
        avg = avg/N;
        std::cout<<"Average: "<<avg<<std::endl;
        
        //standard deviation ~200  // TRANSFORM_REDUCE
        std::vector<double> squared_diffs(N);
        double stand_dev = std::transform_reduce(pstl::execution::par, dataD.begin(), dataD.end(), 0.0,
                                                  std::plus<double>(), [=](double a){return (a-avg)*(a-avg);});
        stand_dev = sqrt(stand_dev/double(N-1));
        
        std::cout<<"Stand. Dev.: "<<stand_dev<<std::endl;
        
        
        // 2) num of above 800, 500, 200  // COUNT   -------------------------------------------------------------------------------------------
        double above_800 = std::count_if(pstl::execution::par, dataD.begin(), dataD.end(), [](double a){return a > 800;});
        double above_500 = std::count_if(pstl::execution::par, dataD.begin(), dataD.end(), [](double a){return a > 500;});
        double above_200 = std::count_if(pstl::execution::par, dataD.begin(), dataD.end(), [](double a){return a > 200;});
        
        std::cout<<"above_800: "<<above_800<<std::endl;
        std::cout<<"above_500: "<<above_500<<std::endl;
        std::cout<<"above_200: "<<above_200<<std::endl;
        
        // min, max  // MINMAX_ELEMENT   -----------------------------------------
        const auto [min_ptr, max_ptr] = std::minmax_element(pstl::execution::par, begin(dataD), end(dataD));
        
        std::cout<<"min = "<<*min_ptr<<" , max = "<<*max_ptr <<std::endl;
        
        // is there equals to 1234 ?  // FIND  --------------------------------------
        auto is_there_1234_PTR = std::find(pstl::execution::par, dataD.begin(), dataD.end(), 1234);      // ?? return value? -- valamiféle pointer  (0 ha nem találta meg, a keresett szám ha megtalálta)   //__pstl::__internal::__enable_if_execution_policy<const __pstl::execution::v1::parallel_policy &, __gnu_cxx::__normal_iterator<double *, std::vector<double, std::allocator<double>>>> 
        
        bool is_there_1234 = true;
        if(*is_there_1234_PTR==0)   
            is_there_1234 = false;
        
        std::cout<<"is_there_1234 = "<<is_there_1234<<std::endl;
        
  	}

  
  
  
  
  	/* Copy both arrays, and create a separate           ?? inputdata and inputkeys? - yes
  	 * array with an index for each score (0->N-1).
  	 * Sort the scores in descending order, along with the group
  	 * and index values
  	 * Hint: use tuples to merge groups and indices into one array
     
  	 * From the best 20 scores, select the ones that are in different
  	 * groups.
     * Hint: use unique, with all
     
  	 * 3 arrays in a single tuple. Keep in mind, that unique expects
  	 * a sorted input (by group in this case).
  	 * Print the indices, groups and scores of these
  	 */
  	{
        // 1) inds, vals  COPY ----------------------------------------------------------------
        std::vector<int> vals(N);
        std::vector<int> inds(N);
        std::copy(pstl::execution::par, inputdata.begin(), inputdata.end(), vals.begin());
        for(int i = 0; i<inds.size(); i++) { inds[i] = i; }
        
        // inds_vals  TRANSFORM
        std::vector<std::pair<int,int>> inds_vals(N);
        std::transform(pstl::execution::par, inds.begin(), inds.end(), vals.begin(), inds_vals.begin(), [](int ind, int val){ std::pair<int,int> tmp = std::make_pair(ind, val); return tmp; });
        
        // SORT
        std::sort(std::execution::par, inds_vals.begin(), inds_vals.end(), [](std::pair<int,int> a, std::pair<int,int> b){
                                                                                                                          if(a.second == b.second) return a.first > b.first;
                                                                                                                          return a.second > b.second;
                                                                                                                        });
        
        
        /////-----  2) best 20 - different values  // UNIQUE     --------------------------------------------------------------------------------------
        // elsõ 20 leválasztás
        int _BESTSIZE = 100;
        std::vector<std::pair<int,int>> best20(_BESTSIZE);
        std::copy(pstl::execution::par, inds_vals.begin(), inds_vals.begin()+_BESTSIZE, best20.begin());
        
        // UNIQUE
        auto last = std::unique(pstl::execution::par, best20.begin(), best20.end(), [](std::pair<int,int> a, std::pair<int,int> b){ return a.second == b.second; });
        best20.erase(last, best20.end());
        
        
        // ez csak a szomszádos ismétlõdéseket veszi ki. ALL:      (most elég mert már sortolva van)
        //    sort();
        //    unique();
        
        // UNIQUE_COPY
        //    unique(par, in.begin(), in.end(), out.bengin(), [](){});
  
        
        
        
        std::cout<<std::endl<<"unique best_"<<_BESTSIZE<<": "<<std::endl;
        for(int i = 0; i<best20.size(); i++){
            std::cout<<i<<"    val:  "<<best20[i].second<<"      ind: "<<best20[i].first<<std::endl;
        }
        std::cout<<"unique best_"<<_BESTSIZE<<" size: "<<best20.size()<<std::endl;
        
              
        
  	
    ///////////////    zip_iterator    // SORT + UNIQUE   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::for_each(inds.begin(), inds.end(), [](int a){});
        std::for_each(vals.begin(), vals.end(), [](int a){});
        
        
        /*
        std::sort(
            pstl::execution::par,
            boost::make_zip_iterator(boost::make_tuple(inds.begin(), vals.begin())),
            boost::make_zip_iterator(boost::make_tuple(inds.end(), vals.end())),
            [](boost::tuples::tuple<int, int> a, boost::tuples::tuple<int, int> b){
                if(a.get<1>() == b.get<1>()) return a.get<0>() > b.get<0>();
                return a.get<1>() > b.get<1>();
            }
        );
        //std::for_each(pstl::execution::par, vals.begin(), vals.end(), [](int &a){a++;});
        */
        
        
        // best 100  // UNIQUE
        
        _BESTSIZE = 100;
        std::vector<std::pair<int,int>> best100_inds(_BESTSIZE);
        std::vector<std::pair<int,int>> best100_vals(_BESTSIZE);
        
        
        auto last_zipit = std::unique(
            pstl::execution::par,
            boost::make_zip_iterator(boost::make_tuple(inds.begin(), vals.begin())),
            boost::make_zip_iterator(boost::make_tuple(inds.begin()+_BESTSIZE, vals.begin()+_BESTSIZE)),
            [](boost::tuples::tuple<int, int> a, boost::tuples::tuple<int, int> b){
                return a.get<1>() == b.get<1>();
            }
        );
        
        
        //best100_inds.erase(last_zipit.get_iterator_tuple().get<0>(), best20.end());
        //best100_vals.erase(&last_zipit->get<1>(), best20.end());
   
    }   
   
   
   
   
   
  
  	return 0;
}
