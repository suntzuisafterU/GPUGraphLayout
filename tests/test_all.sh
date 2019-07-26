
echo -ne "\n\nBeginning tests.  These tests only check for runtime errors.\n\n"

./test_graph_viewer_exec.sh
./test_scoda_exec.sh
./test_stress_exec.sh
