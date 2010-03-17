/**
	\file "sim/state_base.cc"
	$Id: state_base.cc,v 1.3.24.3 2010/03/17 02:11:39 fang Exp $
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
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
static
void
cache_entry_dump(ostream& o, const global_entry_context::cache_entry_type&) {
	o << "pair<frame,offset>";
}
#endif

state_base::~state_base() {
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	dump_cache_stats(std::cerr);
#endif
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
size_t
state_base::halve_cache(void) {
	return frame_cache.halve();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
state_base::dump_frame_cache(ostream& o) const {
	o << "footprint-frame cache (" << frame_cache.size() <<
		" entries):" << std::endl;
	frame_cache.dump(o, &cache_entry_dump) << std::endl;
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
state_base::dump_source_paths(ostream& o) const {
	o << "source paths:" << endl;
	return ifstreams.dump_paths(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// copied from State-prsim.cc
#ifdef HAVE_STL_TREE
#define	sizeof_tree_node(type)	sizeof(std::_Rb_tree_node<type>)
#else
	// assume tree/set/map nodes have 3 pointers +enum color
	static const size_t tree_node_base_size = (3*(sizeof(void*)) +1);
#define	sizeof_tree_node(type)	(sizeof(type) +tree_node_base_size)
#endif

ostream&
state_base::dump_memory_usage(ostream& o) const {
	// TODO: report definitions' footprints' memory usage
	// TODO: sum of frame sizes, accumulate over all entries
	// tree-cache: ability to gather pointers to all entries?
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	typedef	frame_cache_type		value_type;
	const size_t n = frame_cache.size();
	o << "frame-cache: (" << n << " * " << sizeof_tree_node(value_type)
		<< " B/entry) = " << n * sizeof_tree_node(value_type)
		<< " B" << endl;
	return o;
#else
	return mod.get_state_manager().dump_memory_usage(o);
#endif
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

