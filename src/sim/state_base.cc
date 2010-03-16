/**
	\file "sim/state_base.cc"
	$Id: state_base.cc,v 1.3.24.2 2010/03/16 21:23:57 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <iostream>
#include <string>
#include "sim/state_base.h"
#include "Object/module.h"
#include "util/stacktrace.h"

namespace HAC {
namespace SIM {
#include "util/using_ostream.h"
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
using entity::footprint;
#endif

//=============================================================================
// class state_base method defintions

state_base::state_base(const module& m, const string& p) :
		mod(m),
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
		frame_cache(0, std::make_pair(
			footprint_frame(m.get_footprint()), 
			global_offset())), 
		top_context(frame_cache.value.first, frame_cache.value.second), 
#endif
		prompt(p), ifstreams() {
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
	const footprint& topfp(m.get_footprint());
	// default constructed global_offset = 0s
	const global_offset& g(frame_cache.value.second);
	// contruct top footprint frame once, and keep around permanently
	frame_cache.value.first.construct_top_global_context(topfp, g);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if ENABLE_STACKTRACE
static
void
cache_entry_dump(ostream& o, const global_entry_context::cache_entry_type&) {
	o << "pair<frame,offset>";
}
#endif

state_base::~state_base() {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	frame_cache.dump(std::cerr, &cache_entry_dump) << std::endl;
#endif
}

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

