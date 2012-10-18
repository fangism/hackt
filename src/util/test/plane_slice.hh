/**
	\file "plane_slice.hh"
	Rigorous testing class for multikey_qmap.  
	$Id: plane_slice.hh,v 1.3 2006/04/27 00:17:56 fang Exp $
 */

#include "util/macros.h"
#include <string>
#include <list>
#include <iostream>
#include <algorithm>
#include <numeric>
#include "util/sstream.hh"
#include "util/multikey_qmap.tcc"
#include "multikey_qmap_type_helper.hh"

namespace testing {
using std::list;
using std::string;
#include "util/using_ostream.hh"
using std::pair;
using std::accumulate;
using std::ostringstream;
using std::ostream_iterator;
using util::multikey;
using util::multikey_generator;
using util::multikey_map;
using util::qmap;

#if 0
typedef multikey_qmap<1,int,string>	map_1d_type;
typedef multikey_qmap<2,int,string>	map_2d_type;
#else

// typedef multikey_map<1,int,string,qmap>	map_1d_type;
// typedef multikey_map<2,int,string,qmap>	map_2d_type;
typedef __helper_mk_map_type<2>::type		map_2d_type;
#endif
// typedef multikey_generator<1,int>	generator_1d_type;
typedef multikey_generator<2,int>	generator_2d_type;

/**
	Instantiates a truncated cube of points.
 */
class plane_slice {
public:
	const int lower_1;
	const int lower_2;
	const int upper_1;
	const int upper_2;
	const int min_slice;
	const int max_slice;
protected:
	map_2d_type	str_map;
public:
	/**
		\param l1 the x dimension minimum.
		\param l2 the y dimension minimum.
		\param u1 the x dimension maximum.
		\param u2 the y dimension maximum.
		\param min lower slice for the sum of coordinates.
		\param max upper slice for the sum of coordinates.
	 */
	plane_slice(const int l1, const int l2,
		const int u1, const int u2,
		const int min, const int max) : 
			lower_1(l1), lower_2(l2),
			upper_1(l1), upper_2(l2),
			min_slice(min), max_slice(max), str_map() {
		// populate the map with strings
		generator_2d_type key_gen;
		key_gen.lower_corner[0] = l1;
		key_gen.lower_corner[1] = l2;
		key_gen.upper_corner[0] = u1;
		key_gen.upper_corner[1] = u2;
		assert(min <= max);
		key_gen.initialize();
		do {
			const int sum =
				accumulate(key_gen.begin(), key_gen.end(), 0);
			if (sum >= min && sum <= max) {
				ostringstream oss;
				oss << key_gen[0] << ", " << key_gen[1] << endl;
				str_map[key_gen] = oss.str();
				cout << "Added element at index "
					<< key_gen << endl;
			}
			key_gen++;
		} while (key_gen != key_gen.lower_corner);
	}

	// default destructor

	void
	validate_2d_slice(const list<int>& l, const list<int>& u, 
		const pair<list<int>, list<int> >& UNNAMED(probe)) const {
		assert(l.size() == 2 && u.size() == 2);
		// more difficult...
	}

	static void
	report_compaction(const pair<list<int>, list<int> >& probe) {
		if (probe.first.empty()) {
			assert(probe.second.empty());
			cout << "not densely packed.";
		} else {
			assert(!probe.second.empty());
			cout << "densely packed: ";
			cout << "[";
			ostream_iterator<int> osi(cout, ", ");
			copy(probe.first.begin(), probe.first.end(), 
				osi);
			cout << "]..[";
			copy(probe.second.begin(), probe.second.end(),
				osi);
			cout << "]";
		}
	}
                 
	static void
	report_compaction_brief(const pair<list<int>, list<int> >& probe) {
		if (probe.first.empty()) {
			assert(probe.second.empty());
			cout << "not densely packed.";
		} else {
			assert(!probe.second.empty());
			cout << "densely packed.";
		}
	}

	void
	slice_test_1d(void) const {
		map_2d_type::const_iterator start = str_map.begin();
		map_2d_type::const_iterator end = --str_map.end();
		assert(start != str_map.end());
		int min = start->first.front();
		int max = end->first.front();
		list<int> l;
		l.push_back(min);
		for ( ; l.front() <= max; l.front()++) {
			list<int> u(l);
			for ( ; u.front() <= max; u.front()++) {
				const pair<list<int>, list<int> >
					probe = str_map.is_compact_slice(l, u);
				cout << "slice [" << l.front() << ".."
					<< u.front() << "] is ";
				report_compaction(probe);
				cout << endl;
			}
		}
	}

	void
	slice_test_2d(void) const {
		map_2d_type::const_iterator start = str_map.begin();
		// map_2d_type::const_iterator end = --str_map.end();
		assert(start != str_map.end());
		map_2d_type::key_list_pair_type ext(str_map.index_extremities());
		int min1 = ext.first.front();
		int max1 = ext.second.front();
		int min2 = *(++ext.first.begin());
		int max2 = *(++ext.second.begin());
		list<int> l;
		l.push_back(min1);
		l.push_back(min2);
		for ( ; l.front() <= max1; l.front()++) {
		for (l.back() = min2 ; l.back() <= max2; l.back()++) {
			list<int> u(l);
			for ( ; u.front() <= max1; u.front()++) {
			for (u.back() = l.back() ; u.back() <= max2; u.back()++) {
				const pair<list<int>, list<int> >
					probe = str_map.is_compact_slice(l, u);
				cout << "slice [" << l.front() << ".." << u.front()
					<< "][" << l.back() << ".." << u.back()
					<< "] is ";
				report_compaction(probe);
				validate_2d_slice(l, u, probe);
				cout << endl;
			}
			}
		}
		}
	}

};	// end class plane_slice

}	// end namespace testing

