/**
	\file "art_parser_instance.cc"
	Class method definitions for ART::parser for instance-related classes.
	$Id: art_parser_instance.cc,v 1.17 2005/01/28 19:58:39 fang Exp $
 */

#ifndef	__ART_PARSER_INSTANCE_CC__
#define	__ART_PARSER_INSTANCE_CC__

// rule-of-thumb for inline directives:
// only inline constructors if you KNOW that they will not be be needed
// outside of this module, because we don't have a means to export
// inline methods other than defining in the header or using
// -fkeep-inline-functions

#include <exception>
#include <iostream>

#include "art_parser_debug.h"
#include "art_switches.h"
#include "art_parser.tcc"

#include "art_parser_instance.h"
#include "art_parser_expr.h"

#include "art_context.h"
#include "art_object_namespace.h"
#include "art_object_instance.h"
#include "art_object_type_ref_base.h"
#include "art_object_inst_ref.h"
#include "art_object_expr_base.h"
#include "art_object_assign.h"
#include "art_object_connect.h"

#include "what.h"
#include "stacktrace.h"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

//=============================================================================
// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT(ART::parser::connection_argument_list, 
	"(connection-arg-list)")
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
}

//=============================================================================
namespace ART {
namespace parser {
#include "using_ostream.h"
USING_STACKTRACE

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
		instance_management(), alias_list_base(e) {
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

/**
	COMPLETELY REDO THIS, since param_literal has been ELIMINATED
		so that param_instance_reference are now subclasses
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
	TRACE_CHECK_BUILD(
		cerr << c.auto_indent() <<
			"alias_list::check_build(...): FINISH ME!";
	)
if (size() > 0) {		// non-empty
	const never_ptr<const object> ret(NULL);
	// can we just re-use parent's check_build()?
	// yes, because we don't need place-holder on stack.
	alias_list_base::check_build(c);
	// errors in individual items will result in NULL on stack.

	// After list items have been built, check types.  
	// Types connected MUST match,
	// and sizes must match if statically known, 
	// else punt until unroll-time.  

	// pop stack items to a local list
	object_list connect;
	{
		int j = size() -1;
		for ( ; j>=0; j--) {
			connect.push_front(c.pop_top_object_stack());
		}
	}

	// case: left-hand-side is a param_instance_reference
	//	then we have expression assignment.  
	// case: left-hand-side is some-other-instance-reference (physical)
	//	then rhs can be arbitrary chain of 
	//	same type instance references.
	// TO DO: in general this needs much work to complete

	const object_list::const_iterator first_obj = connect.begin();

	if (!*first_obj) {
		cerr << endl << "ERROR in the first item in alias-list."
			<< endl;
		THROW_EXIT;
	} else if (first_obj->is_a<const param_instance_reference>()) {
		// then expect subsequent items to be the same
		// or already param_expr in the case of some constants.
		// However, only the last item may be a constant.  

		excl_ptr<param_expression_assignment> exass = 
			connect.make_param_assignment();

		// if all is well, then add this new list to the context's
		// current scope.  
		// idea for error checking:
		// instead of returning NULL, return partially created
		// list with error-markers, maintained in the object
		// and query the error status.  
		// forbid object writing if there are any errors.  
		if (!exass) {
			cerr << "HALT: at least one error in the "
				"assignment list.  " << where() << endl;
			THROW_EXIT;
		} else {
			excl_ptr<const param_expression_assignment>
				exass_c(exass);
			c.add_assignment(exass_c);
			// and transfer ownership
			INVARIANT(!exass_c);
			INVARIANT(!exass.owned());
		}
	} else if (first_obj->is_a<const instance_reference_base>()) {
		excl_ptr<const aliases_connection> connection =
			connect.make_alias_connection();
		// also type-checks connections
		if (!connection) {
			cerr << "HALT: at least one error in connection list.  "
				<< where() << endl;
			THROW_EXIT;
		} else {
			excl_ptr<const instance_reference_connection>
				ircp = connection.as_a_xfer<const instance_reference_connection>();
			c.add_connection(ircp);
			INVARIANT(!ircp);
			assert(!connection.owned());
		}
	} else {
		// ERROR
		cerr << "WTF? first element of alias_list is not "
			"an instance reference!"
			<< endl;
		THROW_EXIT;
	}

	return ret;
} else {
	// will this ever be empty?  will be caught as error for now.
	DIE;
	return never_ptr<const object>(NULL);
}
}

//=============================================================================
// class connection_argument_list method definition

CONSTRUCTOR_INLINE
connection_argument_list::connection_argument_list(expr_list* e) :
		expr_list() {
	e->release_append(*this);
	excl_ptr<expr_list> delete_me(e);
}

DESTRUCTOR_INLINE
connection_argument_list::~connection_argument_list() {
}

#if 0
ostream&
connection_argument_list::what(ostream& o) const {
	return o << "(connection-arg-list)";       
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(connection_argument_list)
#endif

/**
	Type checks a expression list in the connection argument context.
	TO DO: manipulate context using definition_base.    
	\param c the context object.      
	\return
 */
never_ptr<const object>
connection_argument_list::check_build(context& c) const {
	STACKTRACE("connection_argument_list::check_build()");
	TRACE_CHECK_BUILD(
		cerr << c.auto_indent() <<
			"connection_argument_list::check_build(...): " << endl;
	)
	// enter the connection argument context
	const never_ptr<const object>
		o(expr_list::check_build(c));
	// leave the connection argument context
	return o;
}

//=============================================================================
// class actuals_base method definitions

CONSTRUCTOR_INLINE
actuals_base::actuals_base(const expr_list* a) : 
		instance_management(), actuals(a) {
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

/**
	Just a wrapped call to expr_list::check_build.
 */
never_ptr<const object>
actuals_base::check_build(context& c) const {
	STACKTRACE("actuals_base::check_build()");
	return actuals->check_build(c);
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
#if 0
	return id->what(o << "(declaration-id): ");
#else
	return id->what(o << util::what<instance_base>::name() << ": ");
#endif
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
	TRACE_CHECK_BUILD(
		what(cerr << c.auto_indent())
			<< "instance_base::check_build(...): ";
	)

	// uses c.current_fundamental_type
	const never_ptr<const instance_collection_base>
		inst(c.add_instance(*id));	// check return value?
	if (!inst) {
		cerr << "ERROR with " << *id << " at " << id->where() << endl;
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
#if 0
	return ranges->what(id->what(o << "(declaration-array): "));
#else
	return ranges->what(
		id->what(o << util::what<instance_array>::name() << ": "));
#endif
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
	TRACE_CHECK_BUILD(
		cerr << c.auto_indent() <<
			"instance_array::check_build(...): " << endl;
	)
	if (ranges) {
		ranges->check_build(c);
		// expecting ranges and singe integer expressions
		const count_ptr<object> o(c.pop_top_object_stack());
		// expect constructed (sparse) range_list on object stack
		if (!o) {
			cerr << "ERROR in dimensions!  " << 
				ranges->where() << endl;
			THROW_EXIT;
		}
		const count_ptr<object_list>
			ol(o.is_a<object_list>());
		NEVER_NULL(ol);
		// would rather have excl_ptr...
		const count_ptr<range_expr_list>
			d(ol->make_sparse_range_list());
		if (!d) {
			cerr << "ERROR in building sparse range list!  "
				<< ranges->where() << endl;
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

instance_id_list::instance_id_list(const instance_base* i) : parent(i) { }

instance_id_list::~instance_id_list() { }

//=============================================================================
// class instance_declaration method definitions

/**
	Creates an instance declaration, which may contain a list of 
	identifiers to instantiation.  
	\param t the base type (no array).  
	\param i the identifier list (may contain arrays).
	\param s the terminating semicolon.  
 */
CONSTRUCTOR_INLINE
instance_declaration::instance_declaration(const concrete_type_ref* t, 
	const instance_id_list* i, const terminal* s) :
		instance_management(),
		type(t), ids(i), semi(s) {
	NEVER_NULL(type);
	NEVER_NULL(ids);
}

DESTRUCTOR_INLINE
instance_declaration::~instance_declaration() {
}

#if 0
ostream&
instance_declaration::what(ostream& o) const {
	return o << "(instance-decl)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(instance_declaration)
#endif

line_position
instance_declaration::leftmost(void) const {
	return type->leftmost();
}

line_position
instance_declaration::rightmost(void) const {
	return semi->rightmost();
}

never_ptr<const object>
instance_declaration::check_build(context& c) const {
	STACKTRACE("instance_declaration::check_build()");
	TRACE_CHECK_BUILD(
		what(cerr << c.auto_indent()) <<
			"instance_declaration::check_build(...): ";
	)
	type->check_build(c);
	// useless return value
	const count_ptr<const fundamental_type_reference>
		ftr(c.get_current_fundamental_type());
		// should set the current_fundamental_type
	c.pop_current_definition_reference();
		// no longer need the base definition
	if (ftr) {
		ids->check_build(c);		// return value?
	} else {
		cerr << "ERROR with concrete-type to instantiate at "
			<< type->where() << endl;
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
		const expr_list* a, const terminal* s) :
		instance_base(i), actuals_base(a), semi(s) {
}

DESTRUCTOR_INLINE
instance_connection::~instance_connection() {
}

// remember to check for declaration context when checking id

#if 0
ostream&
instance_connection::what(ostream& o) const {
	return o << "(actuals-connection)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(instance_connection)
#endif

line_position
instance_connection::leftmost(void) const {
	return instance_base::leftmost();
}

line_position
instance_connection::rightmost(void) const {
	if (semi) return semi->rightmost();
	else return actuals->rightmost();
}

never_ptr<const object>
instance_connection::check_build(context& c) const {
	typedef	never_ptr<const object>		return_type;
	STACKTRACE("instance_connection::check_build()");
	TRACE_CHECK_BUILD(
		what(cerr << c.auto_indent()) <<
			"instance_connection::check_build(...): ";
	)
	const never_ptr<const object>
		o(instance_base::check_build(c));
	if (!o) {
		// instance_base already prints error message...
//		cerr << "ERROR with " << *id << " at " << id->where() << endl;
		THROW_EXIT;
		return return_type(NULL);
	}

	// lookup the instantiation we just created
	id->check_build(c);
	// expect instance_reference on object_stack
	count_ptr<const object> obj(c.pop_top_object_stack());
	NEVER_NULL(obj);		// we just created it!
	const count_ptr<const simple_instance_reference>
		inst_ref(obj.is_a<const simple_instance_reference>());
	NEVER_NULL(inst_ref);

	actuals_base::check_build(c);
	obj = c.pop_top_object_stack();
	if (!obj) {
		cerr << "ERROR in object_list produced at "
			<< actuals_base::where() << endl;
		THROW_EXIT;
	}
	const count_ptr<const object_list>
		obj_list(obj.is_a<const object_list>());
	NEVER_NULL(obj_list);

	excl_ptr<const port_connection>
		port_con = obj_list->make_port_connection(inst_ref);
	if (!port_con) {
		cerr << "HALT: at least one error in port connection list.  "
			<< where() << endl;
		THROW_EXIT;
	} else {
		excl_ptr<const instance_reference_connection>
			ircp = port_con.as_a_xfer<const instance_reference_connection>();
		c.add_connection(ircp);
		INVARIANT(!ircp);
		INVARIANT(!port_con.owned());	// explicit transfer
	}
	return return_type(NULL);
}

//=============================================================================
// class connection_statement method definitions

CONSTRUCTOR_INLINE
connection_statement::connection_statement(const expr* l, const expr_list* a, 
		const terminal* s) : actuals_base(a), lvalue(l), semi(s) {
	NEVER_NULL(lvalue);
}

DESTRUCTOR_INLINE
connection_statement::~connection_statement() {
}

#if 0
ostream&
connection_statement::what(ostream& o) const {
	return o << "(connection-statement)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(connection_statement)
#endif

line_position
connection_statement::leftmost(void) const {
	return lvalue->leftmost();
}

line_position
connection_statement::rightmost(void) const {
	if (semi) return semi->rightmost();
	else return actuals->rightmost();
}

/**
	\return NULL always, rather useless.  
 */
never_ptr<const object>
connection_statement::check_build(context& c) const {
	STACKTRACE("connection_statement::check_build()");
	TRACE_CHECK_BUILD(
		what(cerr << c.auto_indent()) <<
			"connection_statement::check_build(...): ";
	)
	lvalue->check_build(c);
	// useless return value, expect instance_reference_base on object_stack
	count_ptr<const object> o(c.pop_top_object_stack());
	if (!o) {
		cerr << "ERROR resolving instance reference of "
			"connection_statement at " << lvalue->where() << endl;
		THROW_EXIT;
	}
	const count_ptr<const simple_instance_reference>
		inst_ref(o.is_a<const simple_instance_reference>());
	NEVER_NULL(inst_ref);

	actuals_base::check_build(c);
	// useless return value, expect an object_list on object_stack
	o = c.pop_top_object_stack();
	if (!o) {
		cerr << "ERROR in object_list produced at "
			<< actuals_base::where() << endl;
		THROW_EXIT;
	}
	const count_ptr<const object_list>
		obj_list(o.is_a<const object_list>());
	NEVER_NULL(obj_list);

	excl_ptr<const port_connection>
		port_con = obj_list->make_port_connection(inst_ref);
	if (!port_con) {
		cerr << "HALT: at least one error in port connection list.  "
			<< where() << endl;
		THROW_EXIT;
	} else {
		excl_ptr<const instance_reference_connection>
			ircp = port_con.as_a_xfer<const instance_reference_connection>();
		c.add_connection(ircp);
		INVARIANT(!ircp);
		INVARIANT(!port_con.owned());	// explicit transfer
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
instance_alias::instance_alias(const token_identifier* i, alias_list* a, 
		const terminal* s) :
		instance_base(i),
		aliases(
			(NEVER_NULL(a),
			// need deep copy of i as an expression, 
			// because already managed by parent, 
			// and list uses count_ptr<const expr>
			a->push_front(count_ptr<const token_identifier>(
				new token_identifier(*i))),
			// caution, unless we add an '=' token to delim_list
			// assertion will be broken, but who cares?
			a)
			// the value of this compound expression is a
		),
		semi(s) {
	NEVER_NULL(aliases);
}

DESTRUCTOR_INLINE
instance_alias::~instance_alias() {
}

#if 0
ostream&
instance_alias::what(ostream& o) const {
	return o << "(alias-assign)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(instance_alias)
#endif

line_position
instance_alias::leftmost(void) const {
	return instance_base::leftmost();
}

line_position
instance_alias::rightmost(void) const {
	if (semi) return semi->rightmost();
	else if (aliases) return aliases->rightmost();
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
	TRACE_CHECK_BUILD(
		what(cerr << c.auto_indent()) <<
			"instance_alias::check_build(...)";
	)
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
loop_instantiation::loop_instantiation(const terminal* l, const terminal* d, 
		const token_identifier* i, const terminal* c1, 
		const range* g, const terminal* c2, 
		const definition_body* b, const terminal* r) :
		instance_management(),
		lp(l), delim(d), index(i), colon1(c1), 
		rng(g), colon2(c2), body(b), rp(r) {
	NEVER_NULL(lp); NEVER_NULL(delim); NEVER_NULL(index);
	NEVER_NULL(colon); NEVER_NULL(rng); NEVER_NULL(body); NEVER_NULL(lp);
}

DESTRUCTOR_INLINE
loop_instantiation::~loop_instantiation() {
}

#if 0
ostream&
loop_instantiation::what(ostream& o) const {
	return o << "(loop-instance)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(loop_instantiation)
#endif

line_position
loop_instantiation::leftmost(void) const {
	if (lp) return lp->leftmost();
	else return delim->leftmost();
}

line_position
loop_instantiation::rightmost(void) const {
	if (rp) return rp->rightmost();
	else if (body) return body->rightmost();
	else if (colon2) return colon2->rightmost();
	else return rng->rightmost();
}

//=============================================================================
// class guarded_definition_body method definitions
      
CONSTRUCTOR_INLINE
guarded_definition_body::guarded_definition_body(const expr* e,
		const terminal* a, const definition_body* b) :
		instance_management(), guard(e), arrow(a), body(b) {
	NEVER_NULL(guard); NEVER_NULL(arrow); NEVER_NULL(body);
}   

DESTRUCTOR_INLINE
guarded_definition_body::~guarded_definition_body() {
}       

#if 0
ostream&
guarded_definition_body::what(ostream& o) const {      
	return o << "(guarded-def-body)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(guarded_definition_body)
#endif

line_position
guarded_definition_body::leftmost(void) const {
	return guard->leftmost();
}       

line_position
guarded_definition_body::rightmost(void) const {
	return body->rightmost();
}

//=============================================================================
// class guarded_definition_body_list method definitions

guarded_definition_body_list::guarded_definition_body_list(
		const guarded_definition_body* g) :
		parent(g) { }

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

#if 0
ostream&
conditional_instantiation::what(ostream& o) const {
	return o << "(conditional-instance)";
}
#else
PARSER_WHAT_DEFAULT_IMPLEMENTATION(conditional_instantiation)
#endif

line_position
conditional_instantiation::leftmost(void) const {
	return gd->leftmost();
}

line_position
conditional_instantiation::rightmost(void) const {
	return gd->rightmost();
}

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __ART_PARSER_INSTANCE_CC__

