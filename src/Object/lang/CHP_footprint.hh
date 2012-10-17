/**
	\file "Object/lang/CHP_footprint.hh"
	Class definitions for unrolled CHP action instances.  
	$Id: CHP_footprint.hh,v 1.4 2007/09/11 06:52:48 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CHP_FOOTPRINT_H__
#define	__HAC_OBJECT_LANG_CHP_FOOTPRINT_H__

#include <iosfwd>
#include <vector>
#include "Object/lang/CHP_event.hh"
// #include "Object/expr/types.hh"

namespace HAC {
namespace entity {
struct expr_dump_context;
namespace CHP {
using std::ostream;
using std::istream;

//=============================================================================
/**
	The CHP footprint consists of a map of locally allocated events, 
	based on the set of CHP actions in the process definition.
	Event successor/predecessor relationships are referenced
	using local indices because events cannot directly cross processes.  
	References, however cannot be resolved at compile-time.  
	During global allocation, each process's events can be bulk allocated
	by essentially copying the event footprint.
 */
class local_event_footprint : public std::vector<local_event> {
	// parent process/channel footprint? need back-link?
	typedef	local_event			event_type;
	typedef	std::vector<event_type>	event_pool_type;
	typedef	event_type::event_index_type	event_index_type;
	event_pool_type				event_pool;
public:
	class dot_graph_wrapper {
		ostream&			ostr;
	public:
		explicit
		dot_graph_wrapper(ostream&, const char* n = NULL);

		~dot_graph_wrapper();
	};	// end struct dot_graph_wrapper
public:
#if 0
	local_event_footprint();
	~local_event_footprint();
#else
	// default for now
#endif
	ostream&
	dump(ostream&, const expr_dump_context&) const;

	ostream&
	dump_struct_dot(ostream&, const SIM::CHPSIM::graph_options&) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class footprint

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CHP_FOOTPRINT_H__

