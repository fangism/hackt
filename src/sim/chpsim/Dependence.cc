/**
	\file "sim/chpsim/Dependence.cc"
	$Id: Dependence.cc,v 1.1.2.1 2006/12/11 00:40:17 fang Exp $
 */

#include "sim/chpsim/Dependence.h"
#include <iostream>
#include <iterator>
#include <algorithm>
#include "util/STL/valarray_iterator.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
using std::begin;
using std::end;
using std::copy;
#include "util/using_ostream.h"
//=============================================================================
// class DependenceSet method definitions

ostream&
DependenceSet::dump(ostream& o) const {
	std::ostream_iterator<node_index_type> osi(o, " ");
if (bool_set.size()) {
	o << "bool deps: ";
	copy(begin(bool_set), end(bool_set), osi);
	o << endl;
}
if (int_set.size()) {
	o << "int deps: ";
	copy(begin(int_set), end(int_set), osi);
	o << endl;
}
if (channel_set.size()) {
	o << "channel deps: ";
	copy(begin(channel_set), end(channel_set), osi);
	o << endl;
}
	return o;
}

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

