gdb -ex 'b graph_viewer_exec.cpp:158' -ex 'run' -x temp_gdb_commands.txt --args ../builds/linux/graph_viewer_exec cpu 10 1 wg 3.0 1.0 approximate regular 55 test_dataset1.edges out/test_dataset1
