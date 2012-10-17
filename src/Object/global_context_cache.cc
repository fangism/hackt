/**
	\file "Object/global_context_cache.cc"
	$Id: $
 */

#define	ENABLE_STACKTRACE				0

#include <iostream>
#include <string>
#include "Object/global_context_cache.hh"
#include "Object/def/footprint.hh"
#include "Object/inst/state_instance.hh"
#include "Object/inst/instance_pool.hh"
#include "Object/traits/proc_traits.hh"
#include "util/tree_cache.tcc"
#include "util/stacktrace.hh"

// explicit template instantiation
namespace util {
using HAC::entity::global_context_cache;
template class tree_cache<size_t, global_context_cache::cache_entry_type>;
}

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"

//=============================================================================
// class global_context_cache method definitions

global_context_cache::global_context_cache(const footprint& topfp) :
		frame_cache(0, cache_entry_type(topfp)),
		top_context(frame_cache.value)
#if HOT_CACHE_FRAMES
		, cache_lru(0)
#endif
		{
	STACKTRACE_VERBOSE;
	// default constructed global_offset = 0s
	// contruct top footprint frame once, and keep around permanently
	frame_cache.value.construct_top_global_context();
#if HOT_CACHE_FRAMES
	// initially empty cache
	hot_cache[0].first = size_t(-1);
	hot_cache[1].first = size_t(-1);
#endif
#if ENABLE_STACKTRACE
	frame_cache.value.dump_frame(STACKTRACE_STREAM << "top frame: ")
		<< endl;
	STACKTRACE_STREAM << "top offset: " << frame_cache.value.offset << endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
cache_entry_dump(ostream& o, const global_context_cache::cache_entry_type&) {
	o << "pair<frame,offset>\n";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
global_context_cache::~global_context_cache() {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	dump_frame_cache(std::cerr);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
global_context_cache::halve_cache(void) {
#if HOT_CACHE_FRAMES
	// to be safe and not leave dangling ref, invalidate these
	hot_cache[0].first = size_t(-1);
	hot_cache[1].first = size_t(-1);
#endif
	return frame_cache.halve();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
global_context_cache::dump_frame_cache(ostream& o) const {
	o << "footprint-frame cache (" << frame_cache.size() <<
		" entries):" << std::endl;
	frame_cache.dump(o, &cache_entry_dump) << std::endl;
	return o;
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
const global_context_cache::frame_cache_type&
global_context_cache::get_global_context(const size_t pid) const {
	STACKTRACE_VERBOSE;
	// special case for top-level
	if (!pid) {
		// this is permanent
		// return top_context.get_footprint_frame();
		return frame_cache;
	}
#if HOT_CACHE_FRAMES
	// check LRU before tree cache
	const size_t second = 1-cache_lru;
	if (hot_cache[cache_lru].first == pid) {
		// hit most LRU
		STACKTRACE_INDENT_PRINT("LRU hit 1 @" << pid << endl);
		return *hot_cache[cache_lru].second;
	} else if (hot_cache[second].first == pid) {
		STACKTRACE_INDENT_PRINT("LRU hit 2 @" << pid << endl);
		// hit second most LRU
		cache_lru = second;
		return *hot_cache[cache_lru].second;
	} else {
		STACKTRACE_INDENT_PRINT("LRU miss  @" << pid << endl);
		// miss hot cache, replace second most LRU
		cache_lru = second;
		const frame_cache_type*& ret(hot_cache[cache_lru].second);
		hot_cache[cache_lru].first = pid;
		// copy over to hot_cache
//		const global_entry_context& tgc(top_context);
		ret = lookup_global_footprint_frame_cache(pid);
#if ENABLE_STACKTRACE
		ret->value.dump_frame(STACKTRACE_INDENT_PRINT("frame:")) << endl;
#endif
		return *ret;
	}
#else
	const frame_cache_type*
		ret = lookup_global_footprint_frame_cache(pid);
#if ENABLE_STACKTRACE
	ret->value.dump_frame(STACKTRACE_INDENT_PRINT("frame:")) << endl;
#endif
	return *ret;
#endif	// HOT_CACHE_FRAMES
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Looks up sub-cache, refactored out.
	\param lpid is 1-based indexed into local footprint.
 */
// inline
global_context_cache::frame_cache_type*
global_context_cache::lookup_local_footprint_frame_cache(const size_t lpid,
		const footprint& topfp, frame_cache_type* cache) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("lpid = " << lpid << endl);
	typedef	process_tag				Tag;
	typedef	state_instance<Tag>::pool_type		pool_type;
	NEVER_NULL(cache);
	const std::pair<frame_cache_type::child_iterator, bool>
		cp(cache->insert_find(lpid));
if (cp.second) {
	// was a cache miss: re-compute
	STACKTRACE_INDENT_PRINT("local cache miss, computing frame" << endl);
	const cache_entry_type& ret(cache->value);	// parent context
	const footprint& f(*ret.frame._footprint);
	const pool_type& p(f.get_instance_pool<Tag>());
	const size_t ports = p.port_entries();
	cache = &const_cast<frame_cache_type&>(*cp.first); // descend
	cache_entry_type& next(cache->value);
	// expensive child frame construction
	if (lpid > ports) {
		next.descend_frame(ret, lpid, &f == &topfp);
	} else {
		next.descend_port(ret, lpid);
	}
} else {
	// else was a cache hit -- saves a lot of work
	STACKTRACE_INDENT_PRINT("local cache hit, re-using frame" << endl);
	cache = &const_cast<frame_cache_type&>(*cp.first); // descend
}
	return cache;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Follows exact same flow as construct_global_footprint_frame(), above.
	This variation returns a referenced to a cache-managed footprint
	frame, which could either exist from before, or be generated 
	on the fly.
	Performance-critical back-ends should use this.
	Is a member-function because of reference to topfp.
	\param gpid is global index, 1-based
 */
const global_context_cache::frame_cache_type*
global_context_cache::lookup_global_footprint_frame_cache(size_t gpid) const {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("gpid = " << gpid << endl);
	typedef	process_tag				Tag;
	typedef	state_instance<Tag>::pool_type		pool_type;
	// the top footprint frame is always cached, and pre-constructed
	const footprint* const topfp = top_context.fpf->_footprint;
	frame_cache_type* cache = &frame_cache;
	// entry type contains both frame and offset (pair)
if (gpid) {
	// iterative instead of recursive implementation, hence pointers
	const cache_entry_type& ret(cache->value);
	const footprint* cf = ret.frame._footprint;	// topfp->footprint
	const pool_type* p = &cf->get_instance_pool<Tag>();
	size_t local = p->local_entries();	// at_top
	STACKTRACE_INDENT_PRINT("local entries = " << local << endl);
	while (gpid > local) {
		const size_t si = gpid -local;	// 1-based index
		STACKTRACE_INDENT_PRINT("remainder = " << si << endl);
		const pool_private_map_entry_type&
			e(p->locate_private_entry(si -1));	// need 0-base!
		const size_t lpid = e.first;
		gpid = si -e.second;		// still 1-based
		cache = lookup_local_footprint_frame_cache(lpid, *topfp, cache);
		const cache_entry_type& nret(cache->value);
		cf = nret.frame._footprint;
		p = &cf->get_instance_pool<Tag>();
		local = p->local_private_entries();
	}	// end while
	STACKTRACE_INDENT_PRINT("deepest level, owner scope" << endl);
	const size_t ports = p->port_entries();
	const size_t lpid = gpid +ports;
	cache = lookup_local_footprint_frame_cache(lpid, *topfp, cache);
}
	// else refers to top-level
	return cache;
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
global_context_cache::dump_memory_usage(ostream& o) const {
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
#if FOOTPRINT_OWNS_CONTEXT_CACHE
global_process_context_ref::global_process_context_ref(const footprint& f) :
	gpid(0), subcache(&f.get_context_cache().get_frame_cache()) {

}
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

