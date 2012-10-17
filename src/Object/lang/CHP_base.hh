/**
	\file "Object/lang/CHP_base.hh"
	Class definitions for CHP-related objects.  
	$Id: CHP_base.hh,v 1.14 2010/08/25 23:26:38 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CHP_BASE_H__
#define	__HAC_OBJECT_LANG_CHP_BASE_H__

#include "util/persistent.hh"
#include "util/memory/count_ptr.hh"
#include "Object/devel_switches.hh"

/**
	Define to 1 to maintain links to parent CHP actions.  
	Useful for printing the precise context in which each action
	is found.  
	CONSEQUENCE: unrolling can long rely on sharing shallow copies, 
		deep copies of actions are required (expressions may still
		be shared), thus unroll MUST deep-copy actions, 
		and reconnect parent links in fresh copies.
		NOTE: unroll_resolve_copy never needs shared-this pointer
		argument anymore!
	Goal: 1
	Rationale: to graduate!
	Status: done, tested (since Sept. 2007)
 */
#define	CHP_ACTION_PARENT_LINK		1

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
typedef	count_ptr<const preal_expr>		delay_ptr_type;

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
#if CHP_ACTION_PARENT_LINK
	/**
		Up-pointer to parent action in syntax tree.  
		Should not be saved persistently, but regenerated
		upon reconstruction by parents.  
		Should NOT be copied, but re-linked.
		Mutable because doesn't change semantic value.  
	 */
	mutable const action*			parent;
#endif
	/**
		Delay expression, eventually resolved to constant.
	 */
	attributes_type				delay;
protected:
	action();

	explicit
	action(const attributes_type&);

#if CHP_ACTION_PARENT_LINK
	// custom copy-ctor
	action(const action&);
#endif
public:
virtual	~action();

	void
	set_delay(const delay_ptr_type&);

	const delay_ptr_type&
	get_delay(void) const { return delay; }

#if CHP_ACTION_PARENT_LINK
	void
	set_parent(const action* a) const { parent = a; }
	// or mutable, doesn't change value

	const action*
	get_parent(void) const { return parent; }
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

#if CHP_ACTION_PARENT_LINK
#define	CHP_UNROLL_ACTION_PROTO						\
	action_ptr_type							\
	unroll_resolve_copy(const unroll_context&) const
#else
#define	CHP_UNROLL_ACTION_PROTO						\
	action_ptr_type							\
	unroll_resolve_copy(const unroll_context&,			\
		const action_ptr_type&) const
#endif

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

