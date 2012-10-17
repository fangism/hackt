/**
	\file "main/create_options.cc"
	$Id: create_options.cc,v 1.1 2010/08/05 18:25:35 fang Exp $
 */

#include <iostream>
#include "main/create_options.hh"
#include "util/IO_utils.tcc"

namespace HAC {
using util::write_value;
using util::read_value;
#include "util/using_ostream.hh"

//=============================================================================
// global variable
create_options
global_create_options;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// textual dump of options
ostream&
create_options::dump(ostream& o) const {
	o << "create-phase options:" << endl;
	o << "\tcanonicalize-shortest-";
	switch (canonicalize_mode) {
	case SHORTEST_HIER_NO_LENGTH: o << "hier"; break;
	case SHORTEST_HIER_MIN_LENGTH: o << "length"; break;
	case SHORTEST_EMULATE_ACT: o << "stage"; break;
	default: o << "???";
	}
	o << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
create_options::write_object(ostream& o) const {
	write_value(o, canonicalize_mode);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
create_options::load_object(istream& i) {
	read_value(i, canonicalize_mode);
}

//=============================================================================
}	// end namespace HAC

