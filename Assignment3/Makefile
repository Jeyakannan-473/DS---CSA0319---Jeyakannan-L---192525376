# Build and run every program of Assignment 3 (CO3)
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic
OPT = -O2
ASAN = -std=c11 -g -fsanitize=address,undefined

all: Task01_BST/q1_bst Task01_BST/q1_experiment Task01_BST/q1_predict Task02_AVL/q2_avl_bench Task02_AVL/q2_avl_demo Task03_BTree/q3_btree_demo Task03_BTree/q3_disk Task04_RedBlack/q4_rb_bench Task04_RedBlack/q4_rb_demo Task05_Splay/q5_splay_demo Task05_Splay/q5_workload Task06_Trie/q6_growth Task06_Trie/q6_predict Task06_Trie/q6_trie_demo Task07_Design/q7_design

Task01_BST/q1_bst: Task01_BST/q1_bst.c
	$(CC) $(CFLAGS) Task01_BST/q1_bst.c -o Task01_BST/q1_bst -lm
Task01_BST/q1_experiment: Task01_BST/q1_experiment.c
	$(CC) $(CFLAGS) Task01_BST/q1_experiment.c -o Task01_BST/q1_experiment -lm
Task01_BST/q1_predict: Task01_BST/q1_predict.c
	$(CC) $(CFLAGS) Task01_BST/q1_predict.c -o Task01_BST/q1_predict -lm
Task02_AVL/q2_avl_bench: Task02_AVL/q2_avl_bench.c
	$(CC) $(CFLAGS) Task02_AVL/q2_avl_bench.c -o Task02_AVL/q2_avl_bench -lm
Task02_AVL/q2_avl_demo: Task02_AVL/q2_avl_demo.c
	$(CC) $(CFLAGS) Task02_AVL/q2_avl_demo.c -o Task02_AVL/q2_avl_demo -lm
Task03_BTree/q3_btree_demo: Task03_BTree/q3_btree_demo.c
	$(CC) $(CFLAGS) Task03_BTree/q3_btree_demo.c -o Task03_BTree/q3_btree_demo -lm
Task03_BTree/q3_disk: Task03_BTree/q3_disk.c
	$(CC) $(CFLAGS) Task03_BTree/q3_disk.c -o Task03_BTree/q3_disk -lm
Task04_RedBlack/q4_rb_bench: Task04_RedBlack/q4_rb_bench.c
	$(CC) $(CFLAGS) Task04_RedBlack/q4_rb_bench.c -o Task04_RedBlack/q4_rb_bench -lm
Task04_RedBlack/q4_rb_demo: Task04_RedBlack/q4_rb_demo.c
	$(CC) $(CFLAGS) Task04_RedBlack/q4_rb_demo.c -o Task04_RedBlack/q4_rb_demo -lm
Task05_Splay/q5_splay_demo: Task05_Splay/q5_splay_demo.c
	$(CC) $(CFLAGS) Task05_Splay/q5_splay_demo.c -o Task05_Splay/q5_splay_demo -lm
Task05_Splay/q5_workload: Task05_Splay/q5_workload.c
	$(CC) $(CFLAGS) Task05_Splay/q5_workload.c -o Task05_Splay/q5_workload -lm
Task06_Trie/q6_growth: Task06_Trie/q6_growth.c
	$(CC) $(CFLAGS) Task06_Trie/q6_growth.c -o Task06_Trie/q6_growth -lm
Task06_Trie/q6_predict: Task06_Trie/q6_predict.c
	$(CC) $(CFLAGS) Task06_Trie/q6_predict.c -o Task06_Trie/q6_predict -lm
Task06_Trie/q6_trie_demo: Task06_Trie/q6_trie_demo.c
	$(CC) $(CFLAGS) Task06_Trie/q6_trie_demo.c -o Task06_Trie/q6_trie_demo -lm
Task07_Design/q7_design: Task07_Design/q7_design.c
	$(CC) $(CFLAGS) Task07_Design/q7_design.c -o Task07_Design/q7_design -lm

timing: Task01_BST/q1_experiment_O2 Task02_AVL/q2_avl_bench_O2 Task03_BTree/q3_disk_O2 Task04_RedBlack/q4_rb_bench_O2 Task06_Trie/q6_growth_O2 Task07_Design/q7_design_O2
Task01_BST/q1_experiment_O2: Task01_BST/q1_experiment.c
	$(CC) $(CFLAGS) $(OPT) Task01_BST/q1_experiment.c -o Task01_BST/q1_experiment_O2 -lm
