/**
	\file "AST/type.cc"
	Class method definitions for type specifier classes.  
	$Id: type.cc,v 1.7.20.1 2007/03/10 02:51:50 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_base.cc,v 1.29.10.1 2005/12/11 00:45:02 fang Exp
 */

#ifndef	__HAC_AST_TYPE_CC__
#define	__HAC_AST_TYPE_CC__

#define	ENABLE_STACKTRACE		0

#include <exception>
#include <iostream>

#include "AST/expr_list.h"
#include "AST/type.h"
#include "AST/identifier.h"
#include "AST/node_list.tcc"
#include "AST/parse_context.h"

#include "Object/def/user_def_chan.h"
#include "Object/type/template_actuals.h"
#include "Object/type/data_type_reference.h"
#include "Object/type/builtin_channel_type_reference.h"
#include "Object/type/channel_type_reference.h"
#include "Object/expr/dynamic_param_expr_list.h"

#include "util/indent.h"
#include "util/what.h"
#include "util/stacktrace.h"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

//=============================================================================
// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::parser::type_id, "(type-id)")
SPECIALIZE_UTIL_WHAT(HAC::parser::chan_type, "(chan-type)")

// purely lazy to update these to be distinct...
SPECIALIZE_UTIL_WHAT(HAC::parser::concrete_type_ref, "(type-ref)")
SPECIALIZE_UTIL_WHAT(HAC::parser::generic_type_ref, "(type-ref)")

namespace memory {
// explicit template instantiations
using HAC::parser::concrete_type_ref;
template class count_ptr<const concrete_type_ref>;
}	// end namespace memory
}	// end namespace util

//=============================================================================
namespace HAC {
namespace parser {
using entity::dynamic_param_expr_list;
using entity::data_type_reference;
using entity::channel_type_reference;
using entity::user_def_chan;
#include "util/using_ostream.h"
using util::indent;
using util::auto_indent;

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

ostream&
type_id::dump(ostream& o) const {
	return o << *base;
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
type_id::check_definition(const context& c) const {
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
chan_type::check_base_chan_type(const context& c) const {
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
chan_type::check_type(const context& c) const {
	const data_type_ref_list::return_type
		ret(dtypes->check_builtin_channel_type(c));
	if (dir)
		ret->set_direction(dir->text[0]);
	return ret;
}

//=============================================================================
// class generic_type_ref method definitions

CONSTRUCTOR_INLINE
generic_type_ref::generic_type_ref(const type_base* n,
		const template_args_type* t, 
		const char_punctuation_type* d) : 
		base(n), temp_spec(t), chan_dir(d) {
	// cerr << "new generic_type_ref @ " << this << endl;
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
generic_type_ref::check_type(const context& c) const {
	// note: this is non-const, whereas we're returning const
	typedef	definition_base::type_ref_ptr_type	local_return_type;
	STACKTRACE("generic_type_ref::check_type()");
	// sets context's current definition
	const never_ptr<const definition_base>
		d(base->check_definition(c));
	// and should return reference to definition
	if (!d) {
		// didn't update the string out of laziness...
		cerr << "concrete_type_ref: bad definition reference \"";
		base->dump(cerr) << "\"!  ERROR! " << where(*base) << endl;
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
generic_type_ref::check_build(const context& c) const {
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
data_type_ref_list::check_builtin_channel_type(const context& c) const {
	typedef	list<concrete_type_ref::return_type>	checked_list_type;
	const count_ptr<builtin_channel_type_reference>
		ret(new builtin_channel_type_reference);
	checked_list_type checked_types;
	check_list(checked_types, &concrete_type_ref::check_type, c);
	// check if it contains NULL
	checked_list_type::const_iterator i(checked_types.begin());
	const checked_list_type::const_iterator e(checked_types.end());
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
template line_position node_list<const concrete_type_ref>::leftmost(void) const;
template line_position node_list<const concrete_type_ref>::rightmost(void) const;
template ostream& node_list<const generic_type_ref>::what(ostream&) const;
// template line_position node_list<const generic_type_ref>::leftmost() const;
// template line_position node_list<const generic_type_ref>::rightmost() const;
#endif

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __HAC_AST_TYPE_CC__

