/**
	\file "util/STL/algorithm_fwd.h"
	Forward declarations of <algorithm> header.  
	$Id: algorithm_fwd.h,v 1.2 2006/04/12 08:53:22 fang Exp $
 */

#ifndef	__UTIL_STL_ALGORITHM_FWD_H__
#define	__UTIL_STL_ALGORITHM_FWD_H__

#include "util/STL/pair_fwd.h"
#include "util/STL/iterator_fwd.h"

namespace std {
//=============================================================================
// swapping

template <typename FI1, typename FI2>
void
iter_swap(FI1, FI2);

template <typename T>
void
swap(T&, T&);

//-----------------------------------------------------------------------------
// selection and comparison

#ifdef	min
#error	Do not define 'min' as a preprocessor macro.
#undef	min
#endif

#ifdef	max
#error	Do not define 'max' as a preprocessor macro.
#undef	max
#endif

template <class T>
const T&
min(const T&, const T&);

template <class T, class Comp>
const T&
min(const T&, const T&, Comp);

template <class T>
const T&
max(const T&, const T&);

template <class T, class Comp>
const T&
max(const T&, const T&, Comp);

//-----------------------------------------------------------------------------
// copy, fill, move, transform, replace, remove

template <typename InIter, typename OutIter>
OutIter
copy(InIter, InIter, OutIter);

template <typename InIter, typename OutIter>
OutIter
copy_backward(InIter, InIter, OutIter);

template <typename FwdIter, typename T>
void
fill(FwdIter, FwdIter, const T&);

template <typename OutIter, typename Sz, typename T>
OutIter
fill_n(OutIter, Sz, const T&);

template <typename FwdIter, typename G>
void
generate(FwdIter, FwdIter, G);

template <typename OutIter, typename Sz, typename G>
OutIter
generate_n(OutIter, Sz, G);

template <typename FwdIter1, typename FwdIter2>
FwdIter2
swap_ranges(FwdIter1, FwdIter1, FwdIter2);

template <typename InIter, typename OutIter, typename UnOp>
OutIter
transform(InIter, InIter, OutIter, UnOp);

template <typename InIter1, typename InIter2, typename OutIter, typename BinOp>
OutIter
transform(InIter1, InIter1, InIter2, OutIter, BinOp);

template <typename FwdIter, typename T>
void
replace(FwdIter, FwdIter, const T&, const T&);

template <typename FwdIter, typename Pred, typename T>
void
replace_if(FwdIter, FwdIter, Pred, const T&);

template <typename InIter, typename OutIter, typename T>
OutIter
replace_copy(InIter, InIter, OutIter, const T&, const T&);

template <typename InIter, typename OutIter, typename Pred, typename T>
OutIter
replace_copy_if(InIter, InIter, OutIter, Pred, const T&);

template <typename FwdIter, typename T>
FwdIter
remove(FwdIter, FwdIter, const T&, const T&);

template <typename FwdIter, typename Pred, typename T>
FwdIter
remove_if(FwdIter, FwdIter, Pred, const T&);

template <typename InIter, typename OutIter, typename T>
OutIter
remove_copy(InIter, InIter, OutIter, const T&, const T&);

template <typename InIter, typename OutIter, typename Pred, typename T>
OutIter
remove_copy_if(InIter, InIter, OutIter, Pred, const T&);

template <typename InIter, typename OutIter>
OutIter
unique_copy(InIter, InIter, OutIter);

template <typename InIter, typename OutIter, typename BinPred>
OutIter
unique_copy(InIter, InIter, OutIter, BinPred);

template <typename FwdIter>
FwdIter
unique(FwdIter, FwdIter);

template <typename FwdIter, typename BinPred>
FwdIter
unique(FwdIter, FwdIter, BinPred);

template <typename BiIter>
void
reverse(BiIter, BiIter);

template <typename BiIter, typename OutIter>
OutIter
reverse_copy(BiIter, BiIter, OutIter);

//-----------------------------------------------------------------------------
// more sequence manipulation algorithms...

template <typename FwdIter>
void
rotate(FwdIter, FwdIter, FwdIter);

template <typename FwdIter, typename OutIter>
OutIter
rotate_copy(FwdIter, FwdIter, FwdIter, OutIter);

template <typename RandIter>
void
random_shuffle(RandIter, RandIter);

template <typename RandIter, typename RNG>
void
random_shuffle(RandIter, RandIter, RNG);

template <typename FwdIter, typename Pred>
FwdIter
partition(FwdIter, FwdIter, Pred);

template <typename FwdIter, typename Pred>
FwdIter
stable_partition(FwdIter, FwdIter, Pred);

template <typename RandIter>
void
sort(RandIter, RandIter);

template <typename RandIter, typename Comp>
void
sort(RandIter, RandIter, Comp);

template <typename RandIter>
void
stable_sort(RandIter, RandIter);

template <typename RandIter, typename Comp>
void
stable_sort(RandIter, RandIter, Comp);

template <typename RandIter>
void
partial_sort(RandIter, RandIter, RandIter);

template <typename RandIter, typename Comp>
void
partial_sort(RandIter, RandIter, RandIter, Comp);

template <typename InIter, typename RandIter>
RandIter
partial_sort_copy(InIter, InIter, RandIter, RandIter);

template <typename InIter, typename RandIter, typename Comp>
RandIter
partial_sort_copy(InIter, InIter, RandIter, RandIter, Comp);

template <typename RandIter>
void
nth_element(RandIter, RandIter, RandIter);

template <typename RandIter, typename Comp>
void
nth_element(RandIter, RandIter, RandIter, Comp);

template <typename BiIter>
bool
next_permutation(BiIter, BiIter);

template <typename BiIter, typename Comp>
bool
next_permutation(BiIter, BiIter, Comp);

template <typename BiIter>
bool
prev_permutation(BiIter, BiIter);

template <typename BiIter, typename Comp>
bool
prev_permutation(BiIter, BiIter, Comp);

//-----------------------------------------------------------------------------
// mismatch, equal, comparison

template <typename InIter1, typename InIter2>
pair<InIter1, InIter2>
mismatch(InIter1, InIter1, InIter2);

template <typename InIter1, typename InIter2, typename BinPred>
pair<InIter1, InIter2>
mismatch(InIter1, InIter1, InIter2, BinPred);

template <typename InIter1, typename InIter2>
bool
equal(InIter1, InIter1, InIter2);

template <typename InIter1, typename InIter2, typename BinPred>
bool
equal(InIter1, InIter1, InIter2, BinPred);

template <typename InIter1, typename InIter2>
bool
lexicographical_compare(InIter1, InIter1, InIter2, InIter2);

template <typename InIter1, typename InIter2, typename Comp>
bool
lexicographical_compare(InIter1, InIter1, InIter2, InIter2, Comp);

//-----------------------------------------------------------------------------
// sequence algorithms and searching

template <typename InIter, typename F>
F
for_each(InIter, InIter, F);

template <typename InIter, typename T>
InIter
find(InIter, InIter, const T&);

template <typename InIter, typename Pred>
InIter
find_if(InIter, InIter, Pred);

template <typename FwdIter>
FwdIter
adjacent_find(FwdIter, FwdIter);

template <typename FwdIter, typename Pred>
FwdIter
adjacent_find(FwdIter, FwdIter, Pred);

template <typename InIter, typename FwdIter>
InIter
find_first_of(InIter, InIter, FwdIter, FwdIter);

template <typename InIter, typename FwdIter, typename BinPred>
InIter
find_first_of(InIter, InIter, FwdIter, FwdIter, BinPred);

template <typename FwdIter1, typename FwdIter2>
FwdIter1
find_end(FwdIter1, FwdIter1, FwdIter2, FwdIter2);

template <typename FwdIter1, typename FwdIter2, typename BinPred>
FwdIter1
find_end(FwdIter1, FwdIter1, FwdIter2, FwdIter2, BinPred);

template <typename InIter, typename T>
typename iterator_traits<InIter>::difference_type
count(InIter, InIter, const T&);

template <typename InIter, typename Pred>
typename iterator_traits<InIter>::difference_type
count_if(InIter, InIter, Pred);

/// finds matching sub-sequence
template <typename FwdIter1, typename FwdIter2>
FwdIter1
search(FwdIter1, FwdIter1, FwdIter2, FwdIter2);

template <typename FwdIter1, typename FwdIter2, typename BinPred>
FwdIter1
search(FwdIter1, FwdIter1, FwdIter2, FwdIter2, BinPred);

template <typename FwdIter1, typename FwdIter2, typename T>
FwdIter1
search_n(FwdIter1, FwdIter1, FwdIter2, FwdIter2, const T&);

template <typename FwdIter1, typename FwdIter2, typename T, typename BinPred>
FwdIter1
search_n(FwdIter1, FwdIter1, FwdIter2, FwdIter2, const T&, BinPred);

template <typename FwdIter, typename T>
FwdIter
lower_bound(FwdIter, FwdIter, const T&);

template <typename FwdIter, typename T, typename Comp>
FwdIter
lower_bound(FwdIter, FwdIter, const T&, Comp);

template <typename FwdIter, typename T>
FwdIter
upper_bound(FwdIter, FwdIter, const T&);

template <typename FwdIter, typename T, typename Comp>
FwdIter
upper_bound(FwdIter, FwdIter, const T&, Comp);

template <typename FwdIter, typename T>
pair<FwdIter, FwdIter>
equal_range(FwdIter, FwdIter, const T&);

template <typename FwdIter, typename T, typename Comp>
pair<FwdIter, FwdIter>
equal_range(FwdIter, FwdIter, const T&, Comp);

template <typename FwdIter, typename T>
bool
binary_search(FwdIter, FwdIter, const T&);

template <typename FwdIter, typename T, typename Comp>
bool
binary_search(FwdIter, FwdIter, const T&, Comp);

template <typename InIter1, typename InIter2>
bool
includes(InIter1, InIter1, InIter2, InIter2);

template <typename InIter1, typename InIter2, typename Comp>
bool
includes(InIter1, InIter1, InIter2, InIter2, Comp);

template <typename FwdIter>
FwdIter
max_element(FwdIter, FwdIter);

template <typename FwdIter, typename Comp>
FwdIter
max_element(FwdIter, FwdIter, Comp);

template <typename FwdIter>
FwdIter
min_element(FwdIter, FwdIter);

template <typename FwdIter, typename Comp>
FwdIter
min_element(FwdIter, FwdIter, Comp);

//-----------------------------------------------------------------------------
// set operations

template <typename InIter1, typename InIter2, typename OutIter>
OutIter
merge(InIter1, InIter1, InIter2, InIter2, OutIter);

template <typename InIter1, typename InIter2, typename OutIter, typename Comp>
OutIter
merge(InIter1, InIter1, InIter2, InIter2, OutIter, Comp);

template <typename BiIter>
void
inplace_merge(BiIter, BiIter, BiIter);

template <typename BiIter, typename Comp>
void
inplace_merge(BiIter, BiIter, BiIter, Comp);

template <typename InIter1, typename InIter2, typename OutIter>
OutIter
set_union(InIter1, InIter1, InIter2, InIter2, OutIter);

template <typename InIter1, typename InIter2, typename OutIter, typename Comp>
OutIter
set_union(InIter1, InIter1, InIter2, InIter2, OutIter, Comp);

template <typename InIter1, typename InIter2, typename OutIter>
OutIter
set_intersection(InIter1, InIter1, InIter2, InIter2, OutIter);

template <typename InIter1, typename InIter2, typename OutIter, typename Comp>
OutIter
set_intersection(InIter1, InIter1, InIter2, InIter2, OutIter, Comp);

template <typename InIter1, typename InIter2, typename OutIter>
OutIter
set_difference(InIter1, InIter1, InIter2, InIter2, OutIter);

template <typename InIter1, typename InIter2, typename OutIter, typename Comp>
OutIter
set_difference(InIter1, InIter1, InIter2, InIter2, OutIter, Comp);

template <typename InIter1, typename InIter2, typename OutIter>
OutIter
set_symmetric_difference(InIter1, InIter1, InIter2, InIter2, OutIter);

template <typename InIter1, typename InIter2, typename OutIter, typename Comp>
OutIter
set_symmetric_difference(InIter1, InIter1, InIter2, InIter2, OutIter, Comp);

//=============================================================================
}	// end namespace std

#endif	// __UTIL_STL_ALGORITHM_FWD_H__

