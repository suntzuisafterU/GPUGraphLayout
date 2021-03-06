
TIMESTAMP=$(date +"%m%d%H%M")
echo "Timestamp: $TIMESTAMP"

for d in datasets/*/out.*
do 
  # Accept dataset or point at directory.
  EDGELIST=$d
  BASENAME=$(basename -- "$EDGELIST")
  OUT_ORIGINAL=out/$BASENAME/original
  OUT_SCODA=out/$BASENAME/scoda

  # # clean up any old output.
  # echo "Removing any old output with: "
  # echo "rm -rf out/$BASENAME"
  # rm -rf out/$BASENAME

  PERCENT_COMM=90

  echo "Creating $OUT_ORIGINAL and $OUT_SCODA if they don't already exist"
  mkdir -p $OUT_ORIGINAL"_percent_comm_"$PERCENT_COMM
  mkdir -p $OUT_SCODA"_percent_comm_"$PERCENT_COMM

  # # write reports to reports directory. For now clean out first.
  # echo "Removing old reports with: "
  # echo "rm -rf reports"
  # rm -rf reports/$BASENAME
  echo "Creating reports directory..."
  mkdir -p reports/$BASENAME

  # produce layouts for our use.
  # Execute with original mode.
  EXPERIMENT_TAG="_STRESS_EXPERIMENT_"
  # Usage: graph_viewer original|single|stacked gpu|cpu max_iterations num_snaps sg|wg scale gravity exact|approximate linlog|regular [percentage_iterations_on_comm_graph] edgelist_path out_path out_file_prefix [png image_w image_h|csv|bin]

  ./graph_viewer_exec "original" gpu 500 1 wg 5.0 1.0 approximate regular $PERCENT_COMM $EDGELIST $OUT_ORIGINAL "$EXPERIMENT_TAG" csv | tee reports/"$BASENAME"/gv_original_output_"$TIMESTAMP".txt 
  ./graph_viewer_exec "single" gpu 500 1 wg 5.0 1.0 approximate regular $PERCENT_COM $EDGELIST $OUT_SCODA "$EXPERIMENT_TAG" csv | tee reports/"$BASENAME"/gv_scoda_pipeline_output_"$TIMESTAMP".txt

  # Usage: stress_exec [edge list path] [first generated layout path] [second generated layout path]
  LAYOUT1=$OUT_ORIGINAL/*"$EXPERIMENT_TAG"* 
  LAYOUT2=$OUT_SCODA/*FULL_LAYOUT*"$EXPERIMENT_TAG"*
  echo "./stress_rand_sampling $EDGELIST $LAYOUT1 $LAYOUT2 | tee reports/${BASENAME}/stress_report_${TIMESTAMP}.txt" # TODO: Does this line work correctly? Not to important.
  ./stress_rand_sampling $EDGELIST $LAYOUT1 $LAYOUT2 | tee reports/"$BASENAME"/stress_report_"$TIMESTAMP".txt

done

