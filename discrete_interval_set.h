// "discrete_interval_set.h"

// Class for a set of intervals over the set of integers
//	or discrete, integer-like values.  

// will be built-upon to make "multidimensional_sparse_set"

// by David Fang, Cornell University, 2004

#ifndef	__DISCRETE_INTERVAL_SET_H__
#define	__DISCRETE_INTERVAL_SET_H__

#include <map>
#include <iostream>

#define	DEBUG_DISCRETE_INTERVAL_SET		0
// eventually delete all that...

#ifndef DISCRETE_INTERVAL_SET_NAMESPACE
#define DISCRETE_INTERVAL_SET_NAMESPACE		fang
#endif

//=============================================================================
/**
	Namespace for discrete interval sets, and related classes.
 */
namespace DISCRETE_INTERVAL_SET_NAMESPACE {
using namespace std;		// for map<>

// forward declarations
template <class T> class discrete_interval_set;

template <class U>
ostream& operator << (ostream& o, const discrete_interval_set<U>& r);

//=============================================================================
/**
	Template parameter class T must be discrete, such as integers, 
	and not continuous such as reals, floats, or doubles.  
	Because differences of 1 will be fused.  
	Implemented as a map of pairs.  
	Pairs are automatically fused together as holes are filled.  

	We choose a map for efficient comparison (<, >) operations.  
	The first element of each pair is a minimum of an inclusive
	bracket, and the second element is the maximum, inclusive.
 */
template <class T>
class discrete_interval_set : public map<T, T> {
public:
	typedef	map<T, T>				parent;
	typedef	typename parent::iterator		iterator;
	typedef	typename parent::const_iterator		const_iterator;
	typedef	typename parent::reverse_iterator	reverse_iterator;
	typedef	typename parent::const_reverse_iterator	const_reverse_iterator;

public:
	/** Standard default constructor. */
	discrete_interval_set() : parent() { }

	/** Constructor initializing with one interval. */
	discrete_interval_set(const T a, const T b) : parent()
		{ assert(!add_range(a, b)); }

	/** Standard copy constructor. */
	discrete_interval_set(const discrete_interval_set<T>& s) : parent(s) { }

	/** Standard default destructor. */
	~discrete_interval_set() { }

protected:
	/**
		Sanity check for consistency of interval map.  
		Only used for debugging purposes.  
	 */
	void check_integrity(void) const {
		if (!this->empty()) {
			T temp;
			const_iterator i = this->begin();
			assert(i->first <= i->second);
			temp = i->second;
			i++;
			while (i != this->end()) {
				assert(i->first <= i->second);
				assert(temp < i->first);
				temp = i->second;
				i++;
			}
		}
	}

	/**
		Queries if value is one of the intervals in the set.  
		\param v the value to probe.
		\return the interal in which v is contained if found, 
			else the end() iterator.
	 */
	iterator contains(const T v) {
#if DEBUG_DISCRETE_INTERVAL_SET
		cerr << "Does " << *this << " contain " << v << "?  ";
#endif
		iterator ret = this->upper_bound(v);
			// b/c lower_bound misses equality condition
		// (ret != this->end()) is irrelevant.  
		if (ret != this->begin()) {
			ret--;		// look at interval before
			assert(ret->first <= v);
			if (ret->second >= v) {
#if DEBUG_DISCRETE_INTERVAL_SET
				cerr << ((ret != this->end()) ? "yes" :
					"no") << endl;
#endif
				return ret;
			}
		} else {
			if (ret->first <= v && v <= ret->second) {
#if DEBUG_DISCRETE_INTERVAL_SET
				cerr << ((ret != this->end()) ? "yes" :
					"no") << endl;
#endif
				return ret;
			}
		}
#if DEBUG_DISCRETE_INTERVAL_SET
		cerr << ((ret != this->end()) ? "yes" : "no") << endl;
#endif
		return this->end();
	}

public:
	/**
		Public const version of contains().  
	 */
	const_iterator contains(const T v) const {
		const_iterator ret = this->upper_bound(v);
			// lower_bound misses equality condition
		// (ret != this->end()) is irrelevant.  
		if (ret != this->begin()) {
			ret--;		// look at interval before
			assert(ret->first <= v);
			if (ret->second >= v)
				return ret;
		} else {
			if (ret->first <= v && v <= ret->second)
				return ret;
		}
		return this->end();
	}

	/**
		Queries whether or not proposed range will overlap
		at all with existing intervals.  
	 */
	bool query_overlap(const T min, const T max) const;

