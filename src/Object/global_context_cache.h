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

#include "Object/global_entry_context.h"
#include "Object/global_entry.h"		// for footprint_frame
#include "util/tree_cache.h"

/**
	Define to 1 to keep around the last two footprint frames (LRU).  
	This brings a little bit more speedup.
 */
#define HOT_CACHE_FRAMES		1


namespace HAC {
namespace entity {
struct global_entry_context;
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
	// CAUTION: dangling pointers if cache ages out?
	mutable std::pair<size_t, const frame_cache_type*>
							hot_cache[2];
	mutable size_t					cache_lru;
#endif
public:
	explicit
	global_context_cache(const footprint&);
	~global_context_cache();

	const frame_cache_type&
	get_global_context(const size_t pid) const;

	frame_cache_type&
	get_frame_cache(void) const { return frame_cache; }

	static
	frame_cache_type*
	lookup_local_footprint_frame_cache(const size_t lpid, 
		const footprint&, frame_cache_type*);

	const frame_cache_type*
	lookup_global_footprint_frame_cache(const size_t gpid) const;

	size_t
	halve_cache(void);

	ostream&
	dump_frame_cache(ostream&) const;

	ostream&
	dump_memory_usage(ostream&) const;

};	// end class global_context_cache

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Same as above, but with extra global process id field.
	This is useful as a return type from instance-reference lookups.
 */
struct global_process_context_ref {
	typedef	global_context_cache::frame_cache_type
						frame_cache_type;
	size_t					gpid;
	// pointer to sub-cache, which contains .value = global_process_context
	frame_cache_type*			subcache;

	// usually default ctor
	global_process_context_ref() : gpid(0), subcache(NULL) { }

	explicit
	global_process_context_ref(const footprint&);

	const global_process_context&
	get_context(void) const {
	//	NEVER_NULL(subcache);
		return subcache->value;
	}

	const footprint_frame&
	get_frame(void) const {
		return get_context().frame;
	}

	const global_offset&
	get_offset(void) const {
		return get_context().offset;
	}

	/**
		Use this for both descend_frame and descend_port.
		Doesn't do any copying, just pointer moving.
	 */
	void
	descend(const size_t lpid, const footprint& topfp) {
		subcache = global_context_cache::
			lookup_local_footprint_frame_cache(
				lpid, topfp, subcache);
		gpid = lpid;
	}

	void
	descend(frame_cache_type* c, 
			const size_t lpid, const footprint& topfp) {
		subcache = global_context_cache::
			lookup_local_footprint_frame_cache(lpid, topfp, c);
		gpid = lpid;
	}

};	// end struct global_process_context_id

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_GLOBAL_CONTEXT_CACHE_H__

