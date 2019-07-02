for d in ../generated_datasets/new/*
do
  echo $d
  ./test_original_graph_viewer.sh -f $d -v -r 3.0 -g 1.0 --num-iters 500 --num-snaps 2 --annotate-path "experiment" --clean
  ./test_current_graph_viewer.sh -f $d -v -r 3.0 -g 1.0 --num-iters 500 --num-snaps 2 --percent-comm 55 --annotate-path "experiment" --clean
done