	/**
		Adds an interval to the current set.
		Intelligently fuses ranges together appropriately.  
		\param min the lower bound, inclusive.  
		\param max the upper bound, inclusive.  
		\return true if there was overlap, else false.
	 */
	bool add_range(const T min, const T max);

	/**
		Removes a range from the set.
		\param min the lower bound, inclusive.  
		\param max the upper bound, inclusive.  
		\return if anything was actually deleted.
	 */
	bool delete_range(const T min, const T max);

	/**
		Removes the range (-INF, m].
		\param m the lower cut-off.
		\return true if anything was deleted.  
	 */
	bool behead(const T m) {
#if DEBUG_DISCRETE_INTERVAL_SET
		cerr << "beheading from " << m << " and below." << endl;
#endif
		iterator li = contains(m);
		if (li != this->end()) {
#if DEBUG_DISCRETE_INTERVAL_SET
			cerr << "li != end()" << endl;
#endif 
			T new_max = li->second;
			T new_min = m +1;
			li++;

#if DEBUG_DISCRETE_INTERVAL_SET
			cerr << "erasing from begin to li:" << endl;
			if (this->begin() != this->end())
				cerr << "begin: " << this->begin()->first <<
					".." << this->begin()->second << endl;
			else	cerr << "begin: (end)" << endl;
			if (li != this->end())
				cerr << "li: " << li->first <<
					".." << li->second << endl;
			else	cerr << "li: (end)" << endl;
			cerr << "new_min = " << new_min <<
				", new_max = " << new_max << endl;
#endif
			erase(this->begin(), li);
			if (new_min <= new_max)		// was != (WRONG)
				(*this)[new_min] = new_max;
#if DEBUG_DISCRETE_INTERVAL_SET
			cerr << "result is " << *this << endl;
#endif
			return true;
		} else {	// no overlap
#if DEBUG_DISCRETE_INTERVAL_SET
			cerr << "li == end()" << endl;
#endif 
			li = lower_bound(m);
			const bool ret = (li != this->begin());
#if DEBUG_DISCRETE_INTERVAL_SET
			cerr << "erasing from begin to li:" << endl;
			if (this->begin() != this->end())
				cerr << "begin: " << this->begin()->first <<
					".." << this->begin()->second << endl;
			else	cerr << "begin: (end)" << endl;
			if (li != this->end())
				cerr << "li: " << li->first <<
					".." << li->second << endl;
			else	cerr << "li: (end)" << endl;
#endif
			erase(this->begin(), li);
			// won't delete li
#if DEBUG_DISCRETE_INTERVAL_SET
			cerr << "result is " << *this << endl;
#endif
			return ret;
		}
	}

	/**
		Removes the range [m, +INF).
		\param m the upper cut-off.
		\return true if anything was deleted.  
	 */
	bool betail(const T m) {
#if DEBUG_DISCRETE_INTERVAL_SET
		cerr << "betailing from " << m << " and above." << endl;
#endif
		iterator ui = contains(m);
		if (ui != this->end()) {
			if (ui->first == m) {
			} else {
				ui->second = m-1;
				ui++;		// don't delete this
			}
			erase(ui, this->end());
#if DEBUG_DISCRETE_INTERVAL_SET
		cerr << "result is " << *this << endl;
#endif
			return true;
		} else {	// no overlap
			ui = lower_bound(m);
			const bool ret = (ui != this->end());
			erase(ui, this->end());
#if DEBUG_DISCRETE_INTERVAL_SET
		cerr << "result is " << *this << endl;
#endif
			return ret;
		}
	}

	/**
		Takes intersection.  
		\param b the list with which to take the intersection. 
		\return true if anything was deleted.  
	 */
	bool meet(const discrete_interval_set<T>& b) {
		bool ret;
		const_iterator i = b.begin();
		if (i != b.end()) {
			T temp = i->second +1;
			ret = behead(i->first -1);
			for (i++ ; i!=b.end(); i++) {
				temp = i->second +1;
				ret = delete_range(temp, i->first -1) || ret;
			}
			ret = betail(temp) || ret;
		} else {	// b is empty!
			ret = !this->empty();
			this->clear();
		}
		return ret;
	}

	bool intersect(const discrete_interval_set<T>& b) { return meet(b); }

	/**
		Takes union.  
		\param b the list with which to take the union. 
		\return true if anything was added.  
	 */
	bool join(const discrete_interval_set<T>& b) {
		bool ret = false;
		const_iterator i = b.begin();
		for ( ; i!=b.end(); i++)
			ret = add_range(i->first, i->second) || ret;
		return ret;
	}

