/**
	\file "Object/lang/CHP.h"
	Class definitions for CHP-related objects.  
	$Id: CHP.h,v 1.19.2.1 2007/06/08 21:39:51 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CHP_H__
#define	__HAC_OBJECT_LANG_CHP_H__

#include <list>
#include <vector>
#include "Object/lang/CHP_base.h"
#include <string>
#include "Object/ref/references_fwd.h"
#include "Object/expr/expr_fwd.h"
#include "Object/unroll/meta_loop_base.h"
#include "util/memory/count_ptr.h"
#include "util/boolean_types.h"
#include "util/static_assert.h"
#include "util/type_traits.h"

namespace HAC {
namespace entity {
class data_nonmeta_instance_reference;
class unroll_context;
class footprint;

namespace CHP {
class footprint;
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
typedef	list<action_ptr_type>			action_list_type;

//=============================================================================
/**
	General purpose attribute.
	Later, if needed: extend to support multiple values per key. 
 */
class attribute {
//	typedef	dynamic_param_expr_list		values_type;
	typedef	count_ptr<const param_expr>	value_type;
//	typedef	const value_type&		const_reference;
//	typedef	value_type&			reference;
private:
	std::string				_key;
	value_type				_value;
public:
	attribute(const std::string&, const value_type&);
	~attribute();

	const std::string&
	key(void) const { return _key; }

	const value_type&
	value(void) const { return _value; }

};	// end class attributes

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
	typedef	list_type::const_iterator	const_iterator;
public:
	action_sequence();
	~action_sequence();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	CHP_DUMP_EVENT_PROTO;
	CHP_UNROLL_ACTION_PROTO;
	CHP_ACTION_ACCEPT_PROTO;

	// helper methods needed for process_definition
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

	PERSISTENT_METHODS_DECLARATIONS
};	// end class action_sequence

//=============================================================================
/**
	Concurrent CHP actions.  
	This is also the top-level CHP object per definition.  
 */
class concurrent_actions : public action, public action_list_type {
private:
	typedef	action				parent_type;
	typedef	action_list_type		list_type;
	typedef	concurrent_actions		this_type;
public:
	typedef	action_list_type::const_iterator	const_iterator;
public:
	concurrent_actions();
	~concurrent_actions();

	ostream&
	what(ostream&) const;

	ostream&
	__dump(ostream&, const expr_dump_context&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	ostream&
	dump(ostream&, const entity::footprint&, 
		const expr_dump_context&) const;

	// helper methods needed for process_definition
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

	CHP_DUMP_EVENT_PROTO;
	CHP_UNROLL_ACTION_PROTO;
	CHP_ACTION_ACCEPT_PROTO;

	good_bool
	unroll(const unroll_context&, entity::footprint&) const;

private:
	good_bool
	__unroll(const unroll_context&, this_type&) const;

public:
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
	typedef	guarded_action			this_type;
public:
	typedef	count_ptr<const bool_expr>	guard_ptr_type;
	typedef	count_ptr<const action>		stmt_ptr_type;
	typedef	count_ptr<const guarded_action>	unroll_return_type;
	/// Functor for evaluating guards
	struct selection_evaluator;
	struct selection_evaluator_ref;
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

	const guard_ptr_type&
	get_guard(void) const { return guard; }

	const stmt_ptr_type&
	get_action(void) const { return stmt; }

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	ostream&
	dump_brief(ostream&, const expr_dump_context&) const;

	unroll_return_type
	unroll_resolve_copy(const unroll_context&,
		const count_ptr<const guarded_action>&) const;

	CHP_DUMP_EVENT_PROTO;
	CHP_ACTION_ACCEPT_PROTO;

	struct unroll_resolver {
		const unroll_context&			_context;

		explicit
		unroll_resolver(const unroll_context& c) : _context(c) { }

		count_ptr<const guarded_action>
		operator () (const count_ptr<const guarded_action>&) const;
	};

	PERSISTENT_METHODS_DECLARATIONS
};	// end class guarded_action

//=============================================================================
/**
	Typical guarded statement list.  
 */
typedef	list<count_ptr<const guarded_action> >	selection_list_type;

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
	dump(ostream&, const expr_dump_context&) const;

