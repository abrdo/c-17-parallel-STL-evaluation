
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
#include <string>
#include <chrono>
#include <math.h>
#include <fstream>
#include <typeinfo>
#include <omp.h>
using namespace std;
/////////// Fordítás, futtatás: ///////////////////

// ------GPU---------
/*
salloc -pgpu2 --nodelist=neumann srun --pty --preserve-env /bin/bash -l
module load gpu/cuda/11.0rc
module load nvhpc/20.9
nvc++ -I/home/shared/software/cuda/hpc_sdk/Linux_x86_64/20.9/compilers/include-stdpar         vector_copy_gpu.cpp -std=c++11 -O3 -o gpu_test -stdpar
./gpu_test
*/
      // GPU Summary:
      //nvprof --print-gpu-summary 


// ------CPU---------
// icpc vector_copy.cpp -std=c++11 -ltbb -qopenmp-simd -O3 -xHOST

// numactl --cpunodebind=0 ./a.out      
      //--> single processor (socket) (amúgy 2 van, ez zajosíthatja az eredményeket, ahogy a szálak a processzorok között vándorolnak)
// ./a.out


///////// Méretek ////////////
// 1 GB --- size: 268 435456
// 1 millio ---- 3.8 MB

///////// Idõk: /////////////////////
// 37p      -- transform_300-400_all-size 
// 24p(?)   -- copy_300-400_all-size 
// (6.5p(?) -- copy_2-101_all-size)
// 3.7 min  -- copy_1GB_500

// 38 min   -- OpenMP minden (4)
// 75 min.. -- 1proc  minden (4)
// 70 min   -- GPU


/////// Parameters //////////////
// for ALL in range
int const Avg_from = 300;
int const K_to_avg = 100;

// for ONE arraysize
int const NN = 268435456; // vectorsize // for measuring 1 arraysize ( 1 GB -- 268435456)
int const Repeats = 500;
//const int How_many_arraydataoperation = 2; // COPY->2, TRANSFORM->3

enum Oper {copy, transform, OMP_copy, OMP_transform};
Oper const COPY_method = Oper::copy;
Oper const TRANSFORM_method = Oper::transform;





//...................................brandwidth and write_to_file............................................................................
//.....................................................................................................................
vector<double> calc_brandwidth(Oper oper, vector<double> times, vector<int> range){
    if(times.size() != range.size()){ cerr<<"Error:  times.size() != range.size()  in function calc_brandwidth"; return vector<double>(1, -1);}
    
    int how_many_arraydataoperation;
    if(oper == Oper::copy || oper == Oper::OMP_copy) how_many_arraydataoperation = 2; // how_many_arraydataoperation
    else if(oper == Oper::transform || oper == Oper::OMP_transform) how_many_arraydataoperation = 3;
    else {cerr<< "Invalid Oper in calc_brandwidth"; return vector<double>(1, -1);}
    
    vector<double> brandwidths;
    for(int i = 0; i<range.size(); i++){
        brandwidths.push_back( how_many_arraydataoperation * double(((double)range[i]*4.0)/times[i])/1024/1024*1000000000/1024);
    }
    return brandwidths;
}



