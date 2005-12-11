/**
	\file "AST/art_parser_base.cc"
	Class method definitions for HAC::parser base classes.
	$Id: art_parser_base.cc,v 1.29.10.1 2005/12/11 00:45:02 fang Exp $
 */

#ifndef	__AST_HAC_PARSER_BASE_CC__
#define	__AST_HAC_PARSER_BASE_CC__

#define	ENABLE_STACKTRACE		0

// rule-of-thumb for inline directives:
// only inline constructors if you KNOW that they will not be be needed
// outside of this module, because we don't have a means to export
// inline methods other than defining in the header or using
// -fkeep-inline-functions

#include <exception>
#include <iostream>

#include "AST/art_parser_expr_base.h"
#include "AST/art_parser_expr_list.h"
#include "AST/art_parser_token.h"
#include "AST/art_parser_token_char.h"
#include "AST/art_parser_root_item.h"
#include "AST/art_parser_type.h"
#include "AST/art_parser_identifier.h"
#include "AST/art_parser_statement.h"
#include "AST/art_parser_definition_item.h"
#include "AST/art_parser_node_list.tcc"
#include "AST/parse_context.h"

#include "Object/def/user_def_chan.h"
#include "Object/type/template_actuals.h"
#include "Object/type/data_type_reference.h"
#include "Object/type/builtin_channel_type_reference.h"
#include "Object/type/channel_type_reference.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/common/namespace.h"

#include "util/indent.h"
#include "util/what.h"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

//=============================================================================
// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::parser::root_item, "(root_item)")
SPECIALIZE_UTIL_WHAT(HAC::parser::def_body_item, "(def-body-item)")
SPECIALIZE_UTIL_WHAT(HAC::parser::type_id, "(type-id)")
SPECIALIZE_UTIL_WHAT(HAC::parser::chan_type, "(chan-type)")
SPECIALIZE_UTIL_WHAT(HAC::parser::incdec_stmt, "(inc/dec-stmt)")
SPECIALIZE_UTIL_WHAT(HAC::parser::assign_stmt, "(assign-stmt)")
SPECIALIZE_UTIL_WHAT(HAC::parser::namespace_body, "namespace-body")
SPECIALIZE_UTIL_WHAT(HAC::parser::namespace_id, "(namespace-id)")
SPECIALIZE_UTIL_WHAT(HAC::parser::using_namespace, "(using-namespace)")

// purely lazy to update these to be distinct...
SPECIALIZE_UTIL_WHAT(HAC::parser::concrete_type_ref, "(type-ref)")
SPECIALIZE_UTIL_WHAT(HAC::parser::generic_type_ref, "(type-ref)")
}

