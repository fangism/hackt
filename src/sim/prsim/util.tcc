/**
	\file "sim/prsim/util.tcc"
	Random utility functions.
	These really belong somewhere else.
 */
#ifndef	__HAC_SIM_PRSIM_UTIL_TCC__
#define	__HAC_SIM_PRSIM_UTIL_TCC__

#include <iostream>
#include "sim/prsim/util.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::endl;

//=============================================================================
/**
	Convenient repetitive dump function.  
 */
template <class MapType>
static
ostream&
dump_pair_map(ostream& o, const MapType& m) {
	typename MapType::const_iterator i(m.begin()), e(m.end());
	for ( ; i!=e; ++i) {
		o << '(' << i->first << "," << i->second << ") ";
	}
	return o << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints an index-value pair.
 */
template <class ListType>
static
ostream&
dump_pair_vector(ostream& o, const ListType& m) {
	typename ListType::const_iterator i(m.begin()), e(m.end());
	size_t j = 0;
	for ( ; i!=e; ++i, ++j) {
		o << '(' << j << "," << *i << ") ";
	}
	return o << endl;
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_UTIL_TCC__


