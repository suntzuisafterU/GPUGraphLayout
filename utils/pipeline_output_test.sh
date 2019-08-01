REASON=FIRST_N_ITERATIONS
# IN=../datasets/ca-AstroPh/out.ca-AstroPh
IN=../datasets/web-Google/out.web-Google
OUT=../out/pipeline_output_tests/$REASON/
mkdir -p "$OUT"
echo -ne '\n\n\tMAKEING GRAPH VIEWER EXEC\n\n'
./make_gv.sh --clean --gv
./execute_graph_viewer.sh --outfile-prefix "$REASON" --execution-mode gpu --append -f "$IN"  -o "$OUT" -v -r 3.0 -g 1.0 --num-iters 500 --num-snaps 1 --percent-comm 55 --annotate-path "pipeline_output_test" --pixels 10000 10000
