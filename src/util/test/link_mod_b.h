/**
	\file "link_mod_b.h"
	Testing ld linker characteristics.
	$Id: link_mod_b.h,v 1.2.46.1 2005/08/16 03:50:34 fang Exp $
 */

#ifndef	__POOL_MODULE_B_H__
#define	__POOL_MODULE_B_H__

#include "util/what.h"

using util::what;

class twiddle_dee {
private:
	typedef	twiddle_dee		this_type;
public:
	twiddle_dee();
	~twiddle_dee();

	void
	foo(void) const;

public:

};	// end class twiddle_dee

namespace util {
	SPECIALIZE_UTIL_WHAT_DECLARATION(twiddle_dee)
}	// end namespace util

#endif	// __POOL_MODULE_B_H__

