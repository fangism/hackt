/**
	\file "cube_slice.h"
	Rigorous testing class for multikey_qmap.  
	$Id: cube_slice.h,v 1.3 2004/12/05 05:08:28 fang Exp $
 */

#include <assert.h>
#include <string>
#include "STL/list.tcc"
#include <iostream>
#include <algorithm>
#include <numeric>
#include "sstream.h"
#include "multikey_qmap.tcc"

namespace testing {
USING_LIST
using std::string;
#include "using_ostream.h"
using std::accumulate;
using std::ostringstream;
using std::ostream_iterator;
using namespace MULTIKEY_NAMESPACE;
using namespace MULTIKEY_MAP_NAMESPACE;

typedef multikey_qmap<1,int,string>     map_1d_type;
typedef multikey_qmap<2,int,string>     map_2d_type;
typedef multikey_qmap<3,int,string>     map_3d_type;
typedef multikey_generator<1,int>       generator_1d_type;
typedef multikey_generator<2,int>       generator_2d_type;
typedef multikey_generator<3,int>       generator_3d_type;

/**
	Instantiates a truncated cube of points.
 */
class cube_slice {
public:
	const int lower_1;
	const int lower_2;
	const int lower_3;
	const int upper_1;
	const int upper_2;
	const int upper_3;
	const int min_slice;
	const int max_slice;
protected:
	map_3d_type	str_map;
public:
	/**
		\param l1 the x dimension minimum.
		\param l2 the y dimension minimum.
		\param l3 the z dimension minimum.
		\param u1 the x dimension maximum.
		\param u2 the y dimension maximum.
		\param u3 the z dimension maximum.
		\param min lower slice for the sum of coordinates.
		\param max upper slice for the sum of coordinates.
	 */
	cube_slice(const int l1, const int l2, const int l3, 
		const int u1, const int u2, const int u3, 
		const int min, const int max) : 
			lower_1(l1), lower_2(l2), lower_3(l3), 
			upper_1(l1), upper_2(l2), upper_3(l3), 
			min_slice(min), max_slice(max), str_map() {
		// populate the map with strings
		generator_3d_type key_gen;
		key_gen.lower_corner[0] = l1;
		key_gen.lower_corner[1] = l2;
		key_gen.lower_corner[2] = l3;
		key_gen.upper_corner[0] = u1;
		key_gen.upper_corner[1] = u2;
		key_gen.upper_corner[2] = u3;
		assert(min <= max);
		key_gen.initialize();
		do {
			const int sum =
				accumulate(key_gen.begin(), key_gen.end(), 0);
			if (sum >= min && sum <= max) {
				ostringstream oss;
				oss << key_gen[0] << ", " << key_gen[1] <<
					", " << key_gen[2] << endl;
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
		const pair<list<int>, list<int> >& probe) const {
		assert(l.size() == 2 && u.size() == 2);
		// more difficult...
	}

	void
	validate_3d_slice(const list<int>& l, const list<int>& u, 
		const pair<list<int>, list<int> >& probe) const {
		assert(l.size() == 3 && u.size() == 3);
		const int l_sum = accumulate(l.begin(), l.end(), 0);
		const int u_sum = accumulate(u.begin(), u.end(), 0);
		if (l_sum >= min_slice && u_sum <= max_slice)
			assert(!probe.first.empty());
		else	assert(probe.first.empty());
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
		map_3d_type::const_iterator start = str_map.begin();
		map_3d_type::const_iterator end = --str_map.end();
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
		map_3d_type::const_iterator start = str_map.begin();
		map_3d_type::const_iterator end = --str_map.end();
		assert(start != str_map.end());
		map_3d_type::key_list_pair_type ext(str_map.index_extremities());
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

	void
	slice_test_3d(void) const {
		map_3d_type::const_iterator start = str_map.begin();
		map_3d_type::const_iterator end = --str_map.end();
		assert(start != str_map.end());
		map_3d_type::key_list_pair_type ext(str_map.index_extremities());
		int min1 = ext.first.front();
		int max1 = ext.second.front();
		int min2 = *(++ext.first.begin());
		int max2 = *(++ext.second.begin());
		int min3 = ext.first.back();
		int max3 = ext.second.back();
		list<int> l;
		l.push_back(min1);
		l.push_back(min2);
		l.push_back(min3);
		int& l_1 = l.front();
		int& l_2 = *(++l.begin());
		int& l_3 = l.back();
		for ( ; l_1 <= max1; l_1++) {
		for (l_2 = min2 ; l_2 <= max2; l_2++) {
		for (l_3 = min3 ; l_3 <= max3; l_3++) {
			list<int> u(l);
			int& u_1 = u.front();
			int& u_2 = *(++u.begin());
			int& u_3 = u.back();
			for ( ; u_1 <= max1; u_1++) {
			for (u_2 = l_2 ; u_2 <= max2; u_2++) {
			for (u_3 = l_3 ; u_3 <= max3; u_3++) {
				const pair<list<int>, list<int> >
					probe = str_map.is_compact_slice(l, u);
				cout << "slice [" << l_1 << ".." << u_1
					<< "][" << l_2 << ".." << u_2
					<< "][" << l_3 << ".." << u_3
					<< "] is ";
				report_compaction_brief(probe);
				validate_3d_slice(l, u, probe);
				cout << endl;
			}
			}
			}
		}
		}
		}
	}


};	// end class cube_slice


}	// end namespace testing

