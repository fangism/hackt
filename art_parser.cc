// "art_parser.cc"
// class method definitions for ART::parser

// template instantiations are at the end of the file

// rule-of-thumb for inline directives:
// only inline constructors if you KNOW that they will not be be needed
// outside of this module, because we don't have a means to export
// inline methods other than defining in the header or using
// -fkeep-inline-functions

#include <iostream>
#include <vector>

#include "art_parser_debug.h"
#include "art_macros.h"
#include "art_switches.h"
#include "art_parser_template_methods.h"

#include "art_symbol_table.h"
#include "art_object.h"
#include "art_object_expr.h"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

//=============================================================================
// debug section

//=============================================================================
namespace ART {
namespace parser {
using namespace std;

//=============================================================================
// global constants
const char none[] = "";		///< delimiter for node_list template argument
const char comma[] = ",";	///< delimiter for node_list template argument
const char semicolon[] = ";";	///< delimiter for node_list template argument
const char scope[] = "::";	///< delimiter for node_list template argument
const char thickbar[] = "[]";	///< delimiter for node_list template argument
const char colon[] = ":";	///< delimiter for node_list template argument
const char alias[] = "=";	///< delimiter for node_list template argument

//=============================================================================
// class node method definitions

/// node's virtual destructor, the mother of all virtual destructors
node::~node() { }

/// reports location spanned by a node in the source file
inline
line_range
node::where(void) const {
	return line_range(leftmost(), rightmost());
}

/**
	Default type-checker and object builder does nothing.  
	Should be re-implemented in all terminal subclasses.  
	Eventually make this pure virtual.  
	Should really take a context&...
 */
never_const_ptr<object>
node::check_build(never_ptr<context> c) const {
	// We DO want to print this message, even in regression testing. 
	what(cerr << c->auto_indent() << 
		"check_build() not implemented yet for ");
	return c->top_namespace();
}

//=============================================================================
// class root_item method definitions

/// Empty constructor
CONSTRUCTOR_INLINE
root_item::root_item() : node() { }

/// Empty virtual destructor
DESTRUCTOR_INLINE
root_item::~root_item() { }

//=============================================================================
// class instance_management method definitions

CONSTRUCTOR_INLINE
instance_management::instance_management() : def_body_item(), root_item() {
}

DESTRUCTOR_INLINE
instance_management::~instance_management() {
}

//=============================================================================
// class template_argument_list method definition

CONSTRUCTOR_INLINE
template_argument_list::template_argument_list(expr_list* e) : expr_list() {
	e->release_append(*this);
}

DESTRUCTOR_INLINE
template_argument_list::~template_argument_list() {
}

ostream&
template_argument_list::what(ostream& o) const {
	return o << "(template-arg-list)";
}

/**
	Type checks a expression list in the template argument context.  
	First builds a list of parameter expression objects.  
	Should expr_list do this automatically?
	Can it be used for both template and port arguments?
	TO DO: manipulate context using definition_base.  
	\param c the context object -- its current_definition_reference
		must be set to a valid definition, because this
		uses that definition to type-check.  
	\return NULL always?  How does caller know something went wrong?
 */
never_const_ptr<object>
template_argument_list::check_build(never_ptr<context> c) const {
	excl_ptr<template_param_list> targs(new template_param_list);
	assert(targs);
	TRACE_CHECK_BUILD(
		cerr << c->auto_indent() <<
			"template_argument_list::check_build(...): " << endl;
	)
	// enter the template argument context
	// o = expr_list::check_build(c);	// DON'T USE, override
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		count_const_ptr<expr> e(*i);
		assert(e);			// ever blank expression?
		// this should cache parameter expressions
		never_const_ptr<object> eret(e->check_build(c));
		if (eret) {
			never_const_ptr<param_expr>
				exref(eret.is_a<param_expr>());
			assert(exref);
			targs->push_back(exref);
		} else {
			// failed!!!  better error handling later
			cerr << "BAD template argument (not an expression)!"
				<< endl;
			exit(1);
		}
	}
	// set context's template arguments
	c->set_current_template_arguments(targs);
	// leave the template argument context
	return never_const_ptr<object>(NULL);
	// set the current_fundamental_type upon returning from this
}

//=============================================================================
// class connection_argument_list method definition

CONSTRUCTOR_INLINE
connection_argument_list::connection_argument_list(expr_list* e) : expr_list() {
	e->release_append(*this);
}

DESTRUCTOR_INLINE
connection_argument_list::~connection_argument_list() {
}

ostream&
connection_argument_list::what(ostream& o) const {
	return o << "(connection-arg-list)";
}

/**
	Type checks a expression list in the connection argument context.  
	TO DO: manipulate context using definition_base.  
	\param c the context object.
	\return 
 */
never_const_ptr<object>
connection_argument_list::check_build(never_ptr<context> c) const {
	never_const_ptr<object> o;
	TRACE_CHECK_BUILD(
		cerr << c->auto_indent() <<
			"connection_argument_list::check_build(...): " << endl;
	)
	// enter the connection argument context
	o = expr_list::check_build(c);
	// leave the connection argument context
	return o;
}

//=============================================================================
// class alias_list method definitions

/**
	\param e should be a member/index expression, an lvalue.  
 */
CONSTRUCTOR_INLINE
alias_list::alias_list(expr* e) : instance_management(), alias_list_base(e) {
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
	\param c the context of the current position in the syntax tree.
	\return pointer to rightmost instance corresponding to the 
		final element in the assignment / alias list
		(if all is consistent, else returns NULL)
 */
never_const_ptr<object>
alias_list::check_build(never_ptr<context> c) const {
	TRACE_CHECK_BUILD(
		cerr << c->auto_indent() <<
			"alias_list::check_build(...): FINISH ME!";
	)
if (size() > 0) {		// non-empty
	// can't we just re-use parent's check_build()?
	// alias_list_base::check_build(c);
	// no, because we need place-holder on stack. 

	never_const_ptr<object> ret(NULL);
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		never_const_ptr<object> o;
		if (*i) {
			// check each expression
			o = (*i)->check_build(c);
			// Useless return value.
			// Each expression or instance referenced
			// will be pushed onto the context's object stack.
		} else {
			// need a place-holder item on stack? No.
			// can never be a NULL item in alias_list, 
			//	according to the grammar.
			// Thus we can use parent's check_build().

			// the caller depends on the size of this list.
			// for now stop.
			assert(0);
		}
	}
	// After list items have been built, check types.  
	// Types connected MUST match,
	// and sizes must match if statically known, 
	// else punt until unroll-time.  

	// pop stack items to a local list
	vector<count_ptr<object> > connect(size());
	{
		int j = size() -1;
		for ( ; j>=0; j--)
			connect[j] = c->pop_top_object_stack();
	}

	// case: left-hand-side is a param_instance_reference
	//	then we have expression assignment.  
	// case: left-hand-side is some-other-instance-reference (physical)
	//	then rhs can be arbitrary chain of 
	//	same type instance references.
	// TO DO: in general this needs much work to complete

	if (!connect[0]) {
		cerr << endl << "ERROR in the first item in alias-list."
			<< endl;
		exit(1);
	} else if (connect[0].is_a<param_instance_reference>()) {
		// then expect subsequent items to be the same
		// or already param_expr in the case of some constants.
		// However, only the last item may be a constant.  
		excl_ptr<param_expression_assignment> exass(
			new param_expression_assignment);
		assert(exass);

		// Mark all but the last expression as initialized 
		// to the right-most expression.  
		// TO DO: FINISH THIS PART
		// rvalue = ...
		// make sure rvalue is validly initialized
		// i.e. is a constant or a formal parameter

		bool err = false;
		size_t last = size() -1;

		count_const_ptr<param_expr> rhse(
			connect[last].is_a<param_expr>());
#if 0
		// rhsi should no longer be necessary...
		count_ptr<param_instance_reference> rhsi(
			connect[last].is_a<param_instance_reference>());
#endif
		if (!connect[last]) {
			cerr << "ERROR: rhs of expression assignment "
				"is malformed (null)" << endl;
			err = true;
		} else if (rhse) {
			// last term must be initialized or be 
			// dependent on formals
			// if collective, conservative: may-be-initialized
			if (!rhse->is_initialized()) {
				rhse->dump(cerr << "ERROR: rhs of expr-"
					"assignment is not initialized or "
					"dependent on formals: ") << endl;
				err = true;
				exit(1);		// temporary
			}
#if 0
		} else if (rhsi) {
			if (!rhsi->is_initialized()) {
				// not definitely initialized
				rhsi->dump(cerr << "ERROR: rhs of expr-"
					"assignment is not initialized or "
					"dependent on formals: ") << endl;
				err = true;
			} else {
				// may be initialized, resolve at unroll
#if 0
				rhse = count_const_ptr<param_expr>(
					rhsi->make_param_literal(rhsi));
#else
				rhse = rhsi;	// error: const to non-const
#endif
			}
		// could also be a param_literal?
#endif
		} else {
			connect[last]->what(
				cerr << "ERROR: rhs is unexpected object: ")
				<< endl;
			err = true;
		}

		size_t k = 0;
		for ( ; k<last; k++) {	// all but last one
			// consider modularizing this loop
			bool for_err = false;
#if 0
			// param_literal now equivalent to
			//	param_instance_reference
			count_ptr<param_literal> pl(
				connect[k].is_a<param_literal>());
#endif
#if 0
			count_ptr<param_instance_reference> ir(
				connect[k].is_a<param_instance_reference>());
#else
			count_ptr<param_expr> ir(connect[k].is_a<param_expr>());
#endif
			if (!connect[k]) {
				cerr << "ERROR: in creating item " << k+1 <<
					" of alias-list." << endl;
				for_err = true;
#if 0
			} else if (pl) {
				// single expression
				// can't handle collections of expressions yet.

				if (pl->is_initialized()) {
					cerr << "ERROR: expression " << k+1 <<
						"is already initialized!"
						<< endl;
					// don't care if it's same value...
					for_err = true;
				} else if (rhse) {
					pl->initialize(rhse);
					assert(pl->is_initialized());
				}	// else already error in rhse
				if (pl->is_static_constant()) {
					// shouldn't happen if pl is literal
					cerr << "ERROR: constants may only "
						"appear as the last term of "
						"assignment lists!" << endl;
					for_err = true;
				}
				if (for_err)
					exass->append_param_expression(
						count_ptr<param_expr>(NULL));
				else exass->append_param_expression(pl);
#endif
			} else if (ir) {
				// a single parameter instance reference
				// make sure not already initialized!
				if (ir->is_initialized()) {
					// definitely initialized or formal
					cerr << "ERROR: expression " << k+1 <<
						"is already initialized!"
						<< endl;
					// don't care if it's same value...
					for_err = true;
				} else if (rhse) {
					// what to do about collective arrays?
#if 0
					// missing virtual method...
					ir->initialize(rhse);
#else
					count_ptr<pbool_instance_reference>
						bir(ir.is_a<pbool_instance_reference>());
					count_ptr<pint_instance_reference>
						pir(ir.is_a<pint_instance_reference>());
					// gotta be one class or the other
					// check this after re-write
					assert(rhse);
					bool init_ret;
					if (bir)
						init_ret = bir->initialize(rhse);
					else {
						assert(pir);
						init_ret = pir->initialize(rhse);
					}
					if (!init_ret) {
						cerr << "Error initializing "
						"item " << k+1 << " of alias-"
						"list.  " << endl;
						for_err = true;
					}
#endif
//					assert(ir->is_initialized());
				} // else already error in rhse
				if (for_err)
					exass->append_param_expression(
						count_ptr<param_expr>(NULL));
#if 0
				else exass->append_param_expression(
						ir->make_param_literal(ir));
#else
				else exass->append_param_expression(ir);
#endif
			} else {
				// is reference to something else
				// or might be collective...
				// ERROR
				cerr << "ERROR: unhandled case for item "
					<< k+1 << " of alias-list." << endl;
				for_err = true;
			}
			if (for_err)
				err = for_err;
		}

		// if all is well, then add this new list to the context's
		// current scope.  
		if (err) {
			cerr << "HALT: at least one error in alias list."
				<< endl;
			exit(1);
		} else {
			c->add_assignment(
				excl_const_ptr<connection_assignment_base>(
					exass));
			// and transfer ownership
			assert(!exass.owned());
		}
	} else if (connect[0].is_a<instance_reference_base>()) {
		cerr << "alias_list::check_build(): not done yet "
			"for non-param/expr instance connections yet.  "
			"Aborting." << endl;
		exit(1);
	} else {
		// ERROR
		cerr << "WTF? first element of alias_list is not an instance!"
			<< endl;
		exit(1);
	}

	return ret;
} else {
	// will this ever be empty?  will be caught as error for now.
	assert(0);
	return never_const_ptr<object>(NULL);
}
}

//=============================================================================
// template class node_list<> method definitions
// had trouble finding reference to template functions defined here...?
// template exporting not implemented in any gcc compiler yet...

// thus all definitions must be in header
// this way, PRS and HSE may use them

//=============================================================================
// class type_base method definitions

CONSTRUCTOR_INLINE
type_base::type_base() : node() { }

DESTRUCTOR_INLINE
type_base::~type_base() { }

//=============================================================================
// class type_id method definitions

/**
	Builds a type-identifier.  
	Also deletes expression list argument after transfering list.  
 */
CONSTRUCTOR_INLINE
type_id::type_id(const qualified_id* b) : node(),
		base(b) {
	assert(base);
}

DESTRUCTOR_INLINE
type_id::~type_id() {
}

ostream&
type_id::what(ostream& o) const {
	return base->what(o << "(type-id): ");
}

line_position
type_id::leftmost(void) const {
	return base->leftmost();
}

line_position
type_id::rightmost(void) const {
	return base->rightmost();
}

/**
	The base name of the type can refer to either user-defined
	data, channel, or process type for instantiation.  
	Use context object to lookup the actual type.  
	\return pointer to type reference.  
 */
never_const_ptr<object>
type_id::check_build(never_ptr<context> c) const {
//	never_const_ptr<object> o;
//	const definition_base* d;
	never_const_ptr<definition_base> d;
	TRACE_CHECK_BUILD(
		cerr << c->auto_indent() <<
			"type_id::check_build(...): " << endl;
	)
	d = c->lookup_definition(*base);
//	o = c->lookup_definition(*base);
//	d = IS_A(const definition_base*, o);
	if (!d) {
//		cerr << "type_id::check_build(never_ptr<context>) : ERROR!" << endl;
		return never_const_ptr<object>(NULL);
	}
	// set type definition reference
	d = d->set_context_definition(*c);	// pure virtual
	// c->set_definition(d);		// don't care which kind...
	return d;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// non-member functions
ostream& operator << (ostream& o, const type_id& id) {
	return o << *id.base;
}

//=============================================================================
// class user_data_type_prototype method definitions

CONSTRUCTOR_INLINE
user_data_type_prototype::user_data_type_prototype(
	const template_formal_decl_list* tf, const token_keyword* df, 
	const token_identifier* n, const token_string* dp, 
	const concrete_type_ref* b, 
	const data_param_decl_list* p, const token_char* s) :
		prototype(), 
		user_data_type_signature(tf, df, n, dp, b, p), 
		semi(s) {
	assert(semi);
}

DESTRUCTOR_INLINE
user_data_type_prototype::~user_data_type_prototype() {
}

ostream&
user_data_type_prototype::what(ostream& o) const {
	return o << "(user-data-type-proto)";
}

line_position
user_data_type_prototype::leftmost(void) const {
	if (def)	return def->leftmost();
	else		return id->leftmost();
}

line_position
user_data_type_prototype::rightmost(void) const {
	if (semi)	return semi->rightmost();
	else		return params->rightmost();
}

never_const_ptr<object>
user_data_type_prototype::check_build(never_ptr<context> c) const {
	TRACE_CHECK_BUILD(
		cerr << c->auto_indent() <<
			"user_data_type_prototype::check_build(...): " << endl;
	)
#if 0
	never_const_ptr<object> o;
	c->declare_datatype(*this);	// really only need name
	// visit template formals
	o = bdt->check_build(c);
	assert(o);
	o = params->check_build(c);
	assert(o);
	c->close_datatype_definition();
#else
	cerr << "TO DO: user_data_type_prototype::check_build();" << endl;
#endif
	return c->top_namespace();
}

//=============================================================================
// class user_data_type_def method definitions

CONSTRUCTOR_INLINE
user_data_type_def::user_data_type_def(const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, const concrete_type_ref* b, 
		const data_param_decl_list* p, const token_char* l, 
		const language_body* s, const language_body* g,
		const token_char* r) :
		definition(), 
		user_data_type_signature(tf, df, n, dp, b, p), 
		lb(l), setb(s), getb(g), rb(r) {
	assert(lb); assert(setb); assert(getb); assert(rb);
}

DESTRUCTOR_INLINE
user_data_type_def::~user_data_type_def() {
}

ostream&
user_data_type_def::what(ostream& o) const {
	return o << "(user-data-type-def)";
}

line_position
user_data_type_def::leftmost(void) const {
	if (def)	return def->leftmost();
	else		return id->leftmost();
}

line_position
user_data_type_def::rightmost(void) const {
	if (rb)		return rb->rightmost();
	else		return getb->rightmost();
}

/*** unveil later...
never_const_ptr<object>
user_data_type_def::check_build(never_ptr<context> c) const {
	never_const_ptr<object> o;
	TRACE_CHECK_BUILD(
		cerr << c->auto_indent() <<
			"user_data_type_def::check_build(...): " << endl;
	)
	c->open_datatype(*this);	// really only need name
	o = bdt->check_build(c);
	assert(o);
	o = params->check_build(c);
	assert(o);
//	setb->check_build(c);
//	getb->check_build(c);
	c->close_datatype_definition();
	return c->top_namespace();
}
***/

//=============================================================================
// class enum_signature method definitions

/**
	Basic constructor.  
	Enums cannot be templated, so we pass NULL to the parent constructor.  
 */
enum_signature::enum_signature(const token_keyword* e, 
		const token_identifier* i) :
		signature_base(NULL, i), en(e) {
}

enum_signature::~enum_signature() {
}

/**
	Just constructs and returns an enumeration definition with 
	the appropriate signature.  There's really no signature, just name.  
 */
never_const_ptr<object>
enum_signature::check_build(never_ptr<context> c) const {
	excl_ptr<definition_base> ed(
		new enum_datatype_def(c->get_current_namespace(), *id));
	// elsewhere would need to add template and port formals
	// no need for set_current_prototype
	never_const_ptr<definition_base> ret(c->add_declaration(ed));
	if (!ret) {
		// error handling?
		cerr << where() << endl;
		exit(1);
	}
	return ret;
//	return c->set_current_prototype(ed);
}

//=============================================================================
// class enum_prototype method definitions

enum_prototype::enum_prototype(const token_keyword* e, 
		const token_identifier* i, const token_char* s) :
		prototype(), enum_signature(e, i), semi(s) {
}

enum_prototype::~enum_prototype() {
}

ostream&
enum_prototype::what(ostream& o) const {
	return o << "(enum-declaration)";
}

line_position
enum_prototype::leftmost(void) const {
	if (en)		return en->leftmost();
	else		return id->leftmost();
}

line_position
enum_prototype::rightmost(void) const {
	if (semi)	return semi->rightmost();
	else		return id->rightmost();
}

/**
	Register this identifier as a enum type.  
	Is acceptable if already declared or defined, 
	Signature has no additional information to match other than name.  
	\return NULL, useless.  
 */
never_const_ptr<object>
enum_prototype::check_build(never_ptr<context> c) const {
	return enum_signature::check_build(c);	// using.
}

//=============================================================================
// class enum_member_list method definitions

enum_member_list::enum_member_list(const token_identifier* i) : parent(i) {
}

enum_member_list::~enum_member_list() {
}

never_const_ptr<object>
enum_member_list::check_build(never_ptr<context> c) const {
	// use current_open_definition
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		assert(*i);
		bool b = c->add_enum_member(**i);
		if (!b) {
			// should've exited by now...
			cerr << "BAD fang!" << endl;
		}
	}
	return never_const_ptr<object>(NULL);
}

//=============================================================================
// class enum_def method definitions

enum_def::enum_def(const token_keyword* e, const token_identifier* i,
		const enum_member_list* m) :
		definition(), enum_signature(e, i), members(m) {
	assert(members);
}

enum_def::~enum_def() {
}

ostream&
enum_def::what(ostream& o) const {
	return o << "(enum-definition)";
}

line_position
enum_def::leftmost(void) const {
	if (en)		return en->leftmost();
	else		return id->leftmost();
}

line_position
enum_def::rightmost(void) const {
	return members->rightmost();
}

/**
	Reminder: don't forget to reset_current_prototype()
 */
never_const_ptr<object>
enum_def::check_build(never_ptr<context> c) const {
	never_const_ptr<object> o(enum_signature::check_build(c));
	if (!o)	return never_const_ptr<object>(NULL);
	// lookup and open definition
	c->open_enum_definition(*id);	// marks as defined
	o = members->check_build(c);	// use current_open_definition
		// always returns NULL
#if 0
	if (!o) {
		cerr << where() << endl;
		exit(1);
	}
#endif
	c->close_enum_definition();
	return never_const_ptr<object>(NULL);
}

//=============================================================================
// class chan_type method definitions

CONSTRUCTOR_INLINE
chan_type::chan_type(const token_keyword* c, const token_char* d, 
		const data_type_ref_list* t) : type_base(),
		chan(c), dir(d), dtypes(t) {
	assert(c);
}

DESTRUCTOR_INLINE
chan_type::~chan_type() {
}

ostream&
chan_type::what(ostream& o) const {
	return o << "(chan-type)";
}

line_position
chan_type::leftmost(void) const {
	return chan->leftmost();
}

line_position
chan_type::rightmost(void) const {
	if (dtypes)
		return dtypes->rightmost();
	else if (dir)
		return dir->rightmost();
	else return chan->rightmost();
}

/**
	Associates a channel or port with a data type, such as a list of 
	ints and bools.  
	\param t is the type list for the channel.  
 */
chan_type*
chan_type::attach_data_types(const data_type_ref_list* t) {
	assert(t); assert(!dtypes);     // sanity check    
	dtypes = excl_const_ptr<data_type_ref_list>(t);
	assert(dtypes);
	return this;
}

never_const_ptr<object>
chan_type::check_build(never_ptr<context> c) const {
	cerr << "chan_type::check_build(): FINISH ME!";
	return never_const_ptr<object>(NULL);
}

//=============================================================================
// class user_chan_type_signature method definitions

CONSTRUCTOR_INLINE
user_chan_type_signature::user_chan_type_signature(
		const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n,
		const token_string* dp, 
		const chan_type* b, const data_param_decl_list* p) :
		signature_base(tf, n), 
		def(df), dop(dp), bct(b), params(p) {
	assert(def); assert(dop);
	assert(bct); assert(params);
}

DESTRUCTOR_INLINE
user_chan_type_signature::~user_chan_type_signature() {
}

/**
	Just constructs and returns an channel definition with 
	the appropriate signature.
 */
never_const_ptr<object>
user_chan_type_signature::check_build(never_ptr<context> c) const {
	cerr << "user_chan_type_signature::check_build() FINISH ME!" << endl;
	excl_ptr<definition_base> dd(
		new user_def_chan(c->get_current_namespace(), *id));
	return c->set_current_prototype(dd);
}


//=============================================================================
// class user_chan_type_prototype method definitions

CONSTRUCTOR_INLINE
user_chan_type_prototype::user_chan_type_prototype(
		const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, const chan_type* b, 
		const data_param_decl_list* p, const token_char* s) :
		prototype(), 
		user_chan_type_signature(tf, df, n, dp, b, p), 
		semi(s) {
	assert(semi);
}

DESTRUCTOR_INLINE
user_chan_type_prototype::~user_chan_type_prototype() {
}

ostream&
user_chan_type_prototype::what(ostream& o) const {
	return o << "(user-chan-type-proto)";
}

line_position
user_chan_type_prototype::leftmost(void) const {
	if (def)	return def->leftmost();
	else		return id->leftmost();
}

line_position
user_chan_type_prototype::rightmost(void) const {
	if (semi)	return semi->rightmost();
	else		return params->rightmost();
}

//=============================================================================
// class user_chan_type_def method definitions

CONSTRUCTOR_INLINE
user_chan_type_def::user_chan_type_def(const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, const chan_type* b, 
		const data_param_decl_list* p, const token_char* l, 
		const language_body* s, const language_body* g, 
		const token_char* r) :
		definition(), 
		user_chan_type_signature(tf, df, n, dp, b, p), 
		lb(l), sendb(s), recvb(g), rb(r) {
	assert(lb); assert(sendb); assert(recvb); assert(rb);
}

DESTRUCTOR_INLINE
user_chan_type_def::~user_chan_type_def() {
}

ostream&
user_chan_type_def::what(ostream& o) const {
	return o << "(user-chan-type-def)";
}

line_position
user_chan_type_def::leftmost(void) const {
	if (def)	return def->leftmost();
	else		return id->leftmost();
}

line_position
user_chan_type_def::rightmost(void) const {
	if (rb)         return rb->rightmost();
	else            return recvb->rightmost();
}

//=============================================================================
// class statement method definitions

CONSTRUCTOR_INLINE
statement::statement() : node() { }

DESTRUCTOR_INLINE
statement::~statement() { }

//=============================================================================
// class incdec_stmt method definitions

CONSTRUCTOR_INLINE
incdec_stmt::incdec_stmt(const expr* n, const terminal* o) : statement(),
		e(n), op(o) {
	assert(e); assert(op);
}

#if 0
CONSTRUCTOR_INLINE
incdec_stmt::incdec_stmt(excl_const_ptr<expr> n, excl_const_ptr<terminal> o) :
		statement(), e(n), op(o) {
	assert(e); assert(op);
}
#endif

DESTRUCTOR_INLINE
incdec_stmt::~incdec_stmt() {
	SAFEDELETE(e); SAFEDELETE(op);
}

/**
	Release operations are needed for destructive transfer of ownership.
	The consumers of the return pointers are thus responsible for the
	memory at their location. 
 */
#if 0
excl_const_ptr<expr>
incdec_stmt::release_expr(void) {
	return e;
}

excl_const_ptr<terminal>
incdec_stmt::release_op(void) {
	return op;
}
#else
const expr*
incdec_stmt::release_expr(void) {
	const expr* ret = e;
	e = NULL;
	return ret;
}

const terminal*
incdec_stmt::release_op(void) {
	const terminal* ret = op;
	op = NULL;
	return ret;
}
#endif

ostream&
incdec_stmt::what(ostream& o) const {
	return o << "(inc/dec-stmt)";
}

line_position
incdec_stmt::leftmost(void) const {
	// if (e)
	return e->leftmost();
}

line_position
incdec_stmt::rightmost(void) const {
	// if (op)
	return op->rightmost();
}

//=============================================================================
// class assign_stmt method definitions

CONSTRUCTOR_INLINE
assign_stmt::assign_stmt(const expr* left, const terminal* o, 
		const expr* right) : statement(),
		lhs(left), op(o), rhs(right) {
	assert(lhs); assert(op); assert(rhs);
}

#if 0
CONSTRUCTOR_INLINE
assign_stmt::assign_stmt(excl_const_ptr<expr> left, excl_const_ptr<terminal> o, 
		excl_const_ptr<expr> right) : statement(),
		lhs(left), op(o), rhs(right) {
	assert(lhs); assert(op); assert(rhs);
}
#endif

DESTRUCTOR_INLINE
assign_stmt::~assign_stmt() {
	SAFEDELETE(lhs); SAFEDELETE(op); SAFEDELETE(rhs);
}

#if 0
excl_const_ptr<expr>
assign_stmt::release_lhs(void) {
	return lhs;
}

excl_const_ptr<terminal>
assign_stmt::release_op(void) {
	return op;
}

excl_const_ptr<expr>
assign_stmt::release_rhs(void) {
	return rhs;
}
#else
const expr*
assign_stmt::release_lhs(void) {
	const expr* ret = lhs;
	lhs = NULL;
	return ret;
}

const terminal*
assign_stmt::release_op(void) {
	const terminal* ret = op;
	op = NULL;
	return ret;
}

const expr*
assign_stmt::release_rhs(void) {
	const expr* ret = rhs;
	rhs = NULL;
	return ret;
}
#endif

ostream&
assign_stmt::what(ostream& o) const {
	return o << "(assign-stmt)";
}

line_position
assign_stmt::leftmost(void) const {
	return lhs->leftmost();
}

line_position
assign_stmt::rightmost(void) const {
	return rhs->rightmost();
}

//=============================================================================
// class def_body_item method definitions

CONSTRUCTOR_INLINE
def_body_item::def_body_item() : node() { }

DESTRUCTOR_INLINE
def_body_item::~def_body_item() { }

//=============================================================================
// class language_body methd definitions

CONSTRUCTOR_INLINE
language_body::language_body(const token_keyword* t) :
		def_body_item(), tag(t) {
}

DESTRUCTOR_INLINE
language_body::~language_body() { }

language_body*
language_body::attach_tag(token_keyword* t) {
	// need to safe-delete first?  nah...
	tag = excl_const_ptr<token_keyword>(t);
	assert(tag);
	return this;
}

line_position
language_body::leftmost(void) const {
	// what if untagged?
	assert(tag);
	return tag->leftmost();
}


//=============================================================================
// class namespace_body method definitions

/**
	Namespace body constructor.  
	\param s the "namespace" keyword. 
	\param n the identifier for the name of the namespace.  
	\param l the left brace.  
	\param b the body (contents), may be NULL.  
	\param r the right brace.  
	\param c the semicolon.  
 */
CONSTRUCTOR_INLINE
namespace_body::
namespace_body(const token_keyword* s, const token_identifier* n, 
		const terminal* l, const root_body* b,
		const terminal* r, const terminal* c) :
		root_item(),       
		ns(s), name(n), lb(l), body(b), rb(r), semi(c) {
	assert(ns); assert(name); assert(lb);
	// body may be NULL
	assert(rb); assert(semi);
}

/// destructor
DESTRUCTOR_INLINE
namespace_body::
~namespace_body() {
}

/// what eeeez it, man?
ostream&
namespace_body::
what(ostream& o) const {
	return o << "(namespace-body: " << *name << ")";
}

line_position
namespace_body::leftmost(void) const {
	if (ns)		return ns->leftmost();
	else		return name->rightmost();
}

line_position
namespace_body::rightmost(void) const {
	if (semi)	return semi->rightmost();
	else if (rb)	return rb->rightmost();
	else if (body)	return body->rightmost();
	else if (lb)	return lb->rightmost();
	else		return name->rightmost();
}

// recursive type-checker
never_const_ptr<object>
namespace_body::
check_build(never_ptr<context> c) const {
	TRACE_CHECK_BUILD(
		cerr << c->auto_indent() << 
			"namespace_body::check_build(...): " << *name;
	)
	// use context lookup: see if namespace already exists in super-scope
		// name_space* ns = c->lookup_namespace(name);
	// if so, open it up, and work with existing namespace
	// otherwise register a new namespace, add it to context
	c->open_namespace(*name);
	if (body)			// may be NULL, which means empty
		body->check_build(c);

//	TRACE_CHECK_BUILD(
//		cerr << c->auto_indent() << "leaving namespace: " << *name;
//	)
	c->close_namespace();
	// if no errors, return pointer to the namespace just processed
	return c->top_namespace();
}

//=============================================================================
// class namespace_id method definitions

namespace_id::namespace_id(qualified_id* i) : node(), qid(i) {
	assert(qid);
}

namespace_id::~namespace_id() {
}

ostream&
namespace_id::what(ostream& o) const {
	return o << "(namespace-id): " << *qid;
}

line_position
namespace_id::leftmost(void) const {
	return qid->leftmost();
}

line_position
namespace_id::rightmost(void) const {
	return qid->rightmost();
}

/*** NOT USED... yet
never_const_ptr<object>
namespace_id::check_build(never_ptr<context> c) const {
}
***/

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// non-member functions

ostream&
operator << (ostream& o, const namespace_id& id) {
	return o << *id.qid;
}

//=============================================================================
// class using_namespace method definitions

/**
	Constructor for using_namespace directive.  
	\param o the "open" keyword.  
	\param i the id_expr qualified identifier.  
	\param s the terminating semicolon.  
 */
CONSTRUCTOR_INLINE
using_namespace::
using_namespace(const token_keyword* o, const namespace_id* i, 
		const token_char* s) : root_item(),
		open(o), id(i), as(NULL), alias(NULL), semi(s) {
	assert(open); assert(id); assert(semi);
}

/**
	Constructor for using_namespace directive.  
	\param o the "open" keyword.  
	\param i the id_expr qualified identifier.  
	\param a the "as" keyword.  
	\param n the alias name.  
	\param s the terminating semicolon.  
 */
CONSTRUCTOR_INLINE
using_namespace::
using_namespace(const token_keyword* o, const namespace_id* i, 
		const token_keyword* a, const token_identifier* n, 
		const token_char* s) : root_item(),
		open(o), id(i), as(a), alias(n), semi(s) {
	assert(open); assert(id); assert(as); assert(alias); assert(semi);
}

/// default destructor
DESTRUCTOR_INLINE
using_namespace::~using_namespace() {
}

ostream&
using_namespace::what(ostream& o) const {
	return o << "(using-namespace)";
}

line_position
using_namespace::leftmost(void) const {
	return open->leftmost();
}

line_position
using_namespace::rightmost(void) const {
	if (semi)	return semi->rightmost();
	else if (alias)	return alias->rightmost();
	else if (as)	return as->rightmost();
	else		return id->rightmost();
}

/// returns a pointer to a valid namespace that's now mapped in this scope
never_const_ptr<object>
using_namespace::
check_build(never_ptr<context> c) const {
if (alias) {
	TRACE_CHECK_BUILD(
		cerr << c->auto_indent() << 
			"using_namespace::check_build(...) (alias): "
			<< *id;
	)
	c->alias_namespace(*id->get_id(), *alias);
} else {
	TRACE_CHECK_BUILD(
		cerr << c->auto_indent() << 
			"using_namespace::check_build(...) (using): "
			<< *id;
	)
	// if aliased... print all, report as error (done inside)
	c->using_namespace(*id->get_id());
}
	return c->top_namespace();
}

//=============================================================================
// class actuals_base method definitions

CONSTRUCTOR_INLINE
actuals_base::actuals_base(const expr_list* a) : 
		instance_management(), actuals(a) {
	assert(actuals);
}

DESTRUCTOR_INLINE
actuals_base::~actuals_base() {
}

//=============================================================================
// class instance_base method definitions

CONSTRUCTOR_INLINE
instance_base::instance_base(const token_identifier* i) :
		instance_management(), id(i) {
	assert(id);
}

DESTRUCTOR_INLINE
instance_base::~instance_base() {
}

ostream&
instance_base::what(ostream& o) const {
	return id->what(o << "(declaration-id): ");
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
never_const_ptr<object>
instance_base::check_build(never_ptr<context> c) const {
	never_const_ptr<instantiation_base> inst;
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent())
			<< "instance_base::check_build(...): ";
	)

