#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

namespace printer{

    void PRINT_vector(const std::vector<int>& vector, std::string label = ""){
        std::cout << label << '\t';
        for(int item : vector){
            std::cout << item << ",\t";
        }
        std::cout<<"\n--------------------"<<std::endl;
    }
    void PRINT_vector(const std::vector<std::pair<int,int>>& vector, std::string first_or_second, std::string label = ""){
        std::cout << label << '\t';
        if (first_or_second == "first")
            for(std::pair<int,int> item : vector){
                std::cout << item.first << ",\t";
            }
        else if (first_or_second == "second")
            for(std::pair<int,int> item : vector){
                std::cout << item.second << ",\t";
            }
        else
            std::cout<<"first or second?"<<std::endl;
        std::cout<<"\n--------------------"<<std::endl;
    }

    template<typename T>
    void to_file(const std::vector<T>& v, std::ofstream &f, std::string var_prefix = ""){
        f << var_prefix << "[ ";
        for(int i = 0; i<v.size(); i++){
            f<<v[i];
            if(i != v.size()-1) f<<", ";
        }
        f<<"]\n\n";
    }

    template<typename T>
    std::string to_str(T i){
        std::stringstream ss;
        std::string s;
        ss<<i;
        ss>>s;
        return s;
    }

} // namespace