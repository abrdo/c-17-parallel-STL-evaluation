#ifndef SORT_BY_KEY_FUNCTIONS
#define SORT_BY_KEY_FUNCTIONS

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

#include <vector>
#include <chrono>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/tuple/tuple.hpp>

#include "pairedvectoriterator.h" // implemented by me and Kompi
#include "tupleit.hh"  // a boost::tuple iterator, implemented by Anthony Williams  - https://pastebin.com/LFkTHdQk  


float sort_by_key_STD_PAIR(std::vector<int> &inputkeys, std::vector<int> &inputdata){
    //--- Init ---//
    int N = inputkeys.size();
    std::vector<std::pair<int,int>> keys_data(N);
    
    //--- Operations - time measuring starts ---//
    auto t_begin = std::chrono::high_resolution_clock::now();
    
    //  TRANSFORM the 2 vector to one --> keys_data
    std::transform(std::execution::par, inputkeys.begin(), inputkeys.end(), inputdata.begin(), keys_data.begin(), [](int key, int data){ std::pair<int,int> tmp = std::make_pair(key, data); return tmp; });
    
    // SORT
    std::sort(std::execution::par, keys_data.begin(), keys_data.end(), [](std::pair<int,int> a, std::pair<int,int> b){ return a.first < b.first;});
    
    // transform back
    std::transform(std::execution::par, keys_data.begin(), keys_data.end(), inputkeys.begin(), [](std::pair<int, int> key_data){ return key_data.first; });
    std::transform(std::execution::par, keys_data.begin(), keys_data.end(), inputdata.begin(), [](std::pair<int, int> key_data){ return key_data.second; });
    
    auto t_end = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration_cast<std::chrono::milliseconds>(t_end-t_begin).count();
    return time;
}
    

float sort_by_key_HELPER_INDICES_VECTOR(std::vector<int>& inputkeys, std::vector<int>& inputdata){
    //--- Init ---//
    long int N = inputkeys.size();
    std::vector<int> indices(N);
    std::iota(indices.begin(), indices.end(), 0);
    
    int* inputkey_ptr = &inputkeys[0];   
    
    std::vector<int> sorted_keys(N);
    std::vector<int> sorted_data(N);
    
    //--- Operations - time measuring starts ---//
    auto t_begin = std::chrono::high_resolution_clock::now();
    
    std::sort(std::execution::par, indices.begin(), indices.end(),
        [=](const int& a, const int& b){
            return inputkey_ptr[a] < inputkey_ptr[b];
        }
    );
    
    // might slow...
    for(long int i = 0; i<N; i++){
        sorted_keys[i] = inputkeys[indices[i]];
        sorted_data[i] = inputdata[indices[i]];
    }
    
    // copy
    std::copy(std::execution::par, sorted_keys.begin(), sorted_keys.end(), inputkeys.begin());
    std::copy(std::execution::par, sorted_data.begin(), sorted_data.end(), inputdata.begin());
    
    auto t_end = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration_cast<std::chrono::milliseconds>(t_end-t_begin).count();
    return time;
}


float sort_by_key_PAIRED_VECTOR_ITERATOR(std::vector<int> &inputkeys, std::vector<int> &inputdata){
    // todo with my paired vector iterator
    return 0;
}



float sort_by_key_BOOSTTUPLEIT(std::vector<int> &inputkeys, std::vector<int> &inputdata){

    // icpc - on CPU - one warning :
    /*include/tupleit.hh(281): warning #1478: class "std::auto_ptr<boost::tuples::cons<int, boost::tuples::cons<int, boost::tuples::null_type>>>" (declared at line 87 of "/usr/include/c++/4.8.5/backward/auto_ptr.h") was declared deprecated
              std::auto_ptr<OwnedType> tupleBuf;
                                       ^
    */
    // on GPU - error
    // g++ - error in the tupleit.hh
    typedef boost::tuple<int&,int&> tup_t;
    
    auto t_begin = std::chrono::high_resolution_clock::now();
    std::sort(
        std::execution::par,
        iterators::makeTupleIterator(inputkeys.begin(), inputdata.begin()),
        iterators::makeTupleIterator(inputkeys.end(), inputdata.end()),
        [](tup_t i, tup_t j){
            return i.get<0>() < j.get<0>();
        }
    );
    auto t_end = std::chrono::high_resolution_clock::now();
    
    float time = std::chrono::duration_cast<std::chrono::milliseconds>(t_end-t_begin).count();
    return time;

}




/*   boost ziphez:
template <typename... T>
auto zip(T&... containers)
    -> boost::iterator_range<decltype(iterators::makeTupleIterator(std::begin(containers)...))> {
  return boost::make_iterator_range(iterators::makeTupleIterator(std::begin(containers)...),
                                      iterators::makeTupleIterator(std::end(containers)...));
}
*/



#endif //SORT_BY_KEY_FUNCTIONS
