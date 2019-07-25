COMMIT_HASH=`git rev-parse --short HEAD`

make graph_viewer && mv graph_viewer graph_viewer_truerandom_kepler_commit_"$COMMIT_HASH"
make DEBUG=1 graph_viewer && mv graph_viewer graph_viewer_truerandom_debug_kepler_commit_"$COMMIT_HASH"