void write_ALL_to_file(ofstream &f, vector<int> range,    vector<double> t_seq, vector<double> bw_seq,
                                                          vector<double> t_unseq, vector<double> bw_unseq,
                                                          vector<double> t_par, vector<double> bw_par,
                                                          vector<double> t_par_unseq, vector<double> bw_par_unseq){
    //TODO: parameters
    
    
    f<<"\n................. range .................\n\n";
    f << "range = [";
    for(int i = 0; i<range.size(); i++){
      f << range[i];
      if(i!=range.size()-1) f <<", ";
    }
    f << "]\n";
    
    
    f<<"\n\n------------------------- Times -------------------------\n\n";
    f << "seq" << "_times = [";
    for(int i = 0; i<t_seq.size(); i++){
      f << t_seq[i];
      if(i != t_seq.size()-1) f <<", ";
    }
    f << "]\n\n";
    
    f << "unseq" << "_times = [";
    for(int i = 0; i<t_unseq.size(); i++){
      f << t_unseq[i];
      if(i != t_unseq.size()-1) f <<", ";
    }
    f << "]\n\n";
    
    f << "par" << "_times = [";
    for(int i = 0; i<t_par.size(); i++){
      f << t_par[i];
      if(i != t_par.size()-1) f <<", ";
    }
    f << "]\n\n";
    
    f << "par_unseq" << "_times = [";
    for(int i = 0; i<t_par_unseq.size(); i++){
      f << t_par_unseq[i];
      if(i != t_par_unseq.size()-1) f<<", ";
    }
    f << "]\n\n";
    
    f<<"\n\n----------------------- Brandwidths -----------------------\n\n";
    
    // brandwidths
    f << "seq" << "_brandwidths = [";
    for(int i = 0; i<bw_seq.size(); i++){
      f << bw_seq[i];
      if(i != bw_seq.size()-1) f <<", ";
    }
    f << "]\n\n";
    
    f << "unseq" << "_brandwidths = [";
    for(int i = 0; i<bw_unseq.size(); i++){
      f << bw_unseq[i];
      if(i != bw_unseq.size()-1) f <<", ";
    }
    f << "]\n\n";
    
    f << "par" << "_brandwidths = [";
    for(int i = 0; i<bw_par.size(); i++){
      f << bw_par[i];
      if(i != bw_par.size()-1) f <<", ";
    }
    f << "]\n\n";
    
    f << "par_unseq" << "_brandwidths = [";
    for(int i = 0; i<bw_par_unseq.size(); i++){
      f << bw_par_unseq[i];
      if(i != bw_par_unseq.size()-1) f <<", ";
    }
    f << "]\n\n";
}
//..............................................................................................................................................



////////// copy / transform for one //////////////////////
//egy vektorméretre egy adatsor
      //- elsõ mennyire más?
      //- mekkora a szórás

template<typename ExePolicy>
vector<double> measure_times_for_one_vectorsize(Oper oper, ExePolicy policy, vector<int> range, int repeats = Repeats /* = k_to_avg a kiatlagolasnal*/, int n = NN){ // n a vektorméret
    vector<double> times;
    
    vector<int>a(n,1);
    vector<int>b(n,2);
    vector<int>c(n,0);
    
    cout<<"It runs "<<repeats+1<<" itarations (Don't forget: Maybe the first measurment is not valid)"<<endl;
    int szamlalo = 0;
    for(int i = 0; i<repeats+1; i++){
        if(szamlalo%50 == 0) cout<<szamlalo<<endl;
        szamlalo++;
        
        auto t1 = std::chrono::high_resolution_clock::now();
        auto t2 = std::chrono::high_resolution_clock::now();
        switch(oper){
            case Oper::copy:
                t1 = std::chrono::high_resolution_clock::now();
                std::copy(policy, a.begin(), a.end(), b.begin());
                t2 = std::chrono::high_resolution_clock::now();
                break;
            case Oper::transform:
                t1 = std::chrono::high_resolution_clock::now();
                std::transform(policy, a.begin(), a.end(), b.begin(), c.begin(), [](double a, double b){return 3*a + b;});
                t2 = std::chrono::high_resolution_clock::now();
                break;
            case Oper::OMP_copy:
                t1 = std::chrono::high_resolution_clock::now();
                #pragma omp parallel for
                for(int k = 0; k<a.size(); k++) b[k] = a[k];
                t2 = std::chrono::high_resolution_clock::now();
                break;
            case Oper::OMP_transform:
                t1 = std::chrono::high_resolution_clock::now();
                #pragma omp parallel for
                for(int k = 0; k<a.size(); k++) c[k] = 3*a[k]+b[k];
                t2 = std::chrono::high_resolution_clock::now();
                break;
            default:
                cerr<<oper<<"is invalid in 'measure_times_for_one_vectorsize' function\n";
                return vector<double>(1, -1);
        }
        
        
        double curr_time = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count(); // ez int lesz, mert a duration_cast<>(t2-t1).count() intet ad vissza.
        times.push_back(curr_time);
    
    }
  
  return times;
}




