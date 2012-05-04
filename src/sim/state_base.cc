/**
	\file "sim/state_base.cc"
	$Id: state_base.cc,v 1.7 2011/05/03 19:20:55 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <iostream>
#include <string>
#include "sim/state_base.h"
#include "Object/module.h"
#include "Object/global_entry.h"
#include "util/stacktrace.h"

namespace HAC {
namespace SIM {
#include "util/using_ostream.h"

//=============================================================================
// class state_base method defintions

state_base::state_base() : prompt(), ifstreams() {
	STACKTRACE_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
state_base::state_base(const string& p) :
		prompt(p), ifstreams() {
	STACKTRACE_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
state_base::~state_base() {
	STACKTRACE_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
state_base::dump_source_paths(ostream& o) const {
	o << "source paths:" << endl;
	return ifstreams.dump_paths(o);
}

//=============================================================================
// class module_state_base method definitions

module_state_base::module_state_base(const module& m, const string& p) :
		state_base(p), 
#if CACHE_GLOBAL_FOOTPRINT_FRAMES && !MODULE_OWNS_CONTEXT_CACHE
		global_context_cache(m.get_footprint()),
#endif
		mod(m) {
	STACKTRACE_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
module_state_base::~module_state_base() {
	STACKTRACE_VERBOSE;
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

