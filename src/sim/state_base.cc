/**
	\file "sim/state_base.cc"
	$Id: state_base.cc,v 1.1.2.1 2006/12/08 22:33:55 fang Exp $
 */

#include <iostream>
#include <string>
#include "sim/state_base.h"

namespace HAC {
namespace SIM {
#include "util/using_ostream.h"

//=============================================================================
// class state_base method defintions

state_base::state_base(const module& m, const string& p) :
		mod(m), prompt(p), ifstreams() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
state_base::~state_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --
ostream&
state_base::dump_source_paths(ostream& o) const {
	o << "source paths:" << endl;
	return ifstreams.dump_paths(o);
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

