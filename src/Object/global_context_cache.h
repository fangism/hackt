/**
	\file "Object/global_context_cache.h"
	Facilities common to all simulator states.  (Recommended)
	$Id: state_base.h,v 1.6 2011/05/03 19:20:55 fang Exp $
 */

#ifndef	__HAC_OBJECT_GLOBAL_CONTEXT_CACHE_H__
#define	__HAC_OBJECT_GLOBAL_CONTEXT_CACHE_H__

#include <iosfwd>

/**
	Define to 1 to keep around a cache of global footprint frames.  
	This brings dramatic speedup, by reducing the number of
	global frame computations.
 */
// #define	CACHE_GLOBAL_FOOTPRINT_FRAMES			1

/**
	Define to 1 to keep around the last two footprint frames (LRU).  
	This brings a little bit more speedup.
 */
#define HOT_CACHE_FRAMES		1

#include "Object/global_entry_context.h"
#include "Object/global_entry.h"		// for footprint_frame
#include "util/tree_cache.h"

namespace HAC {
namespace entity {
using std::ostream;
using std::istream;
using std::string;
using entity::module;

//=============================================================================
/**
	This struct should be associated with each module instance,
	of which there is usually only one.
 */
class global_context_cache {
public:
	typedef	global_process_context	cache_entry_type;
	typedef	util::tree_cache<size_t, cache_entry_type>
							frame_cache_type;

protected:
	mutable frame_cache_type			frame_cache;
	// keep around a permanent top-context
public:
	const global_entry_context			top_context;
private:
#if HOT_CACHE_FRAMES
	// just keep two most recent entries
	// key: global pid
	// value: copies of the most recent cache hits
	mutable std::pair<size_t, cache_entry_type>	hot_cache[2];
	mutable size_t					cache_lru;
#endif
public:
	explicit
	global_context_cache(const footprint&);
	~global_context_cache();

	const footprint_frame&
	get_footprint_frame(const size_t pid) const;

	const cache_entry_type&
	get_global_context(const size_t pid) const;

	const frame_cache_type&
	get_frame_cache(void) const { return frame_cache; }

	static
	frame_cache_type*
	lookup_local_footprint_frame_cache(const size_t lpid, 
		const footprint&, frame_cache_type*);

	const cache_entry_type&
	lookup_global_footprint_frame_cache(const size_t gpid) const;

	size_t
	halve_cache(void);

	ostream&
	dump_frame_cache(ostream&) const;

	ostream&
	dump_memory_usage(ostream&) const;

};	// end class global_context_cache

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_GLOBAL_CONTEXT_CACHE_H__

