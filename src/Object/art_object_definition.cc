/**
	\file "art_object_definition.cc"
	Method definitions for definition-related classes.  
 	$Id: art_object_definition.cc,v 1.32.2.6.4.2 2005/02/20 09:02:43 fang Exp $
 */

#ifndef	__ART_OBJECT_DEFINITION_CC__
#define	__ART_OBJECT_DEFINITION_CC__

#define ENABLE_STACKTRACE		0

#include <exception>
#include <iostream>
#include <functional>

#include "art_parser_base.h"

#include "STL/list.tcc"
#include "hash_qmap.tcc"
#include "hash_specializations.h"		// substitute for the following

#include "art_object_definition.h"
#include "art_object_type_ref.h"
#include "art_object_instance.h"
#include "art_object_instance_param.h"
#include "art_object_inst_ref_base.h"
#include "art_object_inst_stmt.h"
#include "art_object_expr.h"			// for dynamic_param_expr_list
#include "art_object_expr_param_ref.h"
#include "art_object_type_hash.h"

#include "indent.h"
#include "binders.h"
#include "compose.h"
#include "stacktrace.h"
#include "static_trace.h"
#include "persistent_object_manager.tcc"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

//=============================================================================
STATIC_TRACE_BEGIN("object-definition")

namespace util {
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::user_def_chan, USER_DEF_CHAN_DEFINITION_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_definition_alias, CHANNEL_TYPEDEF_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_datatype_def, ENUM_DEFINITION_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::user_def_datatype, USER_DEF_DATA_DEFINITION_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::datatype_definition_alias, DATA_TYPEDEF_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_definition, PROCESS_DEFINITION_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_definition_alias, PROCESS_TYPEDEF_TYPE_KEY)
}	// end namespace util