//=============================================================================
namespace HAC {
namespace parser {
using std::back_inserter;
using entity::dynamic_param_expr_list;
using entity::data_type_reference;
using entity::channel_type_reference;
using entity::user_def_chan;
#include "util/using_ostream.h"
using util::indent;
using util::auto_indent;
USING_STACKTRACE

//=============================================================================
// global constants
// need to be kept here for explicit template instantiations (?)
const char none[] = "";		///< delimiter for node_list template argument
const char comma[] = ",";	///< delimiter for node_list template argument
const char semicolon[] = ";";	///< delimiter for node_list template argument
const char scope[] = "::";	///< delimiter for node_list template argument
const char thickbar[] = "[]";	///< delimiter for node_list template argument
const char colon[] = ":";	///< delimiter for node_list template argument
const char alias[] = "=";	///< delimiter for node_list template argument
const char pound[] = "#";	///< delimiter for node_list template argument

// eventually token keywords here too? or "art_parser_token.cc"

//=============================================================================
// class root_item method definitions

#if 0
/// Empty constructor
CONSTRUCTOR_INLINE
root_item::root_item() { }

/// Empty virtual destructor
DESTRUCTOR_INLINE
root_item::~root_item() { }
#endif

//-----------------------------------------------------------------------------
// class root_body method definitions

root_body::root_body() : parent() { }

root_body::root_body(const root_item* r) : parent(r) { }

root_body::~root_body() { }

//=============================================================================
// class type_base method definitions

#if 0
CONSTRUCTOR_INLINE
type_base::type_base() { }

DESTRUCTOR_INLINE
type_base::~type_base() { }
#endif

//=============================================================================
// class type_id method definitions

/**
	Builds a type-identifier.  
	Also deletes expression list argument after transfering list.  
 */
CONSTRUCTOR_INLINE
type_id::type_id(const qualified_id* b) : base(b) {
	NEVER_NULL(base);
}

DESTRUCTOR_INLINE
type_id::~type_id() {
}

ostream&
type_id::what(ostream& o) const {
	return base->what(o << util::what<type_id>::name() << ": ");
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
	\return pointer to type reference, else NULL if failure.  
 */
type_base::return_type
type_id::check_definition(context& c) const {
	STACKTRACE("type_id::check_build()");
	const type_base::return_type
		d(c.lookup_definition(*base));
	return d;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// non-member functions
ostream& operator << (ostream& o, const type_id& id) {
	return o << *id.base;
}

//=============================================================================
// class chan_type method definitions

CONSTRUCTOR_INLINE
chan_type::chan_type(const generic_keyword_type* c, 
		const char_punctuation_type* d, 
		const data_type_ref_list* t) : parent_type(),
		chan(c), dir(d), dtypes(t) {
	NEVER_NULL(c);
}

DESTRUCTOR_INLINE
chan_type::~chan_type() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(chan_type)

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Associates a channel or port with a data type, such as a list of 
	ints and bools.  
	\param t is the type list for the channel.  
 */
chan_type*
chan_type::attach_data_types(const data_type_ref_list* t) {
	NEVER_NULL(t); INVARIANT(!dtypes);     // sanity check    
	dtypes = excl_ptr<const data_type_ref_list>(t);
	assert(dtypes);
	return this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Wrapped call to check data types that implement the 
	channel definition.  
 */
good_bool
chan_type::check_base_chan_type(context& c) const {
	if (dir) {
		// do something with the direction
		// should be NULL in this context
		cerr << "Hmmm, ... I don\'t know what to do with the "
			"direction in this context: " << where(*this) << endl;
		THROW_EXIT;
	}
	// add data types list to cd
	// list of generic_type_refs
	return dtypes->check_data_types(c);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: guarantee that channels don't depend on relaxed template formals
 */
chan_type::return_type
chan_type::check_type(context& c) const {
	const data_type_ref_list::return_type
		ret(dtypes->check_builtin_channel_type(c));
	if (dir)
		ret->set_direction(dir->text[0]);
	return ret;
}

//=============================================================================
// class statement method definitions

#if 0
CONSTRUCTOR_INLINE
statement::statement() { }

DESTRUCTOR_INLINE
statement::~statement() { }
#endif

//=============================================================================
// class incdec_stmt method definitions

CONSTRUCTOR_INLINE
incdec_stmt::incdec_stmt(const expr* n, const string_punctuation_type* o) :
		statement(), e(n), op(o) {
	NEVER_NULL(e); NEVER_NULL(op);
}

#if 0
CONSTRUCTOR_INLINE
incdec_stmt::incdec_stmt(excl_ptr<const expr> n, excl_const_ptr<string_punctuation_type> o) :
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
excl_ptr<const expr>
incdec_stmt::release_expr(void) {
	return e;
}

excl_ptr<const string_punctuation_type>
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

const string_punctuation_type*
incdec_stmt::release_op(void) {
	const string_punctuation_type* ret = op;
	op = NULL;
	return ret;
}
#endif

PARSER_WHAT_DEFAULT_IMPLEMENTATION(incdec_stmt)

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
assign_stmt::assign_stmt(const expr* left, const char_punctuation_type* o, 
		const expr* right) : statement(),
		lhs(left), op(o), rhs(right) {
	NEVER_NULL(lhs); NEVER_NULL(op); NEVER_NULL(rhs);
}

#if 0
CONSTRUCTOR_INLINE
assign_stmt::assign_stmt(excl_ptr<const expr> left, excl_const_ptr<string_punctuation_type> o, 
		excl_ptr<const expr> right) : statement(),
		lhs(left), op(o), rhs(right) {
	assert(lhs); assert(op); assert(rhs);
}
#endif

DESTRUCTOR_INLINE
assign_stmt::~assign_stmt() {
	SAFEDELETE(lhs); SAFEDELETE(op); SAFEDELETE(rhs);
}

#if 0
excl_ptr<const expr>
assign_stmt::release_lhs(void) {
	return lhs;
}

excl_ptr<const string_punctuation_type>
assign_stmt::release_op(void) {
	return op;
}

excl_ptr<const expr>
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

const char_punctuation_type*
assign_stmt::release_op(void) {
	const char_punctuation_type* ret = op;
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

PARSER_WHAT_DEFAULT_IMPLEMENTATION(assign_stmt)

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

#if 0
CONSTRUCTOR_INLINE
def_body_item::def_body_item() { }

DESTRUCTOR_INLINE
def_body_item::~def_body_item() { }
#endif

//=============================================================================
// class definition_body method definitions

definition_body::definition_body() : parent() { }

definition_body::definition_body(const def_body_item* d) : parent(d) { }

definition_body::~definition_body() { }

//=============================================================================
// class language_body methd definitions

CONSTRUCTOR_INLINE
language_body::language_body(const generic_keyword_type* t) :
		def_body_item(), tag(t) {
}

DESTRUCTOR_INLINE
language_body::~language_body() { }

language_body*
language_body::attach_tag(generic_keyword_type* t) {
	// need to safe-delete first?  nah...
	tag = excl_ptr<const generic_keyword_type>(t);
	NEVER_NULL(tag);
	return this;
}

line_position
language_body::leftmost(void) const {
	// what if untagged?
	NEVER_NULL(tag);
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
 */
CONSTRUCTOR_INLINE
namespace_body::namespace_body(
		const generic_keyword_type* s, const token_identifier* n, 
		const root_body* b) :
		root_item(),       
		ns(s), name(n), body(b) {
	NEVER_NULL(ns); NEVER_NULL(name);
	// body may be NULL
//	NEVER_NULL(semi);	// don't really care about syntax sugar
}

/// destructor
DESTRUCTOR_INLINE
namespace_body::~namespace_body() {
}

/// what eeeez it, man?
ostream&
namespace_body::what(ostream& o) const {
	return o << '(' << util::what<namespace_body>::name() <<
		": " << *name << ')';
}

line_position
namespace_body::leftmost(void) const {
	if (ns)		return ns->leftmost();
	else		return name->rightmost();
}

line_position
namespace_body::rightmost(void) const {
	if (body)	return body->rightmost();
	else		return name->rightmost();
}

// recursive type-checker
never_ptr<const object>
namespace_body::check_build(context& c) const {
	STACKTRACE("namespace_body::check_build()");
	// use context lookup: see if namespace already exists in super-scope
		// name_space* ns = c.lookup_namespace(name);
	// if so, open it up, and work with existing namespace
	// otherwise register a new namespace, add it to context
{
	const context::namespace_frame _nf(c, *name);
	// if there was error, would've exited...
	if (body)			// may be NULL, which means empty
		body->check_build(c);
}
	// if no errors, return pointer to the namespace just processed
	return c.top_namespace();
}

//=============================================================================
// class namespace_id method definitions

namespace_id::namespace_id(qualified_id* i) : qid(i) {
	NEVER_NULL(qid);
}

namespace_id::~namespace_id() {
}

ostream&
namespace_id::what(ostream& o) const {
	return o << util::what<namespace_id>::name() << ": " << *qid;
}

line_position
namespace_id::leftmost(void) const {
	return qid->leftmost();
}

line_position
namespace_id::rightmost(void) const {
	return qid->rightmost();
}

qualified_id*
namespace_id::force_absolute(const string_punctuation_type* s) {
	return qid->force_absolute(s);
}

bool
namespace_id::is_absolute(void) const {
	return qid->is_absolute();
}

/*** NOT USED... yet
never_ptr<const object>
namespace_id::check_build(context& c) const {
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
 */
CONSTRUCTOR_INLINE
using_namespace::using_namespace(
		const generic_keyword_type* o, const namespace_id* i) :
		root_item(), open(o), id(i), alias(NULL) {
	NEVER_NULL(open); NEVER_NULL(id);
}

/**
	Constructor for using_namespace directive.  
	\param o the "open" keyword.  
	\param i the id_expr qualified identifier.  
	\param a the "as" keyword.  
	\param n the alias name.  
 */
CONSTRUCTOR_INLINE
using_namespace::using_namespace(
		const generic_keyword_type* o, const namespace_id* i, 
		const token_identifier* n) :
		root_item(), open(o), id(i), alias(n) {
	NEVER_NULL(open); NEVER_NULL(id); NEVER_NULL(alias);
}

/// default destructor
DESTRUCTOR_INLINE
using_namespace::~using_namespace() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(using_namespace)

line_position
using_namespace::leftmost(void) const {
	return open->leftmost();
}

line_position
using_namespace::rightmost(void) const {
	if (alias)	return alias->rightmost();
	else		return id->rightmost();
}

/// returns a pointer to a valid namespace that's now mapped in this scope
never_ptr<const object>
using_namespace::check_build(context& c) const {
	STACKTRACE("using_namespace::check_build()");
if (alias) {
	// (alias)
	c.alias_namespace(*id->get_id(), *alias);
} else {
	// (using)
	// if aliased... print all, report as error (done inside)
	c.using_namespace(*id->get_id());
}
	return c.top_namespace();
}

//=============================================================================
// class generic_type_ref method definitions

CONSTRUCTOR_INLINE
generic_type_ref::generic_type_ref(const type_base* n,
		const template_args_type* t, 
		const char_punctuation_type* d) : 
		base(n), temp_spec(t), chan_dir(d) {
	NEVER_NULL(base);
}

DESTRUCTOR_INLINE
generic_type_ref::~generic_type_ref() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(generic_type_ref)

line_position
generic_type_ref::leftmost(void) const {
	return base->leftmost();
}

line_position
generic_type_ref::rightmost(void) const {
	if (chan_dir)		return chan_dir->rightmost();
	else if (temp_spec)	return temp_spec->rightmost();
	else			return base->rightmost();
}

never_ptr<const type_base>
generic_type_ref::get_base_def(void) const {
	return base;
}

never_ptr<const generic_type_ref::template_args_type>
generic_type_ref::get_temp_spec(void) const {
	return temp_spec;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Type-check a type reference, a definition with optional template
	arguments.  The type reference is used for creating instantiations.  
	\return valid type-checked type-reference if successful, 
		else NULL (does not exit on failure).  
 */
generic_type_ref::return_type
generic_type_ref::check_type(context& c) const {
	// note: this is non-const, whereas we're returning const
	typedef	definition_base::type_ref_ptr_type	local_return_type;
	STACKTRACE("generic_type_ref::check_type()");
	// sets context's current definition
	const never_ptr<const definition_base>
		d(base->check_definition(c));
	// and should return reference to definition
	if (!d) {
		// didn't update the string out of laziness...
		cerr << "concrete_type_ref: bad definition reference!  "
			"ERROR! " << where(*base) << endl;
		THROW_EXIT;		// temporary
		return return_type(NULL);
	}

	// check template arguments, if given
	local_return_type type_ref;
	if (temp_spec) {
		STACKTRACE("checking template arguments (temp_spec)");
		// FUTURE: need to extend to handle generic template
		// type-argument placeholders.  
		const template_argument_list_pair::return_type
			tpl(temp_spec->check_template_args(c));
		type_ref = d->make_fundamental_type_reference(tpl);
	} else {
		STACKTRACE("empty template arguments (!temp_spec)");
		// if no args are supplied, 
		// make sure that the definition doesn't require template args!
		// Now allows default values for unsupplied arguments.  
		if(!d->check_null_template_argument().good) {
			cerr << "definition expecting template arguments "
				"where none were given!  " <<
				where(*this) << endl;
			return return_type(NULL);
		} else {
			type_ref = d->make_fundamental_type_reference();
		}
	}
	if (chan_dir) {
		STACKTRACE("have channel direction");
		const count_ptr<channel_type_reference>
			ctr(type_ref.is_a<channel_type_reference>());
		if (!ctr) {
			cerr << "ERROR: only channel types "
				"have directionality.  "
				<< where(*chan_dir) << endl;
			return return_type(NULL);
		}
		const char dir(chan_dir->text[0]);
		INVARIANT(dir == '!' || dir == '?');
		ctr->set_direction(dir);
	}
	if (!type_ref) {
		cerr << "ERROR making complete type reference.  "
			<< where(*this) << endl;
		return return_type(NULL);
	} else	return type_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Type-check a type reference, a definition with optional template
	arguments.  The type reference is used for creating instantiations.  
	If successful, this sets the current_fundamental_type in the context.  
	\return NULL, caller needs to check the current_fundamental_type
		set in the context.  
	used to return the current fundamental type reference if successful,
		else NULL.
 */
never_ptr<const object>
generic_type_ref::check_build(context& c) const {
	return_type ret(check_type(c));
	if (ret)
		c.set_current_fundamental_type(ret);
	else	THROW_EXIT;
	return never_ptr<const object>(NULL);
}
#endif

//=============================================================================
// class data_type_ref_list method definitions

data_type_ref_list::data_type_ref_list(const concrete_type_ref* c) :
		parent_type(c) { }

data_type_ref_list::~data_type_ref_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks the list of type references, which must be data types.  
	NOTE: the data types used must be strict, so that we may infer
		that all built-in channel types are strict.  
	This was arbitrarily chosen as the policy (2005-07-06) for no
		reason other than a simplfying constraint.  If you should
		ever find a reason to change this, talk to fangism.  
 */
data_type_ref_list::return_type
data_type_ref_list::check_builtin_channel_type(context& c) const {
	typedef	list<concrete_type_ref::return_type>	checked_list_type;
	const count_ptr<builtin_channel_type_reference>
		ret(new builtin_channel_type_reference);
	checked_list_type checked_types;
	check_list(checked_types, &concrete_type_ref::check_type, c);
	// check if it contains NULL
	checked_list_type::const_iterator i = checked_types.begin();
	const checked_list_type::const_iterator e = checked_types.end();
	const checked_list_type::const_iterator
		ni(find(i, e, concrete_type_ref::return_type(NULL)));
	if (ni != checked_types.end()) {
		cerr << "At least one error in data-type list at " <<
			where(*this) << endl;
		return return_type(NULL);
	} else {
		// copy to user_def_chan
		ret->reserve_datatypes(size());
		const_iterator j(begin());
		for ( ; i!=e; i++, j++) {
			const count_ptr<const data_type_reference>
				dtr(i->is_a<const data_type_reference>());
			if (!dtr) {
				cerr << "Channels can only carry data-types, ";
				(*i)->what(cerr << "but resolved a ") <<
					" at " << where(**j) << endl;
				return return_type(NULL);
			} else if (dtr->is_relaxed()) {
				cerr << "ERROR: data types used in "
					"channel-specifications must be "
					"strictly typed." << endl;
				dtr->dump(cerr << "\tgot: ") <<
					" in " << where(*this) << endl;
				return return_type(NULL);
			} else {
				ret->add_datatype(dtr);
			}
		}
		return ret;
	}
}

//=============================================================================
// explicit class template instantiations

#if 0
template class node_list<const concrete_type_ref>;
template class node_list<const generic_type_ref>;
#else
// template node_list<const concrete_type_ref>::node_list(const concrete_type_ref*);
template ostream& node_list<const concrete_type_ref>::what(ostream&) const;
// template line_position node_list<const concrete_type_ref>::leftmost() const;
// template line_position node_list<const concrete_type_ref>::rightmost() const;
template ostream& node_list<const generic_type_ref>::what(ostream&) const;
// template line_position node_list<const generic_type_ref>::leftmost() const;
// template line_position node_list<const generic_type_ref>::rightmost() const;
#endif
template class node_list<const root_item>;

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __AST_HAC_PARSER_BASE_CC__

