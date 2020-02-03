REASON="EDGES_0_OPACITY"
COMMIT_HASH=`git rev-parse --short HEAD`

make clean && make graph_viewer_exec && mv graph_viewer_exec graph_viewer_exec_"$REASON"_kepler_commit_"$COMMIT_HASH"
make DEBUG=1 graph_viewer_exec && mv graph_viewer_exec graph_viewer_exec_"$REASON"_debug_kepler_commit_"$COMMIT_HASH"