	// ACTUALLY: need current_fundamental_type()
	// uses c->current_fundamental_type
	inst = c->add_instance(*id);		// check return value?
	if (!inst) {
		cerr << "ERROR with " << *id << " at " << id->where() << endl;
		return never_const_ptr<object>(NULL);
	}
	// need current_instance?  no, not using as reference.
	// return inst;
	return c->top_namespace();
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
	return ranges->what(id->what(o << "(declaration-array): "));
}

line_position
instance_array::rightmost(void) const {
	return ranges->rightmost();
}

/**
	Instantiates an array of instances.  
	Dimensions may be dense or sparse.  
 */
never_const_ptr<object>
instance_array::check_build(never_ptr<context> c) const {
	TRACE_CHECK_BUILD(
		cerr << c->auto_indent() <<
			"instance_array::check_build(...): " << endl;
	)
	if (ranges) {
		never_const_ptr<instantiation_base> t;
		ranges->check_build(c);
		// expecting ranges and singe integer expressions
		count_ptr<object> o(c->pop_top_object_stack());
		// expect constructed (sparse) range_list on object stack
		if (!o) {
			cerr << "ERROR in dimensions!  " << 
				ranges->where() << endl;
			exit(1);
		}
		count_ptr<range_expr_list> d(o.is_a<range_expr_list>());
		assert(d);
		t = c->add_instance(*id, d);
		// if there was error, would've exit(1)'d (temporary)
		return t;
	} else {
		return instance_base::check_build(c);
	}
}

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
	assert(type);
	assert(ids);
}

