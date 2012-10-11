/**
	\file "list_vector_pool_test.cc"
	Test for list-vector based allocator.  
	$Id: list_vector_pool_test.cc,v 1.11 2006/04/18 18:42:47 fang Exp $
 */

#ifdef	NDEBUG
#undef	NDEBUG
#endif

#include <cassert>
#include <iostream>
#include <algorithm>
#include <string>
#include "util/qmap.tcc"
#include "util/memory/list_vector_pool.tcc"

#include "util/what.h"

#include "util/using_ostream.h"
using std::string;
using util::memory::list_vector_pool;
using util::default_qmap;
using std::next_permutation;

namespace util {
	SPECIALIZE_UTIL_WHAT(string, "string")
}

//=============================================================================
// bogus test class
class string_manager {
public:
	typedef	default_qmap<size_t, string*>::type	string_map_type;
	typedef list_vector_pool<string>	string_pool_type;
private:
	string_pool_type			string_pool;
	string_map_type				string_map;
public:
	string_manager() : string_pool(16), string_map() { }
	~string_manager() {
		cout << "cleaning up string_manager..." << endl;
	}

	void add_index_string(const size_t i, const string& s) {
		string* sptr = 
			static_cast<const string_map_type&>(string_map)[i];
		if (sptr) {
			*sptr = s;
		} else {
			sptr = string_pool.allocate();
			assert(sptr);
			string_pool.construct(sptr, s);
			string_map[i] = sptr;
		}
	}

	void delete_index(const size_t i) {
		string* sptr = 
			static_cast<const string_map_type&>(string_map)[i];
		if (sptr) {
			string_pool.destroy(sptr);
			string_pool.deallocate(sptr);
			string_map.erase(i);
		}
		// doesn't exist
	}

};	// end class string_manager

//=============================================================================
int
main(int, char*[]) {
	cout << "NOTE: do not be alarmed by memory leak warnings!" << endl <<
		"they are intentional for testing diagnostics." << endl;
{
	cout << "test 1: " << endl;
	string_manager m;
}
{
	char alpha[] = "abcdefgh";
	size_t j = 0;
	for ( ; j<2; j++) {
		cout << "test 2a[" << j << "]: " << endl;
		string_manager m;
		size_t i = 0;
		do {
			m.add_index_string(i++, alpha);
		} while (next_permutation(alpha, alpha+8));
		cout << "added " << i << " strings to map." << endl;
		assert(i == 40320);
	}
}
{
	char alpha[] = "abcdefgh";
	size_t j = 0;
	for ( ; j<2; j++) {
		cout << "test 2b[" << j << "]: " << endl;
		string_manager m;
		size_t i = 0;
		do {
			m.add_index_string(i, alpha);
			m.delete_index(i++);
		} while (next_permutation(alpha, alpha+8));
		cout << "added/deleted " << i << " strings to map." << endl;
		assert(i == 40320);
	}
}
{
	char alpha[] = "abcdefgh";
	size_t j = 0;
	for ( ; j<2; j++) {
		cout << "test 3[" << j << "]: " << endl;
		string_manager m;
		size_t i = 0;
		do {
#if 0
			cout << '.' << i;
			cout.flush();
#endif
			m.add_index_string(i++, alpha);
		} while (next_permutation(alpha, alpha+8));
		cout << "added " << i << " strings to map." << endl;
		i = 0;
		for ( ; i < 40320; i+=10)
			m.delete_index(i);
		cout << "deleted every 10th strings from map." << endl;
	}
}
{
	char alpha[] = "abcdefgh";
	char numer[] = "0123456";
	cout << "test 4: " << endl;
	string_manager m;
	size_t i = 0;
	do {
#if 0
		cout << '.' << i;
		cout.flush();
#endif
		try {
			m.add_index_string(i++, alpha);
		} catch (...) {
			cout << "BUH!?" << endl;
		}
	} while (next_permutation(alpha, alpha+8));
	cout << "added " << i << " strings to map." << endl;
	cout.flush();
	i = 0;
	for ( ; i < 40320; i+=10)
		m.delete_index(i);
	cout << "deleted every 10th strings from map." << endl;
	i = 0;
	do {
		m.add_index_string(i++, numer);
	} while (next_permutation(numer, numer+7));
	cout << "added and replaced with numeric strings" << endl;
}
	return 0;
}

