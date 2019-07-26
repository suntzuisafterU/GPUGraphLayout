LAYOUTFILE=test_dataset1_layouts_origGV/100.csv
make -C ../../builds/linux DEBUG=1 clean stress_exec
../../builds/linux/stress_exec test_dataset1.edges $LAYOUTFILE

