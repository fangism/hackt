/**
	\file "AST/art_parser_instance.cc"
	Class method definitions for ART::parser for instance-related classes.
	$Id: art_parser_instance.cc,v 1.27 2005/06/19 01:58:30 fang Exp $
 */

#ifndef	__AST_ART_PARSER_INSTANCE_CC__
#define	__AST_ART_PARSER_INSTANCE_CC__

#define	ENABLE_STACKTRACE		0

#include <exception>
#include <iostream>
#include <utility>
#include <functional>
#include <numeric>

#include "AST/art_parser_instance.h"
#include "AST/art_parser_expr.h"		// for index_expr
#include "AST/art_parser_expr_list.h"
#include "AST/art_parser_range_list.h"
#include "AST/art_parser_token_string.h"
#include "AST/art_parser_type.h"
#include "AST/art_parser_node_list.tcc"

#include "Object/art_context.h"
#include "Object/art_object_namespace.h"
#include "Object/art_object_instance.h"
#include "Object/art_object_type_ref_base.h"
#include "Object/art_object_inst_ref.h"
#include "Object/art_object_expr_base.h"
#include "Object/art_object_assign.h"
#include "Object/art_object_connect.h"

#include "util/what.h"
#include "util/stacktrace.h"
#include "util/dereference.h"
#include "util/compose.h"
#include "util/binders.h"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

//=============================================================================
// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT(ART::parser::instance_base, 
	"(declaration-id)")
SPECIALIZE_UTIL_WHAT(ART::parser::instance_array, 
	"(declaration-array)")
SPECIALIZE_UTIL_WHAT(ART::parser::instance_declaration, 
	"(instance-decl)")
SPECIALIZE_UTIL_WHAT(ART::parser::instance_connection, 
	"(actuals-connection)")
SPECIALIZE_UTIL_WHAT(ART::parser::connection_statement, 
	"(connection-statement)")
SPECIALIZE_UTIL_WHAT(ART::parser::instance_alias, 
	"(alias-assign)")
SPECIALIZE_UTIL_WHAT(ART::parser::loop_instantiation, 
	"(loop-instance)")
SPECIALIZE_UTIL_WHAT(ART::parser::guarded_definition_body, 
	"(guarded-def-body)")
SPECIALIZE_UTIL_WHAT(ART::parser::conditional_instantiation, 
	"(conditional-instance)")
SPECIALIZE_UTIL_WHAT(ART::parser::type_completion_statement, 
	"(type-completion)")
SPECIALIZE_UTIL_WHAT(ART::parser::type_completion_connection_statement, 
	"(type-completion-connection)")
}