DESTRUCTOR_INLINE
instance_declaration::~instance_declaration() {
}

ostream&
instance_declaration::what(ostream& o) const {
	return o << "(instance-decl)";
}

line_position
instance_declaration::leftmost(void) const {
	return type->leftmost();
}

line_position
instance_declaration::rightmost(void) const {
	return semi->rightmost();
}

never_const_ptr<object>
instance_declaration::check_build(never_ptr<context> c) const {
	never_const_ptr<object> t;
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent()) <<
			"instance_declaration::check_build(...): ";
	)
	t = type->check_build(c);
		// should set the current_fundamental_type
	c->reset_current_definition_reference();
	if (t) {
		assert(ids);
		ids->check_build(c);		// return value?
	} else {
		cerr << "ERROR with concrete-type to instantiate at "
			<< type->where() << endl;
		return never_const_ptr<object>(NULL);
	}
	// instance could be ANY type
	c->reset_current_fundamental_type();	// the type to instantiate
	return c->top_namespace();
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

ostream&
instance_connection::what(ostream& o) const {
	return o << "(actuals-connection)";
}

line_position
instance_connection::leftmost(void) const {
	return instance_base::leftmost();
}

line_position
instance_connection::rightmost(void) const {
	if (semi) return semi->rightmost();
	else return actuals->rightmost();
}

