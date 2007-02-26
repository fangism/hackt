/**
	\file "Object/lang/CHP_base.h"
	Class definitions for CHP-related objects.  
	$Id: CHP_base.h,v 1.9 2007/02/26 22:00:50 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CHP_BASE_H__
#define	__HAC_OBJECT_LANG_CHP_BASE_H__

#include "util/persistent.h"
#include "util/memory/count_ptr.h"
// #include "util/STL/vector_fwd.h"
#include "Object/ref/reference_enum.h"
#include "Object/devel_switches.h"
#include "sim/chpsim/devel_switches.h"
#if CHPSIM_STATE_UPDATE_BIN_SETS
#include "Object/ref/reference_set.h"
#endif

namespace HAC {
namespace SIM {
namespace CHPSIM {
	// we need some sort of CHP_visitor refinement!
	class StateConstructor;
	class nonmeta_context;
	class EventNode;
}	// end namespace CHPSIM
}	// end namespace SIM
namespace entity {
struct expr_dump_context;
class unroll_context;
class nonmeta_state_manager;
#if CHP_ACTION_DELAYS
class preal_expr;
#endif
/**
	Namespace for CHP object classes.  
 */
namespace CHP {
using entity::nonmeta_state_manager;
using std::ostream;
using std::istream;
using SIM::CHPSIM::StateConstructor;
using SIM::CHPSIM::nonmeta_context;
using util::persistent;
using util::persistent_object_manager;
class action;
using util::memory::count_ptr;
typedef	count_ptr<const action>			action_ptr_type;
#if CHP_ACTION_DELAYS
typedef	count_ptr<const  preal_expr>		delay_ptr_type;
#endif

//=============================================================================
/**
	Abstract base class for type-checked CHP statements.  
 */
class action : public persistent {
public:
	typedef	action_ptr_type			unroll_return_type;
#if CHPSIM_STATE_UPDATE_BIN_SETS
	typedef	entity::global_references_set	execute_arg_type;
#else
	typedef	global_references_array_type	execute_arg_type;
#endif
	/**
		TODO: Eventually generalize this to attribute list.  
	 */
	typedef	delay_ptr_type			attributes_type;
protected:
#if CHP_ACTION_DELAYS
	attributes_type				delay;
#endif
	action();
#if CHP_ACTION_DELAYS
	explicit
	action(const attributes_type&);
#endif
public:
virtual	~action();

#if CHP_ACTION_DELAYS
	void
	set_delay(const delay_ptr_type&);

	const delay_ptr_type&
	get_delay(void) const { return delay; }
#endif

virtual	ostream&
	dump(ostream&, const expr_dump_context&) const = 0;

/**
	Dumps single events only, silent for grouped events.  
 */
#define	CHP_DUMP_EVENT_PROTO						\
	ostream&							\
	dump_event(ostream&, const expr_dump_context&) const

virtual	CHP_DUMP_EVENT_PROTO = 0;

	ostream&
	dump_attributes(ostream&, const expr_dump_context&) const;

#if CHP_ACTION_DELAYS
	ostream&
	dump_event_with_attributes(ostream&, const expr_dump_context&) const;
#endif

#define	CHP_DUMP_SUCCESSORS_PROTO					\
	ostream&							\
	dump_successor_edges(ostream&, const SIM::CHPSIM::EventNode&,	\
		const size_t, const expr_dump_context&) const

virtual	CHP_DUMP_SUCCESSORS_PROTO;

	/**
		unroll_context-binding functor.  
	 */
	struct transformer {
		const unroll_context&		_context;

		explicit
		transformer(const unroll_context& c) : _context(c) { }

		action_ptr_type
		operator () (const action_ptr_type&) const;
	};

#define	CHP_UNROLL_ACTION_PROTO						\
	action_ptr_type							\
	unroll_resolve_copy(const unroll_context&,			\
		const action_ptr_type&) const

virtual	CHP_UNROLL_ACTION_PROTO = 0;

#define	CHP_ACTION_ACCEPT_PROTO						\
	void								\
	accept(StateConstructor&) const

virtual	CHP_ACTION_ACCEPT_PROTO = 0;

#define	CHP_EXECUTE_PROTO						\
	void								\
	execute(const nonmeta_context&, execute_arg_type&) const

virtual	CHP_EXECUTE_PROTO = 0;

/**
	\return true if the invoking event should be enqueued.
 */
#define	CHP_RECHECK_PROTO						\
	char								\
	recheck(const nonmeta_context&) const

virtual	CHP_RECHECK_PROTO = 0;

// these were added just for the delay member pointer
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class action

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CHP_BASE_H__

