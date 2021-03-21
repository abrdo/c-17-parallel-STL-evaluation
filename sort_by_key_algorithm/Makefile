sort_cpu:
	icpc sortByLocations.cpp -std=c++17 -O3 -ltbb -qopenmp -simd -xHOST -o sort_cpu
sort_gpu:
	nvc++ -I/home/shared/software/cuda/hpc_sdk/Linux_x86_64/20.9/compilers/include-stdpar sort_by_key_MAIN.cpp -std=c++11 -O3 -o sort_gpu -stdpar -DGPU