	CHP_DUMP_EVENT_PROTO;
	CHP_UNROLL_ACTION_PROTO;
	CHP_ACTION_ACCEPT_PROTO;

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
	dump(ostream&, const expr_dump_context&) const;

	CHP_DUMP_EVENT_PROTO;
	CHP_UNROLL_ACTION_PROTO;
	CHP_ACTION_ACCEPT_PROTO;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class nondeterministic_selection

//=============================================================================
/**
	A construct for compile-time expanding a regular
	selection statement.  
 */
class metaloop_selection : public action, public meta_loop_base {
	typedef	metaloop_selection		this_type;
	typedef	action				parent_type;
public:
	typedef	meta_loop_base::ind_var_ptr_type	ind_var_ptr_type;
	typedef	meta_loop_base::range_ptr_type	range_ptr_type;
	typedef	count_ptr<const guarded_action>	body_ptr_type;
private:
	/**
		The one guarded action is presumably dependent on the 
		induction variable.  Will be expanded into 
		a full selection statement at unroll-time.  
	 */
	body_ptr_type				body;
	/**
		True is for determinstic selection, 
		false is for nondeterministic selection.
		No other types supported yet.  
	 */
	bool					selection_type;
public:
	metaloop_selection();
	metaloop_selection(const ind_var_ptr_type&, const range_ptr_type&, 
		const body_ptr_type&, const bool);
	~metaloop_selection();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	CHP_DUMP_EVENT_PROTO;
	CHP_UNROLL_ACTION_PROTO;
	CHP_ACTION_ACCEPT_PROTO;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class metaloop_selection

//=============================================================================
/**
	A construct for compile-time expanding a regular statement.  
 */
class metaloop_statement : public action, public meta_loop_base {
	typedef	metaloop_statement		this_type;
	typedef	action				parent_type;
public:
	typedef	meta_loop_base::ind_var_ptr_type	ind_var_ptr_type;
	typedef	meta_loop_base::range_ptr_type	range_ptr_type;
	typedef	count_ptr<const action>		body_ptr_type;
private:
	/**
		The one guarded action is presumably dependent on the 
		induction variable.  Will be expanded into 
		a full statement statement at unroll-time.  
	 */
	body_ptr_type				body;
	/**
		True is for concurrent statement, 
		false is for sequential statement.
	 */
	bool					statement_type;
public:
	metaloop_statement();
	metaloop_statement(const ind_var_ptr_type&, const range_ptr_type&, 
		const body_ptr_type&, const bool);
	~metaloop_statement();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	CHP_DUMP_EVENT_PROTO;
	CHP_UNROLL_ACTION_PROTO;
	CHP_ACTION_ACCEPT_PROTO;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class metaloop_statement

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
	typedef	count_ptr<const data_nonmeta_instance_reference>
							lval_ptr_type;
	typedef	count_ptr<const data_expr>		rval_ptr_type;
private:
	lval_ptr_type					lval;
	rval_ptr_type					rval;
public:
	assignment();
	assignment(const lval_ptr_type&, const rval_ptr_type&);
	~assignment();

	const lval_ptr_type&
	get_lval(void) const { return lval; }

	const rval_ptr_type&
	get_rval(void) const { return rval; }

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	CHP_DUMP_EVENT_PROTO;
	CHP_UNROLL_ACTION_PROTO;
	CHP_ACTION_ACCEPT_PROTO;

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
public:
	typedef	count_ptr<const bool_expr>		cond_ptr_type;
private:
	cond_ptr_type					cond;
public:
	condition_wait();

	explicit
	condition_wait(const cond_ptr_type&);

	~condition_wait();

	const cond_ptr_type&
	get_guard(void) const { return cond; }

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	CHP_DUMP_EVENT_PROTO;
	CHP_UNROLL_ACTION_PROTO;
	CHP_ACTION_ACCEPT_PROTO;

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
	typedef	count_ptr<const data_expr>		data_ptr_type;
	typedef	vector<data_ptr_type>			expr_list_type;
	/**
		should be simple_channel_nonmeta_instance_reference.
		Are member_nonmeta_instance_references allowed?
	 */
	typedef	count_ptr<const simple_channel_nonmeta_instance_reference>
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

