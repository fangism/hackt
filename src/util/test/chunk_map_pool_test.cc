/**
	\file "chunk_map_pool_test.cc"
	Testing functionality of chunk_map_pool allocator.  
	$Id: chunk_map_pool_test.cc,v 1.4 2005/09/04 21:15:10 fang Exp $
 */

#include "util/numeric/bigger_ints.h"
#include "util/memory/chunk_map_pool.tcc"
#include "util/memory/excl_ptr.h"
#include <iostream>

#include "util/using_ostream.h"
using util::memory::chunk_map_pool_chunk;
using util::memory::chunk_map_pool;
using util::memory::excl_ptr;

// only enable for testing, produces too much output
#define	ENABLE_HUGE_CHUNK		0
// vary this to some power of two, 64 up to 256
#define	HUGE_SIZE			256

class foo {
#if 0
	foo();
	~foo();
#endif
};

namespace util {
	SPECIALIZE_UTIL_WHAT(foo, "foo")
}

typedef	chunk_map_pool_chunk<foo,8>		small_chunk_type;
typedef	chunk_map_pool_chunk<foo,16>		medium_chunk_type;
typedef	chunk_map_pool_chunk<foo,32>		large_chunk_type;
typedef	chunk_map_pool_chunk<foo, HUGE_SIZE>	huge_chunk_type;

typedef	chunk_map_pool<foo,8>			small_pool_type;
typedef	chunk_map_pool<foo,16>			medium_pool_type;
typedef	chunk_map_pool<foo,32>			large_pool_type;
typedef	chunk_map_pool<foo, HUGE_SIZE>		huge_pool_type;

static
void
small_chunk_test(void);

static
void
medium_chunk_test(void);

static
void
large_chunk_test(void);

#if ENABLE_HUGE_CHUNK
static
void
huge_chunk_test(void);
#endif

static
void
small_pool_test(void);

static
void
medium_pool_test(void);

static
void
large_pool_test(void);

#if ENABLE_HUGE_CHUNK
static
void
huge_pool_test(void);
#endif

int
main(int, char*[]) {
	small_chunk_test();
	medium_chunk_test();
	large_chunk_test();
#if ENABLE_HUGE_CHUNK
	huge_chunk_test();
#endif
	small_pool_test();
	medium_pool_test();
	large_pool_test();
#if ENABLE_HUGE_CHUNK
	huge_pool_test();
#endif
}

void
small_chunk_test(void) {
	small_chunk_type small_chunk;
	INVARIANT(small_chunk.empty());
	INVARIANT(!small_chunk.full());
	small_chunk.status(cout);

	foo* ptr_0 = small_chunk.allocate();
	INVARIANT(!small_chunk.empty());
	INVARIANT(!small_chunk.full());
	small_chunk.status(cout);

	foo* ptr_1 = small_chunk.allocate();
	INVARIANT(!small_chunk.empty());
	INVARIANT(!small_chunk.full());
	small_chunk.status(cout);

	small_chunk.deallocate(ptr_0);
	INVARIANT(!small_chunk.empty());
	INVARIANT(!small_chunk.full());
	small_chunk.status(cout);

	small_chunk.deallocate(ptr_1);
	INVARIANT(small_chunk.empty());
	INVARIANT(!small_chunk.full());
	small_chunk.status(cout);

	foo* ptrs[8];
	size_t i = 0;
	for ( ; i<8; i++) {
		ptrs[i] = small_chunk.allocate();
		small_chunk.status(cout);
	}
	INVARIANT(!small_chunk.empty());
	INVARIANT(small_chunk.full());
//	small_chunk.allocate();		// over-allocated!

	i = 0;
	for ( ; i<8; i++) {
		small_chunk.deallocate(ptrs[i]);
		small_chunk.status(cout);
	}
	INVARIANT(small_chunk.empty());
	INVARIANT(!small_chunk.full());
//	small_chunk.deallocate(ptrs[0]);	// not-allocated!
}

void
medium_chunk_test(void) {
	medium_chunk_type medium_chunk;
	INVARIANT(medium_chunk.empty());
	INVARIANT(!medium_chunk.full());
	medium_chunk.status(cout);

	foo* ptrs[16];
	size_t i = 0;
	for ( ; i<16; i++) {
		ptrs[i] = medium_chunk.allocate();
		medium_chunk.status(cout);
	}
	INVARIANT(!medium_chunk.empty());
	INVARIANT(medium_chunk.full());
//	medium_chunk.allocate();		// over-allocated!

	i = 0;
	for ( ; i<16; i++) {
		// try deallocating in some weird order for kicks...
		medium_chunk.deallocate(ptrs[(i*7)%16]);
		medium_chunk.status(cout);
	}
	INVARIANT(medium_chunk.empty());
	INVARIANT(!medium_chunk.full());
//	medium_chunk.deallocate(ptrs[0]);	// not-allocated!
}

