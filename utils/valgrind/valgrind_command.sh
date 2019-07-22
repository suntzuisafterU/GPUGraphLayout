# Try this without leak-kinds=all and check=full
valgrind -s --track-origins=yes ../builds/linux/graph_viewer_exec cpu 6 2 wg 2.0 1.0 approximate ../datasets/ca-AstroPh/out.ca-AstroPh ../out/TEST_ca-AstroPh
