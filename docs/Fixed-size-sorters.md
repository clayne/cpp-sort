Fixed-size sorters, sometimes called *fixed sorters* for simplicity are a special kind of sorters designed to sort a fixed number of values. Their `operator()` also takes either an iterable or a pair of iterators as well as an optional comparison and projection functions. Most of the time the end iterator is unused, but future versions of the library may start to use it to optionally perform bound-checking.

Fixed-sized sorters are not actual sorters *per se* but class templates that take an `std::size_t` template parameter. Every valid specialization of a fixed-size sorter for a given size yields a "valid" sorter. Several fixed-size sorters have specializations for some sizes only and will trigger a compile-time error when one tries to instantiate a specialization which is not part of the fixed-size sorter's domain (the domain corresponds to the set of valid specializations). Information about fixed-size sorters can be obtained via [`fixed_sorter_traits`](https://github.com/Morwenn/cpp-sort/wiki/Sorter-traits#fixed_sorter_traits). One can also make sure that a given fixed-size sorter is automatically used to sort small fixed-size arrays thanks to [`small_array_adapter`](https://github.com/Morwenn/cpp-sort/wiki/Sorter-adapters#small_array_adapter).

It is possible to include all the fixed-sized sorters at once with the following directive:

```cpp
#include <cpp-sort/fixed_sorters.h>
```

The following fixed-size sorters are available and should work with any type for which `std::less<>` and `utility::identity` work:

### `low_comparisons_sorter`

```cpp
#include <cpp-sort/fixed/low_comparisons_sorter.h>
```

This fixed-size sorter implements specific sorting algorithms whose goal is to keep the number of comparisons performed as low as possible, which may be desirable when the comparisons are expensive but the moves are cheap (*e.g.* strings with large common prefixes). To know which sorting algorithm has the lowest overall number of comparisons for an array of a given size, the following method is used: compute the sum of all the comparisons needed to sort every permutation of the array; we call this the *comparison weight* of the array.

The following table lists the sorting algorithms used by the different specializations of `low_comparisons_sorter` as well as their respective *comparison weights* (algorithms marked with a star actually use smaller specializations of `low_comparisons_sorter` under the hood and insert values into the sorted sequence).

