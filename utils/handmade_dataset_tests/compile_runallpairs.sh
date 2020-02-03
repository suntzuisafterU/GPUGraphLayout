cd ../../builds/linux && make clean && make DEBUG=1 all_pairs_exec;
cd ../../utils/handmade_dataset_tests/
../../builds/linux/all_pairs_exec test_dataset1.edges

