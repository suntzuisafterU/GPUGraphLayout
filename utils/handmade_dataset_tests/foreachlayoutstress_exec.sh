EDGELIST_FILE=$1
LAYOUT_DIR=$2

echo "$EDGELIST_FILE"
echo "$LAYOUT_DIR"

for layout in $LAYOUT_DIR/*.csv; do
  echo -ne "\n\nExecuting stress on $layout\n"
  ../../builds/linux/stress_exec "$EDGELIST_FILE" "$layout"
done
