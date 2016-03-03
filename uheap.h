// This file is part of the uSTL library, an STL implementation.
//
// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "ualgobase.h"

namespace ustl {

/// \brief Returns true if the given range is a heap under \p comp.
/// A heap is a sequentially encoded binary tree where for every node
/// comp(node,child1) is false and comp(node,child2) is false.
/// \ingroup HeapAlgorithms
/// \ingroup ConditionAlgorithms
///
template <typename RandomAccessIterator, typename Compare>
bool is_heap (RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    RandomAccessIterator iChild (first);
    for (; ++iChild < last; ++first)
	if (comp (*first, *iChild) || (++iChild < last && comp (*first, *iChild)))
	    return false;
    return true;
}

/// Utility function to "trickle down" the root item - swaps the root item with its
/// largest child and "recursively" fixes the proper subtree.
template <typename RandomAccessIterator, typename Distance, typename Compare>
void _trickle_down_heap (RandomAccessIterator first, Distance iHole, Distance heapSize, Compare comp)
{
    typedef typename iterator_traits<RandomAccessIterator>::value_type value_type;
    const value_type v (first[iHole]);
    for (Distance iChild; (iChild = 2 * iHole + 1) < heapSize;) {
	if (iChild + 1 < heapSize)
	    iChild += comp(first[iChild], first[iChild + 1]);
	if (comp(v, first[iChild])) {
	    first[iHole] = first[iChild];
	    iHole = iChild;
	} else
	    break;
    }
    first[iHole] = v;
}

/// \brief make_heap turns the range [first, last) into a heap
/// At completion, is_heap (first, last, comp) is true.
/// The algorithm is adapted from "Classic Data Structures in C++" by Timothy Budd.
/// \ingroup HeapAlgorithms
/// \ingroup SortingAlgorithms
///
template <typename RandomAccessIterator, typename Compare>
void make_heap (RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    if (last <= first)
	return;
    typedef typename iterator_traits<RandomAccessIterator>::difference_type distance_type;
    distance_type heapSize(last - first);
    RandomAccessIterator i = first + (last - first - 1)/2;
    while (i >= first) {
	_trickle_down_heap(first, distance_type(i - first), heapSize, comp);
	--i;
    }
}

/// \brief Inserts the *--last into the preceeding range assumed to be a heap.
/// \ingroup HeapAlgorithms
/// \ingroup MutatingAlgorithms
template <typename RandomAccessIterator, typename Compare>
void push_heap (RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    if (last <= first)
	return;
    typedef typename iterator_traits<RandomAccessIterator>::value_type value_type;
    const value_type v (*--last);
    while (first < last) {
	RandomAccessIterator iParent = first + (distance(first, last) - 1) / 2;
	if (comp (v, *iParent))
	    break;
	*last = *iParent;
	last = iParent;
    }
    *last = v;
}

/// Removes the largest element from the heap (*first) and places it at *(last-1)
/// [first, last-1) is a heap after this operation.
/// \ingroup HeapAlgorithms
/// \ingroup MutatingAlgorithms
template <typename RandomAccessIterator, typename Compare>
void pop_heap (RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    if (--last <= first)
	return;
    typedef typename iterator_traits<RandomAccessIterator>::difference_type distance_type;
    iter_swap(first, last);
    _trickle_down_heap(first, distance_type(0), distance_type(last - first), comp);
}

/// Sorts heap [first, last) in descending order according to comp.
/// \ingroup HeapAlgorithms
/// \ingroup SortingAlgorithms
template <typename RandomAccessIterator, typename Compare>
void sort_heap (RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    for (; first < last; --last)
	pop_heap (first, last, comp);
}

#define HEAP_FN_WITH_LESS(rtype, name)	\
template <typename RandomAccessIterator>\
inline rtype name (RandomAccessIterator first, RandomAccessIterator last)		\
{											\
    typedef typename iterator_traits<RandomAccessIterator>::value_type value_type;	\
    return name (first, last, less<value_type>());					\
}
HEAP_FN_WITH_LESS (bool, is_heap)
HEAP_FN_WITH_LESS (void, make_heap)
HEAP_FN_WITH_LESS (void, push_heap)
HEAP_FN_WITH_LESS (void, pop_heap)
HEAP_FN_WITH_LESS (void, sort_heap)
#undef HEAP_FN_WITH_LESS

/// \class priority_queue uheap.h ustl.h
/// \ingroup Sequences
///
/// \brief Sorted queue adapter to uSTL containers.
///
/// Acts just like the queue adapter, but keeps the elements sorted by priority
/// specified by the given comparison operator.
///
template <typename T, typename Ctr = vector<T>, typename Comp = less<typename Ctr::value_type> >
class priority_queue {
public:
    typedef Ctr					base_ctr;
    typedef typename base_ctr::value_type	value_type;
    typedef typename base_ctr::size_type	size_type;
    typedef typename base_ctr::const_pointer	const_pointer;
    typedef typename base_ctr::const_reference	reference;
public:
			priority_queue (const Comp& c = Comp()) : _v(), _c (c) {}
			priority_queue (const_pointer f, const_pointer l, const Comp& c = Comp())
			    : _v (f, l), _c (c) { make_heap (_v.begin(), _v.end(), _c); }
    inline size_type	size (void) const	{ return _v.size(); }
    inline bool		empty (void) const	{ return _v.empty(); }
    inline reference	top (void) const	{ return _v.at(0); }
    inline void		push (reference v)	{ _v.push_back (v); push_heap (_v.begin(), _v.end(), _c); }
    inline void		pop (void)		{ pop_heap (_v.begin(), _v.end()); _v.pop_back(); }
private:
    base_ctr		_v;	///< Element container.
    Comp		_c;	///< Comparison functor by value.
};

} // namespace ustl
