/**
	\file "Object/lang/CHP_base.h"
	Class definitions for CHP-related objects.  
	$Id: CHP_base.h,v 1.11.16.1 2007/09/02 20:49:20 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CHP_BASE_H__
#define	__HAC_OBJECT_LANG_CHP_BASE_H__

#include "util/persistent.h"
#include "util/memory/count_ptr.h"
#include "Object/devel_switches.h"


namespace HAC {
namespace entity {
struct expr_dump_context;
class unroll_context;
class preal_expr;
/**
	Namespace for CHP object classes.  
 */
namespace CHP {
using std::ostream;
using std::istream;
using util::persistent;
using util::persistent_object_manager;
class action;
class chp_visitor;
using util::memory::count_ptr;
typedef	count_ptr<const action>			action_ptr_type;
typedef	count_ptr<const  preal_expr>		delay_ptr_type;

//=============================================================================
/**
	Abstract base class for type-checked CHP statements.  
 */
class action : public persistent {
public:
	typedef	action_ptr_type			unroll_return_type;
	/**
		TODO: Eventually generalize this to attribute list.  
	 */
	typedef	delay_ptr_type			attributes_type;
protected:
	attributes_type				delay;
protected:
	action();

	explicit
	action(const attributes_type&);
public:
virtual	~action();

	void
	set_delay(const delay_ptr_type&);

	const delay_ptr_type&
	get_delay(void) const { return delay; }

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

	ostream&
	dump_event_with_attributes(ostream&, const expr_dump_context&) const;

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
	accept(chp_visitor&) const

virtual	CHP_ACTION_ACCEPT_PROTO = 0;

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

