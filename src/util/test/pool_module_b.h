/**
	\file "pool_module_b.h"
	Header for one module of a multimodule memory pool test.
	$Id: pool_module_b.h,v 1.1.4.1.2.1 2005/01/27 06:04:54 fang Exp $
 */

#ifndef	__POOL_MODULE_B_H__
#define	__POOL_MODULE_B_H__

#include "memory/pointer_classes.h"
#include "memory/list_vector_pool_fwd.h"
#include "what.h"

USING_CONSTRUCT
using util::what;
using namespace util::memory;

class twiddle_dee {
private:
	typedef	twiddle_dee		this_type;
public:
	twiddle_dee();
	~twiddle_dee();

public:
	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_ROBUST_STATIC_DECLARATIONS

};	// end class twiddle_dee

namespace util {
	SPECIALIZE_UTIL_WHAT_DECLARATION(twiddle_dee)
}       // end namespace util 

#endif	// __POOL_MODULE_B_H__

