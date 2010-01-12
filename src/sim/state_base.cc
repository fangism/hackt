/**
	\file "sim/state_base.cc"
	$Id: state_base.cc,v 1.3.24.1 2010/01/12 02:49:01 fang Exp $
 */

#include <iostream>
#include <string>
#include "sim/state_base.h"
#include "Object/module.h"

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
state_base::dump_memory_usage(ostream& o) const {
	// TODO: report definitions' footprints' memory usage
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	return o << "FINISH ME: sim::state_base::dump_memory_usage" << endl;
#else
	return mod.get_state_manager().dump_memory_usage(o);
#endif
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

