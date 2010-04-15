/**
	\file "AST/instance.cc"
	Class method definitions for HAC::parser for instance-related classes.
	$Id: instance.cc,v 1.35 2010/04/15 00:40:19 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_instance.cc,v 1.31.10.1 2005/12/11 00:45:08 fang Exp
 */

#ifndef	__HAC_AST_INSTANCE_CC__
#define	__HAC_AST_INSTANCE_CC__

#define	ENABLE_STACKTRACE		0

#include <exception>
#include <iostream>
#include <utility>
#include <functional>
#include <numeric>

#include "AST/instance.h"
#include "AST/reference.h"
#include "AST/range_list.h"
#include "AST/token_string.h"
#include "AST/token.h"			// for token_else
#include "AST/type.h"
#include "AST/attribute.h"
#include "AST/node_list.tcc"
#include "AST/parse_context.h"

#include "Object/common/namespace.h"
#include "Object/inst/instance_placeholder_base.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/def/definition_base.h"
#include "Object/type/fundamental_type_reference.h"
#include "Object/type/channel_type_reference_base.h"	// reject directions
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/meta_value_reference_base.h"
#include "Object/lang/PRS.h"
#include "Object/lang/SPEC.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/meta_range_expr.h"
#include "Object/expr/meta_range_list.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/unroll/expression_assignment.h"
#include "Object/unroll/alias_connection.h"
#include "Object/unroll/port_connection.h"
#include "Object/unroll/loop_scope.h"
#include "Object/unroll/conditional_scope.h"
#include "Object/unroll/template_type_completion.h"
#if INSTANCE_SUPPLY_OVERRIDES
#include "Object/unroll/implicit_port_override.h"
#include "Object/traits/bool_traits.h"
#endif
#include "Object/traits/proc_traits.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/meta_instance_reference_base.h"
#include "Object/ref/meta_reference_union.h"

#include "common/ICE.h"
#include "common/TODO.h"

#include "util/what.h"
#include "util/stacktrace.h"
#include "util/dereference.h"
#include "util/compose.h"
#include "util/binders.h"
#include "util/memory/count_ptr.tcc"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

//=============================================================================
// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::parser::instance_base, 
	"(declaration-id)")
SPECIALIZE_UTIL_WHAT(HAC::parser::instance_array, 
	"(declaration-array)")
SPECIALIZE_UTIL_WHAT(HAC::parser::instance_declaration, 
	"(instance-decl)")
SPECIALIZE_UTIL_WHAT(HAC::parser::instance_connection, 
	"(actuals-connection)")
SPECIALIZE_UTIL_WHAT(HAC::parser::connection_statement, 
	"(connection-statement)")
SPECIALIZE_UTIL_WHAT(HAC::parser::instance_alias, 
	"(alias-assign)")
SPECIALIZE_UTIL_WHAT(HAC::parser::loop_instantiation, 
	"(loop-instance)")
SPECIALIZE_UTIL_WHAT(HAC::parser::guarded_instance_management, 
	"(guarded-def-body)")
SPECIALIZE_UTIL_WHAT(HAC::parser::conditional_instantiation, 
	"(conditional-instance)")
SPECIALIZE_UTIL_WHAT(HAC::parser::type_completion_statement, 
	"(type-completion)")
SPECIALIZE_UTIL_WHAT(HAC::parser::type_completion_connection_statement, 
	"(type-completion-connection)")

namespace memory {
// explicit template instantiations
using namespace HAC::parser;
template class count_ptr<const guarded_instance_management>;
template class count_ptr<const instance_management>;
template class count_ptr<const instance_base>;
template class count_ptr<const instance_declaration>;

}	// end namespace memory
}	// end namespace util