void
large_chunk_test(void) {
	large_chunk_type large_chunk;
	INVARIANT(large_chunk.empty());
	INVARIANT(!large_chunk.full());
	large_chunk.status(cout);

	foo* ptrs[32];
	size_t i = 0;
	for ( ; i<32; i++) {
		ptrs[i] = large_chunk.allocate();
		large_chunk.status(cout);
	}
	INVARIANT(!large_chunk.empty());
	INVARIANT(large_chunk.full());
//	large_chunk.allocate();		// over-allocated!

	i = 0;
	for ( ; i<32; i++) {
		// try deallocating in some weird order for kicks...
		large_chunk.deallocate(ptrs[(i*37)%32]);
		large_chunk.status(cout);
	}
	INVARIANT(large_chunk.empty());
	INVARIANT(!large_chunk.full());
//	large_chunk.deallocate(ptrs[0]);	// not-allocated!
}

#if ENABLE_HUGE_CHUNK
void
huge_chunk_test(void) {
	huge_chunk_type huge_chunk;
	INVARIANT(huge_chunk.empty());
	INVARIANT(!huge_chunk.full());
	huge_chunk.status(cout);

	foo* ptrs[HUGE_SIZE];
	size_t i = 0;
	for ( ; i<HUGE_SIZE; i++) {
		ptrs[i] = huge_chunk.allocate();
		huge_chunk.status(cout);
	}
	INVARIANT(!huge_chunk.empty());
	INVARIANT(huge_chunk.full());
//	huge_chunk.allocate();		// over-allocated!

	i = 0;
	for ( ; i<HUGE_SIZE; i++) {
		// try deallocating in some weird order for kicks...
		huge_chunk.deallocate(ptrs[(i*43)%HUGE_SIZE]);
		huge_chunk.status(cout);
	}
	INVARIANT(huge_chunk.empty());
	INVARIANT(!huge_chunk.full());
//	huge_chunk.deallocate(ptrs[0]);	// not-allocated!
}
#endif

void
small_pool_test(void) {
	small_pool_type small_pool;
	small_pool.status(cout);

	foo* ptr_0 = small_pool.allocate();
	small_pool.status(cout);

	foo* ptr_1 = small_pool.allocate();
	small_pool.status(cout);

	small_pool.deallocate(ptr_0);
	small_pool.status(cout);

	small_pool.deallocate(ptr_1);
	small_pool.status(cout);

	foo* ptrs[20];
	size_t i = 0;
	for ( ; i<20; i++) {
		ptrs[i] = small_pool.allocate();
		small_pool.status(cout);
	}

	i = 0;
	for ( ; i<20; i++) {
		small_pool.deallocate(ptrs[i]);
		small_pool.status(cout);
	}
}

void
medium_pool_test(void) {
	medium_pool_type medium_pool;
	medium_pool.status(cout);

	foo* ptrs[40];
	size_t i = 0;
	for ( ; i<40; i++) {
		ptrs[i] = medium_pool.allocate();
		medium_pool.status(cout);
	}

	i = 0;
	for ( ; i<40; i++) {
		medium_pool.deallocate(ptrs[(i*13)%40]);
		medium_pool.status(cout);
	}
}

void
large_pool_test(void) {
	large_pool_type large_pool;
	large_pool.status(cout);

	foo* ptrs[80];
	size_t i = 0;
	for ( ; i<80; i++) {
		ptrs[i] = large_pool.allocate();
		large_pool.status(cout);
	}

	i = 0;
	for ( ; i<80; i++) {
		large_pool.deallocate(ptrs[(i*47)%80]);
		large_pool.status(cout);
	}
}

#if ENABLE_HUGE_CHUNK
void
huge_pool_test(void) {
	huge_pool_type huge_pool;
	huge_pool.status(cout);

	foo* ptrs[160];
	size_t i = 0;
	for ( ; i<160; i++) {
		ptrs[i] = huge_pool.allocate();
		huge_pool.status(cout);
	}

	i = 0;
	for ( ; i<160; i++) {
		huge_pool.deallocate(ptrs[(i*27)%160]);
		huge_pool.status(cout);
	}
}
#endif

