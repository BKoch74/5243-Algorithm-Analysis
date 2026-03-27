# Data Structure Comparison Report

## Overview

This report analyzes the performance of four data structures across four distinct workload types at four problem sizes (1000, 5000, 10000, and 20000 operations). The structures compared are:

- **BST** — Binary Search Tree (unbalanced)
- **Hash Table** — Chained hash table with prime capacity resizing
- **Linked List** — Unsorted singly linked list
- **Sorted Array** — Sorted dynamic array using binary search

The metrics tracked are comparisons, inserts, deletes, lookups, resize events, and structural operations.

---

## Workload Definitions

| Workload | Pattern | Operation Mix |
|---|---|---|
| A | Random inserts, then random lookups | 50% insert, 50% contains |
| B | Sorted inserts, then random lookups | 50% insert (sorted), 50% contains |
| C | Mixed random operations | 25% insert, 50% contains, 25% delete |
| D | Insert n items, then 5n lookups | Heavy read workload |

---

## Workload A — Random Inserts + Lookups

### Comparisons

| Structure | 1000 | 5000 | 10000 | 20000 |
|---|---|---|---|---|
| BST | 21,706 | 141,354 | 309,871 | 675,408 |
| Hash Table | 1,771 | 9,320 | 18,597 | 37,111 |
| Linked List | 955,472 | 23,783,856 | 95,357,059 | 381,615,459 |
| Sorted Array | 20,490 | 125,751 | 271,503 | 582,981 |

### Structural Operations

| Structure | 1000 | 5000 | 10000 | 20000 |
|---|---|---|---|---|
| BST | 958 | 4,754 | 9,533 | 19,084 |
| Hash Table | 2,144 | 9,541 | 19,136 | 38,327 |
| Linked List | 958 | 4,754 | 9,533 | 19,084 |
| Sorted Array | 226,754 | 5,663,611 | 22,797,648 | 91,425,339 |

### Resize Events

| Structure | 1000 | 5000 | 10000 | 20000 |
|---|---|---|---|---|
| Hash Table | 8 | 12 | 11 | 12 |
| Sorted Array | 7 | 9 | 10 | 11 |

### Analysis

The hash table dominates Workload A with the fewest comparisons by a wide margin, staying under 40,000 even at n=20000. This reflects its O(1) average-case lookup behavior in which the hash function sends each query directly to the right bucket with very little scanning needed.

The BST and sorted array perform similarly in comparisons, both benefiting from O(log n) search behavior. The BST traverses a tree of depth ~log₂(n) per operation while the sorted array uses binary search.

The linked list is the clear underperformer, with no ordering forcing a full linear scan for every contains call. At n=20000 it performs over 381 million comparisons compared to the hash table's 37,000.

The sorted array's structural operations are dramatically higher than every other structure due to element shifting on every insert. At n=20000 it performs over 91 million structural ops compared to the hash table's 38,000.