never_const_ptr<object>
instance_connection::check_build(never_ptr<context> c) const {
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent()) <<
			"instance_connection::check_build(...): ";
	)
	return node::check_build(c);
}

//=============================================================================
// class connection_statement method definitions

CONSTRUCTOR_INLINE
connection_statement::connection_statement(const expr* l, const expr_list* a, 
		const terminal* s) : actuals_base(a), lvalue(l), semi(s) {
	assert(lvalue);
}

DESTRUCTOR_INLINE
connection_statement::~connection_statement() {
}

ostream&
connection_statement::what(ostream& o) const {
	return o << "(connection-statement)";
}

line_position
connection_statement::leftmost(void) const {
	return lvalue->leftmost();
}

line_position
connection_statement::rightmost(void) const {
	if (semi) return semi->rightmost();
	else return actuals->rightmost();
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
			(assert(a),
			// need deep copy of i as an expression, 
			// because already managed by parent, 
			// and list uses count_const_ptr<expr>
			a->push_front(count_const_ptr<token_identifier>(
				new token_identifier(*i))),
			// caution, unless we add an '=' token to delim_list
			// assertion will be broken, but who cares?
			a)
			// the value of this compound expression is a
		),
		semi(s) {
	assert(aliases);
}

DESTRUCTOR_INLINE
instance_alias::~instance_alias() {
}

