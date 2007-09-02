/**
	\file "util/discrete_interval_set.tcc"
	Template class method definitions for discrete_interval_set.
	$Id: discrete_interval_set.tcc,v 1.6.90.1 2007/09/02 20:49:36 fang Exp $
 */

#ifndef	__UTIL_DISCRETE_INTERVAL_SET_TCC__
#define	__UTIL_DISCRETE_INTERVAL_SET_TCC__

#include "util/discrete_interval_set.h"

// predefine to suppress definition
#ifndef	EXTERN_TEMPLATE_UTIL_DISCRETE_INTERVAL_SET

#include <iostream>
#include "util/macros.h"
#include "util/attributes.h"

namespace util {
using std::ostream;

//=============================================================================
// clsss discrete_interval_set method definitions

DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
discrete_interval_set<T>::discrete_interval_set() : parent() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
discrete_interval_set<T>::discrete_interval_set(const T a, const T b) :
		parent() {
	const bool overlap __ATTRIBUTE_UNUSED__ = add_range(a, b);
	INVARIANT(!overlap);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
discrete_interval_set<T>::discrete_interval_set(
		const discrete_interval_set& s) : parent(s.parent) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
discrete_interval_set<T>::~discrete_interval_set() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sanity check for consistency of interval map.  
	Only used for debugging purposes.  
 */
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
void
discrete_interval_set<T>::check_integrity(void) const {
	if (!parent.empty()) {
		T temp;
		const_iterator i(parent.begin());
		const const_iterator e(parent.end());
		INVARIANT(i->first <= i->second);
		temp = i->second;
		i++;
		while (i != e) {
			INVARIANT(i->first <= i->second);
			INVARIANT(temp < i->first);
			temp = i->second;
			i++;
		}
	}
	// else an empty set qualifies as valid
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Empties out the set.
 */
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
void
discrete_interval_set<T>::clear(void) {
	parent.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
typename discrete_interval_set<T>::size_type
discrete_interval_set<T>::member_size(void) const {
	size_type ret = 0;
	const_iterator i(parent.begin()), e(parent.end());
	for ( ; i!=e; ++i) {
		ret += i->second -i->first +1;
	}
	// or std::accumulate ... select_2nd
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Queries if value is one of the intervals in the set.  
	\param v the value to probe.
	\return the interal in which v is contained if found, 
		else the end() iterator.
 */
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
typename discrete_interval_set<T>::iterator
discrete_interval_set<T>::contains(const T v) {
	iterator ret(parent.upper_bound(v));
		// b/c lower_bound misses equality condition
	// (ret != parent.end()) is irrelevant.  
	if (ret != parent.begin()) {
		ret--;		// look at interval before
		INVARIANT(ret->first <= v);
		if (ret->second >= v) {
			return ret;
		}
	} else {
		if (ret->first <= v && v <= ret->second) {
			return ret;
		}
	}
	return parent.end();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
typename discrete_interval_set<T>::const_iterator
discrete_interval_set<T>::contains(const T v) const {
	const_iterator ret(parent.upper_bound(v));
		// lower_bound misses equality condition
	// (ret != parent.end()) is irrelevant.  
	if (ret != parent.begin()) {
		ret--;		// look at interval before
		INVARIANT(ret->first <= v);
		if (ret->second >= v)
			return ret;
	} else {
		if (ret->first <= v && v <= ret->second)
			return ret;
	}
	return parent.end();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
typename discrete_interval_set<T>::const_iterator
discrete_interval_set<T>::contains_entirely(const T min, const T max) const {
	const const_iterator l(contains(min));
	if (l == parent.end())
		return l;
	const const_iterator u(contains(max));
	return (u == l) ? u : parent.end();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Queries whether or not proposed range will overlap
	at all with existing intervals.  
 */
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
bool
discrete_interval_set<T>::query_overlap(const T min, const T max) const {
	INVARIANT(min <= max);
	return (contains(min) != parent.end() ||
		contains(max) != parent.end() || 
		// short-circuit OR: either one of the ends overlaps
		// or if there is at least one interval in between them
		(distance(parent.lower_bound(min),
			parent.lower_bound(max)) > 0));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
bool
discrete_interval_set<T>::behead(const T m) {
	iterator li(contains(m));
	if (li != parent.end()) {
		T new_max = li->second;
		T new_min = m +1;
		li++;
		parent.erase(parent.begin(), li);
		if (new_min <= new_max)		// was != (WRONG)
			parent[new_min] = new_max;
		return true;
	} else {	// no overlap
		li = parent.lower_bound(m);
		const bool ret = (li != parent.begin());
		parent.erase(parent.begin(), li);
		// won't delete li
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
bool
discrete_interval_set<T>::betail(const T m) {
	iterator ui(contains(m));
	if (ui != parent.end()) {
		if (ui->first == m) {
		} else {
			ui->second = m-1;
			ui++;		// don't delete this
		}
		parent.erase(ui, parent.end());
		return true;
	} else {	// no overlap
		ui = parent.lower_bound(m);
		const bool ret = (ui != parent.end());
		parent.erase(ui, parent.end());
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
bool
discrete_interval_set<T>::meet(const discrete_interval_set& b) {
	bool ret;
	const_iterator i(b.begin());
	if (i != b.end()) {
		T temp = i->second +1;
		ret = behead(i->first -1);
		for (i++ ; i!=b.end(); i++) {
			temp = i->second +1;
			ret = delete_range(temp, i->first -1) || ret;
		}
		ret = betail(temp) || ret;
	} else {	// b is empty!
		ret = !parent.empty();
		parent.clear();
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
bool
discrete_interval_set<T>::join(const discrete_interval_set& b) {
	bool ret = false;
	const_iterator i(b.begin());
	for ( ; i!=b.end(); i++)
		ret = add_range(i->first, i->second) || ret;
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
bool
discrete_interval_set<T>::subtract(const discrete_interval_set<T>& b) {
	bool ret = false;
	const_iterator i(b.begin());
	for ( ; i!=b.end(); i++)
		ret = delete_range(i->first, i->second) || ret;
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	later re-write walking iterators from left to right.  
	\return true if added range overlapped with existing ranges.  
 */
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
bool
discrete_interval_set<T>::add_range(const T min, const T max) {
	const bool overlap = query_overlap(min, max);
	// doesn't catch case where both miss around other intervals...
	iterator li(contains(min -1));
	iterator ui(contains(max +1));
	// fuse: condition for merging off-by-one
	const bool fuse = (li != parent.end() || ui != parent.end());
	T new_min, new_max;
	if (li != parent.end()) {
		// partial overlap
		new_min = li->first;
	} else {
		li = parent.lower_bound(min);
		new_min = min;
	}
	if (ui != parent.end()) {
		// partial overlap
		new_max = ui->second;
	} else {
		ui = parent.lower_bound(max);		// could be end()
		if (ui != parent.begin()) {
			ui--;
		} else {	// ui is begin
			INVARIANT(li == parent.begin());
		}
		new_max = max;
	}
	// check different between li, ui for overlap
#if 0		// for debugging
	cout << "range to delete: ";
	if (li != parent.end())
		cout << "[" << li->first << "," << li->second << "]";
	else cout << "end";
	cout << " to ";
	if (ui != parent.end())
		cout << "[" << ui->first << "," << ui->second << "]";
	else cout << "end";
	cout << ", adding/replacing: [" << new_min << "," << new_max << "]";
	cout << endl << flush;
		// distance() hangs if li > ui
#endif
	// slightly inefficient, delete and re-add
	if (li != parent.end() && ui != parent.end()) {
		if (li == ui) {
			if (overlap || fuse)
				parent.erase(li);	// erase single
		} else if (li->second < ui->first || overlap) {
//			cout << ": distance is " << distance(li,ui) << endl;
			ui++;		// because upper iterator not inclusive
			parent.erase(li, ui);	// erase range left-inclusive
		}
	}
	parent[new_min] = new_max;
//	cout << "overlap = " << overlap << endl;

	check_integrity();	// debug: can comment this out
	return overlap;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Write using iterative delete.  
	Manually logically optimized.  
**/
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
bool
discrete_interval_set<T>::delete_range(const T min, const T max) {
	const bool overlap = query_overlap(min, max);
if (overlap) {
	iterator li(contains(min));
	iterator ui(contains(max));
	const bool lfnem = (li->first != min);
	const bool lueq = (li == ui);

	if (ui != parent.end()) {
		if (ui->second != max) {	// split range into two
			parent[max +1] = ui->second;
		} // else don't need to add entry
		if (!(lfnem && lueq)) {
			ui++;			// erase including ui
		}
	} else {
		ui = parent.lower_bound(max);
	}

	if (li != parent.end()) {
		if (lfnem) {
			li->second = min -1;
			if (!lueq)
				li++;
		}
	} else {	// li == end()
		li = parent.lower_bound(min);
	}

	parent.erase(li, ui);
	check_integrity();	// debug: can comment this out
}
	return overlap;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class U>
ostream&
operator << (ostream& o, const discrete_interval_set<U>& r) {
	o << "{";
	typename discrete_interval_set<U>::const_iterator i(r.begin());
	for ( ; i!=r.end(); i++) {
		U lo = i->first;
		U hi = i->second;
		INVARIANT(hi >= lo);	// consistency check
		o << "[";
		o << lo;
		if (hi != lo) o << "," << hi;
		o << "]";
	}
	return o << "}";
}

//=============================================================================
}	// end namespace util

#endif	// EXTERN_TEMPLATE_UTIL_DISCRETE_INTERVAL_SET

#endif	// __UTIL_DISCRETE_INTERVAL_SET_TCC__

