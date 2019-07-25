COMMIT_HASH=`git rev-parse --short HEAD`

make graph_viewer && mv graph_viewer graph_viewer_seed1234_kepler_commit_"$COMMIT_HASH"
make DEBUG=1 graph_viewer && mv graph_viewer graph_viewer_seed1234_debug_kepler_commit_"$COMMIT_HASH"

