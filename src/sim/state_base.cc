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
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
using entity::footprint;
#endif
using entity::bool_tag;
using entity::global_process_context;

//=============================================================================
// class state_base method defintions

state_base::state_base() : prompt(), ifstreams() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
state_base::state_base(const string& p) :
		prompt(p), ifstreams() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
state_base::~state_base() { }

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
		mod(m)
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
		, frame_cache(0, cache_entry_type(m.get_footprint()))
		, top_context(frame_cache.value)
#if HOT_CACHE_FRAMES
		, cache_lru(0)
#endif
#endif
		{
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
	const footprint& topfp(m.get_footprint());
	// default constructed global_offset = 0s
	const global_offset& g(frame_cache.value.offset);
	// contruct top footprint frame once, and keep around permanently
	frame_cache.value.frame.construct_top_global_context(topfp, g);
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

module_state_base::~module_state_base() {
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
module_state_base::halve_cache(void) {
	return frame_cache.halve();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
module_state_base::dump_frame_cache(ostream& o) const {
	o << "footprint-frame cache (" << frame_cache.size() <<
		" entries):" << std::endl;
	frame_cache.dump(o, &cache_entry_dump) << std::endl;
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only returns the frame portion of the context.
 */
#if !CACHE_GLOBAL_FOOTPRINT_FRAMES
footprint_frame
#else
const footprint_frame&
#endif
module_state_base::get_footprint_frame(const size_t pid) const {
	return get_global_context(pid).frame;
}

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
const module_state_base::cache_entry_type&
module_state_base::get_global_context(const size_t pid) const {
//	cerr << "<pid:" << pid << '>' << endl;
//	STACKTRACE_VERBOSE;
//	STACKTRACE_INDENT_PRINT("pid = " << pid << endl);
	// special case for top-level
	if (!pid) {
		// this is permanent
		// return top_context.get_footprint_frame();
		return frame_cache.value;
	}
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
#if HOT_CACHE_FRAMES
	// check LRU before tree cache
	const size_t second = 1-cache_lru;
	if (hot_cache[cache_lru].first == pid) {
		// hit most LRU
		STACKTRACE_INDENT_PRINT("LRU hit 1 @" << pid << endl);
		return hot_cache[cache_lru].second;
	} else if (hot_cache[second].first == pid) {
		STACKTRACE_INDENT_PRINT("LRU hit 2 @" << pid << endl);
		// hit second most LRU
		cache_lru = second;
		return hot_cache[cache_lru].second;
	} else {
		STACKTRACE_INDENT_PRINT("LRU miss  @" << pid << endl);
		// miss hot cache, replace second most LRU
		cache_lru = second;
		cache_entry_type& ret(hot_cache[cache_lru].second);
		hot_cache[cache_lru].first = pid;
		// copy over to hot_cache
		const global_entry_context tgc(top_context);
		ret = tgc.lookup_global_footprint_frame_cache(
			pid, &frame_cache);
		return ret;
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
	// this was not updated to include global_offset
	const global_process_context pc(mod, pid);
	const footprint_frame& ret(pc.frame);
	return ret.get_frame_map<bool_tag>();	// copy
#endif	// CACHE_GLOBAL_FOOTPRINT_FRAME
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
module_state_base::dump_memory_usage(ostream& o) const {
	// TODO: report definitions' footprints' memory usage
	// TODO: sum of frame sizes, accumulate over all entries
	// tree-cache: ability to gather pointers to all entries?
	typedef	frame_cache_type		value_type;
	const size_t n = frame_cache.size();
	o << "frame-cache: (" << n << " * " << sizeof_tree_node(value_type)
		<< " B/entry) = " << n * sizeof_tree_node(value_type)
		<< " B" << endl;
	return o;
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

