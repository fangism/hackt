/**
	\file "pooled_thing.h"
	Example of a simple pool-allocated class, using the util library.  
	$Id: pooled_thing.h,v 1.2 2005/03/06 04:36:50 fang Exp $
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

	LIST_VECTOR_POOL_DEFAULT_STATIC_DECLARATIONS
};

#endif	// __POOLED_THING_H__

