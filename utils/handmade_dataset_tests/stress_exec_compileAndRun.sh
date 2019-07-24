LAYOUTFILE="ChangeMe"
cd ../../builds/linux && make clean && make DEBUG=1 stress_exec;
cd ../../utils/handmade_dataset_tests/
../../builds/linux/stress_exec test_dataset1.edges $LAYOUTFILE

