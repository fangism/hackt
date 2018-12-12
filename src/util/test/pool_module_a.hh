/**
	\file "pool_module_a.hh"
	Header for one module of a multimodule memory pool test.
	$Id: pool_module_a.hh,v 1.4 2005/09/04 21:15:13 fang Exp $
 */

#ifndef	__POOL_MODULE_A_H__
#define	__POOL_MODULE_A_H__

#include "util/memory/pointer_classes.hh"
#include "util/memory/list_vector_pool_fwd.hh"
#include "util/what.hh"

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
	// LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_ROBUST_STATIC_DECLARATIONS

};	// end class twiddle_dum

namespace util {
	SPECIALIZE_UTIL_WHAT_DECLARATION(twiddle_dum)
}	// end namespace util

#endif	// __POOL_MODULE_A_H__

