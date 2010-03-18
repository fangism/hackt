/**
	\file "sim/state_base.cc"
	$Id: state_base.cc,v 1.3.24.5 2010/03/18 21:58:12 fang Exp $
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
using entity::bool_tag;

//=============================================================================
// class state_base method defintions

state_base::state_base(const module& m, const string& p) :
		mod(m),
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
		frame_cache(0, std::make_pair(
			footprint_frame(m.get_footprint()), 
			global_offset())), 
		top_context(frame_cache.value.first, frame_cache.value.second), 
#if HOT_CACHE_FRAMES
		cache_lru(0),
#endif
#endif
		prompt(p), ifstreams() {
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
	const footprint& topfp(m.get_footprint());
	// default constructed global_offset = 0s
	const global_offset& g(frame_cache.value.second);
	// contruct top footprint frame once, and keep around permanently
	frame_cache.value.first.construct_top_global_context(topfp, g);
#if HOT_CACHE_FRAMES
	// initially empty cache
	hot_cache[0].first = size_t(-1);
	hot_cache[1].first = size_t(-1);
#endif
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
static
void
cache_entry_dump(ostream& o, const global_entry_context::cache_entry_type&) {
	o << "pair<frame,offset>\n";
}
#endif

state_base::~state_base() {
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	dump_frame_cache(std::cerr);
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
/** 
	Returns the local-to-global node translation map for process pid.
	This *really* should be inlined...
	NOTE: this operation could become prohibitively expensive, 
		reconstructing the footprint_frame of global indices each time.
	TODO: create a reasonable size cache to store these, keyed by pid.
	\param pid is 1-based global process index.
	\return frame containing global bool ids for this process
 */
#if MEMORY_MAPPED_GLOBAL_ALLOCATION && !CACHE_GLOBAL_FOOTPRINT_FRAMES
footprint_frame
#else
const footprint_frame&
#endif
state_base::get_footprint_frame(const size_t pid) const {
//	cerr << "<pid:" << pid << '>' << endl;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
//	STACKTRACE_VERBOSE;
//	STACKTRACE_INDENT_PRINT("pid = " << pid << endl);
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
#if HOT_CACHE_FRAMES
	// check LRU before tree cache
	const size_t second = 1-cache_lru;
	if (hot_cache[cache_lru].first == pid) {
		// hit most LRU
		STACKTRACE_INDENT_PRINT("LRU hit 1 @" << pid << endl);
		return hot_cache[cache_lru].second.first;
	} else if (hot_cache[second].first == pid) {
		STACKTRACE_INDENT_PRINT("LRU hit 2 @" << pid << endl);
		// hit second most LRU
		cache_lru = second;
		return hot_cache[cache_lru].second.first;
	} else {
		STACKTRACE_INDENT_PRINT("LRU miss  @" << pid << endl);
		// miss hot cache, replace second most LRU
		cache_lru = second;
		cache_entry_type& ret(hot_cache[cache_lru].second);
		hot_cache[cache_lru].first = pid;
		// copy over
		ret = top_context.lookup_global_footprint_frame_cache(
			pid, &frame_cache);
		return ret.first;
	}
#else
	const footprint_frame&
		ret(top_context.lookup_global_footprint_frame_cache(pid,
		&frame_cache).first);
#if ENABLE_STACKTRACE
	ret.dump_frame(STACKTRACE_INDENT_PRINT("frame:")) << endl;
#endif
	return ret.get_frame_map<bool_tag>();
#endif	// HOT_CACHE_FRAMES
#else
	footprint_frame ret;
	global_offset g;
	const footprint_frame tff(mod.get_footprint());
	const global_entry_context top_context(tff, g);
	top_context.construct_global_footprint_frame(ret, g, pid);
#if ENABLE_STACKTRACE
//	ret.dump_frame(STACKTRACE_INDENT_PRINT("frame:")) << endl;
#endif
	return ret.get_frame_map<bool_tag>();	// copy
#endif	// CACHE_GLOBAL_FOOTPRINT_FRAME
#else
	return get_module().get_state_manager().get_bool_frame_map(pid);
#endif	// MEMORY_MAPPED_GLOBAL_ALLOCATION
}

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

