/**
	\file "free_list_test.cc"
	Testing util::memory::free_list forwarding functions.  
	$Id: free_list_test.cc,v 1.2 2008/03/17 23:03:16 fang Exp $
 */

#include <set>
#include <iostream>
#include "util/memory/free_list.h"
#include "util/numeric/random.h"
#include "util/size_t.h"

using std::set;
using std::cout;
using std::endl;
using util::memory::free_list_acquire;
using util::memory::free_list_release;

typedef	util::numeric::rand48<size_t>	rng_type;
typedef	set<size_t>	int_set_type;

int
main(int, char* arg[]) {
	cout << "testing free-list on std::set..." << endl;
	int_set_type F, A;	// free, allocated
	// pre-load free-list
	F.insert(1);
	F.insert(2);
	F.insert(3);
	size_t i=0;
	for ( ; i<1000000; ++i) {
		const size_t r = 1+ rng_type()() % 3;
		if (F.find(r) != F.end()) {
			A.insert(free_list_acquire(F));
		} else {
			A.erase(r);
			free_list_release(F, r);
		}
	}
	cout << "Done testing free_list on std::set." << endl;
	return 0;
}


