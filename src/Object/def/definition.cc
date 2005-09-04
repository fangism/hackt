/**
	\file "Object/def/definition.cc"
	Method definitions for definition-related classes.  
	This file used to be "Object/art_object_definition.cc".
 	$Id: definition.cc,v 1.3.2.14 2005/09/04 01:58:08 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_DEFINITION_CC__
#define	__OBJECT_ART_OBJECT_DEFINITION_CC__

#define ENABLE_STACKTRACE		0
#define	STACKTRACE_DUMPS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE

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

#include "Object/def/param_definition.h"
#include "Object/def/user_def_datatype.h"
#include "Object/def/built_in_datatype_def.h"
#include "Object/def/datatype_definition_alias.h"
#include "Object/def/enum_datatype_def.h"
#include "Object/def/user_def_chan.h"
#include "Object/def/channel_definition_alias.h"
#include "Object/def/process_definition.h"
#include "Object/def/process_definition_alias.h"
#include "Object/type/data_type_reference.h"
#include "Object/type/builtin_channel_type_reference.h"
#include "Object/type/channel_type_reference.h"
#include "Object/type/process_type_reference.h"
#include "Object/inst/param_value_collection.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/unroll/param_instantiation_statement_base.h"
#include "Object/unroll/instantiation_statement.h"
#include "Object/unroll/datatype_instantiation_statement.h"
#include "Object/unroll/unroll_context.h"
#include "Object/expr/param_expr_list.h"
#include "Object/expr/meta_range_list.h"
#include "Object/persistent_type_hash.h"
#include "Object/common/namespace.h"
#include "Object/traits/pint_traits.h"
#include "Object/traits/pbool_traits.h"
#include "Object/type/canonical_generic_chan_type.h"

#include "common/ICE.h"
#include "common/TODO.h"

#include "util/memory/count_ptr.tcc"
#include "util/indent.h"
#include "util/binders.h"
#include "util/compose.h"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.tcc"

#if STACKTRACE_DUMPS
#define	STACKTRACE_DUMP(x)	STACKTRACE(x)
#else
#define	STACKTRACE_DUMP(x)
#endif

//=============================================================================
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
	STACKTRACE_DTOR("~definition_base()");
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
	STACKTRACE_DUMP(__PRETTY_FUNCTION__);
	const string key = get_key();
	what(o) << ((defined) ? " (defined) " : " (declared) ") << key;
	return template_formals.dump(o);
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
	Only looks up the identifier in the set of template formals.  
 */