ostream&
instance_alias::what(ostream& o) const {
	return o << "(alias-assign)";
}

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
never_const_ptr<object>
instance_alias::check_build(never_ptr<context> c) const {
	never_const_ptr<object> o;
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent()) <<
			"instance_alias::check_build(...)";
	)
	// declare and register the instance first
	o = instance_base::check_build(c);
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
	assert(lp); assert(delim); assert(index);
	assert(colon); assert(rng); assert(body); assert(lp);
}

DESTRUCTOR_INLINE
loop_instantiation::~loop_instantiation() {
}

ostream&
loop_instantiation::what(ostream& o) const {
	return o << "(loop-instance)";
}

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
// class data_param_id method definitions

data_param_id::data_param_id(const token_identifier* i, 
		const dense_range_list* d) :
		node(), id(i), dim(d) {
	assert(id);
	// dim is optional
}

data_param_id::~data_param_id() {
}

ostream&
data_param_id::what(ostream& o) const {
	return o << "(data-param-id)";
}

line_position
data_param_id::leftmost(void) const {
	return id->leftmost();
}

line_position
data_param_id::rightmost(void) const {
	if (dim)	return dim->rightmost();
	else		return id->rightmost();
}

//=============================================================================
// class data_param_decl method definition

data_param_decl::data_param_decl(const concrete_type_ref* t, 
		const data_param_id_list* il) :
		node(), type(t), ids(il) {
	assert(type);
	assert(ids);
}

