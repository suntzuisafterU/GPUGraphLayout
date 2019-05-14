if [ ! -d ../out ]; then
  mkdir ../out
fi

datasets=( "ca-AstroPh" "amazon0601" "ca-cit-HepPh" "ca-cit-HepTh" "opsahl-collaboration" "web-Google" "wiki-Talk" )

for d in ${datasets[@]}; do
  GPU_PATH="$d""GPU"
  CPU_PATH="$d""CPU"
  if [ ! -d ../out/"$GPU_PATH" ]; then
    mkdir ../out/"$GPU_PATH"
  fi

  if [ ! -d ../out/"$CPU_PATH" ]; then
    mkdir ../out/"$CPU_PATH"
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
  ../out/"$GPU_PATH"/ \
  png 10000 10000)  \
  > ../out/"$GPU_PATH"/GPUreport."$d" \
  2> ../out/"$GPU_PATH"/GPUtimeReport."$d"
  
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
  ../out/"$CPU_PATH"/ \
  png 10000 10000)  \
  > ../out/"$CPU_PATH"/CPUreport."$d" \
  2> ../out/"$CPU_PATH"/CPUtimeReport."$d"
  

done

