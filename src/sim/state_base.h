/**
	\file "sim/state_base.h"
	Facilities common to all simulator states.  (Recommended)
	$Id: state_base.h,v 1.6 2011/05/03 19:20:55 fang Exp $
 */

#ifndef	__HAC_SIM_STATE_BASE_H__
#define	__HAC_SIM_STATE_BASE_H__

#include <iosfwd>
#include "Object/devel_switches.h"
#include "util/string_fwd.h"
#include "util/named_ifstream_manager.h"
#include "util/tokenize_fwd.h"

/**
	Define to 1 to keep around a cache of global footprint frames.  
	This brings dramatic speedup, by reducing the number of
	global frame computations.
 */
#define	CACHE_GLOBAL_FOOTPRINT_FRAMES	1

#if CACHE_GLOBAL_FOOTPRINT_FRAMES
#include "Object/global_context_cache.h"
#endif

namespace HAC {
namespace entity {
	class module;
}
namespace SIM {
using std::ostream;
using std::istream;
using std::string;
using entity::module;
using util::string_list;
using util::ifstream_manager;
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
using entity::footprint_frame;
using entity::global_context_cache;
#endif

//=============================================================================
/**
	Common facilities for various simulator states.  
 */
class state_base {
protected:
	/**
		Interpreter prompt string.
	 */
	string						prompt;

	/**
		Interpreter state for the input stream.
		This is not checkpointed.  
	 */
	ifstream_manager				ifstreams;
private:
	/// private, undefined copy-ctor (non-copyable)
	state_base(const state_base&);

public:
	state_base();

	explicit
	state_base(const string&);

	~state_base();

	ifstream_manager&
	get_stream_manager(void) { return ifstreams; }

	const string&
	get_prompt(void) const { return prompt; }

	template <class L>
	void
	import_source_paths(const L& l) {
		typedef	typename L::const_iterator	const_iterator;
		const_iterator i(l.begin()), e(l.end());
		for ( ; i!=e; ++i) {
			ifstreams.add_path(*i);
		}
	}

	void
	add_source_path(const string& s) {
		ifstreams.add_path(s);
	}

	ostream&
	dump_source_paths(ostream&) const;

};	// end class state_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For simulators that use a hac module.
 */
class module_state_base : public state_base
#if CACHE_GLOBAL_FOOTPRINT_FRAMES && !FOOTPRINT_OWNS_CONTEXT_CACHE
	, public global_context_cache
#endif
{
protected:
	/**
		Attachment to the source object that contains
		whole program hierarchical and allocation information.  
		TODO: reduce this to just the top_footprint, now that
		back-ends are hierarchical?
	 */
	const module&					mod;
public:
	module_state_base(const module&, const string&);
	~module_state_base();

	const module&
	get_module(void) const { return mod; }

};	// end class module_state_base

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_STATE_BASE_H__

