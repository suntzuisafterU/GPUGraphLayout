# NOTE: This is only valid if all of the datasets where generated from the EXACT same UGraph.  This 
#       should be the case unless you have changed the code in RPGraph.cpp for the UGraph class that
#       reads in the edge list file.  If you change that code and want to test this, regenerate the
#       data.
../stress_rand_sampling test_dataset1.edges out/test_dataset1.edges_gpu_regular_F_R-5.0_F_G-2.0_iters_500_commPercentage_55/OLD_STYLE_GV_LOOP_INITIAL_LAYOUTdefault_outfile_prefix0001.csv out/test_dataset1.edges_gpu_regular_F_R-5.0_F_G-2.0_iters_500_commPercentage_55/OLD_STYLE_GV_LOOPdefault_outfile_prefix0500.csv
