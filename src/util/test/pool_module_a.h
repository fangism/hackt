/**
	\file "pool_module_a.h"
	Header for one module of a multimodule memory pool test.
	$Id: pool_module_a.h,v 1.3.46.1 2005/08/16 03:50:37 fang Exp $
 */

#ifndef	__POOL_MODULE_A_H__
#define	__POOL_MODULE_A_H__

#include "util/memory/pointer_classes.h"
#include "util/memory/list_vector_pool_fwd.h"
#include "util/what.h"

USING_CONSTRUCT
using util::what;
using namespace util::memory;

class twiddle_dum {
private:
	typedef	twiddle_dum		this_type;
public:
	twiddle_dum();
	~twiddle_dum();

	void
	foo(void) const;

public:
	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_ROBUST_STATIC_DECLARATIONS

};	// end class twiddle_dum

namespace util {
	SPECIALIZE_UTIL_WHAT_DECLARATION(twiddle_dum)
}	// end namespace util

#endif	// __POOL_MODULE_A_H__