data_param_decl::~data_param_decl() {
}

ostream&
data_param_decl::what(ostream& o) const {
	return o << "(data-param-decl)";
}

line_position
data_param_decl::leftmost(void) const {
	return type->leftmost();
}

line_position
data_param_decl::rightmost(void) const {
	return ids->rightmost();
}

//=============================================================================
// class port_formal_id method definitions

CONSTRUCTOR_INLINE
port_formal_id::port_formal_id(const token_identifier* n,
		const dense_range_list* d)
		: node(), name(n), dim(d) {
	assert(name);
	// dim may be NULL
}

DESTRUCTOR_INLINE
port_formal_id::~port_formal_id() {
}

ostream&
port_formal_id::what(ostream& o) const {
	name->what(o << "(port-formal-id): ");
	if (dim) dim->what(o);
	return o;
}

line_position
port_formal_id::leftmost(void) const {
	return name->leftmost();
}

line_position
port_formal_id::rightmost(void) const {
	return dim->rightmost();
}

/**
	Should be very similar to instance_base's check_build.  
	// there should be some open definition already
	// type should already be set in the context
	TO DO:
 */
never_const_ptr<object>
port_formal_id::check_build(never_ptr<context> c) const {
	never_const_ptr<object> o;
	never_const_ptr<instantiation_base> t;
		// should be anything but param_instantiation
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent()) <<
			"port_formal_id::check_build(...): ";
	)

	if (dim) {
		dim->check_build(c);	// useless return value
		count_ptr<object> o(c->pop_top_object_stack());
		if (!o) {
			cerr << "ERROR in array dimensions " <<
				dim->where() << endl;
			exit(1);
		}
		count_ptr<range_expr_list> d(o.is_a<range_expr_list>());
		assert(d);
		// attach array dimensions to current instantiation
		t = c->add_port_formal(*name, d);
	} else {
		t = c->add_port_formal(*name);
	}
	return t;
}

