CUDA_SUPPORT=0
command -v nvcc >/dev/null && CUDA_SUPPORT=1
git pull && cd ../builds/linux && make clean && make DEBUG=1 CUDA_SUPPORT=$CUDA_SUPPORT graph_viewer_exec;
cd ../../tests