Task02_AVL/q2_avl_bench_O2: Task02_AVL/q2_avl_bench.c
	$(CC) $(CFLAGS) $(OPT) Task02_AVL/q2_avl_bench.c -o Task02_AVL/q2_avl_bench_O2 -lm
Task03_BTree/q3_disk_O2: Task03_BTree/q3_disk.c
	$(CC) $(CFLAGS) $(OPT) Task03_BTree/q3_disk.c -o Task03_BTree/q3_disk_O2 -lm
Task04_RedBlack/q4_rb_bench_O2: Task04_RedBlack/q4_rb_bench.c
	$(CC) $(CFLAGS) $(OPT) Task04_RedBlack/q4_rb_bench.c -o Task04_RedBlack/q4_rb_bench_O2 -lm
Task06_Trie/q6_growth_O2: Task06_Trie/q6_growth.c
	$(CC) $(CFLAGS) $(OPT) Task06_Trie/q6_growth.c -o Task06_Trie/q6_growth_O2 -lm
Task07_Design/q7_design_O2: Task07_Design/q7_design.c
	$(CC) $(CFLAGS) $(OPT) Task07_Design/q7_design.c -o Task07_Design/q7_design_O2 -lm

run: all
	cd Task01_BST && ./q1_bst > output/q1_bst.txt
	$(CC) $(CFLAGS) $(OPT) Task01_BST/q1_experiment.c -o Task01_BST/q1_experiment_O2 -lm && cd Task01_BST && ./q1_experiment_O2 > output/q1_experiment.txt
	cd Task01_BST && ./q1_predict > output/q1_predict.txt
	$(CC) $(CFLAGS) $(OPT) Task02_AVL/q2_avl_bench.c -o Task02_AVL/q2_avl_bench_O2 -lm && cd Task02_AVL && ./q2_avl_bench_O2 > output/q2_avl_bench.txt
	cd Task02_AVL && ./q2_avl_demo > output/q2_avl_demo.txt
	cd Task03_BTree && ./q3_btree_demo > output/q3_btree_demo.txt
	$(CC) $(CFLAGS) $(OPT) Task03_BTree/q3_disk.c -o Task03_BTree/q3_disk_O2 -lm && cd Task03_BTree && ./q3_disk_O2 > output/q3_disk.txt
	$(CC) $(CFLAGS) $(OPT) Task04_RedBlack/q4_rb_bench.c -o Task04_RedBlack/q4_rb_bench_O2 -lm && cd Task04_RedBlack && ./q4_rb_bench_O2 > output/q4_rb_bench.txt
	cd Task04_RedBlack && ./q4_rb_demo > output/q4_rb_demo.txt
	cd Task05_Splay && ./q5_splay_demo > output/q5_splay_demo.txt
	cd Task05_Splay && ./q5_workload > output/q5_workload.txt
	$(CC) $(CFLAGS) $(OPT) Task06_Trie/q6_growth.c -o Task06_Trie/q6_growth_O2 -lm && cd Task06_Trie && ./q6_growth_O2 > output/q6_growth.txt
	cd Task06_Trie && ./q6_predict > output/q6_predict.txt
	cd Task06_Trie && ./q6_trie_demo > output/q6_trie_demo.txt
	$(CC) $(CFLAGS) $(OPT) Task07_Design/q7_design.c -o Task07_Design/q7_design_O2 -lm && cd Task07_Design && ./q7_design_O2 > output/q7_design.txt

