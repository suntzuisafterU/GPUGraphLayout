gdb -ex 'b graph_viewer_exec.cpp:158' -ex 'run' -x temp_gdb_commands.txt --args ../builds/linux/graph_viewer_exec cpu 10 10 sg 40.0 1.0 approximate regular 55 KEEP_datagen.edges out/KEEP_datagen
