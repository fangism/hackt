/**
	\file "Object/art_object_CHP.h"
	Class definitions for CHP-related objects.  
	$Id: art_object_CHP.h,v 1.1.2.10 2005/06/18 22:57:27 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_CHP_H__
#define	__OBJECT_ART_OBJECT_CHP_H__

#include <list>
#include <vector>
#include "Object/art_object_CHP_base.h"
#include "Object/art_object_fwd.h"
#include "util/memory/count_ptr.h"
#include "util/boolean_types.h"

namespace ART {
namespace entity {
class simple_datatype_nonmeta_value_reference;
namespace CHP {
using std::list;
using std::vector;
using std::istream;
using util::good_bool;
using util::memory::count_ptr;
using util::persistent_object_manager;
//=============================================================================
/**
	Typical action list.  
 */
typedef	list<count_ptr<action> >		action_list_type;

//=============================================================================
/**
	Sequential CHP actions.  
 */
class action_sequence : public action, public action_list_type {
private:
	typedef	action				parent_type;
	typedef	action_list_type		list_type;
	typedef	action_sequence			this_type;
public:
	action_sequence();
	~action_sequence();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class action_sequence

//=============================================================================
/**
	Concurrent CHP actions.  
 */
class concurrent_actions : public action, public action_list_type {
private:
	typedef	action				parent_type;
	typedef	action_list_type		list_type;
	typedef	concurrent_actions		this_type;
public:
	concurrent_actions();
	~concurrent_actions();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	// helper methods needed for process_definition
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

	PERSISTENT_METHODS_DECLARATIONS
};	// end class concurrent_actions

//=============================================================================
// class for tight concurrency? (bullet)
//=============================================================================
/**
	A guarded action.  
	CHP not part of the meta-language.  
	Do guards need back-references to selection statement?
 */
class guarded_action : public persistent {
public:
	typedef	count_ptr<bool_expr>		guard_ptr_type;
	typedef	count_ptr<action>		stmt_ptr_type;
	typedef	guarded_action			this_type;
protected:
	/**
		In the case of an else-clause, this guard is allowed to 
		be NULL.  
		For any set of guarded statements, at most one guarded
		action may have a NULL guard, which designates
		it as the else clause.  
	 */
	guard_ptr_type				guard;
	/**
		May be a sequence of actions or single action.  
		May also be NULL in the case of a skip action.  
	 */
	stmt_ptr_type				stmt;
public:
	guarded_action();
	guarded_action(const guard_ptr_type&, const stmt_ptr_type&);
	~guarded_action();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class guarded_action

//=============================================================================
/**
	Typical guarded statement list.  
 */
typedef	list<count_ptr<guarded_action> >	selection_list_type;

//=============================================================================
/**
	Deterministic selection statement.  
 */
class deterministic_selection : public action, public selection_list_type {
private:
	typedef	action				parent_type;
	typedef	selection_list_type		list_type;
	typedef	deterministic_selection		this_type;
public:
	deterministic_selection();
	~deterministic_selection();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class deterministic_selection

//=============================================================================
/**
	Non-deterministic selection statement.  
 */
class nondeterministic_selection : public action, public selection_list_type {
private:
	typedef	action				parent_type;
	typedef	selection_list_type		list_type;
	typedef	nondeterministic_selection	this_type;
public:
	nondeterministic_selection();
	~nondeterministic_selection();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class nondeterministic_selection

//=============================================================================
/**
	Variable assignment.  
	Consider sub-typing, like with param_expression_assignment?
	For now, use only generic datatype for brevity.  
 */
class assignment : public action {
private:
	typedef	action					parent_type;
	typedef	assignment				this_type;
public:
	typedef	count_ptr<simple_datatype_nonmeta_value_reference>
							lval_ptr_type;
	typedef	count_ptr<data_expr>			rval_ptr_type;
private:
	lval_ptr_type					lval;
	rval_ptr_type					rval;
public:
	assignment();
	assignment(const lval_ptr_type&, const rval_ptr_type&);
	~assignment();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class assignment

//=============================================================================
// no skip action, just left out!
//=============================================================================
/**
	Block until expression evaluates true.  
	Technically, this could be reduced to a guarded-action
	with a null action?
 */
class condition_wait : public action {
	typedef	action					parent_type;
	typedef	condition_wait				this_type;
	typedef	count_ptr<bool_expr>			cond_ptr_type;
private:
	cond_ptr_type					cond;
public:
	condition_wait();
	condition_wait(const cond_ptr_type&);
	~condition_wait();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class condition_wait

//=============================================================================
/**
	Send list of values across channel.  
 */
class channel_send : public action {
	typedef	action					parent_type;
	typedef	channel_send				this_type;
public:
	typedef	vector<count_ptr<data_expr> >		expr_list_type;
	/**
		should be simple_channel_nonmeta_instnace_reference
	 */
	typedef	count_ptr<simple_channel_nonmeta_instance_reference>
							chan_ptr_type;
private:
	chan_ptr_type					chan;
	expr_list_type					exprs;
private:
	channel_send();
public:
	explicit
	channel_send(const chan_ptr_type&);
	~channel_send();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	template <class L>
	good_bool
	add_expressions(const L&);

	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class channel_send

//=============================================================================
/**
	Receive values from channel. 
 */
class channel_receive : public action {
	typedef	action					parent_type;
	typedef	channel_receive				this_type;
public:
	typedef	count_ptr<simple_datatype_nonmeta_value_reference>
							inst_ref_ptr_type;
	typedef	vector<inst_ref_ptr_type>		inst_ref_list_type;
	typedef	count_ptr<simple_channel_nonmeta_instance_reference>
							chan_ptr_type;
private:
	chan_ptr_type					chan;
	inst_ref_list_type				insts;
private:
	channel_receive();
public:
	channel_receive(const chan_ptr_type&);
	~channel_receive();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	template <class L>
	good_bool
	add_references(const L&);

	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class channel_receive

//=============================================================================
/**
	do-while loop: repeat while at least one guard is true.  
 */
class do_while_loop : public action, public selection_list_type {
	typedef	action					parent_type;
	typedef	selection_list_type			list_type;
	typedef	do_while_loop				this_type;
public:
	do_while_loop();
	~do_while_loop();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class do_while_loop

//=============================================================================
/**
	do-forever loop: *[ never ends ]
 */
class do_forever_loop : public action {
	typedef	action					parent_type;
	typedef	do_forever_loop				this_type;
public:
	typedef	count_ptr<action>			body_ptr_type;
private:
	body_ptr_type					body;
private:
	do_forever_loop();
public:
	explicit
	do_forever_loop(const body_ptr_type&);
	~do_forever_loop();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

public:
	/**
		Substitute for mem_fun_ref predicate functor...
	 */
	template <template <class> class P>
	struct detector {
		typedef	P<const do_forever_loop>	ptr_type;

		ptr_type
		operator () (const P<action>& p) const {
			return p.template is_a<const do_forever_loop>();
		}
	};	// end struct detector
};	// end class do_forever_loop

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_CHP_H__