//=============================================================================
namespace ART {
namespace parser {
#include "util/using_ostream.h"
USING_STACKTRACE
using util::dereference;
using std::transform;
using std::mem_fun_ref;
using ADS::unary_compose;
using std::bind2nd_argval;
using std::accumulate;
using std::_Select1st;
using std::_Select2nd;
using std::find;
using entity::meta_instance_reference_base;
using entity::simple_meta_instance_reference_base;
using entity::aliases_connection_base;
using entity::meta_instance_reference_connection;
using entity::port_connection;

//=============================================================================
// class instance_management method definitions

CONSTRUCTOR_INLINE
instance_management::instance_management() : def_body_item(), root_item() {
}

DESTRUCTOR_INLINE
instance_management::~instance_management() {
}

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
excl_ptr<const entity::param_expression_assignment>
alias_list::make_param_assignment(const checked_meta_exprs_type& temp) {
	typedef	excl_ptr<const entity::param_expression_assignment>
							const_return_type;
	typedef	excl_ptr<entity::param_expression_assignment>
							return_type;
	typedef	checked_meta_exprs_type::value_type		checked_expr_ptr_type;
	// then expect subsequent items to be the same
	// or already param_expr in the case of some constants.
	// However, only the last item may be a constant.  

	bad_bool err(false);
	// right-hand-side source expression
	const checked_expr_ptr_type& last_obj = temp.back();
	const count_ptr<const param_expr>
		rhse = last_obj.is_a<const param_expr>();
	INVARIANT(rhse);

	return_type ret;
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

	entity::param_expression_assignment::meta_instance_reference_appender
		append_it(*ret);
	const checked_meta_exprs_type::const_iterator dest_end = --temp.end();
	checked_meta_exprs_type::const_iterator dest_iter = temp.begin();
	err = accumulate(dest_iter, dest_end, err, append_it);

	// if there are any errors, discard everything?
	// later: track errors in partially constructed objects
	if (err.bad) {
		return const_return_type(NULL);
	} else	return const_return_type(ret);             // is ok
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates an alias connection object, given a list of instance
	references.  Performs type-checking.

	TODO: once we separate objects and stacks into different types,
	then we can eliminate this generic object list altogether.
	(SOON...)
 */
excl_ptr<const entity::aliases_connection_base>
alias_list::make_alias_connection(const checked_meta_refs_type& temp) {
	typedef excl_ptr<const aliases_connection_base> const_return_type;
	typedef excl_ptr<aliases_connection_base> 	return_type;
	checked_meta_refs_type::const_iterator i = temp.begin();
	INVARIANT(temp.size() > 1);          // else what are you connecting?
	const count_ptr<const meta_instance_reference_base> fir(*i);
//		fir(i->is_a<const meta_instance_reference_base>());
	NEVER_NULL(fir);
	return_type ret = 
		entity::meta_instance_reference_base::make_aliases_connection(fir);
	// keep this around for type-checking comparisons
	ret->append_meta_instance_reference(fir);
	// starting with second instance reference, type-check and alias
	int j = 2;
	for (i++; i!=temp.end(); i++, j++) {
		const count_ptr<const meta_instance_reference_base> ir(*i);
		INVARIANT(ir);
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
//	check_list(temp, &expr::check_generic, c);
	postorder_check_meta_generic(temp, c);
	const checked_meta_generic_type::const_iterator first_obj = temp.begin();
	const checked_meta_generic_type::const_iterator end_obj = temp.end();
	if (!first_obj->first && !first_obj->second) {
		cerr << endl << "ERROR in the first item in alias-list."
			<< endl;
		THROW_EXIT;
	} else if (first_obj->first) {
		checked_meta_exprs_type checked_exprs;
		expr_list::select_checked_meta_exprs(temp, checked_exprs);
		
		// then expect subsequent items to be the same
		// or already param_expr in the case of some constants.
		// However, only the last item may be a constant.  

		excl_ptr<const param_expression_assignment>
			exass = make_param_assignment(checked_exprs);

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
	} else if (first_obj->second) {
		checked_meta_refs_type checked_refs;
		expr_list::select_checked_meta_refs(temp, checked_refs);

		excl_ptr<const aliases_connection_base>
			connection = make_alias_connection(checked_refs);
		// also type-checks connections
		if (!connection) {
			cerr << "HALT: at least one error in connection list.  "
				<< where(*this) << endl;
			THROW_EXIT;
		} else {
			excl_ptr<const meta_instance_reference_connection>
				ircp = connection.as_a_xfer<const meta_instance_reference_connection>();
			c.add_connection(ircp);
			INVARIANT(!ircp);
			INVARIANT(!connection.owned());
		}
	} else {
		// ERROR
		cerr << "WTF? first element of alias_list is not "
			"an instance reference!"
			<< endl;
		THROW_EXIT;
	}
} else {
	// will this ever be empty?  will be caught as error for now.
	DIE;
}
	// useless return value
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class actuals_base method definitions

CONSTRUCTOR_INLINE
actuals_base::actuals_base(const expr_list* a) : 
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
actuals_base::check_actuals(expr_list::checked_meta_refs_type& ret,
		context& c) const {
	STACKTRACE("actuals_base::check_actuals()");
	expr_list::checked_meta_generic_type temp;
	actuals->postorder_check_meta_generic(temp, c);
	expr_list::select_checked_meta_refs(temp, ret);
	expr_list::checked_meta_generic_type::const_iterator
		c_iter = temp.begin();
	expr_list::const_iterator e_iter = actuals->begin();
	const expr_list::const_iterator e_end = actuals->end();
	for ( ; e_iter != e_end; e_iter++, c_iter++) {
		if (*e_iter) {
			if (!c_iter->first && !c_iter->second)
				return good_bool(false);
			// both results are NULL => check falied
		}
		// else expression is null; skip it
	}
	// all relevant checks passed
	return good_bool(true);
}

//=============================================================================
// class instance_base method definitions

CONSTRUCTOR_INLINE
instance_base::instance_base(const token_identifier* i) :
		instance_management(), id(i) {
	NEVER_NULL(id);
}

DESTRUCTOR_INLINE
instance_base::~instance_base() {
}

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
	return id->rightmost();
}

/**
	Eventually don't return top_namespace, 
	but a pointer to the created instance_base
	so that it may be used by instance_alias.  
 */
never_ptr<const object>
instance_base::check_build(context& c) const {
	STACKTRACE("instance_base::check_build()");

	// uses c.current_fundamental_type
	const never_ptr<const instance_collection_base>
		inst(c.add_instance(*id));	// check return value?
	if (!inst) {
		cerr << "ERROR with " << *id << " at " << where(*id) << endl;
		return never_ptr<const object>(NULL);
	}
	// need current_instance?  no, not using as reference.
	// return inst;
	return c.top_namespace();
}

//=============================================================================
// class instance_array method definitions

CONSTRUCTOR_INLINE
instance_array::instance_array(const token_identifier* i, 
		const range_list* rl) : instance_base(i), ranges(rl) {
	// ranges may be NULL, equivalent to declaration base
}

DESTRUCTOR_INLINE
instance_array::~instance_array() {
}

ostream&
instance_array::what(ostream& o) const {
	return ranges->what(
		id->what(o << util::what<instance_array>::name() << ": "));
}

line_position
instance_array::rightmost(void) const {
	return ranges->rightmost();
}

/**
	Instantiates an array of instances.  
	Dimensions may be dense or sparse.  
	See also template_formal_id::check_build, 
		port_formal_id::check_build.  
 */
never_ptr<const object>
instance_array::check_build(context& c) const {
	STACKTRACE("instance_array::check_build()");
	if (ranges) {
		const range_list::checked_meta_ranges_type
			d(ranges->check_meta_ranges(c));
		if (!d) {
			cerr << "ERROR in building sparse range list!  " <<
				where(*ranges) << endl;
			THROW_EXIT;
		}
		const never_ptr<const instance_collection_base>
			t(c.add_instance(*id, d));
		// if there was error, would've THROW_EXIT'd (temporary)
		return t;
	} else {
		return instance_base::check_build(c);
	}
}

//=============================================================================
// class instance_id_list method definitions

instance_id_list::instance_id_list(const instance_base* i) : parent_type(i) { }

instance_id_list::~instance_id_list() { }

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

DESTRUCTOR_INLINE
instance_declaration::~instance_declaration() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(instance_declaration)

line_position
instance_declaration::leftmost(void) const {
	return type->leftmost();
}

line_position
instance_declaration::rightmost(void) const {
	return ids->rightmost();
}

never_ptr<const object>
instance_declaration::check_build(context& c) const {
	STACKTRACE("instance_declaration::check_build()");
	const count_ptr<const fundamental_type_reference>
		ftr(type->check_type(c));
	c.set_current_fundamental_type(ftr);

	if (ftr) {
		ids->check_build(c);		// return value?
	} else {
		// already have error message
		return never_ptr<const object>(NULL);
	}
	// instance could be ANY type
	c.reset_current_fundamental_type();	// the type to instantiate
	return c.top_namespace();
}

//=============================================================================
// class instance_connection method definitions

CONSTRUCTOR_INLINE
instance_connection::instance_connection(const token_identifier* i, 
		const expr_list* a) :
		instance_base(i), actuals_base(a) {
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
	const count_ptr<const simple_meta_instance_reference_base>
		inst_ref(obj.is_a<const simple_meta_instance_reference_base>());
	NEVER_NULL(inst_ref);

	expr_list::checked_meta_refs_type temp;
	if (actuals_base::check_actuals(temp, c).good) {

	excl_ptr<const port_connection>
		port_con = connection_statement::make_port_connection(
			temp, inst_ref);
	if (!port_con) {
		cerr << "HALT: at least one error in port connection list.  "
			<< where(*this) << endl;
		THROW_EXIT;
	} else {
		excl_ptr<const meta_instance_reference_connection>
			ircp = port_con.as_a_xfer<const meta_instance_reference_connection>();
		c.add_connection(ircp);
		INVARIANT(!ircp);
		INVARIANT(!port_con.owned());	// explicit transfer
	}
	} else {
		cerr << "ERROR in object_list produced at "
			<< where(*actuals) << endl;
		THROW_EXIT;
	}
	return return_type(NULL);
}

//=============================================================================
// class connection_statement method definitions

CONSTRUCTOR_INLINE
connection_statement::connection_statement(
		const inst_ref_expr* l, const expr_list* a) :
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
excl_ptr<const entity::port_connection>
connection_statement::make_port_connection(
		const expr_list::checked_meta_refs_type& temp,
		const count_ptr<const entity::simple_meta_instance_reference_base>& ir) {
	typedef	excl_ptr<const port_connection>		return_type;
	typedef	expr_list::checked_meta_refs_type		ref_list_type;
	excl_ptr<port_connection>
		ret(new entity::port_connection(ir));
	never_ptr<const definition_base>
		base_def(ir->get_base_def());

	const size_t ir_dim = ir->dimensions();
	if (ir_dim) {
		cerr << "Instance reference port connection must be scalar, "
			"but got a " << ir_dim << "-dim reference!" << endl;
		return return_type(NULL);
	} else if (base_def->certify_port_actuals(temp).good) {
		ref_list_type::const_iterator i = temp.begin();
		const ref_list_type::const_iterator e = temp.end();
		for ( ; i!=e; i++) {
			count_ptr<const meta_instance_reference_base>
				ir(i->is_a<const meta_instance_reference_base>());
			ret->append_meta_instance_reference(ir);
		}
		// transfers ownership
		return return_type(ret);
	} else {
		cerr << "At least one error in port connection.  " << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
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
	const count_ptr<const simple_meta_instance_reference_base>
		inst_ref(o.is_a<const simple_meta_instance_reference_base>());
	NEVER_NULL(inst_ref);

	expr_list::checked_meta_refs_type temp;
	if (actuals_base::check_actuals(temp, c).good) {
	// useless return value, expect an object_list on object_stack
	excl_ptr<const port_connection>
		port_con = make_port_connection(temp, inst_ref);
	if (!port_con) {
		cerr << "HALT: at least one error in port connection list.  "
			<< where(*this) << endl;
		THROW_EXIT;
	} else {
		excl_ptr<const meta_instance_reference_connection>
			ircp = port_con.as_a_xfer<const meta_instance_reference_connection>();
		c.add_connection(ircp);
		INVARIANT(!ircp);
		INVARIANT(!port_con.owned());	// explicit transfer
	}
	} else {
		cerr << "ERROR in object_list produced at "
			<< where(*actuals) << endl;
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
instance_alias::instance_alias(const token_identifier* i, alias_list* a) :
		instance_base(i),
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

/**
	Type-checking for an instance declaration with an 
	expression assignment or alias-connection list.  
	First register the declared indentifier as an instance.  
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
		const definition_body* b, const char_punctuation_type* r) :
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

never_ptr<const object>
loop_instantiation::check_build(context& c) const {
	cerr << "Fang, write loop_instantiation::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class guarded_definition_body method definitions
      
CONSTRUCTOR_INLINE
guarded_definition_body::guarded_definition_body(const expr* e,
		const string_punctuation_type* a, const definition_body* b) :
		instance_management(), guard(e), arrow(a), body(b) {
	NEVER_NULL(guard); NEVER_NULL(arrow); NEVER_NULL(body);
}   

DESTRUCTOR_INLINE
guarded_definition_body::~guarded_definition_body() {
}       

PARSER_WHAT_DEFAULT_IMPLEMENTATION(guarded_definition_body)

line_position
guarded_definition_body::leftmost(void) const {
	return guard->leftmost();
}       

line_position
guarded_definition_body::rightmost(void) const {
	return body->rightmost();
}

never_ptr<const object>
guarded_definition_body::check_build(context& c) const {
	cerr << "Fang, finish guarded_definition_body::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class guarded_definition_body_list method definitions

guarded_definition_body_list::guarded_definition_body_list(
		const guarded_definition_body* g) :
		parent_type(g) { }

guarded_definition_body_list::~guarded_definition_body_list() { }

//=============================================================================
// class conditional_instantiation method definitions

CONSTRUCTOR_INLINE
conditional_instantiation::conditional_instantiation(
		const guarded_definition_body_list* n) :
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

#if 1
never_ptr<const object>
conditional_instantiation::check_build(context& c) const {
	cerr << "Fang, write conditional_instantiation::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}
#endif

//=============================================================================
// class type_completion_statement method definitions

type_completion_statement::type_completion_statement(const index_expr* ir, 
		const expr_list* ta) : inst_ref(ir), args(ta) {
	NEVER_NULL(inst_ref);
	NEVER_NULL(args);
}

type_completion_statement::~type_completion_statement() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(type_completion_statement)

line_position
type_completion_statement::leftmost(void) const {
	return inst_ref->leftmost();
}

line_position
type_completion_statement::rightmost(void) const {
	return args->rightmost();
}

never_ptr<const object>
type_completion_statement::check_build(context& c) const {
	cerr << "FANG, write type_completion_statement::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class type_completion_connection_statement method definitions

type_completion_connection_statement::type_completion_connection_statement(
		const index_expr* ir, const expr_list* ta, const expr_list* p) :
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

never_ptr<const object>
type_completion_connection_statement::check_build(context& c) const {
	cerr << "FANG, write type_completion_connection_statement::check_build()!" << endl;
	return never_ptr<const object>(NULL);
}

//=============================================================================
// explicit template class instantiations

template class node_list<const instance_base>;
template class node_list<const guarded_definition_body>;

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __AST_ART_PARSER_INSTANCE_CC__