//=============================================================================
// class port_formal_decl method definitions

CONSTRUCTOR_INLINE
port_formal_decl::port_formal_decl(const concrete_type_ref* t, 
		const port_formal_id_list* i) : 
		node(), type(t), ids(i) {
	assert(type); assert(ids);
}

DESTRUCTOR_INLINE
port_formal_decl::~port_formal_decl() {
}

ostream&
port_formal_decl::what(ostream& o) const {
	return o << "(port-formal-decl)";
}

line_position
port_formal_decl::leftmost(void) const {
	return type->leftmost();
}

line_position
port_formal_decl::rightmost(void) const {
	return ids->rightmost();
}

//=============================================================================
// class template_formal_id method definitions

/**
	Constructor for a template formal identifier, the formal name for a 
	template parameter, which may be an array, declared with
	dimensions in brackets.  
	\param n the name of the template formal.  
	\param d is the (optional) dimension size expression.
	\param e is an '=' token.  
	\param v is the optional default value expression.  
 */
CONSTRUCTOR_INLINE
template_formal_id::template_formal_id(const token_identifier* n, 
		const dense_range_list* d, const token_char* e, 
		const expr* v) : 
		node(), name(n), dim(d), eq(e), dflt(v) {
	assert(name);
// dim may be NULL
	if (eq) assert(dflt);
}

/**
	
 */

DESTRUCTOR_INLINE
template_formal_id::~template_formal_id() {
}

ostream&
template_formal_id::what(ostream& o) const {
	name->what(o << "(template-formal-id): ");
	if (dim) dim->what(o << " with ");
	return o;
}

line_position
template_formal_id::leftmost(void) const {
	return name->leftmost();
}

line_position
template_formal_id::rightmost(void) const {
	if (dflt)	return dflt->rightmost();
	else if (eq)	return eq->rightmost();
	else if (dim)	return dim->rightmost();
	else return	name->rightmost();
}

/**
	Should be very similar to instance_base's check_build.  
	TO DO: register default value in building
 */
never_const_ptr<object>
template_formal_id::check_build(never_ptr<context> c) const {
	never_const_ptr<object> o;
	never_const_ptr<instantiation_base> t;
		// should be param_instantiation
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent()) <<
			"template_formal_id::check_build(...): ";
	)
	// there should be some open definition already
	// type should already be set in the context
	if (dim) {
		// attach array dimensions to current instantiation
		dim->check_build(c);	// useless return value, check stack
		// should already construct an range_expr_list
		count_ptr<object> o(c->pop_top_object_stack());
		if (!o) {
			cerr << "ERROR in array dimensions " <<
				dim->where() << endl;
			exit(1);
		}
		count_ptr<range_expr_list> d(o.is_a<range_expr_list>());
		assert(d);
		t = c->add_template_formal(*name, d);
	} else {
		t = c->add_template_formal(*name);
	}
	return t;
//	return never_const_ptr<object>(t);
}

//=============================================================================
// class template_formal_decl method definitions

CONSTRUCTOR_INLINE
template_formal_decl::template_formal_decl(
		const token_paramtype* t, 	// why not concrete_type_ref?
		const template_formal_id_list* i) :
		node(), type(t), ids(i) {
	assert(type); assert(ids);
}

DESTRUCTOR_INLINE
template_formal_decl::~template_formal_decl() {
}

ostream&
template_formal_decl::what(ostream& o) const {
	return o << "(template-formal-decl)";
}

line_position
template_formal_decl::leftmost(void) const {
	return type->leftmost();
}

line_position
template_formal_decl::rightmost(void) const {
	return ids->rightmost();
}

/**
	Type-checks a list of template formals with the same type.  
	Adds formal parameters to the context's current_prototype.  
 */
never_const_ptr<object>
template_formal_decl::check_build(never_ptr<context> c) const {
	never_const_ptr<object> o;
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent()) <<
			"template_formal_decl::check_build(...): ";
	)
	o = type->check_build(c);	// sets_current_definition_reference
	c->set_current_fundamental_type();
		// built-in param types pint and pbool
		// have no template parameters...
		// after type is upgraded to a concrete_type_ref
		// then it will already be set by its check_build()
	assert(o);
	ids->check_build(c);	// node_list::check_build: ignore return value
	c->reset_current_fundamental_type();
	return o;
}

//=============================================================================
// class concrete_type_ref method definitions

CONSTRUCTOR_INLINE
concrete_type_ref::concrete_type_ref(const type_base* n, 
		const template_argument_list* t) : 
		node(), base(n), temp_spec(t) {
	assert(base);
}

DESTRUCTOR_INLINE
concrete_type_ref::~concrete_type_ref() {
}

ostream&
concrete_type_ref::what(ostream& o) const {
	return o << "(type-ref)";
}

line_position
concrete_type_ref::leftmost(void) const {
	return base->leftmost();
}

line_position
concrete_type_ref::rightmost(void) const {
	if (temp_spec) return temp_spec->rightmost();
	else return base->rightmost();
}

/**
	Type-check a type reference, a definition with optional template
	arguments.  The type reference is used for creating instantiations.  
	If successful, this sets the current_fundamental_type in the context.  
	\return the current fundamental type reference if successful,
		else NULL.
 */
never_const_ptr<object>
concrete_type_ref::check_build(never_ptr<context> c) const {
	never_const_ptr<object> o;
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent()) <<
			"concrete_type_ref::check_build(...): ";
	)

	// sets context's current definition
	o = base->check_build(c);
	never_const_ptr<definition_base> d(o.is_a<definition_base>());
	// and should return reference to definition
	if (!d) {
		cerr << "concrete_type_ref: bad definition reference!  "
			"ERROR! " << base->where() << endl;
		return never_const_ptr<object>(NULL);
	}

	// check template arguments, if given
	if (temp_spec) {
		// using current_definition_reference
		o = temp_spec->check_build(c);
		// remember to check the list of template formals
		// which aren't yet tied to a definition!
		// each iteration should add one more formal to the
		// current_template_formals list.  

		// should return pointer to template arguments, 
		// which is not an object yet...
		if (!o)	{
			cerr << "concrete_type_ref: "
				"bad template args!  ERROR" << endl;
			return never_const_ptr<object>(NULL);
		}
	} else {
		// if no args are supplied, 
		// make sure that the definition doesn't require template args!
		// Now allows default values for unsupplied arguments.  
		if(!d->check_null_template_argument()) {
			cerr << "definition expecting template arguments "
				"where none were given!" << endl;
			return never_const_ptr<object>(NULL);
		}
	}

	// we've made it!  set the fundamental_type_reference for instantiation
	return c->set_current_fundamental_type();
}