//=============================================================================
namespace HAC {
namespace parser {
#include "util/using_ostream.h"
using util::dereference;
using std::transform;
using std::mem_fun_ref;
using ADS::unary_compose;
using util::bind2nd_argval;
using std::accumulate;
// using std::_Select1st;
// using std::_Select2nd;
using std::find;
using entity::expr_dump_context;
using entity::channel_type_reference_base;
using entity::meta_instance_reference_base;
using entity::meta_value_reference_base;
using entity::simple_meta_indexed_reference_base;
using entity::aliases_connection_base;
using entity::meta_instance_reference_connection;
using entity::port_connection;
using entity::port_connection_base;
using entity::dynamic_param_expr_list;
using entity::meta_range_expr;
using entity::meta_loop_base;
using entity::loop_scope;
using entity::conditional_scope;
using entity::pint_scalar;
using entity::pbool_expr;
using entity::pbool_const;
using entity::process_tag;
using entity::template_type_completion;
#if INSTANCE_SUPPLY_OVERRIDES
using entity::implicit_port_override;
#endif

//=============================================================================
// class instance_management method definitions

#if 0
CONSTRUCTOR_INLINE
instance_management::instance_management() : def_body_item() {
}

DESTRUCTOR_INLINE
instance_management::~instance_management() {
}
#endif

//=============================================================================
// class alias_list method definitions

/**
	\param e should be a member/index expression, an lvalue.  
 */
CONSTRUCTOR_INLINE
alias_list::alias_list(const expr* e) :
		instance_management(), parent_type(e) {
}

DESTRUCTOR_INLINE
alias_list::~alias_list() {
}

ostream&
alias_list::what(ostream& o) const {
	return alias_list_base::what(o);
}

line_position
alias_list::leftmost(void) const {
	return alias_list_base::leftmost();
}

line_position
alias_list::rightmost(void) const {
	return alias_list_base::rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Converts a list into a param_expression_assignment object.
	This is non-const because assignment requires that we 
	initialize parameter instance references, which requires 
	modification.

	Code from here is ripped from the old
	object_list::make_param_assignment().
	\param temp result of postorder_check_meta of items in list, 
		which may contain errors, but they are caught here.  
	\return newly allocated expression assignment object if
		successfully type-checked, else NULL.  
 */
count_ptr<const entity::param_expression_assignment>
alias_list::make_param_assignment(const checked_meta_exprs_type& temp) {
	typedef	count_ptr<const entity::param_expression_assignment>
							const_return_type;
	typedef	count_ptr<entity::param_expression_assignment>
							return_type;
	typedef	checked_meta_exprs_type::value_type	checked_expr_ptr_type;
// experimenting
#define	CONSTRUCT_FROM_LVALUE		0
	// then expect subsequent items to be the same
	// or already param_expr in the case of some constants.
	// However, only the last item may be a constant.  

	// right-hand-side source expression
#if CONSTRUCT_FROM_LVALUE
	const checked_expr_ptr_type& first_obj(temp.front());
	if (!first_obj) {
		cerr << "Error in destination value reference." << endl;
		return const_return_type(NULL);
	}
	// cross-cast check
	const count_ptr<const meta_value_reference_base>
		lhs(first_obj.is_a<const meta_value_reference_base>());
	if (!lhs) {
		cerr << "Error: destination expression of assignment "
			"is not an lvalue." << endl;
		return const_return_type(NULL);
	}
	return_type
		ret(meta_value_reference_base::
			make_param_expression_assignment(lhs));
	NEVER_NULL(ret);
#else
	const checked_expr_ptr_type& last_obj(temp.back());
	if (!last_obj) {
		cerr << "Error in source expression." << endl;
		return const_return_type(NULL);
	}
	const count_ptr<const param_expr>
		rhse = last_obj.is_a<const param_expr>();
	INVARIANT(rhse);
	return_type ret;
#endif
	// later, fold these error messages into static constructor?
	if (!last_obj) {
		cerr << "ERROR: rhs of expression assignment "
			"is malformed (null)" << endl;
		return const_return_type(NULL);
	} else if (rhse) {
		// last term must be initialized or be dependent on formals
		// if collective, conservative: may-be-initialized
		ret = param_expr::make_param_expression_assignment(rhse);
		INVARIANT(ret);
	} else {
		// never reached... caught by INVARIANT check above
		last_obj->what(
			cerr << "ERROR: rhs is unexpected object: ") << endl;
		return const_return_type(NULL);
	}

	bad_bool err(false);
	entity::param_expression_assignment::meta_instance_reference_appender
		append_it(*ret);
	const checked_meta_exprs_type::const_iterator dest_end(temp.end() -1);
	checked_meta_exprs_type::const_iterator dest_iter(temp.begin());
	err = accumulate(dest_iter, dest_end, err, append_it);

	// if there are any errors, discard everything?
	// later: track errors in partially constructed objects
	if (err.bad) {
		// already have error message
		return const_return_type(NULL);
	} else	return const_return_type(ret);             // is ok
#undef	CONSTRUCT_FROM_LVALUE
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates an alias connection object, given a list of instance
	references.  Performs type-checking.
 */
count_ptr<const entity::aliases_connection_base>
alias_list::make_alias_connection(const checked_meta_refs_type& temp) {
	typedef count_ptr<const aliases_connection_base> const_return_type;
	typedef count_ptr<aliases_connection_base> 	return_type;
	checked_meta_refs_type::const_iterator i(temp.begin());
	INVARIANT(temp.size() > 1);          // else what are you connecting?
	const count_ptr<const meta_instance_reference_base>
		fir(i->inst_ref());
	NEVER_NULL(fir);
	return_type ret = 
		entity::meta_instance_reference_base::make_aliases_connection(fir);
	ret->reserve(temp.size());
	// keep this around for type-checking comparisons
	ret->append_meta_instance_reference(fir);
	// starting with second instance reference, type-check and alias
	int j = 2;
	for (i++; i!=temp.end(); i++, j++) {
		const count_ptr<const meta_instance_reference_base>
			ir(i->inst_ref());
		if (!ir) {
			cerr << "ERROR: invalid instance reference at position "
				<< j << " of alias list." << endl;
			return const_return_type(NULL);
		}
		if (!fir->may_be_type_equivalent(*ir)) {
			cerr << "ERROR: type/size of instance reference "
				<< j << " of alias list doesn't match the "
				"type/size of the first instance reference!  "
				<< endl;
			return const_return_type(NULL);
		} else {
			ret->append_meta_instance_reference(ir);
		}
	}
	// transfers ownership
	return const_return_type(ret);        // const-ify
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	COMPLETELY REDO THIS, since param_literal has been ELIMINATED
		so that simple_param_meta_value_reference are now subclasses
		of the respective expression types.  
	TEMPORARILY made deletions so it still works as before...

	Context-sensitive type-checking for parameter expression assignment
	lists and other instance elements.  
	Builds references bottom-up first.  
	Then checks type consistency of elements.  
	Post-condition: leaves connection_assignment_base on stack.  
	\param c the context of the current position in the syntax tree.
	\return pointer to rightmost instance corresponding to the 
		final element in the assignment / alias list
		(if all is consistent, else returns NULL)
 */
never_ptr<const object>
alias_list::check_build(context& c) const {
	STACKTRACE("alias_list::check_build()");
if (size() > 0) {		// non-empty
	const never_ptr<const object> ret(NULL);
	// can we just re-use parent's check_build()?
	// yes, because we don't need place-holder on stack.
	checked_meta_generic_type temp;
	postorder_check_meta_generic(temp, c);
	const checked_meta_generic_type::const_iterator first_obj(temp.begin());
	if (!first_obj->first && !first_obj->second) {
		cerr << endl << "ERROR in the first item in alias-list.  "
			<< where(*this) << endl;
		THROW_EXIT;
	} else if (first_obj->first) {
		checked_meta_exprs_type checked_exprs;
		expr_list::select_checked_meta_exprs(temp, checked_exprs);
		
		// then expect subsequent items to be the same
		// or already param_expr in the case of some constants.
		// However, only the last item may be a constant.  

		const count_ptr<const param_expression_assignment>
			exass(make_param_assignment(checked_exprs));

		// if all is well, then add this new list to the context's
		// current scope.  
		// idea for error checking:
		// instead of returning NULL, return partially created
		// list with error-markers, maintained in the object
		// and query the error status.  
		// forbid object writing if there are any errors.  
		if (!exass) {
			cerr << "HALT: at least one error in the "
				"assignment list.  " << where(*this) << endl;
			THROW_EXIT;
		} else {
			c.add_assignment(exass);
			// and transfer ownership
			INVARIANT(!exass.owned());
		}
	} else {
		INVARIANT(first_obj->second);
		checked_meta_refs_type checked_refs;
		expr_list::select_checked_meta_refs(temp, checked_refs);
		const count_ptr<const aliases_connection_base>
			connection(make_alias_connection(checked_refs));
		// also type-checks connections
		if (!connection) {
			cerr << "HALT: at least one error in connection list.  "
				<< where(*this) << endl;
			THROW_EXIT;
		} else {
			c.add_connection(connection);
		}
	}
} else {
	// will this ever be empty?  will be caught as error for now.
	ICE(cerr, 
		cerr << "Unexpected empty alias_list in AST." << endl;
	);
}
	// useless return value
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class actuals_base method definitions

CONSTRUCTOR_INLINE
actuals_base::actuals_base(const actuals_type* a) : 
//		instance_management(), 
		actuals(a) {
	NEVER_NULL(actuals);
}

DESTRUCTOR_INLINE
actuals_base::~actuals_base() {
}

line_position
actuals_base::leftmost(void) const {
	return actuals->leftmost();
}

line_position
actuals_base::rightmost(void) const {
	return actuals->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just a wrapped call to expr_list::check_build.
	Remember: result type is allowed to be NULL, 
		if no expression was passed in its position.  
 */
good_bool
actuals_base::check_actuals(
#if INSTANCE_SUPPLY_OVERRIDES
		implicit_ports_type& impret,
#endif
		explicit_ports_type& ret,
		context& c) const {
	STACKTRACE_VERBOSE;
	// TODO: check/resolve optional implicit global ports
#if INSTANCE_SUPPLY_OVERRIDES
if (actuals->implicit_ports) {
	actuals->implicit_ports->postorder_check_bool_refs_optional(impret, c);
	inst_ref_expr_list::checked_bool_refs_type::const_iterator
		c_iter(impret.begin());
	inst_ref_expr_list::const_iterator
		e_iter(actuals->implicit_ports->begin()),
		e_end(actuals->implicit_ports->end());
	for ( ; e_iter != e_end; ++e_iter, ++c_iter) {
		if (*e_iter && !*c_iter) {
			cerr << "Error in implicit port overrides.  " <<
				where(*actuals->implicit_ports) << endl;
			return good_bool(false);
		}
	}
}
#endif
if (actuals->actual_ports) {
	expr_list::checked_meta_generic_type temp;
	actuals->actual_ports->postorder_check_meta_generic(temp, c);
	expr_list::select_checked_meta_refs(temp, ret);
	expr_list::checked_meta_generic_type::const_iterator
		c_iter(temp.begin());
	expr_list::const_iterator e_iter(actuals->actual_ports->begin());
	const expr_list::const_iterator e_end(actuals->actual_ports->end());
	for ( ; e_iter != e_end; ++e_iter, ++c_iter) {
		if (*e_iter) {
			if (!c_iter->first && !c_iter->second)
				return good_bool(false);
			// both results are NULL => check failed
		}
		// else expression is null; skip it
	}
}
	// all relevant checks passed
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true of any members of list are not NULL.
 */
bool
actuals_base::has_implicit_overrides(const implicit_ports_type& p) {
	implicit_ports_type::const_iterator
		i(p.begin()), e(p.end());
	for ( ; i!=e; ++i) {
		if (*i) return true;
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
actuals_base::add_instance_port_connections(
		const count_ptr<
			const connection_statement::inst_ref_arg_type>& iref, 
		context& c) const {
#if INSTANCE_SUPPLY_OVERRIDES
	implicit_ports_type itemp;
#endif
	explicit_ports_type temp;
	if (!check_actuals(
#if INSTANCE_SUPPLY_OVERRIDES
		itemp, 
#endif
		temp, c).good) {
		cerr << "ERROR in object_list produced at "
			<< where(*actuals) << endl;
		return good_bool(false);
	}

#if INSTANCE_SUPPLY_OVERRIDES
	// TODO: create implicit port connections
if (has_implicit_overrides(itemp)) {
	const count_ptr<const connection_statement::result_type>
		ovr(connection_statement::make_implicit_port_override(
			itemp, iref));
	if (!ovr) {
		cerr << "HALT: error in implicit port override list.  "
			<< where(*actuals->implicit_ports) << endl;
		return good_bool(false);
	} else {
		c.add_connection(ovr);
	}
}
	// else don't bother will null connection
#endif
if (actuals->actual_ports) {
	const count_ptr<const connection_statement::result_type>
		port_con(connection_statement::make_port_connection(
			temp, iref));
	if (!port_con) {
		cerr << "HALT: at least one error in port connection list.  "
			<< where(*actuals->actual_ports) << endl;
		return good_bool(false);
	} else {
		c.add_connection(port_con);
	}
}
	return good_bool(true);
}

//=============================================================================
// class instance_base method definitions

CONSTRUCTOR_INLINE
instance_base::instance_base(const token_identifier* i, const expr_list* a) :
		instance_management(), id(i), relaxed_args(a) {
	NEVER_NULL(id);
}

CONSTRUCTOR_INLINE
instance_base::instance_base(const count_ptr<const token_identifier>& i,
		const expr_list* a) :
		instance_management(), id(i), relaxed_args(a) {
	NEVER_NULL(id);
}

DESTRUCTOR_INLINE
instance_base::~instance_base() { }

ostream&
instance_base::what(ostream& o) const {
	return id->what(o << util::what<instance_base>::name() << ": ");
}

line_position
instance_base::leftmost(void) const {
	return id->leftmost();
}

line_position
instance_base::rightmost(void) const {
	if (relaxed_args)
		return relaxed_args->rightmost();
	else	return id->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Type checks a scalar instance declaration, and registers
		its instantiation statement.  
	Eventually don't return top_namespace, 
	but a pointer to the created instance_base
	so that it may be used by instance_alias.  
	NOTE: for a relaxed-template construct like "foo<5> x<4>", 
	automatically expand as: "foo<5> x; x<4>;"
	likewise: "foo<5> x<4>[3];" becomes "foo<5> x[3]; x<4>[0..2];"
 */
never_ptr<const object>
instance_base::check_build(context& c) const {
	STACKTRACE("instance_base::check_build()");
	typedef	count_ptr<dynamic_param_expr_list>	relaxed_args_ptr_type;
	// uses c.current_fundamental_type
	const count_ptr<const fundamental_type_reference>
		type(c.get_current_fundamental_type());
	relaxed_args_ptr_type checked_relaxed_actuals;
	INVARIANT(type);
	if (relaxed_args) {
		if (type->is_strict()) {
			type->dump(cerr << "ERROR: current type ") <<
				" is already strict and hence, does not "
				"require relaxed template actuals " <<
				where(*relaxed_args) <<
				" in this instance declaration.  " << endl;
			THROW_EXIT;
			return never_ptr<const object>(NULL);
		}
	}
	// we allow scalar instance declarations to be relaxed
	// for the sake of forming aliases with other instances.  
	// otherwise do nothing different from before.  
	const context::placeholder_ptr_type
		inst(c.add_instance(*id));
	if (!inst) {
		cerr << "ERROR with " << *id << " at " << where(*id) << endl;
		THROW_EXIT;
		return never_ptr<const object>(NULL);
	}
	if (relaxed_args) {
		// add an auxiliary type_completion statement
		const inst_ref_expr::meta_return_type
			ref(id->check_meta_reference(c));
		NEVER_NULL(ref);	// we just created it!
		expr_list::checked_meta_exprs_type temp;
		relaxed_args->postorder_check_meta_exprs(temp, c);
		const count_ptr<const instance_management_base>
			nr(type_completion_statement::create_type_completion(
				ref, temp));
		if (!nr) {
			cerr << "Error in type completion at " <<
				where(*this) << endl;
			THROW_EXIT;
		}
		c.add_instance_management(nr);
		// error handling?
	}
	// need current_instance?  no, not using as reference.
	// return inst;
	return c.top_namespace();
}

//=============================================================================
// class instance_array method definitions

CONSTRUCTOR_INLINE
instance_array::instance_array(const token_identifier* i, 
		const expr_list* a, const range_list* rl) :
		instance_base(i, a), ranges(rl) {
	// ranges may be NULL, equivalent to declaration base
}

DESTRUCTOR_INLINE
instance_array::~instance_array() { }

ostream&
instance_array::what(ostream& o) const {
	return ranges->what(
		id->what(o << util::what<instance_array>::name() << ": "));
}

line_position
instance_array::rightmost(void) const {
	return ranges->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates an array of instances.  
	Dimensions may be dense or sparse.  
	See also template_formal_id::check_build, 
		port_formal_id::check_build.  
	NOTE: does not call parent's check_build, nor should it. 
 */
never_ptr<const object>
instance_array::check_build(context& c) const {
	STACKTRACE("instance_array::check_build()");
if (ranges) {
	typedef	count_ptr<dynamic_param_expr_list>
						relaxed_args_ptr_type;
	const count_ptr<const fundamental_type_reference>
		type(c.get_current_fundamental_type());
	INVARIANT(type);
	if (relaxed_args) {
		if (type->is_strict()) {
			type->dump(cerr << "ERROR: current type ") <<
				" is already strict and hence, does not "
				"require relaxed template actuals " <<
				where(*relaxed_args) <<
				" in this instance declaration.  " << endl;
			return never_ptr<const object>(NULL);
		}
		// TODO: not done yet... need to alter c.add_instance
		// copied from template_argument_list_pair::check_template_args
	}
	const range_list::checked_meta_ranges_type
		d(ranges->check_meta_ranges(c));
	if (!d) {
		cerr << "ERROR in building sparse range list!  " <<
			where(*ranges) << endl;
		THROW_EXIT;
	}
	if (c.get_current_open_definition()) {
		if (c.get_current_prototype() &&
			// is now OK outside of formal context
				d->is_relaxed_formal_dependent()) {
			cerr << "ERROR in instance-array declaration "
				"at " << where(*ranges) <<
				": array sizes are not allowed to "
				"depend on relaxed formal parameters."
				<< endl;
			THROW_EXIT;
		}
	}
	const never_ptr<const instance_placeholder_base>
		t(c.add_instance(*id, d));
	// if there was error, would've THROW_EXIT'd (temporary)
	if (relaxed_args) {
		// add an auxiliary type_completion statement
		// create an index_expr -> reference
		const index_expr
			ie(new id_expr(id),
				ranges->make_explicit_ranges());
		const inst_ref_expr::meta_return_type
			ref(ie.check_meta_reference(c));
		NEVER_NULL(ref);	// we just created it!
		expr_list::checked_meta_exprs_type temp;
		relaxed_args->postorder_check_meta_exprs(temp, c);
		const count_ptr<const instance_management_base>
			nr(type_completion_statement::create_type_completion(
				ref, temp));
		if (!nr) {
			cerr << "Error in type completion at " <<
				where(*this) << endl;
			THROW_EXIT;
		}
		c.add_instance_management(nr);
		// error handling?
	}
	return t;
} else {
	return instance_base::check_build(c);
}
}	// end method instance_array::check_build

//=============================================================================
// class instance_declaration method definitions

/**
	Creates an instance declaration, which may contain a list of 
	identifiers to instantiation.  
	\param t the base type (no array).  
	\param i the identifier list (may contain arrays).
 */
CONSTRUCTOR_INLINE
instance_declaration::instance_declaration(const concrete_type_ref* t, 
		const instance_id_list* i) :
		instance_management(),
		type(t), ids(i) {
	NEVER_NULL(type);
}

CONSTRUCTOR_INLINE
instance_declaration::instance_declaration(
		const count_ptr<const concrete_type_ref>& t, 
		const instance_id_list* i) :
		instance_management(),
		type(t), ids(i) {
	NEVER_NULL(type);
}

DESTRUCTOR_INLINE
instance_declaration::~instance_declaration() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(instance_declaration)

line_position
instance_declaration::leftmost(void) const {
	return type->leftmost();
}

line_position
instance_declaration::rightmost(void) const {
	return ids->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const object>
instance_declaration::check_build(context& c) const {
	STACKTRACE("instance_declaration::check_build()");
	const count_ptr<const fundamental_type_reference>
		ftr(type->check_type(c));
	// hack in: reject channels with directions (allow only in ports)
{
	const count_ptr<const channel_type_reference_base>
		ctr(ftr.is_a<const channel_type_reference_base>());
	if (ctr && (ctr->get_direction() != entity::CHANNEL_TYPE_BIDIRECTIONAL)) {
		cerr << "Error: channel types cannot be declared with "
			"directionality in a non-formal context.  "
			<< where(*type) << endl;
	}
}
	const context::fundamental_type_frame _ftf(c, ftr);

	if (ftr) {
		ids->check_build(c);		// return value?
	} else {
		// already have error message
		THROW_EXIT;		// temporary error handling
		return never_ptr<const object>(NULL);
	}
	// instance could be ANY type
	return c.top_namespace();
}

//=============================================================================
// class instance_connection method definitions

CONSTRUCTOR_INLINE
instance_connection::instance_connection(const token_identifier* i, 
		const expr_list* ta, const actuals_type* pa) :
		instance_base(i, ta), actuals_base(pa) {
}

DESTRUCTOR_INLINE
instance_connection::~instance_connection() {
}

// remember to check for declaration context when checking id

PARSER_WHAT_DEFAULT_IMPLEMENTATION(instance_connection)

line_position
instance_connection::leftmost(void) const {
	return instance_base::leftmost();
}

line_position
instance_connection::rightmost(void) const {
	return actuals->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create a port-style connection object.  
 */
never_ptr<const object>
instance_connection::check_build(context& c) const {
	typedef	never_ptr<const object>		return_type;
	STACKTRACE("instance_connection::check_build()");
	const never_ptr<const object>
		o(instance_base::check_build(c));
	if (!o) {
		// instance_base already prints error message...
//		cerr << "ERROR with " << *id << " at " << where(*id) << endl;
		THROW_EXIT;
		return return_type(NULL);
	}

	const inst_ref_expr::meta_return_type
		obj(id->check_meta_reference(c));

	NEVER_NULL(obj);		// we just created it!
	const count_ptr<const connection_statement::inst_ref_arg_type>
		inst_ref(obj.inst_ref());
	if (!inst_ref) {
		cerr << "Error resolving instance reference of "
			"port connection.  " << where(*id) << endl;
		THROW_EXIT;
	}

	if (!actuals_base::add_instance_port_connections(inst_ref, c).good) {
		THROW_EXIT;
	}
	return return_type(NULL);
}

//=============================================================================
// class connection_statement method definitions

CONSTRUCTOR_INLINE
connection_statement::connection_statement(
		const inst_ref_expr* l, 
		const actuals_type* a) :
		actuals_base(a), lvalue(l) {
	NEVER_NULL(lvalue);
}

DESTRUCTOR_INLINE
connection_statement::~connection_statement() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(connection_statement)

line_position
connection_statement::leftmost(void) const {
	return lvalue->leftmost();
}

line_position
connection_statement::rightmost(void) const {
	return actuals->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a port connection object, given an invoking instance
	reference and a list of port actuals (instance references).
	Copied and ripped from the old object_list::make_port_connection().  

	\param temp the list of checked references.
	\param ir the invoking instance to which port should connect.
 */
count_ptr<const connection_statement::result_type>
connection_statement::make_port_connection(
		const explicit_ports_type& _temp,
		const count_ptr<const inst_ref_arg_type>& ir) {
	typedef	count_ptr<const result_type>	const_return_type;
	typedef	count_ptr<result_type>		return_type;
	typedef	explicit_ports_type		ref_list_type;
	return_type ret(meta_instance_reference_base::make_port_connection(ir));
	NEVER_NULL(ret);
	never_ptr<const definition_base>
		base_def(ir->get_base_def());
	entity::checked_refs_type temp;
	transform(_temp.begin(), _temp.end(), back_inserter(temp), 
		inst_ref_meta_return_type::inst_ref_selector());
	const size_t ir_dim = ir->dimensions();
	if (ir_dim) {
		cerr << "Instance reference port connection must be scalar, "
			"but got a " << ir_dim << "-dim reference!  (";
		ir->dump(cerr, expr_dump_context::default_value) << ")" << endl;
		return const_return_type(NULL);
	} else if (base_def->certify_port_actuals(temp).good) {
		typedef	entity::checked_refs_type::const_iterator
							const_iterator;
		const_iterator i(temp.begin());
		const const_iterator e(temp.end());
		ret->reserve(temp.size());
		for ( ; i!=e; i++) {
			const count_ptr<const meta_instance_reference_base>
				mir(i->is_a<const meta_instance_reference_base>());
			ret->append_meta_instance_reference(mir);
		}
		// transfers ownership
		return const_return_type(ret);
	} else {
		cerr << "At least one error in port connection of (";
		ir->dump(cerr, expr_dump_context::default_value) << ")." << endl;
		return const_return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if INSTANCE_SUPPLY_OVERRIDES
count_ptr<const connection_statement::result_type>
connection_statement::make_implicit_port_override(
		const implicit_ports_type& iports, 
		const count_ptr<const inst_ref_arg_type>& ir) {
	typedef	count_ptr<const result_type>	const_return_type;
	typedef	count_ptr<result_type>		return_type;
	typedef	implicit_ports_type		ref_list_type;
	implicit_port_override::instance_ptr_type
		pr(ir.is_a<const implicit_port_override::instance_type>());
	if (!pr) {
		cerr <<
"Error: only process instances can take implicit port overrides." << endl;
		return return_type(NULL);
	}
	if (iports.size() > 2) {
		cerr <<
"Error: implicit port overrides take at most two ports, Vdd, GND." << endl;
		return return_type(NULL);
	}
	const count_ptr<implicit_port_override>
		ret(new implicit_port_override(pr));
	// NOTE: this order must be consistent with 
	// "Object/def/footprint.cc" auto-connection
	implicit_port_override::port_ptr_type null;
	typedef	implicit_ports_type::const_iterator	const_iterator;
	const_iterator i(iports.begin()), e(iports.end());
	for ( ; i!=e; ++i) {
		ret->append_bool_port(*i ? (*i)->get_bool_var() : null);
	}
#if REVERSE_INSTANCE_SUPPLY_OVERRIDES
	// fill-in remaining spots $() -> $(,)
	size_t j = iports.size();
	for ( ; j<2; ++j) {
		ret->append_bool_port(null);
	}
#endif
	return ret;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Compare this with instance_connection::check_build().
	\return NULL always, rather useless.  
 */
never_ptr<const object>
connection_statement::check_build(context& c) const {
	STACKTRACE("connection_statement::check_build()");
	const inst_ref_expr::meta_return_type
		o(lvalue->check_meta_reference(c));
	if (!o) {
		cerr << "ERROR resolving instance reference of "
			"connection_statement at " << where(*lvalue) << endl;
		THROW_EXIT;
	}
	// is not a complex aggregate instance reference
	const count_ptr<const meta_instance_reference_base>
		inst_ref(o.inst_ref());
	if (!inst_ref) {
		cerr << "Error checking instance reference of "
			"connection statement.  " << where(*lvalue) << endl;
	}

	if (!actuals_base::add_instance_port_connections(inst_ref, c).good) {
		THROW_EXIT;
	}
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class instance_alias method definitions

/**
	Takes a statement like: type foo = bar;
	and virtually produces: type foo; foo = bar;
	\param i the name of the instance being instantiated and connected.  
	\param a right-hand-side alias list, non-const because we will
		prepend i to it, before making it constant.
		This makes it check_build code easier.  
	\param s optional semicolon.  
 */
// CONSTRUCTOR_INLINE
instance_alias::instance_alias(const token_identifier* i, 
		const expr_list* ta, alias_list* a) :
		instance_base(i, ta),
		aliases(
			(NEVER_NULL(a),
			// need deep copy of i as an expression, 
			// because already managed by parent, 
			// and list uses count_ptr<const expr>
			a->push_front(new token_identifier(*i)),
			// caution, unless we add an '=' token to delim_list
			// assertion will be broken, but who cares?
			a)
			// the value of this compound expression is a
		) {
	NEVER_NULL(aliases);
}

DESTRUCTOR_INLINE
instance_alias::~instance_alias() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(instance_alias)

line_position
instance_alias::leftmost(void) const {
	return instance_base::leftmost();
}

line_position
instance_alias::rightmost(void) const {
	if (aliases) return aliases->rightmost();
	else return instance_base::rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Type-checking for an instance declaration with an 
	expression assignment or alias-connection list.  
	First register the declared identifier as an instance.  
	Then type-check the list.  
 */
never_ptr<const object>
instance_alias::check_build(context& c) const {
	STACKTRACE("instance_alias::check_build()");
	// declare and register the instance first
	never_ptr<const object>
		o(instance_base::check_build(c));
	// useless return value (top_namespace)
	// should return reference to the new INSTANCE, not its type
	// what is returned on error?

	// the new instance's identifier is already replicated in the 
	// alias_list, so we can just call it to build 
	// an expression assignment or instance connection object.  
	o = aliases->check_build(c);

	return o;
}

//=============================================================================
// class loop_instantiation method definitions

CONSTRUCTOR_INLINE
loop_instantiation::loop_instantiation(const char_punctuation_type* l,
		const token_identifier* i, const range* g,
		const instance_management_list* b,
		const char_punctuation_type* r) :
		instance_management(),
		lp(l), index(i), rng(g), body(b), rp(r) {
	NEVER_NULL(lp); NEVER_NULL(index);
	NEVER_NULL(rng); NEVER_NULL(body); NEVER_NULL(lp);
}

DESTRUCTOR_INLINE
loop_instantiation::~loop_instantiation() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(loop_instantiation)

line_position
loop_instantiation::leftmost(void) const {
	return lp->leftmost();
}

line_position
loop_instantiation::rightmost(void) const {
	return rp->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: make sure loop bounds do not depend on relaxed formals, 
		if in definition context.
	TODO: open up PRS loop scope? in case one opens up a PRS body?
	TODO: drop empty bodies?
 */
never_ptr<const object>
loop_instantiation::check_build(context& c) const {
	typedef	never_ptr<const object>		return_type;
	const range::meta_return_type r(rng->check_meta_index(c));
	if (!r) {
		cerr << "Error in loop range at " << where(*rng) << endl;
		THROW_EXIT;
	}
	const meta_loop_base::range_ptr_type
		loop_range(meta_range_expr::make_explicit_range(r));
	NEVER_NULL(loop_range);
{
	const context::loop_var_frame _lvf(c, *index);
	const meta_loop_base::ind_var_ptr_type&
		loop_ind(_lvf.var);
	if (!loop_ind) {
		cerr << "Error registering loop variable: " << *index <<
			" at " << where(*index) << endl;
		THROW_EXIT;
	}
	const count_ptr<loop_scope>
		ls(new loop_scope(loop_ind, loop_range));
	excl_ptr<entity::PRS::rule_loop>
		prl(new entity::PRS::rule_loop(loop_ind, loop_range));
	const count_ptr<entity::SPEC::directives_loop>
		srl(new entity::SPEC::directives_loop(loop_ind, loop_range));
	NEVER_NULL(ls);
	NEVER_NULL(prl);
	NEVER_NULL(srl);
	const never_ptr<entity::PRS::rule_loop> prlc(prl);
	c.get_current_prs_body().append_rule(prl);
	c.get_current_spec_body().push_back(srl);
{
	const context::loop_scope_frame _lsf(c, ls);
	const context::prs_body_frame prlf(c, prlc);
	const context::spec_body_frame srlf(c, 
		never_ptr<entity::SPEC::directives_set>(&*srl));
	body->check_build(c);
	// unwind frames upon end of scope
}
	if (prlc->empty()) {
		c.get_current_prs_body().pop_back();
	}
	if (srl->empty()) {
		c.get_current_spec_body().pop_back();
	}
	// otherwise just omit empty loop, is pointless
}
	return return_type(NULL);
}

//=============================================================================
// class guarded_instance_management method definitions
      
CONSTRUCTOR_INLINE
guarded_instance_management::guarded_instance_management(const expr* e,
		const string_punctuation_type* a,
		const instance_management_list* b) :
		instance_management(), guard(e), arrow(a), body(b) {
	NEVER_NULL(guard); NEVER_NULL(arrow);
	// NEVER_NULL(body);
}   

DESTRUCTOR_INLINE
guarded_instance_management::~guarded_instance_management() {
}       

PARSER_WHAT_DEFAULT_IMPLEMENTATION(guarded_instance_management)

line_position
guarded_instance_management::leftmost(void) const {
	return guard->leftmost();
}       

line_position
guarded_instance_management::rightmost(void) const {
	if (body)
		return body->rightmost();
	else	return arrow->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Is else clause handled?
 */
never_ptr<const object>
guarded_instance_management::check_build(context& c) const {
	typedef	never_ptr<const object>		return_type;
	count_ptr<const pbool_expr> guard_expr(NULL);
if (guard && !guard.is_a<const token_else>()) {
	const expr::meta_return_type g(guard->check_meta_expr(c));
	guard_expr = g.is_a<const pbool_expr>();
	if (!guard_expr) {
		cerr << "Error parsing guard expression at " <<
			where(*guard) << endl;
		THROW_EXIT;
	}
}
// allow guard to be NULL for else clause
	// don't forget to open up guarded PRS,SPEC as well (may be empty).
	const count_ptr<const conditional_scope>
		ls(c.get_current_sequential_scope()->back()
			.is_a<const conditional_scope>());
	const never_ptr<entity::PRS::rule_conditional>
		rs(&IS_A(entity::PRS::rule_conditional&, 
			*c.get_current_prs_body().back()));
	const count_ptr<const entity::SPEC::directives_conditional>
		ss(c.get_current_spec_body().back()
			.is_a<const entity::SPEC::directives_conditional>());
	NEVER_NULL(ls);
	NEVER_NULL(rs);
	NEVER_NULL(ss);
	const_cast<conditional_scope&>(*ls)	// kludge
		.append_guarded_clause(guard_expr);	// instance management
	rs->append_guarded_clause(guard_expr);	// PRS
	entity::SPEC::directives_conditional&
		css(const_cast<entity::SPEC::directives_conditional&>(*ss));
	css.append_guarded_clause(guard_expr);	// spec
if (body) {
	const context::conditional_scope_frame _csf(c);
	const context::prs_body_frame _pbf(c, 
		never_ptr<entity::PRS::rule_set_base>(&rs->get_last_clause()));
	const context::spec_body_frame _sbf(c, 
		never_ptr<entity::SPEC::directives_set>(
			&css.get_last_clause()));
	body->check_build(c);
}
	return return_type(NULL);
}

//=============================================================================
// class conditional_instantiation method definitions

CONSTRUCTOR_INLINE
conditional_instantiation::conditional_instantiation(
		const guarded_instance_management_list* n) :
		instance_management(), gd(n) {
	NEVER_NULL(gd);
}

DESTRUCTOR_INLINE
conditional_instantiation::~conditional_instantiation() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(conditional_instantiation)

line_position
conditional_instantiation::leftmost(void) const {
	return gd->leftmost();
}

line_position
conditional_instantiation::rightmost(void) const {
	return gd->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: make sure condition does NOT depend on relaxed formal parameter.  
	Is that restriction really necessary?  (don't think so)
 */
never_ptr<const object>
conditional_instantiation::check_build(context& c) const {
	// ALERT: check_build may throw!
	// to accommodate prs bodies inside conditionals, 
	// we first create an empty conditional_prs, 
	// then let each guarded body append clauses.
	const count_ptr<conditional_scope> ls(new conditional_scope());
	excl_ptr<entity::PRS::rule_conditional>
		rs(new entity::PRS::rule_conditional());
	const count_ptr<entity::SPEC::directives_conditional>
		ss(new entity::SPEC::directives_conditional());
	never_ptr<const entity::PRS::rule_conditional> crs(rs);
	c.get_current_sequential_scope()->push_back(ls);
	c.get_current_prs_body().append_rule(rs);	// xfer ownership
	c.get_current_spec_body().push_back(ss);
	MUST_BE_NULL(rs);
	const never_ptr<const object> ret(gd->check_build(c));
	// empty conditional instance-management and PRS,SPEC can be removed
	if (ls->empty()) {
		c.get_current_sequential_scope()->pop_back();
	}
	if (crs->empty()) {
		c.get_current_prs_body().pop_back();
	}
	if (ss->empty()) {
		c.get_current_spec_body().pop_back();
	}
	return ret;
}

//=============================================================================
// class type_completion_statement method definitions

type_completion_statement::type_completion_statement(const inst_ref_expr* ir, 
		const expr_list* ta, const generic_attribute_list* at) :
		inst_ref(ir), args(ta), attribs(at) {
	NEVER_NULL(inst_ref);
	// NEVER_NULL(args);
}

type_completion_statement::~type_completion_statement() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(type_completion_statement)

line_position
type_completion_statement::leftmost(void) const {
	return inst_ref->leftmost();
}

line_position
type_completion_statement::rightmost(void) const {
	if (attribs)
		return attribs->rightmost();
	else if (args)
		return args->rightmost();
	else	return inst_ref->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note: Reference may be aggregate.
 */
never_ptr<const object>
type_completion_statement::check_build(context& c) const {
	STACKTRACE_VERBOSE;
	// add an auxiliary type_completion statement
	const inst_ref_expr::meta_return_type
		ref(inst_ref->check_meta_reference(c));
	if (!ref) {
		cerr << "Error checking reference at " << where(*inst_ref)
			<< endl;
		THROW_EXIT;
	}
if (args) {
	expr_list::checked_meta_exprs_type temp;
	args->postorder_check_meta_exprs(temp, c);
	// should throw on error
	const count_ptr<const instance_management_base>
		nr(create_type_completion(ref, temp));
	if (!nr) {
		cerr << "Error in type completion at " << where(*this) << endl;
		THROW_EXIT;
	}
	c.add_instance_management(nr);
}
if (attribs) {
	entity::generic_attribute_list_type atts;
	attribs->check_list(atts, &check_generic_attribute, c);
	if (find(atts.begin(), atts.end(), false) != atts.end()) {
		cerr << "ERROR in attributes list.  "
			<< where(*attribs) << endl;
		THROW_EXIT;
	}
	// this will check for registered attributes
	if (ref.value_ref()) {
		cerr << "Error: values do not take attributes.  " 
			<< where(*attribs) << endl;
		THROW_EXIT;
	}
	const count_ptr<meta_instance_reference_base>
		iref(ref.inst_ref());
	NEVER_NULL(iref);
	const count_ptr<const instance_management_base>
		ia(iref->create_instance_attribute(iref, atts));
	if (!ia) {
		cerr << "Error in instance attributes.  "
			<< where(*attribs) << endl;
		THROW_EXIT;
	}
	c.add_instance_management(ia);
}
	// additional error handling?
	return c.top_namespace();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
entity::generic_attribute
type_completion_statement::check_generic_attribute(
		const generic_attribute& a, context& c) {
	typedef	entity::generic_attribute		return_type;
	typedef	expr_list::checked_meta_exprs_type	vals_type;
	typedef	vals_type::const_iterator	const_iterator;
	typedef	vals_type::value_type		val_type;
	// macros are defined per meta-type, check elsewhere
	if (!a.key) {
		cerr << "Error: implicit instance labels not yet supported.  "
			<< where(a) << endl;
		return return_type();
	}
	vals_type vals;
	if (a.values) {
		a.values->postorder_check_meta_exprs(vals, c);
	}
	const const_iterator i(vals.begin()), e(vals.end());
	if (find(i, e, val_type(NULL)) != e) {
		// one of the param expressions failed checking
		// blank will signal error
		cerr << "Error in checking attribute value expressions in "
			<< (a.values ? where(*a.values) : where(a)) << endl;
		return return_type();
	}
	return_type ret(*a.key);
	copy(i, e, std::back_inserter(ret));
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return a template_type_completion statement for unrolling.
 */
count_ptr<const instance_management_base>
type_completion_statement::create_type_completion(
		const inst_ref_expr::meta_return_type& r, 
		const expr_list::checked_meta_exprs_type& a) {
	typedef	template_type_completion<process_tag>	return_type;
	STACKTRACE_VERBOSE;
	const count_ptr<dynamic_param_expr_list> p(new dynamic_param_expr_list);
	copy(a.begin(), a.end(), back_inserter(*p));
	const count_ptr<const entity::process_meta_instance_reference_base>
		pr(r.inst_ref().is_a<const entity::process_meta_instance_reference_base>());
	const never_ptr<const definition_base> d(pr->get_base_def());
	const size_t drf = d->num_relaxed_formals();
	if (drf != a.size()) {
		cerr << "Error: base definition `" << d->get_qualified_name()
			<< "\' expects " << drf <<
			" relaxed parameters, but got " << a.size() <<
			".  " << endl;
		return count_ptr<const return_type>(NULL);
	}
	const count_ptr<const return_type> ret(new return_type(pr, p));
	return ret;
}

//=============================================================================
// class type_completion_connection_statement method definitions

type_completion_connection_statement::type_completion_connection_statement(
		const inst_ref_expr* ir, const expr_list* ta, 
		const actuals_type* p) :
		type_completion_statement(ir, ta), 
		actuals_base(p) {
}

type_completion_connection_statement::~type_completion_connection_statement() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(type_completion_connection_statement)

line_position
type_completion_connection_statement::leftmost(void) const {
	return type_completion_statement::leftmost();
}

line_position
type_completion_connection_statement::rightmost(void) const {
	return actuals_base::rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Combined statement: bind relaxed template parameter
		and make connection.
	Note: reference must be scalar.  
 */
never_ptr<const object>
type_completion_connection_statement::check_build(context& c) const {
	typedef never_ptr<const object> return_type;
	// attach relaxed actuals before connecting
	if (!type_completion_statement::check_build(c)) {
		return return_type(NULL);
	}
	// kind of wasteful to check reference again, but whatever...
	const inst_ref_expr::meta_return_type
		ref(inst_ref->check_meta_reference(c));
	const count_ptr<const connection_statement::inst_ref_arg_type>
		iref(ref.inst_ref());

	if (!actuals_base::add_instance_port_connections(iref, c).good) {
		THROW_EXIT;
	}
	return c.top_namespace();
}

//=============================================================================
// explicit template class instantiations

template class node_list<const instance_base>;
template class node_list<const instance_management>;
template class node_list<const guarded_instance_management>;

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __HAC_AST_INSTANCE_CC__

