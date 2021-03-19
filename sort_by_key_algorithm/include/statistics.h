#ifndef STATISTICS_H
#define STATISTICS_H

#include <vector>

template<typename T>
T avg(std::vector<T> input){
    T sum = 0;
    for(T ii : input){
        sum += ii;
    }
    T avg = sum / input.size();
    return avg;
}

template<typename T>
T std_dev(std::vector<T> input){
    T avg_ = avg(input);
    T sum = 0;
    for(T ii : input) {
        sum += (ii-avg_)*(ii-avg_);
    }
    T std_dev = sqrt(sum / (input.size()-1));
    return std_dev;
}


#endif //STATISTICS_H
