/**
	\file "hash_map_iterator_O2_bug_test.cc"
	This test case is known to seg-fault on Apple's g++-3.3
	(builds 1640, 1819) with -O2 optimization on.  
	(Less optimization works, and also works on 
	i686-linux-g++-3.3.3, works on g++-4.0, 4.1).
 */

// #include <iostream>
#include "util/STL/hash_map.h"

// using std::cerr;
// using std::endl;
using HASH_MAP_NAMESPACE::hash_map;

struct foo {
	typedef	hash_map<int, int>			map_type;
	typedef	map_type::iterator			iterator;

	map_type					_map;

	foo();

	void
	bar(void);
};

foo::foo() : _map() {
	_map[1] = 11;
	_map[2] = 22;
}

void
foo::bar(void) {
	const iterator b(_map.begin()), e(_map.end());
	// cerr << _map.size() << endl;		// inserting this prevent bug
{
	// iterator i(_map.begin());		// work around
	iterator i(b);				// dies on -O2
	for ( ; i!=e; ++i) {
		// cerr << i->first << " -> " << i->second << endl;
	}
}
}

int
main(int, char*[]) {
	foo f;
	f.bar();
	return 0;
}


