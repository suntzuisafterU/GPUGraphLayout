
# Accept dataset or point at directory.
EDGELIST=datasets/ca-AstroPh/out.ca-AstroPh 
BASENAME=$(basename -- "$EDGELIST")
OUT_ORIGINAL=out/$BASENAME/original
OUT_SCODA=out/$BASENAME/scoda

# clean up any old output.
echo "Removing any old output with: "
echo "rm -rf out/$BASENAME"
rm -rf out/$BASENAME

echo "Creating $OUT_ORIGINAL and $OUT_SCODA if they don't already exist"
mkdir -p $OUT_ORIGINAL
mkdir -p $OUT_SCODA

# write reports to reports directory. For now clean out first.
echo "Removing old reports with: "
echo "rm -rf reports"
rm -rf reports/$BASENAME
echo "Creating reports directory..." && mkdir -p reports/$BASENAME

# produce layouts for our use.
# Execute with original mode.
EXPERIMENT_TAG="_STRESS_EXPERIMENT_"
# Usage: graph_viewer original|single|stacked gpu|cpu max_iterations num_snaps sg|wg scale gravity exact|approximate linlog|regular [percentage_iterations_on_comm_graph] edgelist_path out_path out_file_prefix [png image_w image_h|csv|bin]

./graph_viewer_exec "original" gpu 500 1 wg 5.0 1.0 approximate regular 55 $EDGELIST $OUT_ORIGINAL "$EXPERIMENT_TAG" csv | tee reports/"$BASENAME"/gv_original_output.txt 
./graph_viewer_exec "single" gpu 500 1 wg 5.0 1.0 approximate regular 55 $EDGELIST $OUT_SCODA "$EXPERIMENT_TAG" csv | tee reports/"$BASENAME"/gv_scoda_pipeline_output.txt

# Usage: stress_exec [edge list path] [first generated layout path] [second generated layout path]
LAYOUT1=$OUT_ORIGINAL/*"$EXPERIMENT_TAG"* 
LAYOUT2=$OUT_SCODA/*FULL_LAYOUT*"$EXPERIMENT_TAG"*
echo "./stress_rand_sampling $EDGELIST $LAYOUT1 $LAYOUT2 | tee reports/"$BASENAME"/stress_report.txt"
./stress_rand_sampling $EDGELIST $LAYOUT1 $LAYOUT2 | tee reports/"$BASENAME"/stress_report.txt


