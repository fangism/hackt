/**
	\file "allocator_adaptor_test.cc"
	Testing functionality of chunk_map_pool allocator 
	with allocator_adaptor.  
	$Id: allocator_adaptor_test.cc,v 1.1 2010/04/05 00:18:48 fang Exp $
 */

#ifdef	NDEBUG
#undef	NDEBUG
#endif

#include <set>
#include <iostream>
#include "util/memory/chunk_map_pool.tcc"
#include "util/memory/allocator_adaptor.tcc"

#include "util/using_ostream.h"
using util::memory::chunk_map_pool;
using util::memory::allocator_adaptor;

typedef	size_t						value_type;
typedef	chunk_map_pool<value_type, sizeof(size_t) << 3>	pool_type;
typedef	std::set<value_type, std::less<value_type>,
		allocator_adaptor<pool_type> >
							set_type;

namespace util {
namespace memory {
	// explicit instantiation
	template class allocator_adaptor<pool_type>;
}
}

static
void
set_op_test(set_type& s, const size_t m) {
	size_t i = 0;
	cout << "inserting 0.." << m-1 << endl;
	for (i=0; i<m; ++i) {
		s.insert(i);
	}
	cout << "removing 0.." << m-1 << endl;
	for (i=0; i<m; ++i) {
		s.erase(i);
	}
	cout << "inserting (1 mod 2)" << endl;
	for (i=1; i<m; i+=2) {
		s.insert(i);
	}
	cout << "inserting (0 mod 3)" << endl;
	for (i=0; i<m; i+=3) {
		s.insert(i);
	}
	cout << "inserting (0 mod 4)" << endl;
	for (i=0; i<m; i+=4) {
		s.insert(i);
	}
	cout << "removing (0..5 mod 10)" << endl;
	for (i=0; i<m; i+=10) {
	size_t j = 0;
	for ( ; j<5; ++j) {
		s.erase(i+j);
	}
	}
}

int
main(int, char*[]) {
{
	const size_t m = 10000;
	set_type s;
	set_op_test(s, m);
	set_op_test(s, m << 1);
}{
	const size_t m = 100000;
	set_type s;
	set_op_test(s, m);
}
	return 0;
}

