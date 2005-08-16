/**
	\file "link_mod_a.h"
	Testing ld linker characteristics.
	$Id: link_mod_a.h,v 1.2.46.1 2005/08/16 03:50:34 fang Exp $
 */

#ifndef	__POOL_MODULE_A_H__
#define	__POOL_MODULE_A_H__

#include "util/what.h"

using util::what;

class twiddle_dum {
private:
	typedef	twiddle_dum		this_type;
public:
	twiddle_dum();
	~twiddle_dum();

	void
	foo(void) const;

public:

};	// end class twiddle_dum

namespace util {
	SPECIALIZE_UTIL_WHAT_DECLARATION(twiddle_dum)
}	// end namespace util

#endif	// __POOL_MODULE_A_H__

