// "discrete_interval_set_fwd.h"
// forward declarations

// Class for a set of intervals over the set of integers
//	or discrete, integer-like values.  

// will be built-upon to make "multidimensional_sparse_set"

// by David Fang, Cornell University, 2004

#ifndef	__DISCRETE_INTERVAL_SET_FWD_H__
#define	__DISCRETE_INTERVAL_SET_FWD_H__

#include <iosfwd>

#ifndef DISCRETE_INTERVAL_SET_NAMESPACE
#define DISCRETE_INTERVAL_SET_NAMESPACE		fang
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

