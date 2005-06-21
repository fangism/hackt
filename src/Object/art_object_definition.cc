/**
	\file "Object/art_object_definition.cc"
	Method definitions for definition-related classes.  
 	$Id: art_object_definition.cc,v 1.50 2005/06/21 21:26:34 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_DEFINITION_CC__
#define	__OBJECT_ART_OBJECT_DEFINITION_CC__

#define ENABLE_STACKTRACE		0

//=============================================================================
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <exception>
#include <iostream>
#include <functional>

#include "AST/art_parser_delim.h"
#include "AST/art_parser_token_string.h"

#include "util/STL/list.tcc"
#include "util/hash_qmap.tcc"
#include "util/hash_specializations.h"		// substitute for the following

#include "Object/art_object_definition.h"
#include "Object/art_object_definition_chan.h"
#include "Object/art_object_definition_proc.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_instance.h"
#include "Object/art_object_instance_param.h"
#include "Object/art_object_inst_ref_base.h"
#include "Object/art_object_inst_stmt.h"
#include "Object/art_object_inst_stmt_data.h"
#include "Object/art_object_expr.h"		// for dynamic_param_expr_list
#include "Object/art_object_expr_param_ref.h"
#include "Object/art_object_type_hash.h"

#include "util/memory/count_ptr.tcc"
#include "util/indent.h"
#include "util/binders.h"
#include "util/compose.h"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.tcc"

namespace util {
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::user_def_chan, USER_DEF_CHAN_DEFINITION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_definition_alias, CHANNEL_TYPEDEF_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_datatype_def, ENUM_DEFINITION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::user_def_datatype, USER_DEF_DATA_DEFINITION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::datatype_definition_alias, DATA_TYPEDEF_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_definition, PROCESS_DEFINITION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_definition_alias, PROCESS_TYPEDEF_TYPE_KEY, 0)
}	// end namespace util

//=============================================================================
namespace ART {
namespace entity {
using std::_Select2nd;
#include "util/using_ostream.h"
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
		persistent(), object(), 
		template_formals(), 
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
	template_formals.dump(o);
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
#if 0
UNVEIL LATER
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only looks up the identifier in the set of template formals.  
 */
