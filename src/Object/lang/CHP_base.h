/**
	\file "Object/lang/CHP_base.h"
	Class definitions for CHP-related objects.  
	$Id: CHP_base.h,v 1.7.32.7 2006/12/26 21:26:07 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CHP_BASE_H__
#define	__HAC_OBJECT_LANG_CHP_BASE_H__

#include "util/persistent.h"
#include "util/memory/count_ptr.h"
#include "util/STL/vector_fwd.h"
#include "Object/devel_switches.h"
#include "sim/chpsim/type_enum.h"	// for instance_reference

namespace HAC {
namespace SIM {
namespace CHPSIM {
	// we need some sort of CHP_visitor refinement!
	class StateConstructor;
	class nonmeta_context;
}	// end namespace CHPSIM
}	// end namespace SIM
namespace entity {
struct expr_dump_context;
class unroll_context;
class nonmeta_state_manager;
/**
	Namespace for CHP object classes.  
 */
namespace CHP {
using entity::nonmeta_state_manager;
using std::ostream;
using SIM::CHPSIM::StateConstructor;
using SIM::CHPSIM::nonmeta_context;
using util::persistent;
using util::persistent_object_manager;
class action;
using util::memory::count_ptr;
typedef	count_ptr<const action>			action_ptr_type;
typedef	SIM::CHPSIM::instance_reference		global_reference;
typedef	std::default_vector<global_reference>::type
					update_reference_array_type;

//=============================================================================
/**
	Abstract base class for type-checked CHP statements.  
 */
class action : public persistent {
public:
	typedef	action_ptr_type			unroll_return_type;

	action() { }
virtual	~action() { }

virtual	ostream&
	dump(ostream&, const expr_dump_context&) const = 0;

/**
	Dumps single events only, silent for grouped events.  
 */
#define	CHP_DUMP_EVENT_PROTO						\
	ostream&							\
	dump_event(ostream&, const expr_dump_context&) const

virtual	CHP_DUMP_EVENT_PROTO = 0;

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

#if ENABLE_CHP_EXECUTE
#define	CHP_EXECUTE_PROTO						\
	void								\
	execute(const nonmeta_context&, update_reference_array_type&) const

virtual	CHP_EXECUTE_PROTO = 0;

#define	CHP_RECHECK_PROTO						\
	void								\
	recheck(const nonmeta_context&) const

virtual	CHP_RECHECK_PROTO = 0;
#endif

};	// end class action

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CHP_BASE_H__

