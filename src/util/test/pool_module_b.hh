/**
	\file "pool_module_b.hh"
	Header for one module of a multimodule memory pool test.
	$Id: pool_module_b.hh,v 1.4 2005/09/04 21:15:13 fang Exp $
 */

#ifndef	__POOL_MODULE_B_H__
#define	__POOL_MODULE_B_H__

#include "util/memory/pointer_classes.hh"
#include "util/memory/list_vector_pool_fwd.hh"
#include "util/what.hh"

USING_CONSTRUCT
using util::what;
using namespace util::memory;

class twiddle_dee {
private:
	typedef	twiddle_dee		this_type;
public:
	twiddle_dee();
	~twiddle_dee();

	void
	foo(void) const;

public:
	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_ROBUST_STATIC_DECLARATIONS

};	// end class twiddle_dee

namespace util {
	SPECIALIZE_UTIL_WHAT_DECLARATION(twiddle_dee)
}       // end namespace util 

#endif	// __POOL_MODULE_B_H__