//=============================================================================
// class definition method definitions

CONSTRUCTOR_INLINE
definition::definition() : root_item() {
}

DESTRUCTOR_INLINE
definition::~definition() {
}

//=============================================================================
// class signature_base method definitions

DESTRUCTOR_INLINE
signature_base::~signature_base() {
}

//=============================================================================
// class process_signature method definitions

CONSTRUCTOR_INLINE
process_signature::process_signature(const template_formal_decl_list* tf, 
		const token_keyword* d, const token_identifier* i, 
		const port_formal_decl_list* p) :
		signature_base(tf,i), def(d), ports(p) {
	assert(def);
	assert(ports);		// not any more!
}

DESTRUCTOR_INLINE
process_signature::~process_signature() {
}

const token_identifier&
process_signature::get_name(void) const {
	return *id;
}

/**
	FINISH ME.
	Creates and returns a process definition objects with signature.
	\param c context is modifiable in case new concrete-types update
		the type-cache.  
 */
never_const_ptr<object>
process_signature::check_build(never_ptr<context> c) const {
//	cerr << "process_signature::check_build() FINISH ME!" << endl;
	excl_ptr<definition_base> ret(
		new process_definition(c->get_current_namespace(), *id));
	c->set_current_prototype(ret);
	if (temp_spec) {
		never_const_ptr<object> o(temp_spec->check_build(c));
		if (!o) {
			cerr << temp_spec->where() << endl;
			exit(1);
		}
	}
	if (ports && !ports->empty()) {
		never_const_ptr<object> o(ports->check_build(c));
		if (!o) {
			cerr << ports->where() << endl;
			exit(1);
		}
	}
	return c->add_declaration(c->get_current_prototype());
//	return c->set_current_prototype(ret);
}

//=============================================================================
// class process_prototype method definitions

CONSTRUCTOR_INLINE
process_prototype::process_prototype(const template_formal_decl_list* tf, 
		const token_keyword* d, const token_identifier* i, 
		const port_formal_decl_list* p, const token_char* s) :
		prototype(),
		process_signature(tf, d, i, p), 
		semi(s) {
	assert(semi);
}

DESTRUCTOR_INLINE
process_prototype::~process_prototype() {
}

ostream&
process_prototype::what(ostream& o) const {
	return o << "(process-prototype)";
}

line_position
process_prototype::leftmost(void) const {
	// temp_spec->leftmost()?
	return def->leftmost();
}

line_position
process_prototype::rightmost(void) const {
	return semi->rightmost();
}

/**
	TO DO: complete me!
 */
never_const_ptr<object>
process_prototype::check_build(never_ptr<context> c) const {
	return process_signature::check_build(c);
}

//=============================================================================
// class process_def method definitions

CONSTRUCTOR_INLINE
process_def::process_def(const template_formal_decl_list* tf, 
		const token_keyword* d, const token_identifier* i, 
		const port_formal_decl_list* p, const definition_body* b) :
		definition(),
		process_signature(tf, d, i, p), 
		body(b) {
	assert(body);		// body may be empty, is is not NULL
}

DESTRUCTOR_INLINE
process_def::~process_def() {
}

ostream&
process_def::what(ostream& o) const {
	return o << "(process-definition)";
}

line_position
process_def::leftmost(void) const {
	return def->leftmost();
}

line_position
process_def::rightmost(void) const {
	return body->rightmost();
}

/**
	To do: port_formals in process_signature...
 */
never_const_ptr<object>
process_def::check_build(never_ptr<context> c) const {
	return node::check_build(c);		// temporary
#if 0
	never_const_ptr<object> o;
	TRACE_CHECK_BUILD(
		idt->what(cerr << c->auto_indent() << 
			"process_def::check_build(...): ");
	)
	o = idt->check_build(c);
	assert(o);
	o = ports->check_build(c);
	assert(o);

	c->open_process(get_name());		// will handle errors
	o = body->check_build(c);
	assert(o);
	c->close_process_definition();
	// nothing better to do
	return c->top_namespace();
#endif
}

//=============================================================================
// class user_data_type_signature method definitions

user_data_type_signature::user_data_type_signature(
		const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, 
		const concrete_type_ref* b, 
		const data_param_decl_list* p) :
		signature_base(tf,n), def(df), dop(dp), bdt(b), params(p) {
	assert(def); assert(dop); assert(bdt); assert(params);
}

user_data_type_signature::~user_data_type_signature() {
}

/**
	Just constructs and returns an datatype definition with 
	the appropriate signature.
 */
never_const_ptr<object>
user_data_type_signature::check_build(never_ptr<context> c) const {
	cerr << "user_data_type_signature::check_build() FINISH ME!" << endl;
	excl_ptr<definition_base> dd(
		new user_def_datatype(c->get_current_namespace(), *id));
	return c->set_current_prototype(dd);
}


//=============================================================================
// class guarded_definition_body method definitions

CONSTRUCTOR_INLINE
guarded_definition_body::guarded_definition_body(const expr* e, 
		const terminal* a, const definition_body* b) :
		instance_management(), guard(e), arrow(a), body(b) {
	assert(guard); assert(arrow); assert(body);
}

DESTRUCTOR_INLINE
guarded_definition_body::~guarded_definition_body() {
}

ostream&
guarded_definition_body::what(ostream& o) const {
	return o << "(guarded-def-body)";
}

line_position
guarded_definition_body::leftmost(void) const {
	return guard->leftmost();
}

line_position
guarded_definition_body::rightmost(void) const {
	return body->rightmost();
}

//=============================================================================
// class conditional_instantiation method definitions

CONSTRUCTOR_INLINE
conditional_instantiation::conditional_instantiation(
		const guarded_definition_body_list* n) :
		instance_management(), gd(n) {
	assert(gd);
}

DESTRUCTOR_INLINE
conditional_instantiation::~conditional_instantiation() {
}

ostream&
conditional_instantiation::what(ostream& o) const {
	return o << "(conditional-instance)";
}

line_position
conditional_instantiation::leftmost(void) const {
	return gd->leftmost();
}

line_position
conditional_instantiation::rightmost(void) const {
	return gd->rightmost();
}

//=============================================================================
// abstract class prototype method definitions

inline
prototype::prototype() : root_item() { }

DESTRUCTOR_INLINE
prototype::~prototype() { }


//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes
							// also known as...
template class node_list<root_item>;			// root_body
template class node_list<concrete_type_ref,comma>;	// data_type_ref_list
template class node_list<def_body_item>;		// definition_body
template class node_list<instance_base,comma>;		// instance_id_list
template class node_list<data_param_id,comma>;		// data_param_id_list
template class node_list<data_param_decl,semicolon>;	// data_param_decl_list
template class node_list<port_formal_id,comma>;		// port_formal_id_list
template 
class node_list<port_formal_decl,semicolon>;		// port_formal_decl_list
template 
class node_list<template_formal_id,comma>;	// template_formal_id_list
template 
class node_list<template_formal_decl,semicolon>; // template_formal_decl_list
template 
class node_list<guarded_definition_body,thickbar>;
						// guarded_definition_body_list

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE


