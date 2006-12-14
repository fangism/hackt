/**
	\file "sim/chpsim/Dependence.cc"
	$Id: Dependence.cc,v 1.1.2.2 2006/12/14 00:13:59 fang Exp $
 */

#include "sim/chpsim/Dependence.h"
#include "sim/chpsim/DependenceCollector.h"
#include <iostream>
#include <iterator>
#include <algorithm>
#include "util/STL/valarray_iterator.h"
#include "util/iterator_more.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
using std::begin;
using std::end;
using std::copy;
#include "util/using_ostream.h"
//=============================================================================
// class DependenceSet method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
DependenceSet::import(const DependenceSetCollector& d) {
	bool_set.resize(d.bool_set.size());
	copy(d.bool_set.begin(), d.bool_set.end(), begin(bool_set));
	int_set.resize(d.int_set.size());
	copy(d.int_set.begin(), d.int_set.end(), begin(int_set));
	channel_set.resize(d.channel_set.size());
	copy(d.channel_set.begin(), d.channel_set.end(), begin(channel_set));
}

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