	bool unite(const discrete_interval_set<T>& b) { return join(b); }

// non-member functions
template <class U>
friend ostream& operator << (ostream& o, const discrete_interval_set<U>& r);

};	// end class discrete_interval_set

//=============================================================================
// discrete_interval_set method definitions

template <class T>
bool
discrete_interval_set<T>::query_overlap(const T min, const T max) const {
	assert(min <= max);
	return (contains(min) != this->end() || contains(max) != this->end() || 
		// short-circuit OR: either one of the ends overlaps
		// or if there is at least one interval in between them
		(distance(lower_bound(min), lower_bound(max)) > 0));
}

/**
	later re-write walking iterators from left to right.  
**/
template <class T>
bool
discrete_interval_set<T>::add_range(const T min, const T max) {
#if DEBUG_DISCRETE_INTERVAL_SET
	cerr << "adding [" << min << "," << max << "]" << endl;
#endif
	const bool overlap = query_overlap(min, max);
	// doesn't catch case where both miss around other intervals...
	iterator li = contains(min -1);
	iterator ui = contains(max +1);
	// fuse: condition for merging off-by-one
	const bool fuse = (li != this->end() || ui != this->end());
	T new_min, new_max;
	if (li != this->end()) {
		// partial overlap
		new_min = li->first;
	} else {
		li = lower_bound(min);
		new_min = min;
	}
	if (ui != this->end()) {
		// partial overlap
		new_max = ui->second;
	} else {
		ui = lower_bound(max);		// could be end()
		if (ui != this->begin()) {
			ui--;
		} else {	// ui is begin
			assert(li == this->begin());
		}
		new_max = max;
	}
	// check different between li, ui for overlap
#if 0		// for debugging
	cout << "range to delete: ";
	if (li != this->end())
		cout << "[" << li->first << "," << li->second << "]";
	else cout << "end";
	cout << " to ";
	if (ui != this->end())
		cout << "[" << ui->first << "," << ui->second << "]";
	else cout << "end";
	cout << ", adding/replacing: [" << new_min << "," << new_max << "]";
	cout << endl << flush;
		// distance() hangs if li > ui
#endif
	// slightly inefficient, delete and re-add
	if (li != this->end() && ui != this->end()) {
		if (li == ui) {
			if (overlap || fuse)
				erase(li);	// erase single
		} else if (li->second < ui->first || overlap) {
//			cout << ": distance is " << distance(li,ui) << endl;
			ui++;		// because upper iterator not inclusive
			erase(li, ui);	// erase range left-inclusive
		}
	}
	(*this)[new_min] = new_max;
//	cout << "overlap = " << overlap << endl;

	this->check_integrity();	// debug: can comment this out
	return overlap;
}

//-----------------------------------------------------------------------------
/**
	Write using iterative delete.  
	Manually logically optimized.  
**/
template <class T>
bool
discrete_interval_set<T>::delete_range(const T min, const T max) {
#if DEBUG_DISCRETE_INTERVAL_SET
	cerr << "deleting [" << min << "," << max << "]" << endl;
#endif
	const bool overlap = query_overlap(min, max);
	if (overlap) {
	iterator li = contains(min);
	iterator ui = contains(max);
	const bool lfnem = (li->first != min);
	const bool lueq = (li == ui);

	if (ui != this->end()) {
		if (ui->second != max) {	// split range into two
			(*this)[max +1] = ui->second;
		} // else don't need to add entry
		if (!(lfnem && lueq)) {
			ui++;			// erase including ui
		}
	} else {
		ui = lower_bound(max);
	}

	if (li != this->end()) {
		if (lfnem) {
			li->second = min -1;
			if (!lueq)
				li++;
		}
	} else {	// li == end()
		li = lower_bound(min);
	}

	erase(li, ui);
	this->check_integrity();	// debug: can comment this out
	}
	return overlap;
}

//-----------------------------------------------------------------------------
template <class U>
ostream&
operator << (ostream& o, const discrete_interval_set<U>& r) {
	o << "{";
	typename discrete_interval_set<U>::const_iterator i = r.begin();
	for ( ; i!=r.end(); i++) {
		U lo = i->first;
		U hi = i->second;
		assert(hi >= lo);	// consistency check
		o << "[";
		o << lo;
		if (hi != lo) o << "," << hi;
		o << "]";
	}
	return o << "}";
}

//=============================================================================
}	// end namespace DISCRETE_INTERVAL_SET_NAMESPACE

#endif	//	__DISCRETE_INTERVAL_SET_H__

