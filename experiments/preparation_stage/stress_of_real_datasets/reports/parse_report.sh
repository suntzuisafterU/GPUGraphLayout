grep '# SCoDA' -A 10 gv_scoda_pipeline_output.txt | tail -n 11 | tee final_report.txt; grep 'lower' -B 33 stress_report.txt | tee -a final_report.txt