[[Workload A Graph]((https://github.com/BKoch74/5243-Algorithm-Analysis/blob/main/Assignments/04-P02/Results_Resources/workload_a_comparisons.html)](https://github.com/BKoch74/5243-Algorithm-Analysis/blob/main/Assignments/04-P02/Results_Resources/workload_a_comparisons.html)

---

## Workload B — Sorted Inserts + Lookups

### Comparisons

| Structure | 1000 | 5000 | 10000 | 20000 |
|---|---|---|---|---|
| BST | 962,786 | 23,806,546 | 95,270,425 | 381,426,548 |
| Hash Table | 1,758 | 9,463 | 18,798 | 37,402 |
| Linked List | 935,573 | 23,167,477 | 93,134,766 | 373,068,086 |
| Sorted Array | 19,906 | 122,922 | 265,823 | 571,735 |

### Structural Operations

| Structure | 1000 | 5000 | 10000 | 20000 |
|---|---|---|---|---|
| BST | 958 | 4,754 | 9,533 | 19,084 |
| Hash Table | 2,144 | 9,541 | 19,136 | 38,327 |
| Linked List | 958 | 4,754 | 9,533 | 19,084 |
| Sorted Array | 2,228 | 9,864 | 19,763 | 39,554 |

### Analysis

Workload B gives us an interesting set of results. This is especially true for the binary search tree in which sorted input breaks the structure.

With random data (Workload A), the BST produces a roughly balanced tree with average depth ~log₂(n). With sorted input, each new value is always larger than the previous, meaning every insert goes down the right branch. The tree degenerates into a linked list shape with depth n. At n=20000 the BST performs 381 million comparisons — nearly identical to the actual linked list's 373 million. This is a very key point to note for BSTs in that the structure is very vulnerable to how data is being inserted.

The sorted array, by contrast, actually performs *better* on sorted inserts than random ones (structural ops drop from 91 million to 39,000 at n=20000). Because values are already in order, new inserts always append to the end of the array and require no shifting at all.

The hash table is completely unaffected by input ordering, as expected. Its comparisons on Workload B are nearly identical to Workload A, confirming that hashing has no sensitivity to key distribution patterns.

---

## Workload C — Mixed Operations (Insert / Lookup / Delete)

### Comparisons

| Structure | 1000 | 5000 | 10000 | 20000 |
|---|---|---|---|---|
| BST | 9,072 | 52,492 | 119,860 | 263,914 |
| Hash Table | 947 | 6,045 | 11,127 | 19,847 |
| Linked List | 19,562 | 241,233 | 751,309 | 1,344,803 |
| Sorted Array | 8,915 | 56,558 | 127,397 | 249,684 |

### Operation Counts (consistent across all structures)

| Metric | 1000 | 5000 | 10000 | 20000 |
|---|---|---|---|---|
| Inserts | 500 | 2,500 | 5,000 | 10,000 |
| Lookups | 1,012 | 5,064 | 10,080 | 20,079 |
| Deletes | 488 | 2,436 | 4,920 | 9,921 |

### Structural Operations

| Structure | 1000 | 5000 | 10000 | 20000 |
|---|---|---|---|---|
| BST | 988 | 4,934 | 9,918 | 19,917 |
| Hash Table | 1,055 | 5,076 | 10,057 | 20,058 |
| Linked List | 986 | 4,934 | 9,915 | 19,916 |
| Sorted Array | 6,360 | 84,271 | 275,059 | 440,196 |

### Analysis

Workload C introduces deletes, which stress structures differently. The operation counts are consistent across all structures (as expected since they all process the same workload file), confirming the instrumentation is working correctly.

The hash table again leads in comparisons, followed closely by the BST and sorted array. With smaller population sizes due to the mixed workload, the BST tree stays shallower, keeping comparisons low.

The sorted array's structural ops are elevated again due to shifting, but less extreme than Workload A since the population size stays smaller as inserts and deletes roughly balance out.

---

## Workload D — Heavy Lookup (n inserts, 5n lookups)

### Comparisons

| Structure | 1000 | 5000 | 10000 | 20000 |
|---|---|---|---|---|
| BST | 70,150 | 452,237 | 984,172 | 2,133,839 |
| Hash Table | 4,135 | 23,372 | 47,378 | 94,345 |
| Linked List | 4,605,800 | 114,538,571 | 458,208,545 | 1,834,724,093 |
| Sorted Array | 64,208 | 391,480 | 842,860 | 1,805,444 |

### Lookups (5x inserts)

| Size | Inserts | Lookups |
|---|---|---|
| 1000 | 1,000 | 5,000 |
| 5000 | 5,000 | 25,000 |
| 10000 | 10,000 | 50,000 |
| 20000 | 20,000 | 100,000 |

### Analysis

Workload D is the most revealing test for lookup-heavy systems, which represent the majority of real-world use cases. The 5:1 lookup-to-insert ratio amplifies any weakness in search performance.

The hash table is the clear winner, reaching only 94,345 comparisons at n=20000 across 100,000 lookups. This is less than 1 comparison per lookup on average, which is O(1) lookup behavior in practice.

The BST and sorted array perform comparably, both benefiting from O(log n) search. At n=20000 they land around 2 million and 1.8 million comparisons respectively across 100,000 lookups, which works out to roughly 18 to 21 comparisons per lookup, consistent with log₂(20000) ≈ 14–17 depth expectations.

The linked list approaches 1.8 billion comparisons at n=20000. This is a direct result of linear scanning across a population of ~20,000 elements for each of 100,000 queries. This structure is fundamentally unsuitable for read-heavy workloads.

---

## Overall Summary

### Comparisons at n=20000 Across All Workloads

| Structure | Workload A | Workload B | Workload C | Workload D |
|---|---|---|---|---|
| BST | 675,408 | 381,426,548 | 263,914 | 2,133,839 |
| Hash Table | 37,111 | 37,402 | 19,847 | 94,345 |
| Linked List | 381,615,459 | 373,068,086 | 1,344,803 | 1,834,724,093 |
| Sorted Array | 582,981 | 571,735 | 249,684 | 1,805,444 |

### Key Takeaways

**Hash Table** is the best all-around performer. It has consistently low comparisons across every workload type and is completely unaffected by input ordering. Its cost is occasional resize events and slightly higher structural ops due to rehashing, but these are infrequent and predictable.

**BST** performs well on random data but catastrophically on sorted input. An unbalanced BST is a liability in any system where input ordering cannot be guaranteed. A self-balancing variant (AVL, Red-Black) would eliminate this weakness entirely.

**Sorted Array** provides reliable O(log n) lookup via binary search and is not sensitive to input ordering for lookups. Its weakness is insert and delete cost due to element shifting, making it poorly suited for write-heavy workloads but reasonable for stable datasets.

**Linked List** is consistently the worst performer in every workload. Its O(n) lookup cost makes it unsuitable for any system where contains operations are frequent. Its only advantage is O(1) prepend and pointer-based delete once the node is found.

### Structure Selection Guide

| Use Case | Recommended Structure | Reason |
|---|---|---|
| Read-heavy, random data | Hash Table | O(1) average lookup |
| Read-heavy, sorted data | Sorted Array | O(log n) binary search, stable |
| Mixed workload, random data | Hash Table or BST | Both handle inserts and lookups well |
| Mixed workload, sorted data | Hash Table or Sorted Array | BST degenerates on sorted input |
| Write-heavy, frequent inserts | Linked List or Hash Table | Avoid structures with shifting costs |