// COPY AND TRANSFORM -- (c = 3*a + b) +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
template<typename ExePolicy>
vector<double> measure_time_for_operation(Oper oper, ExePolicy policy, vector<int> range, int k_to_avg = K_to_avg, int avg_from = Avg_from){
    vector<double>times;
    
    int szamlalo = 0;
    for(int n : range){ //n is the size of the vector, what we copy
        cout<<szamlalo<<endl;
        szamlalo++;
        
        vector<int>a(n,1);
        vector<int>b(n,2);
        vector<int>c(n,0);
        
        std::transform(policy, a.begin(), a.end(), b.begin(), c.begin(), [](double a, double b){return 3*a + b;});
        double sum_time = 0;
        for(int i = 0; i<avg_from+k_to_avg; i++){
        
            auto t1 = std::chrono::high_resolution_clock::now();
            auto t2 = std::chrono::high_resolution_clock::now();
            switch(oper){
                case Oper::copy:
                    t1 = std::chrono::high_resolution_clock::now();
                    std::copy(policy, a.begin(), a.end(), b.begin());
                    t2 = std::chrono::high_resolution_clock::now();
                    break;
                case Oper::transform:
                    t1 = std::chrono::high_resolution_clock::now();
                    std::transform(policy, a.begin(), a.end(), b.begin(), c.begin(), [](double a, double b){return 3*a + b;});
                    t2 = std::chrono::high_resolution_clock::now();
                    break;
                case Oper::OMP_copy:
                    t1 = std::chrono::high_resolution_clock::now();
                    #pragma omp parallel for
                    for(int k = 0; k<a.size(); k++) b[k] = a[k];
                    t2 = std::chrono::high_resolution_clock::now();
                    break;
                case Oper::OMP_transform:
                    t1 = std::chrono::high_resolution_clock::now();
                    #pragma omp parallel for
                    for(int k = 0; k<a.size(); k++) c[k] = 3*a[k]+b[k];
                    t2 = std::chrono::high_resolution_clock::now();
                    break;
                default:
                    cerr<<oper<<"is invalid in 'measure_time_for_operation' function\n";
                    return vector<double>(1, -1);
            }
            
            if(avg_from<=i){
              double curr_time = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count();
              sum_time += curr_time;
            }
        }
        
        times.push_back(sum_time/k_to_avg);
    }
    return times;
}

