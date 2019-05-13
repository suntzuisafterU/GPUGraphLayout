datasets=( "ca-AstroPh" "amazon0601" "ca-cit-HepPh" "ca-cit-HepTh" "opsahl-collaboration" "web-Google" "wiki-Talk" )

for d in ${datasets[@]}; do
  if [ ! -d ../out/"$d" ]; then
    mkdir ../out/"$d"
  fi

  (time \
  graph_viewer \
  gpu \
  500 \
  1 \
  sg \
  80.0 \
  1.0 \
  approximate \
  ../datasets/"$d"/out."$d" \
  ../out/"$d"/)  \
  > ../out/"$d"/GPUreport."$d" \
  2> ../out/"$d"/GPUtimeReport."$d"
  

  (time \
  graph_viewer \
  cpu \
  500 \
  1 \
  sg \
  80.0 \
  1.0 \
  approximate \
  ../datasets/"$d"/out."$d" \
  ../out/"$d"/)  \
  > ../out/"$d"/CPUreport."$d" \
  2> ../out/"$d"/CPUtimeReport."$d"
  

done

