/**
	\file "discrete_interval_set_fwd.h"
	Forward declarations for interval set class over integer-like values.
	$Id: discrete_interval_set_fwd.h,v 1.3 2004/11/02 07:52:13 fang Exp $
 */

// by David Fang, Cornell University, 2004

#ifndef	__DISCRETE_INTERVAL_SET_FWD_H__
#define	__DISCRETE_INTERVAL_SET_FWD_H__

#include <iosfwd>

#ifndef DISCRETE_INTERVAL_SET_NAMESPACE
#define DISCRETE_INTERVAL_SET_NAMESPACE		intvl_set_ns
#endif

//=============================================================================
/**
	Namespace for discrete interval sets, and related classes.
 */
namespace DISCRETE_INTERVAL_SET_NAMESPACE {
using namespace std;		// for map<>

// forward declarations
template <class T> class discrete_interval_set;

template <class U>
ostream& operator << (ostream& o, const discrete_interval_set<U>& r);

//=============================================================================
}	// end namespace DISCRETE_INTERVAL_SET_NAMESPACE

#endif	//	__DISCRETE_INTERVAL_SET_FWD_H__