//===========================================================  m a i n  ====================================================================================
int main(int argc, char** argv){


    // logarithmic range generation
    vector<int> rangeA;
    for(int n = 0; n<31; n++){
        int N = 1<<n;
        rangeA.push_back(N);
    }
    

    vector<int> range1 = vector<int>(Repeats+1, NN);
    
    //__________________________________________________________________________________________________________________
    auto T1 = chrono::high_resolution_clock::now();
    
    /////////////////////////////////////////////////////////////  C O P Y  ///////////////////////////////////////////////////////////////////////////////////////////
    Oper op = COPY_method;
    vector<int> range = range1;
    
    cout<<"seq"<<endl;
    vector<double> SEQ_times = measure_times_for_one_vectorsize(op, execution::seq, range); // in nanoseconds
    vector<double> SEQ_brandwidths = calc_brandwidth(op, SEQ_times, range);
    
    cout<<"unseq"<<endl;
    vector<double> UNSEQ_times = measure_times_for_one_vectorsize(op, execution::unseq, range); // in nanoseconds
    vector<double> UNSEQ_brandwidths = calc_brandwidth(op, UNSEQ_times, range);
    
    cout<<"par"<<endl;
    vector<double> PAR_times = measure_times_for_one_vectorsize(op, execution::par, range); // in nanoseconds
    vector<double> PAR_brandwidths = calc_brandwidth(op, PAR_times, range);
    
    cout<<"par_unseq"<<endl;
    vector<double> PAR_UNSEQ_times = measure_times_for_one_vectorsize(op, execution::par_unseq, range); // in nanoseconds
    vector<double> PAR_UNSEQ_brandwidths = calc_brandwidth(op, PAR_UNSEQ_times, range);
    
    
    // to file
    ofstream data1("data1.txt");
    write_ALL_to_file(data1, range,    SEQ_times, SEQ_brandwidths,
                                      UNSEQ_times, UNSEQ_brandwidths,
                                      PAR_times, PAR_brandwidths,
                                      PAR_UNSEQ_times, PAR_UNSEQ_brandwidths);
    data1.close();
    
    
    // COPY_all .......................................................
    range = rangeA;
    
    cout<<"seq"<<endl;
    SEQ_times = measure_time_for_operation(op, execution::seq, range); // in nanoseconds
    SEQ_brandwidths = calc_brandwidth(op, SEQ_times, range);
    
    cout<<"unseq"<<endl;
    UNSEQ_times = measure_time_for_operation(op, execution::unseq, range); // in nanoseconds
    UNSEQ_brandwidths = calc_brandwidth(op, UNSEQ_times, range);
    
    cout<<"par"<<endl;
    PAR_times = measure_time_for_operation(op, execution::par, range); // in nanoseconds
    PAR_brandwidths = calc_brandwidth(op, PAR_times, range);
    
    cout<<"par_unseq"<<endl;
    PAR_UNSEQ_times = measure_time_for_operation(op, execution::par_unseq, range); // in nanoseconds
    PAR_UNSEQ_brandwidths = calc_brandwidth(op, PAR_UNSEQ_times, range);
    
    
    
    // to file
    ofstream data2("data2.txt");
    write_ALL_to_file(data2, range,    SEQ_times, SEQ_brandwidths,
                                      UNSEQ_times, UNSEQ_brandwidths,
                                      PAR_times, PAR_brandwidths,
                                      PAR_UNSEQ_times, PAR_UNSEQ_brandwidths);
    data2.close();
    
    ////////////////////////////////////////////////////////////  T R A N S F R O R M  ////////////////////////////////////////////////////////////////////////////
    op = TRANSFORM_method;
    range = range1;
    
    
    cout<<"seq"<<endl;
    SEQ_times = measure_times_for_one_vectorsize(op, execution::seq, range); // in nanoseconds
    SEQ_brandwidths = calc_brandwidth(op, SEQ_times, range);
    
    cout<<"unseq"<<endl;
    UNSEQ_times = measure_times_for_one_vectorsize(op, execution::unseq, range); // in nanoseconds
    UNSEQ_brandwidths = calc_brandwidth(op, UNSEQ_times, range);
    
    cout<<"par"<<endl;
    PAR_times = measure_times_for_one_vectorsize(op, execution::par, range); // in nanoseconds
    PAR_brandwidths = calc_brandwidth(op, PAR_times, range);
    
    cout<<"par_unseq"<<endl;
    PAR_UNSEQ_times = measure_times_for_one_vectorsize(op, execution::par_unseq, range); // in nanoseconds
    PAR_UNSEQ_brandwidths = calc_brandwidth(op, PAR_UNSEQ_times, range);
    
    
    // to file
    ofstream data12("data12.txt");
    write_ALL_to_file(data12, range,    SEQ_times, SEQ_brandwidths,
                                      UNSEQ_times, UNSEQ_brandwidths,
                                      PAR_times, PAR_brandwidths,
                                      PAR_UNSEQ_times, PAR_UNSEQ_brandwidths);
    data12.close();
    
    
    range = rangeA;
    
    // TRANSFORM_all ...............................................
    cout<<"seq"<<endl;
    SEQ_times = measure_time_for_operation(op, execution::seq, range); // in nanoseconds
    SEQ_brandwidths = calc_brandwidth(op, SEQ_times, range);
    
    cout<<"unseq"<<endl;
    UNSEQ_times = measure_time_for_operation(op, execution::unseq, range); // in nanoseconds
    UNSEQ_brandwidths = calc_brandwidth(op, UNSEQ_times, range);
    
    cout<<"par"<<endl;
    PAR_times = measure_time_for_operation(op, execution::par, range); // in nanoseconds
    PAR_brandwidths = calc_brandwidth(op, PAR_times, range);
    
    cout<<"par_unseq"<<endl;
    PAR_UNSEQ_times = measure_time_for_operation(op, execution::par_unseq, range); // in nanoseconds
    PAR_UNSEQ_brandwidths = calc_brandwidth(op, PAR_UNSEQ_times, range);
    
    
    
    // to file
    ofstream data22("data22.txt");
    write_ALL_to_file(data22, range,    SEQ_times, SEQ_brandwidths,
                                      UNSEQ_times, UNSEQ_brandwidths,
                                      PAR_times, PAR_brandwidths,
                                      PAR_UNSEQ_times, PAR_UNSEQ_brandwidths);
    data22.close();
    
    
    
    
    auto T2 = chrono::high_resolution_clock::now();
    double duration = std::chrono::duration_cast<std::chrono::seconds>(T2-T1).count();
    cout<<"It took "<<duration/60.0<<" min\n";
    
    
    /*
    // to file
    ofstream data("data.txt");
    write_ALL_to_file(data, range,    SEQ_times, SEQ_brandwidths,
                                      UNSEQ_times, UNSEQ_brandwidths,
                                      PAR_times, PAR_brandwidths,
                                      PAR_UNSEQ_times, PAR_UNSEQ_brandwidths);
    data.close();
    */
    
    
    return 0;
}
