eval_cpu:
	icpc eval_with_diff_arraysizes.cpp -std=c++17 -O3 -ltbb -qopenmp-simd -xHOST -o eval_cpu
eval_gpu:
	nvc++ -I/home/shared/software/cuda/hpc_sdk/Linux_x86_64/20.9/compilers/include-stdpar eval_with_diff_arraysizes.cpp -std=c++11 -O3  -stdpar -DGPU -o eval_gpu