	const chan_ptr_type&
	get_chan(void) const { return chan; }

	const expr_list_type&
	get_exprs(void) const { return exprs; }

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	CHP_DUMP_EVENT_PROTO;
	CHP_UNROLL_ACTION_PROTO;
	CHP_ACTION_ACCEPT_PROTO;

	template <class L>
	good_bool
	add_expressions(const L&);

	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class channel_send

//=============================================================================
/**
	Receive values from channel. 
	Now also overloaded to be used as peek actions
	(non-blocking read assignment)
 */
class channel_receive : public action {
	typedef	action					parent_type;
	typedef	channel_receive				this_type;
public:
	typedef	count_ptr<const data_nonmeta_instance_reference>
							inst_ref_ptr_type;
	typedef	vector<inst_ref_ptr_type>		inst_ref_list_type;
	/**
		should be simple_channel_nonmeta_instance_reference.
		Are member_nonmeta_instance_references allowed?
	 */
	typedef	count_ptr<const simple_channel_nonmeta_instance_reference>
							chan_ptr_type;
private:
	chan_ptr_type					chan;
	inst_ref_list_type				insts;
	/**
		true if this is interpreted as a peek action.
	 */
	bool						peek;
private:
	channel_receive();
public:
	channel_receive(const chan_ptr_type&, const bool);
	~channel_receive();

	bool
	is_peek(void) const { return peek; }

	const chan_ptr_type&
	get_chan(void) const { return chan; }

	const inst_ref_list_type&
	get_insts(void) const { return insts; }

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	CHP_DUMP_EVENT_PROTO;
	CHP_UNROLL_ACTION_PROTO;
	CHP_ACTION_ACCEPT_PROTO;

	template <class L>
	good_bool
	add_references(const L&);

	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class channel_receive

//=============================================================================
/**
	do-while loop: repeat while at least one guard is true.  
	NOTE: this is classified as a selection statement, but not a branch.
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
	dump(ostream&, const expr_dump_context&) const;

	CHP_DUMP_EVENT_PROTO;
	CHP_UNROLL_ACTION_PROTO;
	CHP_ACTION_ACCEPT_PROTO;

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
	typedef	count_ptr<const action>			body_ptr_type;
private:
	body_ptr_type					body;
private:
	do_forever_loop();
public:
	explicit
	do_forever_loop(const body_ptr_type&);
	~do_forever_loop();

	const body_ptr_type&
	get_body(void) const { return body; }

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	CHP_DUMP_EVENT_PROTO;
	CHP_UNROLL_ACTION_PROTO;
	CHP_ACTION_ACCEPT_PROTO;

	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

public:
	/**
		Substitute for mem_fun_ref predicate functor...
		\param P pointer class whose element type is 'action'
			should probably be a count_ptr, since we're not
			doing exclusive transfers.  
	 */
	template <class P>
	struct detector {
		typedef	typename P::template rebind<const do_forever_loop>::type
							ptr_type;

		ptr_type
		operator () (const P& p) const {
			typedef	util::is_same<action, typename P::element_type>
					__type_constraint1;
			UTIL_STATIC_ASSERT_DEPENDENT(__type_constraint1::value);
			return p.template is_a<const do_forever_loop>();
		}
	};	// end struct detector
};	// end class do_forever_loop

//=============================================================================
/**
	Context-binding functor.  
	Relocate this to another file if it ever becomes useful elsewhere.  
 */
struct data_expr_unroll_resolver {
	const unroll_context&			_context;

	explicit
	data_expr_unroll_resolver(const unroll_context& c) : _context(c) { }

	count_ptr<const data_expr>
	operator () (const count_ptr<const data_expr>&) const;
};	// end struct data_expr_unroll_resolver

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Context-binding functor.  
	Relocate this to another file if it ever becomes useful elsewhere.  
 */
struct data_ref_unroll_resolver {
	const unroll_context&			_context;

	explicit
	data_ref_unroll_resolver(const unroll_context& c) : _context(c) { }

	count_ptr<const data_nonmeta_instance_reference>
	operator () (
		const count_ptr<const data_nonmeta_instance_reference>&) const;
};	// end struct data_ref_unroll_resolver

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CHP_H__

