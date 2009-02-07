/**
	\file "sim/prsim/util.h"
	Random utility functions.
	These really belong somewhere else.
 */
#ifndef	__HAC_SIM_PRSIM_UTIL_H__
#define	__HAC_SIM_PRSIM_UTIL_H__

#include <iosfwd>

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::ostream;

//=============================================================================
/**
	Convenient repetitive dump function.  
 */
template <class MapType>
static
ostream&
dump_pair_map(ostream& o, const MapType& m);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints an index-value pair.
 */
template <class ListType>
static
ostream&
dump_pair_vector(ostream& o, const ListType& m);

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_UTIL_H__


