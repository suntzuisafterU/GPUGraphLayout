#!/bin/bash

SCRIPT_NAME="$0"
USAGE="Usage: \n $SCRIPT_NAME [-g | --debug] [--(no-)cuda] [--clean]\nClean and then compile all objects from source plus the main executable for graph_viewer. "

DEBUG=0
CUDA_SUPPORT=1

while [[ $# -gt 0 ]]
do op="$1"
  case $op in
    -g | --debug )
      DEBUG=1
      shift
      ;;
    --cuda )
      CUDA_SUPPORT=1
      shift
      ;;
    --no-cuda )
      CUDA_SUPPORT=0
      shift
      ;;
    --clean )
      CLEAN=1
      shift
      ;;
  esac
done

if [ "$CLEAN" ];then
  make clean -C ../builds/linux
fi

make -k -C ../builds/linux CUDA_SUPPORT=$CUDA_SUPPORT DEBUG=$DEBUG graph_viewer
