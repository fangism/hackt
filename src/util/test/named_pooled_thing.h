/**
	\file "named_pooled_thing.h"
	Named, pooled object for testing.  
	$Id: named_pooled_thing.h,v 1.1.4.1 2005/01/23 01:34:04 fang Exp $
 */

#ifndef	__NAMED_POOLED_THING_H__
#define	__NAMED_POOLED_THING_H__

#include <iosfwd>
#include <string>
#include "memory/list_vector_pool_fwd.h"
#include "memory/pointer_classes.h"
#include "what.h"

#define	ENABLE_POOL			1
// DEBUG memory pool using stacktrace, seeing double destruction!

using util::what;
using std::string;
using namespace util::memory;

#if 1
class named_thing {
	typedef	named_thing		this_type;
public:
	named_thing();
	named_thing(const string& s);
	~named_thing();

#if ENABLE_POOL
	LIST_VECTOR_POOL_ROBUST_STATIC_DECLARATIONS
#endif
};
#elif 0
class named_thing {
	typedef	named_thing		this_type;
private:
	mutable int*		_int;
public:
	named_thing();
	named_thing(const string& s);
	named_thing(const named_thing& n);
	~named_thing();
#if ENABLE_POOL
	LIST_VECTOR_POOL_ROBUST_STATIC_DECLARATIONS
#endif
};

#else
class named_thing {
	typedef	named_thing		this_type;
private:
	string		name;
	mutable int*	_int;
public:
	named_thing();
	named_thing(const string& s);
	named_thing(const named_thing& n);
	~named_thing();
#if ENABLE_POOL
	LIST_VECTOR_POOL_ROBUST_STATIC_DECLARATIONS
#endif
};	// end class named_thing
#endif

namespace util {
	SPECIALIZE_UTIL_WHAT_DECLARATION(named_thing);
}

#endif	// __NAMED_POOLED_THING_H__