Size | Comparison weight | Algorithm
---- | ----------------- | ---------
0 | 0 | Nothing
1 | 0 | Nothing
2 | 2 | Compare and exchange
3 | 16 | Insertion sort
4 | 112 | Insertion sort*
5 | 832 | Merge-insertion sort
6 | 6912 | Merge-insertion sort
7 | 62784 | Insertion sort*
8 | 623232 | Insertion sort*
9 | 6759936 | Merge-insertion sort
10 | 79937280 | Insertion sort*
11 | 1020833280 | Insertion sort*
12 | 15167554560 | [Double gnome sort*](https://github.com/Morwenn/cpp-sort/wiki/Original-research#double-insertion-sort)
13 | 223436206080 | Double gnome sort*

While `low_comparisons_sorter` is optimal from 0 through 8 with regard to the *comparison weight*, it is worth noting that [`merge_insertion_sorter`](https://github.com/Morwenn/cpp-sort/wiki/Sorters#merge_insertion_sorter) performs fewer comparisons on average than some other specializations. However the algorithm is rather complex and has a high runtime cost, which makes it unsuitable for such a sorter, which seeks to provide tiny and fast algorithm.

It is worth noting that the algorithm used to sort 9 elements in not strictly a merge-insertion sort: instead it uses an equivalent algorithm described in *A variant of the Ford–Johnson algorithm that is more space efficient* by Ayala-Rincón et al. That said I did not implement it correctly since it still performs more comparisons than `merge_insertion_sorter` (but it's still better than the previous solution, so that's ok for now).

```cpp
template<std::size_t N>
struct low_comparisons_sorter;
```

### `low_moves_sorter`

```cpp
#include <cpp-sort/fixed/low_moves_sorter.h>
```

This fixed-size sorter implements specific sorting algorithms whose goal is to keep the number of moves performed low, which may be useful when comparisons are cheap but moves are expensive (large objects that only use one field for the comparison for example). To know which sorting algorithm performs the lowest number of move operations overall for an array of a given size, the following method is used: count the total number of moves needed to sort all the permutations of the array, we will call this the *move weight* of the array.

The following table illustrates the *move weight* of the algorithms used by the different specializations of `low_moves_sorter`. If you ever find an algorithm that beats one of those without a big memory footprint, do not hesitate to contribute:

Size | Move weight
---- | -----------
0 | 0
1 | 0
2 | 2
3 | 17
4 | 122
5 | 898
6 | 7188
7 | 63276
8 | 612048
9 | 6476112
10 | 74558880
11 | 929011680
12 | 12465394560
13 | 179294186880

The algorithms 0 to 3 use an unrolled insertion sort. The algorithm 4 uses a simple selection sort. The following algorithms use a recursive bidirectional selection sort, sometimes known as cocktail selection sort or minmax sort. While it does not perform fewer moves than a selection sort, it still performs fewer comparisons on average. This sorter has no upper bound, it can sort an array of size 155 if needed, but then it might generate too much code, so try to keep the size low if possible.

```cpp
template<std::size_t N>
struct low_moves_sorter;
```

Note that this fixed-sized sorter is *not* move-optimal: it tries to perform a few moves without wasting too much memory and with a somewhat reasonable number of comparisons for small collections. If you really need a sorting algorithm that performs the lowest possible number of move operations, you can use the library's [`indirect_adapter`](https://github.com/Morwenn/cpp-sort/wiki/Sorter-adapters#indirect_adapter) instead, but it comes at the cost of a higher memory footprint. You probably want to use if only when the objects are *really* expensive to copy.

### `sorting_network_sorter`

```cpp
#include <cpp-sort/fixed/sorting_network_sorter.h>
```

This sorter provides [sorting network](https://en.wikipedia.org/wiki/Sorting_network) algorithms to sort collections of size 0 thru 32. While using a generic algorithm for the task such as a Batcher's odd-even mergesort may be too slow to be usable, the resulting unrolled sorting networks may be fast enough and even tend to be faster than everything else when it comes to sorting small arrays of integers without requiring additional memory.

```cpp
template<std::size_t N>
struct sorting_network_sorter;
```

The following table gives the number of *compare-exchange units* (CEUs) used to sort a fixed collection of a given size. These numbers should correspond to the best-known size-optimal sorting networks at the time of writing (as opposed to depth-optimal sorting networks). If you ever find a sorting network using fewer CEUs for one of these sizes, don't hesitate to let me know, but you might as well write a research paper about it.

Size | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16
:-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-:
**CEUs** | 0 | 1 | 3 | 5 | 9 | 12 | 16 | 19 | 25 | 29 | 35 | 39 | 45 | 51 | 56 | 60
**Size** | **17** | **18** | **19** | **20** | **21** | **22** | **23** | **24** | **25** | **26** | **27** | **28** | **29** | **30** | **31** | **32**
**CEUs** | 71 | 77 | 85 | 91 | 100 | 107 | 115 | 120 | 132 | 139 | 150 | 155 | 165 | 172 | 180 | 185

One of the main advantages of sorting networks is the fixed number of CEUs required to sort a collection: this means that sorting networks are far more resistant to time and cache attacks since the number of performed comparisons does not depend on the contents of the collection. However, additional care (not provided by the library) is required to ensure that the algorithms always perform the same amount of memory loads and stores. For example, one could create a `constant_time_iterator` with a dedicated `iter_swap` tuned to perform a constant-time compare-exchange operation.

*Note:* don't be fooled by the name; none of the algorithms in this fixed-size sorter explicitly perform any operation in parallel. Everything is sequential. The algorithms are but long sequences of compare-exchange units.

*Changed in version 1.2.0:* sorting 21 inputs requires 100 CEUs instead of 101.

*Changed in version 1.3.0:* sorting 23, 24, 25 and 26 inputs respectively require 115, 120, 132 and 139 CEUs instead of 116, 121, 133 and 140.

*Changed in version 1.8.0:* sorting 18 inputs requires 77 CEUs instead of 78.