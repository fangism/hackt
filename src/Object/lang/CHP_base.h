/**
	\file "Object/lang/CHP_base.h"
	Class definitions for CHP-related objects.  
	$Id: CHP_base.h,v 1.7.32.4 2006/12/16 03:05:46 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CHP_BASE_H__
#define	__HAC_OBJECT_LANG_CHP_BASE_H__

#include "util/persistent.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
	// we need some sort of CHP_visitor refinement!
	class StateConstructor;
}	// end namespace CHPSIM
}	// end namespace SIM
namespace entity {
struct expr_dump_context;
class unroll_context;
#if 0
namespace PRS {
	class cflat_visitor;
}
#endif
/**
	Namespace for CHP object classes.  
 */
namespace CHP {
using std::ostream;
#if 0
using entity::PRS::cflat_visitor;
#else
using SIM::CHPSIM::StateConstructor;
#endif
using util::persistent;
using util::persistent_object_manager;
class action;
using util::memory::count_ptr;
typedef	count_ptr<const action>			action_ptr_type;

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

};	// end class action

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CHP_BASE_H__

