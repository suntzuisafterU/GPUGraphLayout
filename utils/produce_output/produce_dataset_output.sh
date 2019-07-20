for d in ../datasets/*/out.*
do
  echo $d
  OUTFILE_PREFIX=$(basename -- $d)
  echo "OUTFILE_PREFIX: $OUTFILE_PREFIX"
  OUTPATH_ANNOTATION="COMM_AND_FULL_OUTPUT"
  echo "OUTPATH_ANNOTATION: $OUTPATH_ANNOTATION"
  ./test_current_graph_viewer.sh -f $d -v -r 3.0 -g 1.0 --num-iters 5000 --num-snaps 2 --percent-comm 55 --annotate-path "$OUTPATH_ANNOTATION" --clean --outfile-prefix "$OUTFILE_PREFIX"
done


