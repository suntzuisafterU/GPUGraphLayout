for d in ../generated_datasets/new/*
do
  echo $d
  OUTFILE_PREFIX=$(head -n 1 $d | cut -d" " -f 3)
  echo $OUTFILE_PREFIX
  ./test_current_graph_viewer.sh -f $d -v -r 3.0 -g 1.0 --num-iters 500 --num-snaps 2 --percent-comm 55 --annotate-path "experiment" --clean --outfile-prefix "$OUTFILE_PREFIX"
  # ./test_original_graph_viewer.sh -f $d -v -r 3.0 -g 1.0 --num-iters 500 --num-snaps 2 --annotate-path "experiment" --clean --outfile-prefix "$OUTFILE_PREFIX"
done

