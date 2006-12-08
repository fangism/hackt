/**
	\file "sim/state_base.h"
	Facilities common to all simulator states.  (Recommended)
	$Id: state_base.h,v 1.1.2.1 2006/12/08 22:33:57 fang Exp $
 */

#ifndef	__HAC_SIM_STATE_BASE_H__
#define	__HAC_SIM_STATE_BASE_H__

#include <iosfwd>
#include "util/string_fwd.h"
#include "util/named_ifstream_manager.h"
#include "util/tokenize_fwd.h"

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

//=============================================================================
/**
	Common facilities for various simulator states.  
 */
class state_base {
protected:
	/**
		Attachment to the source object that contains
		whole programm hierarchical and allocation information.  
	 */
	const module&					mod;
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
	state_base(const module&, const string&);
	~state_base();

	const module&
	get_module(void) const { return mod; }

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

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_STATE_BASE_H__

