# Data Structures Assignment 3 (CO3): Optimized Data Storage and Retrieval Using Tree-Based Structures

Student: Jeyakannan L  |  SIMATS Engineering, Department of Information Technology  |  50 marks

GitHub repository: https://github.com/<your-user-name>/ds-assignment-3-co3   (replace after pushing)

## Contents

| Folder | Task | Programs |
|---|---|---|
| Task01_BST | Q1 Binary search trees | bst.h, q1_predict.c, q1_bst.c, q1_experiment.c |
| Task02_AVL | Q2 AVL trees | avl.h, bst.h, q2_avl_demo.c, q2_avl_bench.c |
| Task03_BTree | Q3 B-trees | btree.h, q3_btree_demo.c, q3_disk.c |
| Task04_RedBlack | Q4 Red-black trees | rbtree.h, q4_rb_demo.c, q4_rb_bench.c |
| Task05_Splay | Q5 Splay trees | splay.h, q5_splay_demo.c, q5_workload.c |
| Task06_Trie | Q6 Tries | trie.h, q6_predict.c, q6_trie_demo.c, q6_growth.c |
| Task07_Design | Q7 Design from first principles | trie.h, q7_design.c |

Each task folder has an `output/` directory with the captured console output of every program.

## Build and run

    make all        # compile every program: gcc -std=c11 -Wall -Wextra -pedantic (no warnings)
    make run        # run all programs and refresh the output/ folders (timing programs use -O2)
    make asan       # build with -fsanitize=address,undefined and run (no errors, no leaks)
    make clean

Requirements: gcc 13 or newer, GNU make, Linux. No input files are needed: every program generates its
own data with a fixed xorshift64 seed (88172645463325252), so counts are identical on every run.
Timings depend on the machine; re-run `make run` on your own computer and replace the timing tables.

## Notes

* Reference machine for the numbers in the report: Intel Xeon 2.30 GHz, 1 vCPU, gcc 13.3.0, Ubuntu 24.04.
* The sanitizer builds of q2_avl_bench.c, q4_rb_bench.c and q7_design.c use -DSMALL (smaller sizes, same code).
* The first sanitizer run found an overlapping strcpy in q7_design.c; it is fixed and the programs are clean.

## Suggested commits (one per task)

    git add Task01_BST && git commit -m "Task01: BST, traversals, delete cases, experiments"
    git add Task02_AVL && git commit -m "Task02: AVL rotations, validator, 100000-key comparison"
    git add Task03_BTree && git commit -m "Task03: B-tree insert, split, order-100 disk reads"
    git add Task04_RedBlack && git commit -m "Task04: red-black tree, checker, scheduler workload"
    git add Task05_Splay && git commit -m "Task05: splay tree, access lemma check, workloads"
    git add Task06_Trie && git commit -m "Task06: trie, autocomplete, growth table"
    git add Task07_Design && git commit -m "Task07: design comparison of five structures"
    git add README.md Makefile .gitignore && git commit -m "Add README and Makefile"
