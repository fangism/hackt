/**
	\file "discrete_interval_set_fwd.h"
	Forward declarations for interval set class over integer-like values.
	$Id: discrete_interval_set_fwd.h,v 1.6 2005/02/27 22:54:21 fang Exp $
 */

// by David Fang, Cornell University, 2004

#ifndef	__UTIL_DISCRETE_INTERVAL_SET_FWD_H__
#define	__UTIL_DISCRETE_INTERVAL_SET_FWD_H__

#include <iosfwd>

#define DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE	template <class T>

//=============================================================================
namespace util {
using std::ostream;

// forward declarations
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
class discrete_interval_set;

template <class U>
ostream& operator << (ostream& o, const discrete_interval_set<U>& r);

//=============================================================================
}	// end namespace util

#endif	// __UTIL_DISCRETE_INTERVAL_SET_FWD_H__

