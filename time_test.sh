start_time=$(date +%s.%3N)

for i in {1..100}
do
   ./elffile
done

end_time=$(date +%s.%3N)

# elapsed time with second resolution
elapsed=$(echo "scale=3; $end_time - $start_time" | bc)
echo "Elapsed time (ms):"
echo $elapsed

start_time=$(date +%s.%3N)

for i in {1..100}
do
   ./cpu
done

end_time=$(date +%s.%3N)

# elapsed time with second resolution
elapsed=$(echo "scale=3; $end_time - $start_time" | bc)
echo "Elapsed time (ms):"
echo $elapsed
