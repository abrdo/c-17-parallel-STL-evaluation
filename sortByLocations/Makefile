sort_cpu:
	icpc ./apps/main.cpp -std=c++17 -O3 -ltbb -qopenmp -simd -xHOST -o sort_cpu
sort_gpu:
	nvc++ -I/home/shared/software/cuda/hpc_sdk/Linux_x86_64/20.9/compilers/include-stdpar ./apps/main.cpp -std=c++11 -O3 -o sort_gpu -stdpar -DGPU
dsort_cpu:
	icpc ./apps/main.cpp -std=c++17 -O0 -ltbb -qopenmp -simd -xHOST -g -o sort_cpu
dsort_gpu:
	nvc++ -I/home/shared/software/cuda/hpc_sdk/Linux_x86_64/20.9/compilers/include-stdpar ./apps/main.cpp -std=c++11 -O0 -g -G -o sort_gpu -stdpar -DGPU
 