never_ptr<const param_instance_collection>
definition_base::lookup_template_formal(const string& id) const {
	return template_formals.lookup_template_formal(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the (1-indexed) position of the referenced parameter 
		in the list if found, else 0 if not found.
 */
size_t
definition_base::lookup_template_formal_position(const string& id) const {
	return template_formals.lookup_template_formal_position(id);
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
good_bool
definition_base::check_null_template_argument(void) const {
	STACKTRACE("definition_base::check_null_template_argument()");
	return template_formals.check_null_template_argument();
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
	return template_formals.equivalent_template_formals(
		d->template_formals);
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
good_bool
definition_base::certify_template_arguments(
		const never_ptr<dynamic_param_expr_list> ta) const {
	return template_formals.certify_template_arguments(ta);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default: return false (if unimplemented)
	Temporarily prints an error message.  
 */
good_bool
definition_base::certify_port_actuals(const checked_refs_type&) const {
	cerr << "Default definition_base::certify_port_actuals() = false."
		<< endl;
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: replace with pair of parameter lists
	Prerequisiste for calling this: must satisfy
		check_null_template_arguments.  
	\returns a list of default parameter expressions.  
 */
excl_ptr<dynamic_param_expr_list>
definition_base::make_default_template_arguments(void) const {
	return template_formals.make_default_template_arguments();
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
definition_base::add_strict_template_formal(
		const never_ptr<instantiation_statement_base> i, 
		const token_identifier& id) {
	STACKTRACE("definition_base::add_strict_template_formal()");
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

	template_formals.add_strict_template_formal(pf);

	// sanity check
	INVARIANT(lookup_template_formal(id));
	// later return a never_ptr<>
	return pf;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Same as add_strict_template_formal, except distinguished as
	a relaxed template formal parameter.  
 */
never_ptr<const instance_collection_base>
definition_base::add_relaxed_template_formal(
		const never_ptr<instantiation_statement_base> i, 
		const token_identifier& id) {
	STACKTRACE("definition_base::add_relaxed_template_formal()");
	typedef	never_ptr<const instance_collection_base>	return_type;
	{
	const never_ptr<const object>
		probe(lookup_object_here(id));
	if (probe) {
		probe->what(cerr << " already taken as a ") << " ERROR!";
		return return_type(NULL);
	} 
	}
	scopespace* ss = IS_A(scopespace*, this);
	NEVER_NULL(ss);
	const never_ptr<const param_instance_collection>
		pf(ss->add_instance(i, id).is_a<const param_instance_collection>());
	NEVER_NULL(pf);
	INVARIANT(pf->get_name() == id);	// consistency check
	template_formals.add_relaxed_template_formal(pf);
	INVARIANT(lookup_template_formal(id));
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
		const never_ptr<instantiation_statement_base> f, 
		const token_identifier& i) {
	DIE;
	return never_ptr<const instance_collection_base>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper for consistent interface.
 */
void
definition_base::collect_transient_info_base(
		persistent_object_manager& m) const {
	template_formals.collect_transient_info_base(m);
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
	template_formals_manager::write_object_base_fake(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
definition_base::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	write_value(o, defined);
	template_formals.write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
definition_base::load_object_base(
		const persistent_object_manager& m, istream& f) {
	read_value(f, defined);
	template_formals.load_object_base(m, f);
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
	template_formals.dump(o) << " = ";
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
	if (certify_template_arguments(ta).good) {
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

/// Private empty constructor.  
user_def_chan::user_def_chan() :
		definition_base(), 
		channel_definition_base(), 
		scopespace(),
		sequential_scope(), 
		key(), parent(), 
#if 0
		datatype_list(), 
#else
		base_chan_type_ref(), 
#endif
		port_formals() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
user_def_chan::user_def_chan(const never_ptr<const name_space> o, 
		const string& name) :
		definition_base(), 
		channel_definition_base(), 
		scopespace(),
		sequential_scope(), 
		key(name), parent(o), 
#if 0
		datatype_list(), 
#else
		base_chan_type_ref(), 
#endif
		port_formals() {
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
	definition_base::dump(o);	// dump template signature first
	INDENT_SECTION(o);

	o << endl;
	// the list of datatype(s) carried by this channel
#if 0
	{
		o << auto_indent << "chan (" << endl;
		{
		const indent __indent__(o);
		datatype_list_type::const_iterator
			i(datatype_list.begin());
		const datatype_list_type::const_iterator
			e(datatype_list.end());
		for ( ; i!=e; i++) {
			(*i)->dump(o << auto_indent) << endl;
		}
		}
		o << auto_indent << ")" << endl;
	}
#else
	base_chan_type_ref->dump_long(o << auto_indent);
#endif
	// now dump ports
	port_formals.dump(o);

	// now dump rest of contents
//	list<never_ptr<const ...> > bin;		// later sort
	o << auto_indent <<
		"In channel definition \"" << key << "\", we have: {" << endl;
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
		// TODO: send and recv bodies
	}	// end indent scope
	return o << auto_indent << "}" << endl;
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
#if 0
void
user_def_chan::add_datatype(const count_ptr<const data_type_reference>& dtr) {
	INVARIANT(dtr);
	datatype_list.push_back(dtr);
}
#else
void
user_def_chan::attach_base_channel_type(
		const count_ptr<const builtin_channel_type_reference>& bc) {
	NEVER_NULL(bc);
	INVARIANT(!base_chan_type_ref);
	base_chan_type_ref = bc;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/**
	Ripped off from process_definition's.
	Last touched: 2005-05-25.
 */
never_ptr<const instance_collection_base>
user_def_chan::add_port_formal(const never_ptr<instantiation_statement_base> f, 
		const token_identifier& id) {
	typedef	never_ptr<const instance_collection_base>	return_type;
	NEVER_NULL(f);
	INVARIANT(f.is_a<data_instantiation_statement>());
	// check and make sure identifier wasn't repeated in formal list!
	{
	const never_ptr<const object>
		probe(lookup_object_here(id));
	if (probe) {
		probe->what(cerr << " already taken as a ") << " ERROR!";
		return return_type(NULL);
	}
	}

	const return_type pf(add_instance(f, id));
	NEVER_NULL(pf);
	INVARIANT(pf->get_name() == id);

	// since we already checked used_id_map, there cannot be a repeat
	// in the port_formals_list!
	port_formals.add_port_formal(pf);
	INVARIANT(lookup_port_formal(id));
	return pf;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Override's definition_base's port formal lookup.  
	\return pointer to port's instantiation if found, else NULL.  
 */
never_ptr<const instance_collection_base>
user_def_chan::lookup_port_formal(const string& id) const {
	return port_formals.lookup_port_formal(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/**
	Wrapped call to port formal's certifier.  
 */
good_bool
user_def_chan::certify_port_actuals(const checked_refs_type& cr) const {
	return port_formals.certify_port_actuals(cr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void
user_def_chan::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	definition_base::collect_transient_info_base(m);
	scopespace::collect_transient_info_base(m);
	// recursively visit members...
#if 0
	m.collect_pointer_list(datatype_list);
#else
	base_chan_type_ref->collect_transient_info(m);
#endif
#if 0
	// unnecessary, pointers covered by scopespace already
	port_formals.collect_transient_info_base(m);
#endif
	sequential_scope::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/**
	Not recursive, manager will call this once.  
 */
void
user_def_chan::write_object(
		const persistent_object_manager& m, ostream& f) const {
	write_string(f, key);
	m.write_pointer(f, parent);
	definition_base::write_object_base(m, f);
	scopespace::write_object_base(m, f);
#if 0
	m.write_pointer_list(f, datatype_list);
#else
	m.write_pointer(f, base_chan_type_ref);
#endif
	port_formals.write_object_base(m, f);
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
	scopespace::load_object_base(m, f);
#if 0
	m.read_pointer_list(f, datatype_list);
#else
	m.read_pointer(f, base_chan_type_ref);
#endif
	port_formals.load_object_base(m, f);
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
/// Private empty constructor.
channel_definition_alias::channel_definition_alias() :
		definition_base(), 
		channel_definition_base(), 
		typedef_base(), 
		key(), parent(), base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_definition_alias::channel_definition_alias(
		const string& n, const never_ptr<const scopespace> p) :
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
	if (certify_template_arguments(ta).good) {
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

// built-in definitions (int) only use strict template formals so far
	template_formals.add_strict_template_formal(pf);
//	template_formals.add_relaxed_template_formal(pf);

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
/// Private empty constructor.  
enum_datatype_def::enum_datatype_def() :
		definition_base(), 
		datatype_definition_base(), 
		scopespace(), 
		key(), 
		parent() {
}

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
good_bool
enum_datatype_def::require_signature_match(
		const never_ptr<const definition_base> d) const {
	NEVER_NULL(d);
	INVARIANT(key == d->get_name());
	const never_ptr<const enum_datatype_def>
		ed(d.is_a<const enum_datatype_def>());
	if (ed) {
		// only names need to match...
		// no other signature information!  easy.
		return good_bool(true);
	} else {
		// class type doesn't even match!  report error.
		d->what(cerr << key << " is already declared as a ")
			<< " but is being redeclared as a ";
		what(cerr) << "  ERROR!  ";
		return good_bool(false);
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
/// private empty constructor
user_def_datatype::user_def_datatype() :
		definition_base(), 
		datatype_definition_base(), 
		scopespace(),
		sequential_scope(), 
		key(), 
		parent() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// constructor for user defined type
user_def_datatype::user_def_datatype(
		const never_ptr<const name_space> o,
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
	if (certify_template_arguments(ta).good) {
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
/// private empty constructor
datatype_definition_alias::datatype_definition_alias() :
		definition_base(), 
		datatype_definition_base(), 
		typedef_base(), 
		key(), 
		parent() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_definition_alias::datatype_definition_alias(
		const string& n, const never_ptr<const scopespace> p) :
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
	if (certify_template_arguments(ta).good) {
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
good_bool
datatype_definition_alias::require_signature_match(
		const never_ptr<const definition_base> d) const {
	cerr << "TO DO: finish datatype_definition_alias::require_signature_match()!" << endl;
	return good_bool(false);
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
	Private empty constructor.
 */
process_definition::process_definition() :
		definition_base(), 
		process_definition_base(),
		scopespace(),
		sequential_scope(), 
		key(), 
		parent(), 
		port_formals(), 
		prs(), chp() {
	// no null check: because of partial reconstruction
}

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
		port_formals(), 
		prs(), chp() {
	// fill me in...
	NEVER_NULL(o);
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
//	STACKTRACE_VERBOSE;
	definition_base::dump(o);	// dump template signature first
	// unique ID not working with INDENT_SECTION marco... :(
	const indent _ind__(o);	
	// now dump ports
	port_formals.dump(o);

	// now dump rest of contents
//	list<never_ptr<const ...> > bin;		// later sort
	o << auto_indent <<
		"In definition \"" << key << "\", we have: {" << endl;
	{	// begin indent level
		INDENT_SECTION(o);
		used_id_map_type::const_iterator
			i(used_id_map.begin());
		const used_id_map_type::const_iterator
			e(used_id_map.end());
		for ( ; i!=e; i++) {
			// pair_dump?
			o << auto_indent << i->first << " = ";
			// i->second->what(o) << endl;	// 1 level for now
			i->second->dump(o) << endl;
		}
		// PRS
		if (!prs.empty()) {
			o << auto_indent << "prs:" << endl;
			prs.dump(o);	// << endl;
		}
		// PRS
		if (!chp.empty()) {
			o << auto_indent << "chp:" << endl;
			chp.dump(o << auto_indent) << endl;
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
	return port_formals.lookup_port_formal(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Validates a list of objects (instance references) against
	the port formal specification.  
	\return true if type checks (is conservative).
 */
good_bool
process_definition::certify_port_actuals(const checked_refs_type& ol) const {
	return port_formals.certify_port_actuals(ol);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
process_definition::make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list>& ta) const {
	typedef count_ptr<const fundamental_type_reference>	return_type;
	if (certify_template_arguments(ta).good) {
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
		const never_ptr<instantiation_statement_base> f, 
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

	// since we already checked used_id_map, there cannot be a repeat
	// in the port_formals_list!
	port_formals.add_port_formal(pf);
	INVARIANT(lookup_port_formal(id));
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
good_bool
process_definition::require_signature_match(
		const never_ptr<const definition_base> d) const {
	NEVER_NULL(d);
	const never_ptr<const process_definition>
		pd(d.is_a<const process_definition>());
	if (!pd) {
		cerr << "ERROR: definition " << d->get_name() <<
			" is not even a process!" << endl;
		return good_bool(false);
	}
	// check for name match
	if (key != pd->get_name()) {
		cerr << "ERROR: names " << key << " and " << d->get_name() <<
			" don\'t even match!" << endl;
		return good_bool(false);
	}
	// check for owner-namespace match
	if (parent != pd->parent) {
		cerr << "ERROR: definition owner namespaces don\'t match: "
			<< endl << "\tgot: " << parent->get_qualified_name()
			<< " and " << pd->parent->get_qualified_name() << endl;
		return good_bool(false);
	}
	// check for template formal list match (in order)
	if (!equivalent_template_formals(pd)) {
		cerr << "ERROR: template formals do not match!  " << endl;
		return good_bool(false);
	}
	// check for port formal list match (in order)
	if (!port_formals.equivalent_port_formals(pd->port_formals)) {
		return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers a production rule with this process definition.  
	Automatically expands complements.  
	\param r pointer to newly created and checked production rule.  
		Transfers ownership to the definition.  
	\post r is NULL, no longer owned by the passer.  
 */
void
process_definition::add_production_rule(excl_ptr<PRS::rule>& r) {
	NEVER_NULL(r);
	r->check();		// paranoia
	excl_ptr<PRS::rule> cmpl = r->expand_complement();
	prs.push_back(PRS::rule_set::value_type());
	prs.back() = r;
	INVARIANT(!r);
	if (cmpl) {
		prs.push_back(PRS::rule_set::value_type());
		prs.back() = cmpl;
		INVARIANT(!cmpl);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped call to prs's compact_referenes.  
 */
void
process_definition::compact_prs_references(void) {
	prs.compact_references();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped call to prs's expand_complements.  
 */
void
process_definition::expand_prs_complements(void) {
	prs.expand_complements();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_definition::add_concurrent_chp_body(const count_ptr<CHP::action>& a) {
	NEVER_NULL(a);
	chp.push_back(a);
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
#if 0
	port_formals.collect_transient_info_base(m);	// is a NO-OP, actually
	// pointers already covered by scopespace::collect
#endif
	// PRS
	prs.collect_transient_info_base(m);
	chp.collect_transient_info_base(m);
}
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
	port_formals.write_object_base(m, f);
	scopespace::write_object_base(m, f);
	// connections and assignments
	sequential_scope::write_object_base(m, f);
	// PRS
	prs.write_object_base(m, f);
	chp.write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_definition::load_object(
		const persistent_object_manager& m, istream& f) {
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, parent);
	definition_base::load_object_base(m, f);
	port_formals.load_object_base(m, f);
	scopespace::load_object_base(m, f);
	// connections and assignments
	sequential_scope::load_object_base(m, f);
	// PRS
	prs.load_object_base(m, f);
	chp.load_object_base(m, f);
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
		const never_ptr<const scopespace> p) :
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
	if (certify_template_arguments(ta).good) {
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

DEFAULT_STATIC_TRACE_END

#endif	// __OBJECT_ART_OBJECT_DEFINITION_CC__

