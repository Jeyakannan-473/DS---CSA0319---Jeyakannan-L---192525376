#!/bin/sh
set -e
make
mkdir -p out
./bin/test_slotlist  > out/test_slotlist.txt
./bin/test_parking   > out/test_parking.txt
./bin/test_parking2  > out/test_parking2.txt
for t in task4_arrays task5_recursion task6_steps task7_cases task8_space; do
  gcc -O0 -w src/$t.c -o bin/$t && ./bin/$t > out/$t.txt; done
python3 src/bitmap.py > out/bitmap_py.txt
./bin/bench > out/bench_raw.txt
echo "all done - see out/"
