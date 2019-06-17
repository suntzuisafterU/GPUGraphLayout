#!/bin/bash

SCRIPT_NAME="$0"
USAGE="Usage: \n $SCRIPT_NAME"

DEBUG=0
CUDA_SUPPORT=0

while [[ $# -gt 0 ]]
do op="$1"
  case $op in
    -g )
      DEBUG=1
      shift
      ;;
    --cuda )
      CUDA_SUPPORT=1
      shift
      ;;
  esac
done

make clean -C ../builds/linux && make -C ../builds/linux CUDA_SUPPORT=$CUDA_SUPPORT DEBUG=$DEBUG graph_viewer
