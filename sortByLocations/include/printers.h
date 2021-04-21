#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

namespace printer{

    void PRINT_vector(const std::vector<int>& inputkeys, std::string label = ""){
        std::cout << label;
        for(int key : inputkeys){
            std::cout << key << ",\t";
        }
        std::cout<<"\n--------------------\n";
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