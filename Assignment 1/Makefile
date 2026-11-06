CC=gcc
CF=-std=c11 -O2 -Wall -Wextra
all: bin/test_slotlist bin/test_parking bin/test_parking2 bin/bench
bin: ; mkdir -p bin
bin/test_slotlist: src/test_slotlist.c src/slotlist.c | bin ; $(CC) $(CF) $^ -o $@
bin/test_parking:  src/test_parking.c src/parking.c src/hashmap.c | bin ; $(CC) $(CF) $^ -o $@
bin/test_parking2: src/test_parking2.c src/parking2.c src/hashmap.c | bin ; $(CC) $(CF) $^ -o $@
bin/bench: src/bench.c src/parking.c src/parking2.c src/hashmap.c src/slotlist.c | bin ; $(CC) $(CF) $^ -o $@
clean: ; rm -rf bin
