#!/bin/bash

SCRIPT_NAME="$0"
USAGE=$(cat << END_OF_MESSAGE 
Usage: 
$SCRIPT_NAME -f <in file path> -o <out file path> [other options to override defaults]]
    --default ) # Just run with all default args.
    -h | --help )
    -f ) INPATH
    -o ) OUTPATH_PREFIX # Will create a sub directory.
    --execution-mode ) [gpu|cpu]
    --gravity-type ) [wg|sg]
    -g | --gravity-strength )
    -r | --repulsion-strength )
    --num-iters )
    --num-snaps )
    --linlog )
    --percent-comm )
    --outfile-prefix ) # for book keeping.
    --pixels ) [width height]
    --output-format ) only png maters.
    --annotate-path ) # more book keeping.
    --debug ) # run with gdb --args 
    --clean ) # cleans output dir if not empty.
    --append ) # appends to output dir if not empty.
    -v ) # verbose
END_OF_MESSAGE
)

if [[ $# -eq 0 ]]; then
  echo -e "$USAGE"
  exit 1
fi


# defaults
EXECUTION_MODE="gpu"
INPATH='../datasets/ca-AstroPh/out.ca-AstroPh'
OUTPATH_PREFIX='../out/'
GRAVITY='wg'
F_G=2.0
F_R=5.0
NUM_ITERATIONS=500
NUM_SNAPS=10
LINLOG="regular"
PERCENT_ITERS_ON_COMM=55
OUTFILE_PREFIX="default_outfile_prefix"
HEIGHT=10000
WIDTH=10000
OUTPUT_FORMAT="png"

while [[ $# -gt 0 ]]
do op="$1"
  case $op in
    -h | --help )
      echo -e "$USAGE"
      exit 0
      ;;
    -f )
      INPATH="$2"
      shift # past -f
      shift # past value
      ;;
    -o )
      OUTPATH_PREFIX="$2"
      shift
      shift
      ;;
    --execution-mode )
      EXECUTION_MODE="$2"
      shift
      shift
      ;;
    --gravity-type )
      GRAVITY="$2"
      shift
      shift
      ;;
    -g | --gravity-strength )
      F_G="$2"
      shift
      shift
      ;;
    -r | --repulsion-strength )
      F_R="$2"
      shift
      shift
      ;;
    --num-iters )
      NUM_ITERATIONS="$2"
      shift
      shift
      ;;
    --num-snaps )
      NUM_SNAPS="$2"
      shift
      shift
      ;;
    --linlog )
      LINLOG="linlog"
      shift
      ;;
    --percent-comm )
      PERCENT_ITERS_ON_COMM="$2"
      shift
      shift
      ;;
    --outfile-prefix )
      OUTFILE_PREFIX="$2"
      shift
      shift
      ;;
    --pixels )
      HEIGHT="$2"
      WIDTH="$3"
      shift
      shift
      shift
      ;;
    --output-format )
      OUTPUT_FORMAT="$2"
      shift
      shift
      ;;
    --annotate-path )
      ANNOTATION="$2"
      shift
      shift
      ;;
    --clean )
      CLEAN="TRUE"
      shift
      ;;
    --append )
      APPEND="TRUE"
      shift
      ;;
    --debug )
      DEBUG="TRUE"
      shift
      ;;
    -v )
      VERBOSE="TRUE"
      shift
      ;;
    --default ) # do nothing (don't die)
      shift
      ;;
    * )
      echo "Unknown option: $1"
      exit 1
  esac
done

# Validate command line args
if [ "$INPATH" = "" ] || [ "$OUTPATH_PREFIX" = "" ]; then
  echo -e "$USAGE"
  exit 1
fi

# source for next 2 lines: https://stackoverflow.com/questions/965053/extract-filename-and-extension-in-bash
BASENAME=$(basename -- "$INPATH") # Extract base file name here.
# EXT="${BASENAME##*.}"# Extract extension.
echo "BASENAME: $BASENAME"

# automatically name outpath
OUTPATH="$OUTPATH_PREFIX""$BASENAME""_""$EXECUTION_MODE""_""$LINLOG""_F_R-$F_R""_F_G-$F_G""_iters_""$NUM_ITERATIONS""_commPercentage_""$PERCENT_ITERS_ON_COMM"

# Annotate path with purpose for output
if [ "$ANNOTATION" ]; then
  OUTPATH+="_""$ANNOTATION"
fi

# Finish OUTPATH so graph_viewer is happy.
OUTPATH+="/"

# Make output directory, exit (or clean) if output directory exists.
echo "Creating path to $OUTPATH if it does not already exist"
mkdir -p $OUTPATH

# Check if directory is empty.  Clean it or exit.
if [ "$(ls -A $OUTPATH)" ]; then
  echo -ne "$OUTPATH is not empty. \nHandling...\n"
  if [ "$APPEND" ]; then
    echo "Appending to directory. May overwrite some files"
  elif [ "$CLEAN" ]; then
    echo "Cleaning"
    echo "rm -f $OUTPATH*"
    rm -f "$OUTPATH*" 
  else
    echo "Output directory is not empty and --append or --clean was not specified"
    exit 1
  fi
fi

# TODO: TEMPORARY to see if we can use multiple devices on our kepler machines that have 3 discrete GPUs
# export CUDA_VISIBLE_DEVICES=0,1,2 # doesn't seem to do anything

if [ "$DEBUG" ]; then
  echo "gdb --args ../builds/linux/graph_viewer_exec $EXECUTION_MODE $NUM_ITERATIONS $NUM_SNAPS $GRAVITY $F_R $F_G approximate $LINLOG $PERCENT_ITERS_ON_COMM $INPATH $OUTPATH $OUTFILE_PREFIX $OUTPUT_FORMAT $HEIGHT $WIDTH"

  gdb --args ../builds/linux/graph_viewer_exec "$EXECUTION_MODE" "$NUM_ITERATIONS" "$NUM_SNAPS" "$GRAVITY" "$F_R" "$F_G" approximate "$LINLOG" "$PERCENT_ITERS_ON_COMM" "$INPATH" "$OUTPATH" "$OUTFILE_PREFIX" "$OUTPUT_FORMAT" "$HEIGHT" "$WIDTH"
else
  echo "../builds/linux/graph_viewer_exec $EXECUTION_MODE $NUM_ITERATIONS $NUM_SNAPS $GRAVITY $F_R $F_G approximate $LINLOG $PERCENT_ITERS_ON_COMM $INPATH $OUTPATH $OUTFILE_PREFIX $OUTPUT_FORMAT $HEIGHT $WIDTH"

  ../builds/linux/graph_viewer_exec "$EXECUTION_MODE" "$NUM_ITERATIONS" "$NUM_SNAPS" "$GRAVITY" "$F_R" "$F_G" approximate "$LINLOG" "$PERCENT_ITERS_ON_COMM" "$INPATH" "$OUTPATH" "$OUTFILE_PREFIX" "$OUTPUT_FORMAT" "$HEIGHT" "$WIDTH"
fi

