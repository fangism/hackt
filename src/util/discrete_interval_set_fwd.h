/**
	\file "discrete_interval_set_fwd.h"
	Forward declarations for interval set class over integer-like values.
	$Id: discrete_interval_set_fwd.h,v 1.5 2004/12/16 01:08:53 fang Exp $
 */

// by David Fang, Cornell University, 2004

#ifndef	__DISCRETE_INTERVAL_SET_FWD_H__
#define	__DISCRETE_INTERVAL_SET_FWD_H__

#include <iosfwd>

#ifndef DISCRETE_INTERVAL_SET_NAMESPACE
#define DISCRETE_INTERVAL_SET_NAMESPACE		util
#endif

#define DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE	template <class T>

//=============================================================================
namespace DISCRETE_INTERVAL_SET_NAMESPACE {
using std::ostream;

// forward declarations
DISCRETE_INTERVAL_SET_TEMPLATE_SIGNATURE
class discrete_interval_set;

template <class U>
ostream& operator << (ostream& o, const discrete_interval_set<U>& r);

//=============================================================================
}	// end namespace DISCRETE_INTERVAL_SET_NAMESPACE

#endif	//	__DISCRETE_INTERVAL_SET_FWD_H__

