/**
	\file "pooled_thing.hh"
	Example of a simple pool-allocated class, using the util library.  
	$Id: pooled_thing.hh,v 1.3 2005/09/04 21:15:13 fang Exp $
 */

#ifndef	__POOLED_THING_H__
#define	__POOLED_THING_H__

#include "util/size_t.h"	// "util/size_t.h"
#include "util/memory/list_vector_pool_fwd.hh"

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