asan:
	$(CC) $(ASAN) Task01_BST/q1_bst.c -o Task01_BST/q1_bst_asan -lm && cd Task01_BST && ./q1_bst_asan > /dev/null
	$(CC) $(ASAN) Task01_BST/q1_experiment.c -o Task01_BST/q1_experiment_asan -lm && cd Task01_BST && ./q1_experiment_asan > /dev/null
	$(CC) $(ASAN) Task01_BST/q1_predict.c -o Task01_BST/q1_predict_asan -lm && cd Task01_BST && ./q1_predict_asan > /dev/null
	$(CC) $(ASAN) Task02_AVL/q2_avl_bench.c -o Task02_AVL/q2_avl_bench_asan -lm && cd Task02_AVL && ./q2_avl_bench_asan > /dev/null
	$(CC) $(ASAN) Task02_AVL/q2_avl_demo.c -o Task02_AVL/q2_avl_demo_asan -lm && cd Task02_AVL && ./q2_avl_demo_asan > /dev/null
	$(CC) $(ASAN) Task03_BTree/q3_btree_demo.c -o Task03_BTree/q3_btree_demo_asan -lm && cd Task03_BTree && ./q3_btree_demo_asan > /dev/null
	$(CC) $(ASAN) Task03_BTree/q3_disk.c -o Task03_BTree/q3_disk_asan -lm && cd Task03_BTree && ./q3_disk_asan > /dev/null
	$(CC) $(ASAN) Task04_RedBlack/q4_rb_bench.c -o Task04_RedBlack/q4_rb_bench_asan -lm && cd Task04_RedBlack && ./q4_rb_bench_asan > /dev/null
	$(CC) $(ASAN) Task04_RedBlack/q4_rb_demo.c -o Task04_RedBlack/q4_rb_demo_asan -lm && cd Task04_RedBlack && ./q4_rb_demo_asan > /dev/null
	$(CC) $(ASAN) Task05_Splay/q5_splay_demo.c -o Task05_Splay/q5_splay_demo_asan -lm && cd Task05_Splay && ./q5_splay_demo_asan > /dev/null
	$(CC) $(ASAN) Task05_Splay/q5_workload.c -o Task05_Splay/q5_workload_asan -lm && cd Task05_Splay && ./q5_workload_asan > /dev/null
	$(CC) $(ASAN) Task06_Trie/q6_growth.c -o Task06_Trie/q6_growth_asan -lm && cd Task06_Trie && ./q6_growth_asan > /dev/null
	$(CC) $(ASAN) Task06_Trie/q6_predict.c -o Task06_Trie/q6_predict_asan -lm && cd Task06_Trie && ./q6_predict_asan > /dev/null
	$(CC) $(ASAN) Task06_Trie/q6_trie_demo.c -o Task06_Trie/q6_trie_demo_asan -lm && cd Task06_Trie && ./q6_trie_demo_asan > /dev/null
	$(CC) $(ASAN) Task07_Design/q7_design.c -o Task07_Design/q7_design_asan -lm && cd Task07_Design && ./q7_design_asan > /dev/null

clean:
	rm -f Task01_BST/q1_bst Task01_BST/q1_bst_O2 Task01_BST/q1_bst_asan Task01_BST/q1_experiment Task01_BST/q1_experiment_O2 Task01_BST/q1_experiment_asan Task01_BST/q1_predict Task01_BST/q1_predict_O2 Task01_BST/q1_predict_asan Task02_AVL/q2_avl_bench Task02_AVL/q2_avl_bench_O2 Task02_AVL/q2_avl_bench_asan Task02_AVL/q2_avl_demo Task02_AVL/q2_avl_demo_O2 Task02_AVL/q2_avl_demo_asan Task03_BTree/q3_btree_demo Task03_BTree/q3_btree_demo_O2 Task03_BTree/q3_btree_demo_asan Task03_BTree/q3_disk Task03_BTree/q3_disk_O2 Task03_BTree/q3_disk_asan Task04_RedBlack/q4_rb_bench Task04_RedBlack/q4_rb_bench_O2 Task04_RedBlack/q4_rb_bench_asan Task04_RedBlack/q4_rb_demo Task04_RedBlack/q4_rb_demo_O2 Task04_RedBlack/q4_rb_demo_asan Task05_Splay/q5_splay_demo Task05_Splay/q5_splay_demo_O2 Task05_Splay/q5_splay_demo_asan Task05_Splay/q5_workload Task05_Splay/q5_workload_O2 Task05_Splay/q5_workload_asan Task06_Trie/q6_growth Task06_Trie/q6_growth_O2 Task06_Trie/q6_growth_asan Task06_Trie/q6_predict Task06_Trie/q6_predict_O2 Task06_Trie/q6_predict_asan Task06_Trie/q6_trie_demo Task06_Trie/q6_trie_demo_O2 Task06_Trie/q6_trie_demo_asan Task07_Design/q7_design Task07_Design/q7_design_O2 Task07_Design/q7_design_asan