never_ptr<const param_value_collection>
definition_base::lookup_template_formal(const string& id) const {
	return template_formals.lookup_template_formal(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
definition_base::probe_relaxed_template_formal(const string& id) const {
	return template_formals.probe_relaxed_template_formal(id);
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
	By default, definitions don't have port formals, unless 
	defined otherwise.  
	Overridden in children classes.  
	\return 0, signaling port formal not found.  
 */
size_t
definition_base::lookup_port_formal_position(
		const instance_collection_base&) const {
	STACKTRACE_VERBOSE;
	return 0;
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
#if 0
string
definition_base::get_name(void) const {
	return get_key();
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
definition_base::get_qualified_name(void) const {
	const string& key(get_key());
	const never_ptr<const scopespace> parent(get_parent());
	if (parent)
		return parent->get_qualified_name() +scope +key;
	else return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
definition_base::dump_qualified_name(ostream& o) const {
	const string& key(get_key());
	const never_ptr<const scopespace> parent(get_parent());
	if (parent)
		parent->dump_qualified_name(o) << scope;
	return o << key;
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
definition_base::certify_template_arguments(template_actuals& ta) const {
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
	Prerequisiste for calling this: must satisfy
		check_null_template_arguments.  
	\returns a list of default parameter expressions.  
 */
definition_base::make_type_ptr_type
definition_base::make_default_template_arguments(void) const {
	return template_formals.make_default_template_arguments();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper for making a type reference with default template args.  
 */
definition_base::type_ref_ptr_type
definition_base::make_fundamental_type_reference(void) const {
	// assign, not copy construct!
	return make_fundamental_type_reference(
		make_default_template_arguments());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	DO ME NOW!
	Adds an instantiation to the current definition's scope, and 
	also registers it in the list of template formals for 
	template argument checking.  
	What if template formal is an array, or collective?
	TO DO: convert to pointer-classes...
	\param f needs to be a param_value_collection... what about array?
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
	const never_ptr<const param_value_collection>
		pf(ss->add_instance(i, id).is_a<const param_value_collection>());
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
	const never_ptr<const param_value_collection>
		pf(ss->add_instance(i, id).is_a<const param_value_collection>());
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
#if PHYSICAL_PORTS
never_ptr<const physical_instance_collection>
#else
never_ptr<const instance_collection_base>
#endif
definition_base::add_port_formal(
		const never_ptr<instantiation_statement_base> f, 
		const token_identifier& i) {
	ICE_NEVER_CALL(cerr);
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
	STACKTRACE_DTOR("~typedef_base()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
typedef_base::get_qualified_name(void) const {
	return definition_base::get_qualified_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
typedef_base::dump_qualified_name(ostream& o) const {
	return definition_base::dump_qualified_name(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Description dump works for all alias types.  
 */
ostream&
typedef_base::dump(ostream& o) const {
	STACKTRACE_DUMP(__PRETTY_FUNCTION__);
	what(o) << ": " << get_key();
	template_formals.dump(o) << endl;
	{
		// pretty-printing
		INDENT_SECTION(o);
		o << auto_indent << "= ";
		get_base_type_ref()->dump(o);
	}
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
	STACKTRACE_DTOR("~datatype_definition_base()");
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
definition_base::type_ref_ptr_type
datatype_definition_base::make_fundamental_type_reference(
		make_type_arg_type ta) const {
	typedef type_ref_ptr_type	return_type;
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
definition_base::type_ref_ptr_type
channel_definition_base::make_fundamental_type_reference(
		make_type_arg_type ta) const {
	typedef type_ref_ptr_type	return_type;
	if (certify_template_arguments(ta).good) {
		return return_type(new channel_type_reference(
			never_ptr<const channel_definition_base>(this), ta));
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
		base_chan_type_ref(), 
		port_formals(), 
		send_chp(), recv_chp() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
user_def_chan::user_def_chan(const never_ptr<const name_space> o, 
		const string& name) :
		definition_base(), 
		channel_definition_base(), 
		scopespace(),
		sequential_scope(), 
		key(name), parent(o), 
		base_chan_type_ref(), 
		port_formals(), 
		send_chp(), recv_chp() {
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
	STACKTRACE_DUMP(__PRETTY_FUNCTION__);
	definition_base::dump(o) << endl;	// dump template signature first
	INDENT_SECTION(o);
	// the list of datatype(s) carried by this channel
	base_chan_type_ref->dump_long(o << auto_indent) << endl;
	// now dump ports
	port_formals.dump(o << auto_indent) << endl;

	// now dump rest of contents
//	list<never_ptr<const ...> > bin;		// later sort
	o << auto_indent <<
		"In channel definition \"" << key << "\", we have: {" << endl;
	{	// begin indent level
		INDENT_SECTION(o);
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
		// CHP
		if (!send_chp.empty()) {
			o << auto_indent << "send-CHP:" << endl;
			send_chp.dump(o << auto_indent) << endl;
		}
		if (!recv_chp.empty()) {
			o << auto_indent << "recv-CHP:" << endl;
			recv_chp.dump(o << auto_indent) << endl;
		}
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
ostream&
user_def_chan::dump_qualified_name(ostream& o) const {
	if (parent)
		parent->dump_qualified_name(o);
	return o << scope << key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
user_def_chan::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
user_def_chan::commit_arity(void) {
	// nothing until a footoprint_manager is added
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const object>
user_def_chan::lookup_object_here(const string& id) const {
	return scopespace::lookup_object_here(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void
user_def_chan::attach_base_channel_type(
		const count_ptr<const builtin_channel_type_reference>& bc) {
	NEVER_NULL(bc);
	INVARIANT(!base_chan_type_ref);
	base_chan_type_ref = bc;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/**
	Ripped off from process_definition's.
	Last touched: 2005-05-25.
 */
#if PHYSICAL_PORTS
never_ptr<const physical_instance_collection>
#else
never_ptr<const instance_collection_base>
#endif
user_def_chan::add_port_formal(const never_ptr<instantiation_statement_base> f, 
		const token_identifier& id) {
#if PHYSICAL_PORTS
	typedef	never_ptr<const physical_instance_collection>	return_type;
#else
	typedef	never_ptr<const instance_collection_base>	return_type;
#endif
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

#if PHYSICAL_PORTS
	const return_type pf(add_instance(f, id)
		.is_a<const physical_instance_collection>());
#else
	const return_type pf(add_instance(f, id));
#endif
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
	\return 1-indexed position of port-formal, else 0 if not found.  
 */
size_t
user_def_chan::lookup_port_formal_position(
		const instance_collection_base& i) const {
	STACKTRACE_VERBOSE;
	return port_formals.lookup_port_formal_position(i.get_name());
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
canonical_generic_chan_type
user_def_chan::make_canonical_type(const template_actuals& a) const {
	typedef	canonical_generic_chan_type	return_type;
	return return_type(never_ptr<const this_type>(this), a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
count_ptr<const channel_type_reference_base>
user_def_chan::make_canonical_fundamental_type_reference(
		const template_actuals& a) const {
	return make_fundamental_type_reference(a)
		.is_a<const channel_type_reference_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void
user_def_chan::register_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	// nothing until this has a footprint manager
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
good_bool
user_def_chan::unroll_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	// nothing until this has a footprint manager
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
good_bool
user_def_chan::create_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	// nothing until this has a footprint manager
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void
user_def_chan::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	definition_base::collect_transient_info_base(m);
	scopespace::collect_transient_info_base(m);
	// recursively visit members...
	base_chan_type_ref->collect_transient_info(m);
#if 0
	// unnecessary, pointers covered by scopespace already
	port_formals.collect_transient_info_base(m);
#endif
	sequential_scope::collect_transient_info_base(m);
	send_chp.collect_transient_info_base(m);
	recv_chp.collect_transient_info_base(m);
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
	m.write_pointer(f, base_chan_type_ref);
	port_formals.write_object_base(m, f);
	// connections and assignments
	sequential_scope::write_object_base(m, f);
	send_chp.write_object_base(m, f);
	recv_chp.write_object_base(m, f);
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
	m.read_pointer(f, base_chan_type_ref);
	port_formals.load_object_base(m, f);
	// connections and assignments
	sequential_scope::load_object_base(m, f);
	send_chp.load_object_base(m, f);
	recv_chp.load_object_base(m, f);
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
	Canonicalizes channel type to built-in or user-defined.  
 */
canonical_generic_chan_type
channel_definition_alias::make_canonical_type(const template_actuals& a) const {
	const template_actuals& ba(base->get_template_params());
	const template_actuals
		ta(ba.transform_template_actuals(a, template_formals));
	return base->get_base_chan_def()->make_canonical_type(ta);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const channel_type_reference_base>
channel_definition_alias::make_canonical_fundamental_type_reference(
		const template_actuals& a) const {
	const template_actuals& ba(base->get_template_params());
	const template_actuals
		ta(ba.transform_template_actuals(a, template_formals));
	return base->get_base_chan_def()
		->make_canonical_fundamental_type_reference(ta);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_definition_alias::register_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
channel_definition_alias::unroll_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
channel_definition_alias::create_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	ICE_NEVER_CALL(cerr);
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
		excl_ptr<param_value_collection> p) :
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
	STACKTRACE_DTOR("~built_in_datatype_def()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
built_in_datatype_def::what(ostream& o) const {
	return o << "built-in-datatype-def";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
built_in_datatype_def::dump(ostream& o) const {
	STACKTRACE_DUMP(__PRETTY_FUNCTION__);
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
ostream&
built_in_datatype_def::dump_qualified_name(ostream& o) const {
	return datatype_definition_base::dump_qualified_name(o);
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
definition_base::type_ref_ptr_type
built_in_datatype_def::make_fundamental_type_reference(
		make_type_arg_type ta) const {
	typedef	type_ref_ptr_type	return_type;
	if (certify_template_arguments(ta).good) {
		return return_type(new data_type_reference(
			never_ptr<const built_in_datatype_def>(this), ta));
	} else {
		cerr << "ERROR: failed to make built_in_data_type_reference "
			"because template argument types do not match." << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This definition is already canonical.  
 */
canonical_generic_datatype
built_in_datatype_def::make_canonical_type(const template_actuals& a) const {
	return canonical_generic_datatype(never_ptr<const this_type>(this), a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Q: how much special case handling does this require?
	For now try the easiest thing, and fix later.  
 */
count_ptr<const data_type_reference>
built_in_datatype_def::make_canonical_fundamental_type_reference(
		const template_actuals& a) const {
	// INVARIANT(a.is_constant());	// NOT true
	return make_fundamental_type_reference(a)
		.is_a<const data_type_reference>();
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
	const never_ptr<const param_value_collection>
		pf(f.is_a<const param_value_collection>());
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
void
built_in_datatype_def::register_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	// nothing, built-in types have no footprint manater
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
built_in_datatype_def::unroll_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	// nothing, built-in types have no footprint manater
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
built_in_datatype_def::create_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	// nothing, built-in types have no footprint manater
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Leeching off of datatype definition.  
	Will be handled specially (replaced) by data_type_reference.  
 */
void
built_in_datatype_def::collect_transient_info(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("built_in_data::collect_transients()");
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
	STACKTRACE_PERSISTENT("built_in_data::write_object()");
	static const port_formals_manager port_formals;
	static const CHP::action_sequence fake_chp;
	write_string(f, key);
	// use bogus parent pointer
	m.write_pointer(f, never_ptr<const name_space>(NULL));
	// bogus template and port formals
	definition_base::write_object_base_fake(m, f);	// is empty
	scopespace::write_object_base_fake(m, f);
	m.write_pointer(f, count_ptr<const data_type_reference>(NULL));
	port_formals.write_object_base(m, f);
	// connections and assignments
	sequential_scope::write_object_base_fake(m, f);
	// fake definition body
	fake_chp.write_object_base(m, f);
	fake_chp.write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
built_in_datatype_def::load_object(
		const persistent_object_manager& m, istream& f) {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
built_in_datatype_def::load_used_id_map_object(excl_ptr<persistent>& o) {
	ICE_NEVER_CALL(cerr);
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
	ICE_NEVER_CALL(cerr);
	return excl_ptr<definition_base>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This implementation is deprecated, no longer constructing
	param_type_reference, always using built-in types
	declared in "Object/art_object_{pint,pbool}_traits.h",
	initialized in "Object/art_built_ins.cc".
	\param ta template arguments are never used.  
 */
definition_base::type_ref_ptr_type
built_in_param_def::make_fundamental_type_reference(
		make_type_arg_type ta) const {
	ICE_NEVER_CALL(cerr);
	return type_ref_ptr_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_METHODS_DUMMY_IMPLEMENTATION(built_in_param_def)

//=============================================================================
// class enum_member method definitions

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
	STACKTRACE_DUMP(__PRETTY_FUNCTION__);
	what(o) << ": " << key;
	if (defined) {
		INDENT_SECTION(o);
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
ostream&
enum_datatype_def::dump_qualified_name(ostream& o) const {
	if (parent)
		parent->dump_qualified_name(o);
	return o << scope << key;
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
definition_base::type_ref_ptr_type
enum_datatype_def::make_fundamental_type_reference(
		make_type_arg_type ta) const {
	typedef type_ref_ptr_type	return_type;
	INVARIANT(!ta);
	return return_type(new data_type_reference(
		never_ptr<const datatype_definition_base>(this)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_generic_datatype
enum_datatype_def::make_canonical_type(const template_actuals& a) const {
	INVARIANT(!a);
	return canonical_generic_datatype(never_ptr<const this_type>(this), a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const data_type_reference>
enum_datatype_def::make_canonical_fundamental_type_reference(
		const template_actuals& a) const {
	INVARIANT(!a);
	return make_fundamental_type_reference(a)
		.is_a<const data_type_reference>();
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
void
enum_datatype_def::register_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	// nothing, doesn't have a footprint manager
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
enum_datatype_def::unroll_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	// nothing, doesn't have a footprint manager
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
enum_datatype_def::create_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	// nothing, doesn't have a footprint manager
	return good_bool(true);
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
	ICE_NEVER_CALL(cerr);
}

//=============================================================================
// class user_def_datatype method definitions

/// private empty constructor
user_def_datatype::user_def_datatype() :
		definition_base(), 
		datatype_definition_base(), 
		scopespace(),
		sequential_scope(), 
		key(), 
		parent(), 
		base_type(), 
		port_formals(), 
		set_chp(), get_chp() {
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
		parent(o), 
		base_type(), 
		port_formals(), 
		set_chp(), get_chp() {
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
	STACKTRACE_DUMP(__PRETTY_FUNCTION__);
//	return what(o) << ": " << key;
	definition_base::dump(o) << endl;	// dump template signature first
	INDENT_SECTION(o);
	base_type->dump(o << auto_indent) << endl;
	// now dump ports
	port_formals.dump(o << auto_indent) << endl;

	// now dump rest of contents
//	list<never_ptr<const ...> > bin;		// later sort
	o << auto_indent <<
		"In datatype definition \"" << key << "\", we have: {" << endl;
	{	// begin indent level
		INDENT_SECTION(o);
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
		// CHP
		if (!set_chp.empty()) {
			o << auto_indent << "set-CHP:" << endl;
			set_chp.dump(o << auto_indent) << endl;
		}
		if (!get_chp.empty()) {
			o << auto_indent << "get-CHP:" << endl;
			get_chp.dump(o << auto_indent) << endl;
		}
	}	// end indent scope
	return o << auto_indent << "}" << endl;
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
ostream&
user_def_datatype::dump_qualified_name(ostream& o) const {
	if (parent)
		parent->dump_qualified_name(o);
	return o << scope << key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
user_def_datatype::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
user_def_datatype::commit_arity(void) {
	// nothing, until a footprint_manager is added
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
void
user_def_datatype::attach_base_data_type(
		const count_ptr<const data_type_reference>& d) {
	INVARIANT(!base_type);
	NEVER_NULL(d);
	base_type = d;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/**
	Shamelessly ripped off from user_def_chan's, 
	Ripped off from process_definition's.
 */
#if PHYSICAL_PORTS
never_ptr<const physical_instance_collection>
#else
never_ptr<const instance_collection_base>
#endif
user_def_datatype::add_port_formal(
		const never_ptr<instantiation_statement_base> f, 
		const token_identifier& id) {
#if PHYSICAL_PORTS
	typedef	never_ptr<const physical_instance_collection>	return_type;
#else
	typedef	never_ptr<const instance_collection_base>	return_type;
#endif
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

#if PHYSICAL_PORTS
	const return_type pf(add_instance(f, id)
		.is_a<const physical_instance_collection>());
#else
	const return_type pf(add_instance(f, id));
#endif
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
user_def_datatype::lookup_port_formal(const string& id) const {
	return port_formals.lookup_port_formal(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/**
	\return 1-indexed position of port-formal, else 0 if not found.  
 */
size_t
user_def_datatype::lookup_port_formal_position(
		const instance_collection_base& i) const {
	STACKTRACE_VERBOSE;
	return port_formals.lookup_port_formal_position(i.get_name());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
definition_base::type_ref_ptr_type
user_def_datatype::make_fundamental_type_reference(
		make_type_arg_type ta) const {
	typedef type_ref_ptr_type	return_type;
	if (certify_template_arguments(ta).good) {
		return return_type(new data_type_reference(
			never_ptr<const datatype_definition_base>(this), ta));
	} else {
		cerr << "ERROR: failed to make data_type_reference "
			"because template argument types do not match." << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_generic_datatype
user_def_datatype::make_canonical_type(const template_actuals& a) const {
	INVARIANT(a.is_constant());
	return canonical_generic_datatype(never_ptr<const this_type>(this), a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Fairly straightforward.  
	TODO: have make_fundamental_type_reference call this instead.  
 */
count_ptr<const data_type_reference>
user_def_datatype::make_canonical_fundamental_type_reference(
		const template_actuals& a) const {
	INVARIANT(a.is_constant());
	return make_fundamental_type_reference(a)
		.is_a<const data_type_reference>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
user_def_datatype::register_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	// doesnt't have a gootprint manager .. yet
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
user_def_datatype::unroll_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	// doesnt't have a gootprint manager .. yet
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
user_def_datatype::create_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	// doesnt't have a gootprint manager .. yet
	return good_bool(true);
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
	STACKTRACE_PERSISTENT("user_def_datatype::collect_transient()");
// later: template formals
	sequential_scope::collect_transient_info_base(m);
	scopespace::collect_transient_info_base(m);
	if (base_type)
		base_type->collect_transient_info(m);
	// no need to call port_formals.collect_transient_info, 
	// already covered by scopespace::collect...
	set_chp.collect_transient_info_base(m);
	get_chp.collect_transient_info_base(m);
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
	STACKTRACE_PERSISTENT("user_def_datatype::write_object()");
	write_string(f, key);
	m.write_pointer(f, parent);
	definition_base::write_object_base(m, f);
//	write_object_port_formals(m);
	scopespace::write_object_base(m, f);
	m.write_pointer(f, base_type);
	port_formals.write_object_base(m, f);
	// connections and assignments
	sequential_scope::write_object_base(m, f);
	// body
	set_chp.write_object_base(m, f);
	get_chp.write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
user_def_datatype::load_object(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("user_def_datatype::load_object()");
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, parent);
	definition_base::load_object_base(m, f);
//	load_object_port_formals(m);
	scopespace::load_object_base(m, f);
	m.read_pointer(f, base_type);
	port_formals.load_object_base(m, f);
	// connections and assignments
	sequential_scope::load_object_base(m, f);
	// body
	set_chp.load_object_base(m, f);
	get_chp.load_object_base(m, f);
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
	STACKTRACE_DTOR("~data_def_alias()");
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
definition_base::type_ref_ptr_type
datatype_definition_alias::make_fundamental_type_reference(
		make_type_arg_type ta) const {
	typedef	type_ref_ptr_type	return_type;
	if (certify_template_arguments(ta).good) {
		return return_type(new data_type_reference(
			never_ptr<const datatype_definition_alias>(this), ta));
	} else {
		cerr << "ERROR: failed to make data_type_alias type reference "
			"because template argument types do not match." << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_generic_datatype
datatype_definition_alias::make_canonical_type(
		const template_actuals& a) const {
	const template_actuals& ba(base->get_template_params());
	const template_actuals
		ta(ba.transform_template_actuals(a, template_formals));
	return base->get_base_datatype_def()->make_canonical_type(ta);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: error handling
 */
count_ptr<const data_type_reference>
datatype_definition_alias::make_canonical_fundamental_type_reference(
		const template_actuals& a) const {
	const template_actuals& ba(base->get_template_params());
	const template_actuals
		ta(ba.transform_template_actuals(a, template_formals));
	return base->get_base_datatype_def()
		->make_canonical_fundamental_type_reference(ta);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
datatype_definition_alias::require_signature_match(
		const never_ptr<const definition_base> d) const {
	FINISH_ME(Fang);
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
datatype_definition_alias::register_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
datatype_definition_alias::unroll_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	ICE_NEVER_CALL(cerr);
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
datatype_definition_alias::create_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	ICE_NEVER_CALL(cerr);
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
	STACKTRACE_PERSISTENT("data_def_alias::write_object()");
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
	STACKTRACE_PERSISTENT("data_def_alias::load_object()");
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
		prs(), chp(), 
		footprint_map() {
	// no null check: because of partial reconstruction
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructor for a process definition symbol table entry.  
	TODO: when is the footprint_map's arity set? after adding 
		template-formal parameter.  
 */
process_definition::process_definition(
		const never_ptr<const name_space> o, 
		const string& s) :
		definition_base(), 
		process_definition_base(),
		scopespace(),
		sequential_scope(), 
		key(s), 
		parent(o), 
		port_formals(), 
		prs(), chp(), 
		footprint_map() {
	// fill me in...
	NEVER_NULL(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_definition::~process_definition() { }

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
	TODO: dump the footprint map.  
 */
ostream&
process_definition::dump(ostream& o) const {
	STACKTRACE_DUMP(__PRETTY_FUNCTION__);
//	STACKTRACE_VERBOSE;
	definition_base::dump(o);	// dump template signature first
	// unique ID not working with INDENT_SECTION marco... :(
	INDENT_SECTION(o);	
	// now dump ports
	port_formals.dump(o) << endl;

	// now dump rest of contents
//	list<never_ptr<const ...> > bin;		// later sort
	o << auto_indent <<
		"In definition \"" << key << "\", we have: {" << endl;
	{	// begin indent level
		INDENT_SECTION(o);
		// we dump ports even if body is undefined
		used_id_map_type::const_iterator i(used_id_map.begin());
		const used_id_map_type::const_iterator e(used_id_map.end());
		for ( ; i!=e; i++) {
			// pair_dump?
			o << auto_indent << i->first << " = ";
			// i->second->what(o) << endl;	// 1 level for now
			i->second->dump(o) << endl;
		}
		if (defined) {
			// PRS
			if (!prs.empty()) {
				o << auto_indent << "prs:" << endl;
				prs.dump(o);	// << endl;
			}
			// CHP
			if (!chp.empty()) {
				o << auto_indent << "chp:" << endl;
				chp.dump(o << auto_indent) << endl;
			}
			if (footprint_map.size()) {
				footprint_map.dump(o << auto_indent) << endl;
			}
		}
	}	// end indent scope
	return o << auto_indent << '}' << endl;
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
ostream&
process_definition::dump_qualified_name(ostream& o) const {
	return parent->dump_qualified_name(o) << scope << key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
process_definition::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_definition::commit_arity(void) {
	footprint_map.set_arity(template_formals.arity());
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/**
	\return 1-indexed position of port-formal, else 0 if not found.  
 */
size_t
process_definition::lookup_port_formal_position(
		const instance_collection_base& i) const {
	STACKTRACE_VERBOSE;
#if 0
	dump(cerr) << endl;
#endif
	return port_formals.lookup_port_formal_position(i.get_name());
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
definition_base::type_ref_ptr_type
process_definition::make_fundamental_type_reference(
		make_type_arg_type ta) const {
	typedef type_ref_ptr_type	return_type;
	if (certify_template_arguments(ta).good) {
		return return_type(new process_type_reference(
			never_ptr<const process_definition>(this), ta));
	} else {
		cerr << "ERROR: failed to make process_type_reference "
			"because template argument types do not match." << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_process_type
process_definition::make_canonical_type(const template_actuals& a) const {
	return canonical_process_type(never_ptr<const this_type>(this), a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	We are at an actual base definition of a process, 
	just return the fundamental type. which is canonical.  
	TODO: implement here, have make_fundamental call this.  
 */
count_ptr<const process_type_reference>
process_definition::make_canonical_fundamental_type_reference(
		const template_actuals& a) const {
	return make_fundamental_type_reference(a)
		.is_a<const process_type_reference>();;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a port formal instance to this process definition.  
 */
#if PHYSICAL_PORTS
never_ptr<const physical_instance_collection>
#else
never_ptr<const instance_collection_base>
#endif
process_definition::add_port_formal(
		const never_ptr<instantiation_statement_base> f, 
		const token_identifier& id) {
#if PHYSICAL_PORTS
	typedef	never_ptr<const physical_instance_collection>	return_type;
#else
	typedef	never_ptr<const instance_collection_base>	return_type;
#endif
	NEVER_NULL(f);
	INVARIANT(!f.is_a<param_instantiation_statement_base>());
	// check and make sure identifier wasn't repeated in formal list!
	{
	const never_ptr<const object>
	probe(lookup_object_here(id));
	if (probe) {
		probe->what(cerr << " already taken as a ") << " ERROR!";
		return return_type(NULL);
	}
	}

#if PHYSICAL_PORTS
	const return_type pf(add_instance(f, id)
		.is_a<const physical_instance_collection>());
#else
	const return_type pf(add_instance(f, id));
#endif
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
const footprint&
process_definition::get_footprint(
		const count_ptr<const const_param_expr_list>& p) const {
	if (p) {
		INVARIANT(p->size() == footprint_map.arity());
		return footprint_map[*p];
	} else {
		return footprint_map.only();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre the arity of the footprint_manager must be set.  
 */
void
process_definition::register_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	STACKTRACE_VERBOSE;
	// this has a fooprint manager
	if (p) {
		INVARIANT(p->size() == footprint_map.arity());
		footprint& f = footprint_map[*p];
		f.import_scopespace(*this);
	} else {
		INVARIANT(!footprint_map.arity());
		// register the only map only if it doesn't exist yet
		// otherwise will force a comparison of null pointers
		if (!footprint_map.size()) {
			// create the one-and-only entry
			footprint& f = footprint_map.only();
			f.import_scopespace(*this);
		}
		// else it was already registered
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: catch mutual recursion of types?
 */
good_bool
process_definition::unroll_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	// unroll using the footprint manager
	STACKTRACE_VERBOSE;
if (defined) {
	footprint* f;
	if (p) {
		INVARIANT(p->size() == footprint_map.arity());
		f = &footprint_map[*p];
	} else {
		INVARIANT(!footprint_map.arity());
		f = &footprint_map.only();
	}
	if (!f->is_unrolled()) {
		const canonical_type_base canonical_params(p);
		const template_actuals
			canonical_actuals(canonical_params.get_template_params(
				template_formals.num_strict_formals()));
		const canonical_process_type
			cpt(make_canonical_type(canonical_actuals));
		const unroll_context
			c(canonical_actuals, template_formals, f);
		if (sequential_scope::unroll(c).good) {
			f->mark_unrolled();
		} else {
			// already have partial error message
			// cpt.dump(cerr << "Instantiated from ") << endl;
			return good_bool(false);
		}
	}
	return good_bool(true);
} else {
	cerr << "ERROR: cannot unroll incomplete process type " <<
			get_qualified_name() << endl;
	// parent should print: "instantiated from here"
	return good_bool(false);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create the definition footprint fr a complete process_type.  
 */
good_bool
process_definition::create_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	STACKTRACE_VERBOSE;
if (defined) {
	footprint* f;
	if (p) {
		INVARIANT(p->size() == footprint_map.arity());
		f = &footprint_map[*p];
	} else {
		INVARIANT(!footprint_map.arity());
		f = &footprint_map.only();
	}
	// will automatically unroll first if not already unrolled
	if (!f->is_unrolled() && !unroll_complete_type(p).good) {
		// already have error message
		return good_bool(false);
	}
	if (!f->is_created()) {
		const canonical_type_base canonical_params(p);
		const template_actuals
			canonical_actuals(canonical_params.get_template_params(
				template_formals.num_strict_formals()));
		const canonical_process_type
			cpt(make_canonical_type(canonical_actuals));
#if ENABLE_STACKTRACE
		cpt.dump(STACKTRACE_INDENT << "proc type: ") << endl;
#endif
		const unroll_context
			c(canonical_actuals, template_formals, f);
#if CREATE_DEPENDENT_TYPES_FIRST
		// TODO: need to create dependent types first
		// to replay internal aliases!
		if (!f->create_dependent_types().good) {
			// error message
			return good_bool(false);
		}
#endif
		if (sequential_scope::create_unique(c, *f).good) {
			f->evaluate_port_aliases(port_formals);
			f->mark_created();
		} else {
			// already have partial error message
			// cpt.dump(cerr << "Instantiated from ") << endl;
			return good_bool(false);
		}
	}
	return good_bool(true);
} else {
	cerr << "ERROR: cannot create incomplete process type " <<
			get_qualified_name() << endl;
	// parent should print: "instantiated from here"
	return good_bool(false);
}
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
	footprint_map.collect_transient_info_base(m);
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
	footprint_map.write_object_base(m, f);
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
	footprint_map.load_object_base(m, f);
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
definition_base::type_ref_ptr_type
process_definition_alias::make_fundamental_type_reference(
		make_type_arg_type ta) const {
	typedef	type_ref_ptr_type	return_type;
	if (certify_template_arguments(ta).good) {
		return return_type(new process_type_reference(
			never_ptr<const process_definition_alias>(this), ta));
	} else {
		cerr << "ERROR: failed to make process_definition_alias "
			"type reference because template argument types "
			"do not match." << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_process_type
process_definition_alias::make_canonical_type(const template_actuals& a) const {
	const template_actuals& ba(base->get_template_params());
	const template_actuals
		ta(ba.transform_template_actuals(a, template_formals));
	return base->get_base_proc_def()->make_canonical_type(ta);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Work to be done here... substitute expressions with local context.
 */
count_ptr<const process_type_reference>
process_definition_alias::make_canonical_fundamental_type_reference(
		const template_actuals& a) const {
#if 0
	typedef	count_ptr<const process_type_reference>	return_type;
	cerr << "Fang. write process_definition_alias::make_canonical_fundamental_type_reference()!" << endl;
	return return_type(NULL);
#else
	const template_actuals& ba(base->get_template_params());
	const template_actuals
		ta(ba.transform_template_actuals(a, template_formals));
	return base->get_base_proc_def()->make_canonical_fundamental_type_reference(ta);
#endif
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

