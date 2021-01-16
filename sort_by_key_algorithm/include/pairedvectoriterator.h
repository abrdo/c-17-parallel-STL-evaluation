// implemented by me and Kompi

#ifndef PAIRED_VECTOR_ITERATOR_H
#define PAIRED_VECTOR_ITERATOR_H


#include <boost/tuple/tuple.hpp>


class PairedVectorIterator{
  // TODO: operator[] for GPU
    
    std::vector<int>& vals;
    std::vector<int>& indices;
    std::size_t index;
    
public:
    PairedVectorIterator(int index, std::vector<int>& data_, std::vector<int>& data2_)
        : index(index),
        indices(data_),
        vals(data2_)
    {
        //_ASSERT(data_.size() == data2_.size());
    }
    

    boost::tuple<int&,int&> operator*(){
        //std::pair<int&,int&> r (&(indices[index]), &(vals[index]));  // valami ilyesmi kéne hogy cím szerint adjuk át
        //std::pair<int,int> r = std::make_pair(indices[index], vals[index]);
        boost::tuple<int&,int&> r (indices[index], vals[index]); 
        return r;
    }
    //ez lenne a jó  -- de nem tud swapolni mert nincs definiálva std::pair-ra --- át kéne írni (template specializáció)
    // Error:
    // /usr/include/c++/4.8.5/bits/stl_algobase.h(147): error: no instance of overloaded function "std::swap" matches the argument list
    //       argument types are: (std::pair<int, int>, std::pair<int, int>)
    //   swap(*__a, *__b);
    //   ^

/*
    int& operator*(){
    
    
        //-------Probalgatasok:--------
    
        //std::vector<int&> r;// (indices[index], vals[index]); 
        //std::tuple <char, int&, float> geek; 
        //int& a;
        
        //SIKERR: boost::tuple<int&,int&> r (indices[index], vals[index]); 
        
    
        
        //---------------
    
    
        return indices[index];
    }
*/
    const int& operator*() const{
        return indices[index];
    }

    PairedVectorIterator& operator++(){
        index++;
        return *this;
    }

    PairedVectorIterator& operator--(){
        index--;
        return *this;
    }

    PairedVectorIterator& operator= (const PairedVectorIterator& rhs){
        index = rhs.index;
        vals = rhs.vals;
        indices = rhs.indices;

        return *this;
    }
    
    friend int operator- (const PairedVectorIterator& lhs, const PairedVectorIterator& rhs){
        return lhs.index - rhs.index;
    }
    

    friend PairedVectorIterator operator-(PairedVectorIterator const& lhs, int rhs){
        return PairedVectorIterator(lhs.index - rhs, lhs.vals, lhs.indices);
    }

    friend PairedVectorIterator operator+(PairedVectorIterator const& lhs, int rhs){
        return PairedVectorIterator(lhs.index + rhs, lhs.vals, lhs.indices);
    }

    friend PairedVectorIterator operator+(int lhs, PairedVectorIterator const& rhs){
        return PairedVectorIterator(lhs + rhs.index, rhs.vals, rhs.indices);
    }

    friend PairedVectorIterator& operator+= (PairedVectorIterator& lhs, const int& rhs){
        lhs.index += rhs;
        return lhs;
    }

    friend PairedVectorIterator& operator-= (PairedVectorIterator& lhs, const int& rhs){
        lhs.index -= rhs;
        return lhs;
    }

    friend bool operator== (const PairedVectorIterator& lhs, const PairedVectorIterator& rhs){
        return lhs.index == rhs.index;
    }

    friend bool operator!= (const PairedVectorIterator& lhs, const PairedVectorIterator& rhs){
        return lhs.index != rhs.index;
    }

    friend bool operator<= (const PairedVectorIterator& lhs, const PairedVectorIterator& rhs){
        return lhs.index <= rhs.index;
    }

    friend bool operator>= (const PairedVectorIterator& lhs, const PairedVectorIterator& rhs){
        return lhs.index >= rhs.index;
    }

    friend bool operator< (const PairedVectorIterator& lhs, const PairedVectorIterator& rhs){
        return lhs.index < rhs.index;
    }
    
    using difference_type = int;
    using value_type = boost::tuple<int&,int&>; //int& //std::pair<int,int>;
    using pointer = int*;
    using reference = int&;
    using iterator_category = std::random_access_iterator_tag;

};



#endif //PAIRED_VECTOR_ITERATOR