//=============================================================================
namespace ART {
namespace entity {
using std::_Select2nd;
#include "using_ostream.h"
using parser::scope;
using util::indent;
using util::auto_indent;
USING_STACKTRACE
using namespace ADS;
using util::write_value;
using util::read_value;
using util::write_string;
using util::read_string;
using util::persistent_traits;

//=============================================================================
// class definition_base method definitions

const never_ptr<const definition_base>
definition_base::null(NULL);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Definition basic constructor.  
 */
inline
definition_base::definition_base() :
		object(), persistent(), 
		template_formals_map(), 
		template_formals_list(), 
		defined(false) {
	// synchronize template formals with used_id_map
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline
definition_base::~definition_base() {
	STACKTRACE("~definition_base()");
#if 0
	cerr << "\t@ " << this << endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only dumps the basic template information of the definition.  
	Default behavior may be overridden by subclasses.  
 */
ostream&
definition_base::dump(ostream& o) const {
	const string key = get_key();
	what(o) << ((defined) ? " (defined) " : " (declared) ") << key;
	dump_template_formals(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints key in addition to dumping, including newline.  
	Called by the name_space::dump().
 */
ostream&
definition_base::pair_dump(ostream& o) const {
	o << auto_indent << get_key() << " = ";
	return dump(o) << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sequentially prints template formal parameters.  
 */
ostream&
definition_base::dump_template_formals(ostream& o) const {
	STACKTRACE("definition_base::dump_template_formals()");
	// sanity check
	INVARIANT(template_formals_list.size() == template_formals_map.size());
	if (!template_formals_list.empty()) {
		indent tfl_ind(o);
		o << "<" << endl;	// continued from last print
		template_formals_list_type::const_iterator
			i = template_formals_list.begin();
		const template_formals_list_type::const_iterator
			e = template_formals_list.end();
		for ( ; i!=e; i++) {
			// sanity check
			NEVER_NULL(*i);
			INVARIANT((*i)->is_template_formal());
			(*i)->dump(o << auto_indent) << endl;
		}
		o << auto_indent << ">" << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a map from template formals to actual values.  
	Precondition: list passed by reference must be initially empty.
	Considering making appropriate virtual call interface in 
		param_expr_list?
 */
void
definition_base::fill_template_actuals_map(
		template_actuals_map_type& am, 
		const param_expr_list& al) const {
	INVARIANT(am.empty());
	INVARIANT(template_formals_list.size() == al.size());
	// convert to virtual call interface to param_expr_list?
	const const_param_expr_list* cpl =
		IS_A(const const_param_expr_list*, &al);
	const dynamic_param_expr_list* dpl =
		IS_A(const dynamic_param_expr_list*, &al);
	template_formals_list_type::const_iterator f_iter =
		template_formals_list.begin();
if (cpl) {
	const_param_expr_list::const_iterator i = cpl->begin();
	for ( ; f_iter!=template_formals_list.end(); f_iter++, i++) {
		// const-reference saves unnecessary copying
		const template_formals_value_type& tf(*f_iter);
		// reminder: value type is pointer to param_instance_collection
		NEVER_NULL(tf);
		// reminder: actuals map is of count_ptr
		NEVER_NULL(*i);
		am[tf->get_name()] = *i;
	}
} else {
	NEVER_NULL(dpl);
	dynamic_param_expr_list::const_iterator i = dpl->begin();
	for ( ; f_iter!=template_formals_list.end(); f_iter++, i++) {
		// const-reference saves unnecessary copying
		const template_formals_value_type& tf(*f_iter);
		// reminder: value type is pointer to param_instance_collection
		NEVER_NULL(tf);
		// reminder: actuals map is of count_ptr
		NEVER_NULL(*i);
		am[tf->get_name()] = *i;
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only looks up the identifier in the set of template formals.  
 */
never_ptr<const param_instance_collection>
definition_base::lookup_template_formal(const string& id) const {
	return static_cast<const template_formals_map_type&>
		(template_formals_map)[id];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the (1-indexed) position of the referenced parameter 
		in the list if found, else 0 if not found.
 */
size_t
definition_base::lookup_template_formal_position(const string& id) const {
	const never_ptr<const param_instance_collection>
		pp(lookup_template_formal(id));
	const template_formals_list_type::const_iterator
		pb = template_formals_list.begin();
	// default, uses pointer comparison
	return (pp) ? distance(pb,
			std::find(pb, template_formals_list.end(), pp)) +1
		: 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Searches template formals set *ONLY* for a matching object.  
	Subclasses should override this to search their respective scopes.  
 */
never_ptr<const object>
definition_base::lookup_object_here(const string& id) const {
	return lookup_template_formal(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Used for checking when a type should have null template arguments.  
	Really just a special case of general template argument checking.  
	\return true if this definition is not templated, 
		or the template formals signature is empty, 
		or default parameters are available for all formals.  
 */
bool
definition_base::check_null_template_argument(void) const {
//	STACKTRACE("definition_base::check_null_template_argument()");
	if (template_formals_list.empty())
		return true;
	// else make sure each formal has a default parameter value
	template_formals_list_type::const_iterator i =
		template_formals_list.begin();
	for ( ; i!=template_formals_list.end(); i++) {
		const never_ptr<const param_instance_collection> p(*i);
		NEVER_NULL(p);
		p.must_be_a<const param_instance_collection>();
		// if any formal is missing a default value, then this 
		// definition cannot have null template arguments
		if (!(*p).default_value()) {
			return false;
		}
		// else continue;	// keep checking
	}
	// if we've reached end of list, we're good!
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A default lookup that always returns NULL.  
	Overridden in process_definition.  
 */
never_ptr<const instance_collection_base>
definition_base::lookup_port_formal(const string& id) const {
	return never_ptr<const instance_collection_base>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Compares the sequence of template formals for a generic definition.  
	\return true if they are equivalent.  
 */
bool
definition_base::equivalent_template_formals(
		const never_ptr<const definition_base> d) const {
	NEVER_NULL(d);
	const template_formals_list_type& dtemp = d->template_formals_list;
	if (template_formals_list.size() != dtemp.size()) {
		// useful error message here
		cerr << "ERROR: number of template formal parameters "
			"doesn\'t match!" << endl;
		return false;
	}
	template_formals_list_type::const_iterator i =
		template_formals_list.begin();
	template_formals_list_type::const_iterator j = dtemp.begin();
	for ( ; i!=template_formals_list.end() && j!=dtemp.end(); i++, j++) {
		const never_ptr<const param_instance_collection> itf(*i);
		const never_ptr<const param_instance_collection> jtf(*j);
		NEVER_NULL(itf);	// template formals not optional
		NEVER_NULL(jtf);	// template formals not optional
		// only type and size need to be equal, not name
		if (!itf->template_formal_equivalent(jtf)) {
			// useful error message goes here
			cerr << "ERROR: template formals do not match!" << endl;
			return false;
		}
		// else continue checking
	}
	// sanity check, we made sure sizes match.
	INVARIANT(i == template_formals_list.end() && j == dtemp.end());
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
definition_base::get_name(void) const {
	return get_key();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
definition_base::get_qualified_name(void) const {
	const string key = get_key();
	const never_ptr<const scopespace> parent(get_parent());
	if (parent)
		return parent->get_qualified_name() +scope +key;
	else return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Certifies the template arguments against this definition's
	template signature.  
	This also replaces NULL arguments in the list with defaults
	where appropriate.  
	\param ta modifiable list of template argument expressions.
		If this list is empty, however, it will try to 
		construct the default arguments entirely.  
	\return true if arguments successfully type-checked 
		and default arguments supplied in missing places.  
 */
bool
definition_base::certify_template_arguments(
		const never_ptr<dynamic_param_expr_list> ta) const {
if (ta) {
	// first, number of arguments must match
	const size_t a_size = ta->size();
	const size_t f_size = template_formals_list.size();
	const template_formals_list_type::const_iterator f_end =
		template_formals_list.end();
	template_formals_list_type::const_iterator f_iter =
		template_formals_list.begin();
	if (a_size != f_size) {
		if (a_size)
			return false;
		// else a_size == 0, passed actuals list is empty, 
		// try to fill in all default arguments
		for ( ; f_iter!=f_end; f_iter++) {
			const never_ptr<const param_instance_collection>
				pinst(*f_iter);
			NEVER_NULL(pinst);
			const count_ptr<const param_expr>
				default_expr(pinst->default_value());
			if (!default_expr) {
				// no default value to supply
				return false;
			} else {
				ta->push_back(default_expr);
			}
		}
		// if it fails, then list will be incomplete.  
		// if this point is reached, then fill-in was successfull
		return true;
	}
	dynamic_param_expr_list::iterator p_iter = ta->begin();
	for ( ; f_iter!=f_end; p_iter++, f_iter++) {
		// need method to check param_instance_collection against param_expr
		// eventually also work for complex aggregate types!
		// "I promise this pointer is only local."  
		const count_ptr<const param_expr> pex(*p_iter);
		const never_ptr<const param_instance_collection>
			pinst(*f_iter);
		NEVER_NULL(pinst);
		if (pex) {
			// type-check assignment, conservative w.r.t. arrays
			if (!pinst->type_check_actual_param_expr(*pex)) {
				// error message?
				return false;
			}
			// else continue checking successive arguments
		} else {
			// no parameter expression given, 
			// check for default -- if exists, use it, 
			// else is error
			const count_ptr<const param_expr>
				default_expr(pinst->default_value());
			if (!default_expr) {
				// error message?
				return false;
			} else {
				// else, actually assign it a copy in the list
				*p_iter = default_expr;
			}
		}
	}
	// end of checking reached, everything passed
	return true;
} else {
	// no arguments supplied, make sure template specification is
	// null, or every formal has default values.  
	return check_null_template_argument();
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default: return false (if unimplemented)
	Temporarily prints an error message.  
 */
bool
definition_base::certify_port_actuals(const object_list& ol) const {
	cerr << "Default definition_base::certify_port_actuals() = false."
		<< endl;
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prerequisiste for calling this: must satisfy
		check_null_template_arguments.  
	\returns a list of default parameter expressions.  
 */
excl_ptr<dynamic_param_expr_list>
definition_base::make_default_template_arguments(void) const {
	typedef	excl_ptr<dynamic_param_expr_list>	return_type;
	INVARIANT(check_null_template_argument());
	if (template_formals_list.empty())
		return return_type(NULL);
	// defaulting to dynamic_param_expr_list
	return_type ret(new dynamic_param_expr_list);
	template_formals_list_type::const_iterator i = 
		template_formals_list.begin();
	for ( ; i!=template_formals_list.end(); i++) {
		const count_ptr<const param_expr> d((*i)->default_value());
		NEVER_NULL(d);	// everything must have default
		ret->push_back(d);
	}
	// should transfer ownership
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper for making a type reference with default template args.  
 */
count_ptr<const fundamental_type_reference>
definition_base::make_fundamental_type_reference(void) const {
	// assign, not copy construct!
	excl_ptr<dynamic_param_expr_list>
		dplp = make_default_template_arguments();
	return make_fundamental_type_reference(dplp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	DO ME NOW!
	Adds an instantiation to the current definition's scope, and 
	also registers it in the list of template formals for 
	template argument checking.  
	What if template formal is an array, or collective?
	TO DO: convert to pointer-classes...
	\param f needs to be a param_instance_collection... what about array?
		need to be non-const? storing to hash_map_of_ptr...
		must be modifiable for used_id_map
 */
never_ptr<const instance_collection_base>
definition_base::add_template_formal(
		const never_ptr<instantiation_statement> i, 
		const token_identifier& id) {
	STACKTRACE("definition_base::add_template_formal()");
	typedef	never_ptr<const instance_collection_base>	return_type;
	// const string id(pf->get_name());	// won't have name yet!
	// check and make sure identifier wasn't repeated in formal list!
	{
	const never_ptr<const object>
		probe(lookup_object_here(id));
	if (probe) {
		probe->what(cerr << " already taken as a ") << " ERROR!";
		return return_type(NULL);
	} 
	}

	// since we already checked used_id_map, there cannot be a repeat
	// in the template_formals_list!
	// template_formals_list and _map are strict subsets of used_id_map

	// COMPILE: pf is const, but used_id_map members are not
	// wrap around with object_handle?

	// this construction is ugly, TO DO: define clean interface
	scopespace* ss = IS_A(scopespace*, this);
	NEVER_NULL(ss);
	// this creates and adds to the definition
	// and bi-links statement to collection
	const never_ptr<const param_instance_collection>
		pf(ss->add_instance(i, id).is_a<const param_instance_collection>());
	NEVER_NULL(pf);
	INVARIANT(pf->get_name() == id);	// consistency check

	template_formals_list.push_back(pf);
	template_formals_map[id] = pf;

	// sanity check
	INVARIANT(lookup_template_formal(id));
	// later return a never_ptr<>
	return pf;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Virtually pure, not purely virtual...
	Only temporary.
	Override in appropriate subclasses.  
 */
never_ptr<const instance_collection_base>
definition_base::add_port_formal(
		const never_ptr<instantiation_statement> f, 
		const token_identifier& i) {
	DIE;
	return never_ptr<const instance_collection_base>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note: calling this is unnecessary if one guarantees that
	the template formals are a strict subset of the used_id_map.  
	However, it can't hurt to revisit pointers.
 */
inline
void
definition_base::collect_template_formal_pointers(
		persistent_object_manager& m) const {
	STACKTRACE("definition_base::collect_transients()");
	template_formals_list_type::const_iterator
		iter = template_formals_list.begin();
	const template_formals_list_type::const_iterator
		end = template_formals_list.end();
	for ( ; iter!=end; iter++) {
		(*iter)->collect_transient_info(m);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper for consistent interface.
 */
void
definition_base::collect_transient_info_base(
		persistent_object_manager& m) const {
	collect_template_formal_pointers(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Template formals will need to be in list order.
	Just write out the list, the hash_qmap is redundant.  
 */
inline
void
definition_base::write_object_template_formals(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE("definition_base::write_object_template_formals()");
	INVARIANT(template_formals_list.size() == template_formals_map.size());
	m.write_pointer_list(o, template_formals_list);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Fake empty list.  
 */
void
definition_base::write_object_base_fake(
		const persistent_object_manager& m, ostream& o) {
	static const template_formals_list_type dummy;
	// this is a non-member function, emulating write_object_base
	static const bool fake_defined = false;	// value doesn't matter
	write_value(o, fake_defined);
	m.write_pointer_list(o, dummy);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
definition_base::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	write_value(o, defined);
	write_object_template_formals(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Template formals are loaded in list order.
	Remember that the redundant hash_map also needs to be reconstructed.  
	Another method will add the entries to the corresponding
	used_id_map where appropriate.  
 */
inline
void
definition_base::load_object_template_formals(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE("definition_base::load_object_template_formals()");
	m.read_pointer_list(f, template_formals_list);
	// then copy list into hash_map to synchronize
	template_formals_list_type::const_iterator
		iter = template_formals_list.begin();
	const template_formals_list_type::const_iterator
		end = template_formals_list.end();
	for ( ; iter!=end; iter++) {
		STACKTRACE("for-loop: load a map entry");
		const template_formals_value_type inst_ptr = *iter;
		NEVER_NULL(inst_ptr);
		// we need to load the instantiation to use its key!
		m.load_object_once(const_cast<param_instance_collection*>(
			&*inst_ptr));
		template_formals_map[inst_ptr->get_name()] = inst_ptr;
	}
	INVARIANT(template_formals_list.size() == template_formals_map.size());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
definition_base::load_object_base(
		const persistent_object_manager& m, istream& f) {
	read_value(f, defined);
	load_object_template_formals(m, f);
}

//=============================================================================
// class typedef_base method definitions

#if 0
typedef_base::typedef_base() : 
		definition_base(), scopespace(), sequential_scope() {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// inline
typedef_base::~typedef_base() {
	STACKTRACE("~typedef_base()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
typedef_base::get_qualified_name(void) const {
	return definition_base::get_qualified_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Description dump works for all alias types.  
 */
ostream&
typedef_base::dump(ostream& o) const {
	what(o) << ": " << get_key();
	dump_template_formals(o) << " = ";
	get_base_type_ref()->dump(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
UNVEIL LATER
/**
	Should this recursively resolve typedefs?
	\param pa the actual parameters passed.  
		Should we require that it be "initialized" i.e. constant or 
		dependent on other template formals (in the case of typedefs
		within template definitions)?
		If dependent on template formals, this type cannot be 
		fully resolved.  
		Only accept const_param_expr_list?  Yeah.  
	\return fully expanded type ONLY IF all parameters are bound
		to static scalar constants, otherwise NULL.  
		Non-scalars and formal values are only resolved at unroll-time.
		Returning NULL is not error, is just conservative w.r.t.
		type-checking.  
 */
excl_ptr<const fundamental_type_reference>
typedef_base::resolve_complete_type(never_ptr<const param_expr_list> pa) const {
	typedef	excl_ptr<const fundamental_type_reference>	return_type;
	never_ptr<const fundamental_type_ref>
		btr(get_base_type_ref());
	never_ptr<const definition_base>
		bd(btr->get_base_def());
	// what if base definition is another typedef?
	excl_ptr<param_expr_list>
		pl(btr->get_copy_template_params());
if (pa) {
	// precondition: all arguments are already supplied, 
	// perform recursive expression substitutions as necessary.
} else {
	// assuming it was type-checked before
	// this is a valid null-template type.  
	// need to return a private exclusive copy of the type-reference
	// no substitution necessary, should already be a complete type
	if (pl) {
		if (pl->is_static_constant()) {
			excl_ptr<dynamic_param_expr_list>
				dpl(pl.is_a_xfer<dynamic_param_expr_list>());
			NEVER_NULL(dpl);	// temporary
			count_ptr<const fundamental_type_reference>
				cftr(bd->make_fundamental_type_reference(dpl));
			INVARIANT(cftr.refs() == 1);
			return return_type(cftr.exclusive_release());
		} else {
			// not static constant scalar, conservatively, 
			return return_type(NULL);
		}
	} else {
		count_ptr<const fundamental_type_reference>
			cftr(bd->make_fundamental_type_reference());
		INVARIANT(cftr.refs() == 1);
		return return_type(cftr.exclusive_release());
	}
}
}
#endif

//=============================================================================
// class datatype_definition_base method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
inline
datatype_definition_base::datatype_definition_base() :
		definition_base() {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline
datatype_definition_base::~datatype_definition_base() {
	STACKTRACE("~datatype_definition_base()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a typedef (definition alias) from this definition.  
	\param s the namespace in which the alias resides.  
	\param id the local name of the alias.  
 */
excl_ptr<definition_base>
datatype_definition_base::make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const {
	return excl_ptr<definition_base>(
		new datatype_definition_alias(id, s));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// now pure virtual
count_ptr<const fundamental_type_reference>
datatype_definition_base::make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list> ta) const {
	typedef count_ptr<const fundamental_type_reference>	return_type;
	if (certify_template_arguments(ta)) {
		return return_type(
			new data_type_reference(
				never_ptr<const datatype_definition_base>(this), 
				excl_ptr<const param_expr_list>(ta)));
	} else {
		cerr << "ERROR: failed to make data_type_reference "
			"because template argument types do not match." << endl;
		return return_type(NULL);
	}
}
#endif

//=============================================================================
// class channel_definition_base method definitions

#if 0
// make sure that this constructor is never invoked outside this file
inline
channel_definition_base::channel_definition_base() :
		definition_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_definition_base::~channel_definition_base() {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<definition_base>
channel_definition_base::make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const {
	return excl_ptr<definition_base>(
		new channel_definition_alias(id, s));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
channel_definition_base::make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list>& ta) const {
	typedef count_ptr<const fundamental_type_reference>	return_type;
	if (certify_template_arguments(ta)) {
		excl_ptr<const param_expr_list> plp(ta);
		return return_type(
			new channel_type_reference(
				never_ptr<const channel_definition_base>(this), 
				plp));
	} else {
		cerr << "ERROR: failed to make channel_type_reference "
			"because template argument types do not match." << endl;
		return return_type(NULL);
	}
}

//=============================================================================
// class user_def_chan method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
user_def_chan::user_def_chan(never_ptr<const name_space> o, 
		const string& name) :
		definition_base(), 
		channel_definition_base(), 
		scopespace(),
		sequential_scope(), 
		key(name), 
		parent(o) {
	// FINISH ME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
user_def_chan::~user_def_chan() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
user_def_chan::what(ostream& o) const {
	return o << "user-def-chan";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
user_def_chan::dump(ostream& o) const {
	return o << "fang, get off your lazy ass and "
		"write user_def_chan::dump()!" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
user_def_chan::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
user_def_chan::get_qualified_name(void) const {
	if (parent)
		return parent->get_qualified_name() + scope + key;
	else return string(scope) + key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
user_def_chan::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const object>
user_def_chan::lookup_object_here(const string& id) const {
	return scopespace::lookup_object_here(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void
user_def_chan::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {

	// recursively visit members...
	sequential_scope::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/**
	Just allocate with bogus arguments.  
 */
persistent*
user_def_chan::construct_empty(const int i) {
	return new user_def_chan(never_ptr<const name_space>(NULL), "");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not recursive, manager will call this once.  
 */
void
user_def_chan::write_object(
		const persistent_object_manager& m, ostream& f) const {
	write_string(f, key);
	m.write_pointer(f, parent);
	definition_base::write_object_base(m, f);
//	write_object_port_formals(m);
	scopespace::write_object_base(m, f);
	// connections and assignments
	sequential_scope::write_object_base(m, f);
	// body
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads in fields from input stream.  
 */
void
user_def_chan::load_object(const persistent_object_manager& m, istream& f) {
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, parent);
	definition_base::load_object_base(m, f);
//	load_object_port_formals(m);
	scopespace::load_object_base(m, f);
	// connections and assignments
	sequential_scope::load_object_base(m, f);
	// body
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
user_def_chan::load_used_id_map_object(excl_ptr<persistent>& o) {
	if (o.is_a<instance_collection_base>()) {
		excl_ptr<instance_collection_base>
			icbp = o.is_a_xfer<instance_collection_base>();
		add_instance(icbp);
		INVARIANT(!icbp);
	} else {
		o->what(cerr << "TO DO: define method for adding ")
			<< " back to user-def channel definition." << endl;
	}
}

//=============================================================================
// class channel_definition_alias method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_definition_alias::channel_definition_alias(
		const string& n, never_ptr<const scopespace> p) :
		definition_base(), 
		channel_definition_base(), 
		typedef_base(), 
		key(n), parent(p), base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_definition_alias::~channel_definition_alias() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_definition_alias::what(ostream& o) const {
	return o << "channel-definition-alias";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
channel_definition_alias::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
channel_definition_alias::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const fundamental_type_reference>
channel_definition_alias::get_base_type_ref(void) const {
	return base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_definition_alias::assign_typedef(
		excl_ptr<const fundamental_type_reference>& f) {
	NEVER_NULL(f);
	base = f.is_a_xfer<const channel_type_reference>();
	NEVER_NULL(base);
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively collects reachable pointers and register them
	with the persistent object manager.  
 */
void
channel_definition_alias::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	base->collect_transient_info(m);
	definition_base::collect_transient_info_base(m);
	scopespace::collect_transient_info_base(m);	// covers formals?
	sequential_scope::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Empty allocator.  
 */
persistent*
channel_definition_alias::construct_empty(const int i) {
	return new channel_definition_alias("",
		never_ptr<const scopespace>(NULL));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not recursive, manager will call this once.  
 */
void
channel_definition_alias::write_object(
		const persistent_object_manager& m, ostream& f) const {
	write_string(f, key);
	m.write_pointer(f, parent);
	m.write_pointer(f, base);
	definition_base::write_object_base(m, f);
	scopespace::write_object_base(m, f);
	sequential_scope::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_definition_alias::load_object(
		const persistent_object_manager& m, istream& f) {
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, parent);
	m.read_pointer(f, base);
	definition_base::load_object_base(m, f);
	scopespace::load_object_base(m, f);
	sequential_scope::load_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Virtually called from scopespace to load formal parameters
	into the typedef's scopespace, which contains only formal parameters.  
	Really, typedefs shouldn't have any non-formal members...
 */
void
channel_definition_alias::load_used_id_map_object(excl_ptr<persistent>& o) {
#if 0
	cerr << "WARNING: didn't expect to call "
		"channel_definition_alias::load_used_id_map_object()." << endl;
#endif
	if (o.is_a<instance_collection_base>()) {
		excl_ptr<instance_collection_base>
			icbp = o.is_a_xfer<instance_collection_base>();
		add_instance(icbp);
		INVARIANT(!icbp);
	} else {
		o->what(cerr << "TO DO: define method for adding ")
			<< " back to channel typedef." << endl;
	}
}

//=============================================================================
// class built_in_datatype_def method definitions

/**
	Built-in data type marks itself as already defined.  
 */
built_in_datatype_def::built_in_datatype_def(
		never_ptr<const name_space> o, 
		const string& n) :
		definition_base(), 
		datatype_definition_base(),
		scopespace(), 
		key(n), 
		parent(o) {
	mark_defined();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// special case phased out
/**
	Same constructor, but with one template formal parameter.  
	This constructor is dedicated to int<pint width>.  
 */
built_in_datatype_def::built_in_datatype_def(
		never_ptr<const name_space> o, 
		const string& n, 
		excl_ptr<param_instance_collection> p) :
		definition_base(), 
		datatype_definition_base(), 
		scopespace(), 
		key(n), 
		parent(o) {
	NEVER_NULL(p);
//	const string param_str(p->get_name());
	add_template_formal(p.as_a_xfer<instance_collection_base>());
	mark_defined();
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
built_in_datatype_def::~built_in_datatype_def() {
	STACKTRACE("~built_in_datatype_def()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
built_in_datatype_def::what(ostream& o) const {
	return o << "built-in-datatype-def";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
built_in_datatype_def::dump(ostream& o) const {
	return datatype_definition_base::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
built_in_datatype_def::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
built_in_datatype_def::get_qualified_name(void) const {
	return datatype_definition_base::get_qualified_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
built_in_datatype_def::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const datatype_definition_base>
built_in_datatype_def::resolve_canonical_datatype_definition(void) const {
	return never_ptr<const datatype_definition_base>(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
built_in_datatype_def::make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list>& ta) const {
	typedef	count_ptr<const fundamental_type_reference>	return_type;
	if (certify_template_arguments(ta)) {
		excl_ptr<const param_expr_list> plp(ta);
		return return_type(
			new data_type_reference(
				never_ptr<const built_in_datatype_def>(this), 
				plp));
	} else {
		cerr << "ERROR: failed to make built_in_data_type_reference "
			"because template argument types do not match." << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Since built-in types do not correspond to scopespaces, 
	we have to override definition_base::add_template_formal.  
	Used in construction of built-in types in art_built_ins.cc.
	KLUDGE: redesign interface classes, please!
	\param f the param instance collection, will keep ownership.  
 */
never_ptr<const instance_collection_base>
built_in_datatype_def::add_template_formal(
		excl_ptr<instance_collection_base>& f) {
	STACKTRACE("add_template_formal(excl_ptr<>)");
	const never_ptr<const param_instance_collection>
		pf(f.is_a<const param_instance_collection>());
	NEVER_NULL(pf);
	// check and make sure identifier wasn't repeated in formal list!
	const never_ptr<const object>
		probe(datatype_definition_base::lookup_object_here(
			pf->get_name()));
	if (probe) {
		probe->what(cerr << " already taken as a ") << " ERROR!";
		return never_ptr<const instance_collection_base>(NULL);
	}

	template_formals_list.push_back(pf);
	template_formals_map[pf->hash_string()] = pf;
	// since we already checked used_id_map, there cannot be a repeat
	// in the template_formals_list!
	// template_formals_list and _map are strict subsets of used_id_map

	// no used_id_map to update, b/c this is not a scopespace!

	// sanity check
	INVARIANT(lookup_template_formal(pf->hash_string()));
	// later return a never_ptr<>
	return pf;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Leeching off of datatype definition.  
	Will be handled specially (replaced) by data_type_reference.  
 */
void
built_in_datatype_def::collect_transient_info(
		persistent_object_manager& m) const {
	STACKTRACE("built_in_data::collect_transients()");
	m.register_transient_object(this, 
		persistent_traits<user_def_datatype>::type_key);
	// NOTE: not using this_type is INTENTIONAL
	// don't bother with parent pointer to built-in namespace
#if 0
	definition_base::collect_transient_info_base(m);
	// STOP: definition is built in! don't recur!
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	HACKERY ALERT!
	Since we're mimicking a user-defined datatype, this should
	match the format for user_def_datatype.  
	This object will not live long.  
	We assume that no user-defined datatype can have the same
	name as the built-in types because they are reserved keywords.  
 */
void
built_in_datatype_def::write_object(
		const persistent_object_manager& m, ostream& f) const {
	STACKTRACE("built_in_data::write_object()");
	write_string(f, key);
	// use bogus parent pointer
	m.write_pointer(f, never_ptr<const name_space>(NULL));
	// bogus template and port formals
	definition_base::write_object_base_fake(m, f);	// is empty
	scopespace::write_object_base_fake(m, f);
	// connections and assignments
	sequential_scope::write_object_base_fake(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
built_in_datatype_def::load_object(
		const persistent_object_manager& m, istream& f) {
	cerr << "ERROR: built_in_datatype_def::load_object() "
		"should never be called!" << endl;
	DIE;
	THROW_EXIT;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
built_in_datatype_def::load_used_id_map_object(excl_ptr<persistent>& o) {
	cerr << "ERROR: built_in_datatype_def::load_used_id_map_object() "
		"should never be called!" << endl;
	DIE;
	THROW_EXIT;
}

//=============================================================================
// class built_in_param_def method definitions

/**
	Built-in param marks itself as already defined.  
 */
built_in_param_def::built_in_param_def(
		never_ptr<const name_space> p,
		const string& n) :
		definition_base(),
		key(n), 
		parent(p) {
	mark_defined();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
built_in_param_def::~built_in_param_def() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
built_in_param_def::what(ostream& o) const {
	return o << "built-in-param-def";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
built_in_param_def::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
built_in_param_def::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Cannot alias built-in parameter types!
	Why? Because I said so.  
 */
excl_ptr<definition_base>
built_in_param_def::make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const {
	DIE;
	return excl_ptr<definition_base>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	PROBLEM: built_in types cannot be owned with excl_ptr!!!
	There's one shared static built-in reference for each type.  
	One solution: do away with built-in type_reference?
	Or have a caller check for built-ins and replace?
	Constructed built-in type references won't be used
	in param_instance_collections, should check but then ignore.  
	Managed cache may solve this...
	\param ta template arguments are never used.  
 */
count_ptr<const fundamental_type_reference>
built_in_param_def::make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list>& ta) const {
	INVARIANT(!ta);
	return count_ptr<const fundamental_type_reference>(
		new param_type_reference(
			never_ptr<const built_in_param_def>(this)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

PERSISTENT_METHODS_DUMMY_IMPLEMENTATION(built_in_param_def)

//=============================================================================
// class enum_member method definitions

#if 0
enum_member::enum_member(const string& n) : object(), id(n) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
enum_member::~enum_member() { }
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
enum_member::what(ostream& o) const {
	return o << "enum-member";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
enum_member::dump(ostream& o) const {
	return o << id;
}

//=============================================================================
// class enum_datatype_def method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
enum_datatype_def::enum_datatype_def(never_ptr<const name_space> o, 
		const string& n) : 
		definition_base(), 
		datatype_definition_base(), 
		scopespace(), 
		key(n), 
		parent(o) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
enum_datatype_def::~enum_datatype_def() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
enum_datatype_def::what(ostream& o) const {
	return o << "enum-datatype-def";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
enum_datatype_def::dump(ostream& o) const {
	what(o) << ": " << key;
	if (defined) {
		indent enum_ind(o);
		o << endl << auto_indent << "{ ";
		used_id_map_type::const_iterator i = used_id_map.begin();
		const used_id_map_type::const_iterator e = used_id_map.end();
		for ( ; i!=e; i++) {
			o << i->first << ", ";
		}
		o << " }";
	} else {
		o << " (undefined)";
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
enum_datatype_def::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
enum_datatype_def::get_qualified_name(void) const {
	if (parent)
		return parent->get_qualified_name() + scope + key;
	else	return string(scope) + key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
enum_datatype_def::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const datatype_definition_base>
enum_datatype_def::resolve_canonical_datatype_definition(void) const {
	return never_ptr<const datatype_definition_base>(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: ta is not used.
 */
count_ptr<const fundamental_type_reference>
enum_datatype_def::make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list>& ta) const {
	typedef count_ptr<const fundamental_type_reference>	return_type;
	INVARIANT(!ta);
	return return_type(
		new data_type_reference(
			never_ptr<const datatype_definition_base>(this)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not the same as type-equivalence, which requires precise 
	pointer equality.  
	This is used for comparing prototypes and signatures of 
	declarations and definitions.  
	Report errors to stderr or stdout?
	Template this?  Nah...
	\param d the definition (signature) to compare, 
		the name MUST match, else comparison is pointless!
 */
bool
enum_datatype_def::require_signature_match(
		const never_ptr<const definition_base> d) const {
	NEVER_NULL(d);
	INVARIANT(key == d->get_name());
	const never_ptr<const enum_datatype_def>
		ed(d.is_a<const enum_datatype_def>());
	if (ed) {
		// only names need to match...
		// no other signature information!  easy.
		return true;
	} else {
		// class type doesn't even match!  report error.
		d->what(cerr << key << " is already declared as a ")
			<< " but is being redeclared as a ";
		what(cerr) << "  ERROR!  ";
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if successfully added, false if there was conflict.  
 */
bool
enum_datatype_def::add_member(const token_identifier& em) {
	const never_ptr<const object>
		probe(scopespace::lookup_object_here(em));
	if (probe) {
		const never_ptr<const enum_member>
			probe_em(probe.is_a<const enum_member>());
		NEVER_NULL(probe_em);	// can't contain enything else
		return false;
	} else {
#if 0
		// gcc-3.4.0 rejects, thinking that excl_ptr is const
		used_id_map[em] = excl_ptr<enum_member>(
			new enum_member(em));
#else
		excl_ptr<enum_member> member_ptr(new enum_member(em));
		used_id_map[em] = member_ptr;
		INVARIANT(!member_ptr);
#endif
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively collects reachable pointers and register them
	with the persistent object manager.  
 */
void
enum_datatype_def::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	definition_base::collect_transient_info_base(m);
		// but no templates
	scopespace::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Empty allocator.  
 */
persistent*
enum_datatype_def::construct_empty(const int i) {
	return new enum_datatype_def(never_ptr<const name_space>(NULL), "");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not recursive, manager will call this once.  
	Since enum-members are merely strings, we just write them out as 
	strings.  
 */
void
enum_datatype_def::write_object(
		const persistent_object_manager& m, ostream& f) const {
	write_string(f, key);
	m.write_pointer(f, parent);
	definition_base::write_object_base(m, f);

	// can't use scopespace because enum_member is not persistent
//	scopespace::write_object_base(m, f);
	{
		const size_t s = used_id_map.size();
		write_value(f, s);
		used_id_map_type::const_iterator i = used_id_map.begin();
		const used_id_map_type::const_iterator e = used_id_map.end();
		for ( ; i!=e; i++) {
			write_value(f, i->first);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not recursive, manager will call this once.  
	Since enum-members are merely strings, we just read them in as 
	strings.  
 */
void
enum_datatype_def::load_object(const persistent_object_manager& m, istream& f) {
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, parent);
	definition_base::load_object_base(m, f);

	// can't use scopespace because enum_member is not persistent
//	scopespace::load_object_base(m, f);
	{
		size_t s;
		read_value(f, s);
		size_t i = 0;
		for ( ; i<s; i++) {
			string temp;
			read_value(f, temp);
			// copied from ::add_member, member function
			excl_ptr<enum_member> member_ptr(new enum_member(temp));
			used_id_map[temp] = member_ptr;
			INVARIANT(!member_ptr);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Enums have no members other than strings.  
	This should never be called.  
 */
void
enum_datatype_def::load_used_id_map_object(excl_ptr<persistent>& o) {
	cerr << "ERROR: not supposed to call "
		"enum_datatype_def::load_used_id_map_object()!" << endl;
	DIE;
}

//=============================================================================
// class user_def_datatype method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// constructor for user defined type
user_def_datatype::user_def_datatype(
		never_ptr<const name_space> o,
		const string& name) :
		definition_base(), 
		datatype_definition_base(), 
		scopespace(),
		sequential_scope(), 
		key(name), 
		parent(o) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
user_def_datatype::~user_def_datatype() {
}

ostream&
user_def_datatype::what(ostream& o) const {
	return o << "user-defined-datatype";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
user_def_datatype::dump(ostream& o) const {
	return what(o) << ": " << key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
user_def_datatype::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
user_def_datatype::get_qualified_name(void) const {
	if (parent)
		return parent->get_qualified_name() + scope + key;
	else return string(scope) + key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
user_def_datatype::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const datatype_definition_base>
user_def_datatype::resolve_canonical_datatype_definition(void) const {
	return never_ptr<const datatype_definition_base>(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const object>
user_def_datatype::lookup_object_here(const string& id) const {
	return scopespace::lookup_object_here(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
user_def_datatype::make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list>& ta) const {
	typedef count_ptr<const fundamental_type_reference>	return_type;
	if (certify_template_arguments(ta)) {
		excl_ptr<const param_expr_list> plp(ta);
		return return_type(
			new data_type_reference(
				never_ptr<const datatype_definition_base>(this), 
				plp));
	} else {
		cerr << "ERROR: failed to make data_type_reference "
			"because template argument types do not match." << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively collects reachable pointers and register them
	with the persistent object manager.  
 */
void
user_def_datatype::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {

// later: template formals
	sequential_scope::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Empty allocator.  
 */
persistent*
user_def_datatype::construct_empty(const int i) {
	return new user_def_datatype(never_ptr<const name_space>(NULL), "");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not recursive, manager will call this once.  
	Note: this must be kept consistent with 
	built_in_datatype_def::write_object.  
 */
void
user_def_datatype::write_object(
		const persistent_object_manager& m, ostream& f) const {
	write_string(f, key);
	m.write_pointer(f, parent);
	definition_base::write_object_base(m, f);
//	write_object_port_formals(m);
	scopespace::write_object_base(m, f);
	// connections and assignments
	sequential_scope::write_object_base(m, f);
	// body
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
user_def_datatype::load_object(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE("user_def_datatype::load_object()");
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, parent);
	definition_base::load_object_base(m, f);
//	load_object_port_formals(m);
	scopespace::load_object_base(m, f);
	// connections and assignments
	sequential_scope::load_object_base(m, f);
	// body
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
user_def_datatype::load_used_id_map_object(excl_ptr<persistent>& o) {
	if (o.is_a<instance_collection_base>()) {
		excl_ptr<instance_collection_base>
			icbp = o.is_a_xfer<instance_collection_base>();
		add_instance(icbp);
		INVARIANT(!icbp);
	} else {
		o->what(cerr << "TO DO: define method for adding ")
			<< " back to user-def data definition." << endl;
	}
}

//=============================================================================
// class datatype_definition_alias method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_definition_alias::datatype_definition_alias(
		const string& n, never_ptr<const scopespace> p) :
		definition_base(), 
		datatype_definition_base(), 
		typedef_base(), 
		key(n), 
		parent(p) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_definition_alias::~datatype_definition_alias() {
	STACKTRACE("~data_def_alias()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
datatype_definition_alias::what(ostream& o) const {
	return o << "datatype-definition-alias";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
datatype_definition_alias::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
datatype_definition_alias::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implementation: recursively resolves canonical definition, 
	without any template arguments.  
	No fear of circular typedefs!!!
 */
never_ptr<const datatype_definition_base>
datatype_definition_alias::resolve_canonical_datatype_definition(void) const {
	return base->get_base_datatype_def()
		->resolve_canonical_datatype_definition();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const fundamental_type_reference>
datatype_definition_alias::get_base_type_ref(void) const {
	return base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
datatype_definition_alias::assign_typedef(
		excl_ptr<const fundamental_type_reference>& f) {
	NEVER_NULL(f);
	base = f.is_a_xfer<const data_type_reference>();
	INVARIANT(!f);
	NEVER_NULL(base);
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
datatype_definition_alias::make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list>& ta) const {
	typedef	count_ptr<const fundamental_type_reference>	return_type;
	if (certify_template_arguments(ta)) {
		excl_ptr<const param_expr_list> plp(ta);
		return return_type(
			new data_type_reference(
				never_ptr<const datatype_definition_alias>(this), 
				plp));
	} else {
		cerr << "ERROR: failed to make data_type_alias type reference "
			"because template argument types do not match." << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
datatype_definition_alias::require_signature_match(
		const never_ptr<const definition_base> d) const {
	cerr << "TO DO: finish datatype_definition_alias::require_signature_match()!" << endl;
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively collects reachable pointers and register them
	with the persistent object manager.  
 */
void
datatype_definition_alias::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	base->collect_transient_info(m);
//	scopespace::collect_transient_info_base(m);	// covers formals?
	definition_base::collect_transient_info_base(m);
	scopespace::collect_transient_info_base(m);
	sequential_scope::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Empty allocator.  
 */
persistent*
datatype_definition_alias::construct_empty(const int i) {
	return new datatype_definition_alias("",
		never_ptr<const scopespace>(NULL));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not recursive, manager will call this once.  
 */
void
datatype_definition_alias::write_object(
		const persistent_object_manager& m, ostream& f) const {
	STACKTRACE("data_def_alias::write_object()");
	write_string(f, key);
	m.write_pointer(f, parent);
	m.write_pointer(f, base);
	definition_base::write_object_base(m, f);
	scopespace::write_object_base(m, f);
	sequential_scope::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
datatype_definition_alias::load_object(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE("data_def_alias::load_object()");
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, parent);
	m.read_pointer(f, base);
	definition_base::load_object_base(m, f);
	scopespace::load_object_base(m, f);
	sequential_scope::load_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Really, typedefs shouldn't have any non-formal members...
 */
void
datatype_definition_alias::load_used_id_map_object(excl_ptr<persistent>& o) {
#if 0
	cerr << "WARNING: didn't expect to call "
		"datatype_definition_alias::load_used_id_map_object()." << endl;
#endif
	if (o.is_a<instance_collection_base>()) {
		excl_ptr<instance_collection_base>
			icbp = o.is_a_xfer<instance_collection_base>();
		add_instance(icbp);
		INVARIANT(!icbp);
	} else {
		o->what(cerr << "TO DO: define method for adding ")
			<< " back to datatype typedef." << endl;
	}
}

//=============================================================================
// class process_definition_base method definitions

#if 0
process_definition_base::process_definition_base() :
		definition_base() {
}

process_definition_base::~process_definition_base() { }
#endif

excl_ptr<definition_base>
process_definition_base::make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const {
	return excl_ptr<definition_base>(
		new process_definition_alias(id, s));
}

//=============================================================================
// class process_definition method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructor for a process definition symbol table entry.  
 */
process_definition::process_definition(
		never_ptr<const name_space> o, 
		const string& s) :
		definition_base(), 
		process_definition_base(),
		scopespace(),
		sequential_scope(), 
		key(s), 
		parent(o), 
		port_formals_list(), 
		port_formals_map() {
	// fill me in...
	// NEVER_NULL(o);		// no: because of partial reconstruction
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_definition::~process_definition() {
	// fill me in...
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_definition::what(ostream& o) const {
	return o << "process-definition";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Spill contents of the used_id_map.
	\param o the output stream.
	\return the same output stream.
 */
ostream&
process_definition::dump(ostream& o) const {
	definition_base::dump(o);	// dump template signature first
	const indent __proc_indent__(o);
	// now dump ports
	{
		o << auto_indent << "(" << endl;
		{
			const indent __indent__(o);
			port_formals_list_type::const_iterator
				i = port_formals_list.begin();
			const port_formals_list_type::const_iterator
				e = port_formals_list.end();
			for ( ; i!=e; i++) {
				(*i)->dump(o << auto_indent) << endl;
			}
		}
		o << auto_indent << ")" << endl;
	}

	// now dump rest of contents
//	list<never_ptr<const ...> > bin;		// later sort
	o << auto_indent <<
		"In definition \"" << key << "\", we have: {" << endl;
	{	// begin indent level
		const indent __indent__(o);
		used_id_map_type::const_iterator
			i = used_id_map.begin();
		const used_id_map_type::const_iterator
			e = used_id_map.end();
		for ( ; i!=e; i++) {
			// pair_dump?
			o << auto_indent << i->first << " = ";
			// i->second->what(o) << endl;	// 1 level for now
			i->second->dump(o) << endl;
		}
	}	// end indent scope
	return o << auto_indent << "}" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
process_definition::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
process_definition::get_qualified_name(void) const {
	return parent->get_qualified_name() + scope + key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
process_definition::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const object>
process_definition::lookup_object_here(const string& s) const {
	return scopespace::lookup_object_here(s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Override's definition_base's port formal lookup.  
	\return pointer to port's instantiation if found, else NULL.  
 */
never_ptr<const instance_collection_base>
process_definition::lookup_port_formal(const string& id) const {
	return static_cast<const port_formals_map_type&>(port_formals_map)[id];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Validates a list of objects (instance references) against
	the port formal specification.  
	\return true if type checks (is conservative).
 */
bool
process_definition::certify_port_actuals(const object_list& ol) const {
#if 0
	cerr << "process_definition::certify_port_actuals(): FINISH ME!"
		<< endl;
#endif
	const size_t num_formals = port_formals_list.size();
	const size_t num_actuals = ol.size();
	if (port_formals_list.size() != ol.size()) {
		cerr << "Number of port actuals (" << num_actuals <<
			") doesn\'t match the number of port formals (" <<
			num_formals << ").  ERROR!  " << endl;
		return false;
	}
	object_list::const_iterator
		a_iter = ol.begin();
	port_formals_list_type::const_iterator
		f_iter = port_formals_list.begin();
	const port_formals_list_type::const_iterator
		f_end = port_formals_list.end();
	size_t i = 1;
	for ( ; f_iter!=f_end; f_iter++, a_iter++, i++) {
		const count_ptr<const object> a_obj(*a_iter);
		if (a_obj) {
			const count_ptr<const instance_reference_base>
				a_iref(a_obj.is_a<const instance_reference_base>());
			const never_ptr<const instance_collection_base>
				f_inst(*f_iter);
			// FINISH ME
			const count_ptr<const instance_reference_base>
				f_iref(f_inst->make_instance_reference());
			if (!f_iref->may_be_type_equivalent(*a_iref)) {
				cerr << "ERROR: actual instance reference "
					<< i << " of port connection "
					"doesn\'t match the formal type/size."
					<< endl << "\tgot: ";
				a_iref->dump_type_size(cerr);
				f_iref->dump_type_size(
					cerr << ", expected: ") << endl;
				return false;
			}
			// else continue checking
		}
		// else is NULL, no connection to check, just continue
	}
	// if we've made it here, then no errors!
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
process_definition::make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list>& ta) const {
	typedef count_ptr<const fundamental_type_reference>	return_type;
	if (certify_template_arguments(ta)) {
		excl_ptr<const param_expr_list> plp(ta);
		return return_type(new process_type_reference(
				never_ptr<const process_definition>(this),
				plp));
	} else {
		cerr << "ERROR: failed to make process_type_reference "
			"because template argument types do not match." << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a port formal instance to this process definition.  
 */
never_ptr<const instance_collection_base>
process_definition::add_port_formal(
		const never_ptr<instantiation_statement> f, 
		const token_identifier& id) {
	NEVER_NULL(f);
	INVARIANT(!f.is_a<param_instantiation_statement>());
	// check and make sure identifier wasn't repeated in formal list!
	{
	const never_ptr<const object>
	probe(lookup_object_here(id));
	if (probe) {
		probe->what(cerr << " already taken as a ") << " ERROR!";
		return never_ptr<const instance_collection_base>(NULL);
	}
	}

	const never_ptr<const instance_collection_base>
		pf(add_instance(f, id));
	NEVER_NULL(pf);
	INVARIANT(pf->get_name() == id);

	{
	// since we already checked used_id_map, there cannot be a repeat
	// in the port_formals_list!
	port_formals_list.push_back(pf);
	port_formals_map[id] = pf;
	INVARIANT(lookup_port_formal(id));
	}

	return pf;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks that template formals set and port formals set 
	are equivalent.  
	Equivalence should be commutative?
	Question: what do we do about template parameters with defaults?
	Need to update grammar: only allow defaults in the definition.  
	\param d the definition to check against.  
	\return true if equivalent, else false.  
 */
bool
process_definition::require_signature_match(
		const never_ptr<const definition_base> d) const {
	NEVER_NULL(d);
	const never_ptr<const process_definition>
		pd(d.is_a<const process_definition>());
	if (!pd) {
		cerr << "ERROR: definition " << d->get_name() <<
			" is not even a process!" << endl;
		return false;
	}
	// check for name match
	if (key != pd->get_name()) {
		cerr << "ERROR: names " << key << " and " << d->get_name() <<
			" don\'t even match!" << endl;
		return false;
	}
	// check for owner-namespace match
	if (parent != pd->parent) {
		cerr << "ERROR: definition owner namespaces don\'t match: "
			<< endl << "\tgot: " << parent->get_qualified_name()
			<< " and " << pd->parent->get_qualified_name() << endl;
		return false;
	}
	// check for template formal list match (in order)
	if (!equivalent_template_formals(pd)) {
		cerr << "ERROR: template formals do not match!  " << endl;
		return false;
	}
	// check for port formal list match (in order)
	if (!equivalent_port_formals(pd)) {
		return false;
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Port formals are equivalent if their order of instantiations
	matches exactly, type, size, and even name.  
 */
bool
process_definition::equivalent_port_formals(
		const never_ptr<const process_definition> p) const {
	NEVER_NULL(p);
	const port_formals_list_type& pports = p->port_formals_list;
	if (port_formals_list.size() != pports.size()) {
		cerr << "ERROR: number of port formal parameters "
			"doesn\'t match!" << endl;
		return false;
	}
	port_formals_list_type::const_iterator
		i = port_formals_list.begin();
	port_formals_list_type::const_iterator
		j = pports.begin();
	for ( ; i!=port_formals_list.end() && j!=pports.end(); i++, j++) {
		const never_ptr<const instance_collection_base> ipf(*i);
		const never_ptr<const instance_collection_base> jpf(*j);
		NEVER_NULL(ipf);
		NEVER_NULL(jpf);
		if (!ipf->port_formal_equivalent(jpf)) {
			// descriptive error message, please
			cerr << "ERROR: port formals do not match!" << endl;
			return false;
		}
	}
	INVARIANT(i == port_formals_list.end() && j == pports.end());
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively collects reachable pointers and register them
	with the persistent object manager.  
 */
void
process_definition::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	// no need to visit template formals, port formals, separately, 
	// b/c they're all registered in the used_id_map.  
	scopespace::collect_transient_info_base(m);
	sequential_scope::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Temporary allocation.  
 */
persistent*
process_definition::construct_empty(const int i) {
	return new process_definition(never_ptr<const name_space>(NULL), "");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not recursive, manager will call this once.  
 */
void
process_definition::write_object(
		const persistent_object_manager& m, ostream& f) const {
	write_string(f, key);
	m.write_pointer(f, parent);
	definition_base::write_object_base(m, f);
	write_object_port_formals(m, f);
	scopespace::write_object_base(m, f);
	// connections and assignments
	sequential_scope::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_definition::load_object(
		const persistent_object_manager& m, istream& f) {
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, parent);
	definition_base::load_object_base(m, f);
	load_object_port_formals(m, f);
	scopespace::load_object_base(m, f);
	// connections and assignments
	sequential_scope::load_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_definition::load_used_id_map_object(excl_ptr<persistent>& o) {
	if (o.is_a<instance_collection_base>()) {
		excl_ptr<instance_collection_base>
			icbp = o.is_a_xfer<instance_collection_base>();
		add_instance(icbp);
		INVARIANT(!icbp);
	} else {
		o->what(cerr << "TO DO: define method for adding ")
			<< " back to process definition." << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Port formals will need to be in list order.
	Just write out the list, the hash_qmap is redundant.  
 */
void
process_definition::write_object_port_formals(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer_list(f, port_formals_list);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Template formals are loaded in list order.
	Remember that the redundant hash_map also needs to be reconstructed.  
	Another method will add the entries to the corresponding
	used_id_map where appropriate.  
 */
void
process_definition::load_object_port_formals(
		const persistent_object_manager& m, istream& f) {
//	istream& f = m.lookup_read_buffer(this);
	m.read_pointer_list(f, port_formals_list);
	// then copy list into hash_map to synchronize
	port_formals_list_type::const_iterator
		iter = port_formals_list.begin();
	const port_formals_list_type::const_iterator
		end = port_formals_list.end();
	for ( ; iter!=end; iter++) {
		const port_formals_value_type inst_ptr = *iter;
		NEVER_NULL(inst_ptr);
		m.load_object_once(const_cast<instance_collection_base*>(
			&*inst_ptr));
		port_formals_map[inst_ptr->get_name()] = inst_ptr;
	}
}

//=============================================================================
// class process_definition_alias method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
process_definition_alias::process_definition_alias() :
		definition_base(), 
		process_definition_base(), 
		typedef_base(), 
		key(), 
		parent(NULL), base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_definition_alias::process_definition_alias(const string& n, 
		never_ptr<const scopespace> p) :
		definition_base(), 
		process_definition_base(), 
		typedef_base(), 
		key(n), 
		parent(p), base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_definition_alias::~process_definition_alias() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_definition_alias::what(ostream& o) const {
	return o << "process-definition-alias";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
process_definition_alias::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
process_definition_alias::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const fundamental_type_reference>
process_definition_alias::get_base_type_ref(void) const {
	return base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns a complete type reference (possibly using formal parameters)
	to this typedef.  
	Can only be done once, for obvious reasons.  
 */
bool
process_definition_alias::assign_typedef(
		excl_ptr<const fundamental_type_reference>& f) {
	NEVER_NULL(f);
	base = f.is_a_xfer<const process_type_reference>();
	NEVER_NULL(base);
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
process_definition_alias::make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list>& ta) const {
	typedef	count_ptr<const fundamental_type_reference>	return_type;
	if (certify_template_arguments(ta)) {
		excl_ptr<const param_expr_list> plp(ta);
		return return_type(
			new process_type_reference(
				never_ptr<const process_definition_alias>(this), 
				plp));
	} else {
		cerr << "ERROR: failed to make process_definition_alias "
			"type reference because template argument types "
			"do not match." << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively collects reachable pointers and register them
	with the persistent object manager.  
 */
void
process_definition_alias::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	base->collect_transient_info(m);
	definition_base::collect_transient_info_base(m);
	scopespace::collect_transient_info_base(m);	// covers formals?
	sequential_scope::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Empty allocator.  
 */
persistent*
process_definition_alias::construct_empty(const int i) {
	return new process_definition_alias();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not recursive, manager will call this once.  
 */
void
process_definition_alias::write_object(
		const persistent_object_manager& m, ostream& f) const {
	write_string(f, key);
	m.write_pointer(f, parent);
	m.write_pointer(f, base);
	definition_base::write_object_base(m, f);
	scopespace::write_object_base(m, f);
	sequential_scope::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_definition_alias::load_object(
		const persistent_object_manager& m, istream& f) {
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, parent);
	m.read_pointer(f, base);
	definition_base::load_object_base(m, f);
	scopespace::load_object_base(m, f);
	sequential_scope::load_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Really, typedefs shouldn't have any non-formal members...
 */
void
process_definition_alias::load_used_id_map_object(excl_ptr<persistent>& o) {
#if 0
	cerr << "WARNING: didn't expect to call "
		"process_definition_alias::load_used_id_map_object()." << endl;
#endif
	if (o.is_a<instance_collection_base>()) {
		excl_ptr<instance_collection_base>
			icbp = o.is_a_xfer<instance_collection_base>();
		add_instance(icbp);
		INVARIANT(!icbp);
	} else {
		o->what(cerr << "TO DO: define method for adding ")
			<< " back to process typedef." << endl;
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

STATIC_TRACE_END("object-definition")

#endif	// __ART_OBJECT_DEFINITION_CC__

