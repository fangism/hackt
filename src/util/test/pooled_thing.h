/**
	\file "pooled_thing.h"
	Example of a simple pool-allocated class, using the util library.  
	$Id: pooled_thing.h,v 1.1 2005/01/16 02:44:27 fang Exp $
 */

#ifndef	__POOLED_THING_H__
#define	__POOLED_THING_H__

#include <cstddef>		// for size_t
// forward declarations is enough
#include "memory/list_vector_pool_fwd.h"

using util::memory::list_vector_pool;

class pooled_thing {
private:
	typedef	pooled_thing		this_type;
public:
	int value;
	pooled_thing(const int d = 0) : value(d) { }
	~pooled_thing() { }

	LIST_VECTOR_POOL_STATIC_DECLARATIONS
};

#endif	// __POOLED_THING_H__

