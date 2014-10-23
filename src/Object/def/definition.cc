/**
	\file "Object/def/definition.cc"
	Method definitions for definition-related classes.  
	This file used to be "Object/art_object_definition.cc".
 	$Id: definition.cc,v 1.54 2011/04/02 01:45:55 fang Exp $
 */

#define ENABLE_STACKTRACE		0
#define	STACKTRACE_DUMPS		(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_DESTRUCTORS		(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS		(0 && ENABLE_STACKTRACE)

//=============================================================================
#include "util/static_trace.hh"
DEFAULT_STATIC_TRACE_BEGIN

#include <exception>
#include <iostream>
#include <functional>
#include <list>

#include "AST/delim.hh"
#include "AST/token_string.hh"

#include "util/hash_specializations.hh"		// substitute for the following

#include "Object/def/param_definition.hh"
#include "Object/def/user_def_datatype.hh"
#include "Object/def/built_in_datatype_def.hh"
#include "Object/def/datatype_definition_alias.hh"
#include "Object/def/enum_datatype_def.hh"
#include "Object/def/user_def_chan.hh"
#include "Object/def/channel_definition_alias.hh"
#include "Object/def/process_definition.hh"
#include "Object/def/process_definition_alias.hh"
#include "Object/def/fundamental_channel_footprint.hh"
#include "Object/def/footprint.hh"
#include "Object/module.hh"
#include "Object/type/data_type_reference.hh"
#include "Object/type/builtin_channel_type_reference.hh"
#include "Object/type/channel_type_reference.hh"
#include "Object/type/process_type_reference.hh"
#include "Object/inst/param_value_placeholder.hh"
#include "Object/inst/physical_instance_placeholder.hh"
#include "Object/inst/instance_pool.hh"
#include "Object/inst/state_instance.hh"
#include "Object/unroll/instantiation_statement.hh"
#include "Object/unroll/datatype_instantiation_statement.hh"
#include "Object/unroll/unroll_context.hh"
#include "Object/expr/expr_dump_context.hh"
#include "Object/expr/dynamic_param_expr_list.hh"
#include "Object/expr/meta_range_list.hh"
#include "Object/persistent_type_hash.hh"
#include "Object/common/namespace.hh"
#include "Object/common/dump_flags.hh"
#include "Object/traits/pint_traits.hh"
#include "Object/traits/pbool_traits.hh"
#include "Object/traits/preal_traits.hh"
#include "Object/traits/bool_traits.hh"	// for built_in_definition
#include "Object/traits/int_traits.hh"	// for built_in_definition
#include "Object/traits/enum_traits.hh"	// for type_tag_enum_value
#include "Object/type/canonical_generic_chan_type.hh"
#include "Object/nonmeta_channel_manipulator.hh"
#include "Object/nonmeta_variable.hh"

#include "common/ICE.hh"
#include "common/TODO.hh"

#include "util/memory/count_ptr.tcc"
#include "util/indent.hh"
#include "util/binders.hh"
#include "util/compose.hh"
#include "util/stacktrace.hh"
#include "util/persistent_object_manager.tcc"

#if STACKTRACE_DUMPS
#define	STACKTRACE_DUMP(x)	STACKTRACE(x)
#else
#define	STACKTRACE_DUMP(x)
#endif

//=============================================================================
namespace util {
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::user_def_chan, USER_DEF_CHAN_DEFINITION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::channel_definition_alias, CHANNEL_TYPEDEF_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::enum_datatype_def, ENUM_DEFINITION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::user_def_datatype, USER_DEF_DATA_DEFINITION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::datatype_definition_alias, DATA_TYPEDEF_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::process_definition, PROCESS_DEFINITION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::process_definition_alias, PROCESS_TYPEDEF_TYPE_KEY, 0)
}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
using std::_Select2nd;
#include "util/using_ostream.hh"
using parser::scope;
using util::indent;
using util::auto_indent;
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
definition_base::definition_base() :
		persistent(), object(), 
		template_formals(), 
		defined(false) {
	// synchronize template formals with used_id_map
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
	const string key(get_key());
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
#if REQUIRE_DEFINITION_EXPORT
bool
definition_base::is_exported(void) const {
	return exported;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only looks up the identifier in the set of template formals.  
 */
never_ptr<const definition_base::value_placeholder_type>
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
	TODO: in the fah fah future, (template-)dependent names' lookups
		will have to be deferred until instantiation time.  
		But what do we return in that case?
		Some placeholder to allow for instantiation-time
		name-lookup during unrolling and beyond.  
	TODO: how should typedefs handle lookups?
		Should template parameters be referenceable/lookup-able?
		What if the same ID is used in template parameters
		of typedefs and their base definitions?
		How should template parameter members be handled w.r.t.
		typedefs?
	Proposal: when looking up template parameter (if we allow), 
		only the outermost template parameters are visible, 
		while ALL the base type's parameters are masked out.  
		All other non-parameter member lookups will, of course, 
		go directly to the canonical base definition.  
		What do we do when we don't know what we're looking up?
		Silly: Always know what you're trying to lookup!
		Consequence: a lookup function for each type:
			definition, namespace, instance, value
	See documentation on template parameter references at the end
		of the templates chapter of the languages spec.  
		(added 20060118)
 */
never_ptr<const object>
definition_base::lookup_member(const string& id) const {
#if 1
	// try this to see what breaks: so far, nothing!
	return never_ptr<const object>(NULL);
#else
	return lookup_template_formal(id);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is needed to skip over parameter members.
	We only return non-parameter members.  
	Q: would lookup_member have the same effect as 
		getting the scopespace and __lookup_member?
 */
never_ptr<const object>
definition_base::lookup_nonparameter_member(const string& id) const {
	typedef	never_ptr<const object>	return_type;
	const never_ptr<const scopespace> s(get_scopespace());
	if (s) {
#if PROCESS_DEFINITION_IS_NAMESPACE
		const return_type ret(s->lookup_local(id));
#else
		const return_type ret(s->__lookup_member(id));
#endif
		if (ret && !lookup_template_formal(id)) {
			return ret;
		} 
	}
	return return_type(NULL);
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
	STACKTRACE_VERBOSE;
	return template_formals.check_null_template_argument();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Overrideable default.  
 */
never_ptr<const scopespace>
defintion_base::get_scopespace(void) const {
	return never_ptr<const scopespace>(NULL);
}
#else
/***
	We're going to treat all member-of-scopespace lookups 
	the same.
	Let get_scopespace be pure-virtual.  
	Right now, definitions are derived from scopespace, which means
	that a dynamic_cast will achieve the same thing, 
	however, in the future, we will move away from this
	and use a policy-based containership.  (20060119)
***/
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overrideable default.  
	Every definition type with a port_formals_manager should override.
	TODO: use class_traits to override (pure virtual) automatically.
 */
never_ptr<const port_formals_manager>
definition_base::get_port_formals_manager(void) const {
	return never_ptr<const port_formals_manager>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A default lookup that always returns NULL.  
	Overridden in process_definition.  
 */
never_ptr<const definition_base::placeholder_base_type>
definition_base::lookup_port_formal(const string& id) const {
	const never_ptr<const port_formals_manager>
		pfm(get_port_formals_manager());
	if (pfm) {
		return pfm->lookup_port_formal(id);
	} else {
		return never_ptr<const placeholder_base_type>(NULL);
	}
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
		const instance_placeholder_type& i) const {
	STACKTRACE_VERBOSE;
	const never_ptr<const port_formals_manager>
		pfm(get_port_formals_manager());
	if (pfm) {
		return pfm->lookup_port_formal_position(i.get_name());
	} else {
		return port_formals_manager::INVALID_POSITION;
	}
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
	if (parent && !parent->is_global_namespace())
		return parent->get_qualified_name() +scope +key;
	else return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Parent may be a definition_base in the case of a typedef.
	In the future, there will be support for nested typedefs.  
	(Probably not nested definitions.)
 */
ostream&
definition_base::dump_qualified_name(ostream& o, const dump_flags& df) const {
if (df.show_definition_owner) {
	const string& key(get_key());
	const never_ptr<const scopespace> parent(get_parent());
	if (parent &&
#if PROCESS_DEFINITION_IS_NAMESPACE
		!parent.is_a<const module>()
#else
		!parent->is_global_namespace()
#endif
		) {
		parent->dump_qualified_name(o, df) << scope;
	}
	return o << key;
} else	return o;
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
#if 0
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
#endif

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
	TODO: convert to pointer-classes...
	\param f needs to be a param_value_collection... what about array?
		need to be non-const? storing to hash_map_of_ptr...
		must be modifiable for used_id_map
 */
never_ptr<const definition_base::value_placeholder_type>
definition_base::add_strict_template_formal(
		const count_ptr<instantiation_statement_base>& i, 
		const token_identifier& id) {
	STACKTRACE("definition_base::add_strict_template_formal()");
	typedef	never_ptr<const value_placeholder_type>	return_type;
	// const string id(pf->get_name());	// won't have name yet!
	// check and make sure identifier wasn't repeated in formal list!
	{
	const never_ptr<const object>
		probe(lookup_member(id));
	if (probe) {
		probe->what(cerr << id << " already taken as a ") << " ERROR!";
		return return_type(NULL);
	} 
	}

	// since we already checked used_id_map, there cannot be a repeat
	// in the template_formals_list!
	// template_formals_list and _map are strict subsets of used_id_map

	// COMPILE: pf is const, but used_id_map members are not
	// wrap around with object_handle?

	// this construction is ugly, TODO: define clean interface
	scopespace* ss = IS_A(scopespace*, this);
	NEVER_NULL(ss);
	// this creates and adds to the definition
	// and bi-links statement to collection
	const return_type
		pf(ss->add_instance(i, id, false)
			.is_a<const value_placeholder_type>());
		// false -- formals are never conditional
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
never_ptr<const definition_base::value_placeholder_type>
definition_base::add_relaxed_template_formal(
		const count_ptr<instantiation_statement_base>& i, 
		const token_identifier& id) {
	STACKTRACE("definition_base::add_relaxed_template_formal()");
	typedef	never_ptr<const value_placeholder_type>	return_type;
	{
	const never_ptr<const object>
		probe(lookup_member(id));
	if (probe) {
		probe->what(cerr << id << " already taken as a ") << " ERROR!";
		return return_type(NULL);
	} 
	}
	scopespace* ss = IS_A(scopespace*, this);
	NEVER_NULL(ss);
	const return_type
		pf(ss->add_instance(i, id, false)
			.is_a<const value_placeholder_type>());
		// false -- formals are never conditional
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
never_ptr<const definition_base::instance_placeholder_type>
definition_base::add_port_formal(
		const count_ptr<instantiation_statement_base>&, 
		const token_identifier&) {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Does nothing by default.
	Overridden by subclasses.
 */
good_bool
definition_base::unroll_lang(const unroll_context&) const {
	return good_bool(true);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper for consistent interface.
 */
void
definition_base::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
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
	STACKTRACE_PERSISTENT_VERBOSE;
	write_value(o, fake_defined);
	template_formals_manager::write_object_base_fake(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
definition_base::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	write_value(o, defined);
	template_formals.write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
definition_base::load_object_base(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT_VERBOSE;
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
#if REQUIRE_DEFINITION_EXPORT
bool
typedef_base::is_exported(void) const {
	return get_base_type_ref()->get_base_def()->is_exported();
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
typedef_base::get_qualified_name(void) const {
	return definition_base::get_qualified_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
typedef_base::dump_qualified_name(ostream& o, const dump_flags& df) const {
	return definition_base::dump_qualified_name(o, df);
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
bool
datatype_definition_base::less_ordering(
		const datatype_definition_base& r) const {
	const unsigned char le = get_meta_type();
	const unsigned char re = r.get_meta_type();
	return (le < re) || ((le == re) && (this < &r));
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
meta_type_tag_enum
channel_definition_base::get_meta_type(void) const {
	return META_TYPE_CHANNEL;
}

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
		spec(), 
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
		spec(), 
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
/**
	TODO: dump members alphabetically.
 */
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
	o << auto_indent <<
		"In channel definition \"" << key << "\", we have: {" << endl;
	{
		scopespace::dump_for_definitions(o);
		const expr_dump_context dc(this);
		o << auto_indent << "unroll sequence: " << endl;
		{	INDENT_SECTION(o);
			sequential_scope::dump(o, dc);
		}
		// SPEC
		if (!spec.empty()) {
			o << auto_indent << "spec:" << endl;
			INDENT_SECTION(o);
			const PRS::rule_dump_context rdc(*this);
			spec.dump(o, rdc);	// << endl;
		}
		// CHP
		if (!send_chp.empty()) {
			o << auto_indent << "send-CHP:" << endl;
			INDENT_SECTION(o);
			send_chp.dump(o << auto_indent, dc) << endl;
		}
		if (!recv_chp.empty()) {
			o << auto_indent << "recv-CHP:" << endl;
			INDENT_SECTION(o);
			recv_chp.dump(o << auto_indent, dc) << endl;
		}
		// TODO: dump footprints?
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
	return definition_base::get_qualified_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
user_def_chan::dump_qualified_name(ostream& o, const dump_flags& df) const {
	return definition_base::dump_qualified_name(o, df);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
user_def_chan::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
user_def_chan::get_scopespace(void) const {
	return never_ptr<const scopespace>(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
user_def_chan::commit_arity(void) {
	// nothing until a footoprint_manager is added
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const object>
user_def_chan::lookup_member(const string& id) const {
#if PROCESS_DEFINITION_IS_NAMESPACE
	return scopespace::lookup_local(id);
#else
	return scopespace::lookup_member(id);
#endif
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
never_ptr<const definition_base::instance_placeholder_type>
user_def_chan::add_port_formal(
		const count_ptr<instantiation_statement_base>& f, 
		const token_identifier& id) {
	typedef	never_ptr<const instance_placeholder_type>	return_type;
	NEVER_NULL(f);
	INVARIANT(f.is_a<data_instantiation_statement>());
	// check and make sure identifier wasn't repeated in formal list!
	{
	const never_ptr<const object>
#if PROCESS_DEFINITION_IS_NAMESPACE
		probe(lookup_local(id));
#else
		probe(lookup_member(id));
#endif
	if (probe) {
		probe->what(cerr << id << " already taken as a ") << " ERROR!";
		return return_type(NULL);
	}
	}

	const return_type pf(add_instance(f, id, false)
		.is_a<const instance_placeholder_type>());
		// false -- formals are never conditional
	NEVER_NULL(pf);
	INVARIANT(pf->get_name() == id);

	// since we already checked used_id_map, there cannot be a repeat
	// in the port_formals_list!
	port_formals.add_port_formal(pf);
	INVARIANT(lookup_port_formal(id));
	return pf;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const port_formals_manager>
user_def_chan::get_port_formals_manager(void) const {
	return never_ptr<const port_formals_manager>(&port_formals);
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
	return return_type(never_ptr<const this_type>(this), a,
		CHANNEL_TYPE_BIDIRECTIONAL);	// default direction
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
user_def_chan::create_complete_type(
		const count_ptr<const const_param_expr_list>& p, 
		const footprint& top) const {
	// nothing until this has a footprint manager
	// don't forget spec
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void
user_def_chan::collect_transient_info(persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
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
	spec.collect_transient_info_base(m);
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
	STACKTRACE_PERSISTENT_VERBOSE;
	write_string(f, key);
	m.write_pointer(f, parent);
	definition_base::write_object_base(m, f);
	scopespace::write_object_base(m, f);
	m.write_pointer(f, base_chan_type_ref);
	port_formals.write_object_base(m, f);
	// connections and assignments
	sequential_scope::write_object_base(m, f);
	spec.write_object_base(m, f);
	send_chp.write_object_base(m, f);
	recv_chp.write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads in fields from input stream.  
 */
void
user_def_chan::load_object(const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT_VERBOSE;
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, parent);
	definition_base::load_object_base(m, f);
	scopespace::load_object_base(m, f);
	m.read_pointer(f, base_chan_type_ref);
	port_formals.load_object_base(m, f);
	// connections and assignments
	sequential_scope::load_object_base(m, f);
	spec.load_object_base(m, f);
	send_chp.load_object_base(m, f);
	recv_chp.load_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
user_def_chan::load_used_id_map_object(excl_ptr<persistent>& o) {
	STACKTRACE_PERSISTENT_VERBOSE;
	if (o.is_a<placeholder_base_type>()) {
		excl_ptr<placeholder_base_type>
			icbp = o.is_a_xfer<instance_placeholder_base>();
		add_instance(icbp);
		INVARIANT(!icbp);
	} else {
		o->what(cerr << "TODO: define method for adding ")
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
never_ptr<const scopespace>
channel_definition_alias::get_scopespace(void) const {
	return base->get_base_chan_def()->get_scopespace();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const fundamental_type_reference>
channel_definition_alias::get_base_type_ref(void) const {
	return base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const port_formals_manager>
channel_definition_alias::get_port_formals_manager(void) const {
	return base->get_base_chan_def()->get_port_formals_manager();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/**
	Untested.
 */
good_bool
channel_definition_alias::certify_port_actuals(
		const checked_refs_type& cr) const {
	return base->get_base_chan_def()->certify_port_actuals(cr);
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
channel_definition_alias::create_complete_type(
		const count_ptr<const const_param_expr_list>& p, 
		const footprint& top) const {
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
	STACKTRACE_PERSISTENT_VERBOSE;
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
	STACKTRACE_PERSISTENT_VERBOSE;
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
	STACKTRACE_PERSISTENT_VERBOSE;
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
	if (o.is_a<placeholder_base_type>()) {
		excl_ptr<placeholder_base_type>
			icbp = o.is_a_xfer<instance_placeholder_base>();
		add_instance(icbp);
		INVARIANT(!icbp);
	} else {
		o->what(cerr << "TODO: define method for adding ")
			<< " back to channel typedef." << endl;
	}
}

//=============================================================================
// global static definition
#if USE_TOP_DATA_TYPE
/**
	Magic definition placeholder that represents the TOP type for 
	data types.  
 */
const
built_in_datatype_def
top_data_definition(never_ptr<const name_space>(NULL), "DATA_TOP");
#endif

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
#if REQUIRE_DEFINITION_EXPORT
	mark_export();
#endif
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
built_in_datatype_def::dump_qualified_name(ostream& o,
		const dump_flags& df) const {
	return datatype_definition_base::dump_qualified_name(o, df);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
built_in_datatype_def::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
built_in_datatype_def::get_scopespace(void) const {
	return never_ptr<const scopespace>(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const datatype_definition_base>
built_in_datatype_def::resolve_canonical_datatype_definition(void) const {
	return never_ptr<const datatype_definition_base>(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
good_bool
built_in_datatype_def::certify_port_actuals(
		const checked_refs_type& cr) const {
	cerr << "No built-in data types have public ports." << endl;
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: Possibly check that int's width parameter is > 0?
	Or punt check until unroll/instantiation time?
 */
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
never_ptr<const definition_base::value_placeholder_type>
built_in_datatype_def::add_template_formal(
		excl_ptr<value_placeholder_type>& f) {
	STACKTRACE("add_template_formal(excl_ptr<>)");
	const never_ptr<const value_placeholder_type>
		pf(f.is_a<const value_placeholder_type>());
	NEVER_NULL(pf);
	// check and make sure identifier wasn't repeated in formal list!
	const string& id(pf->get_name());
	const never_ptr<const object>
		probe(datatype_definition_base::lookup_member(id));
	if (probe) {
		probe->what(cerr << id << " already taken as a ") << " ERROR!";
		return never_ptr<const value_placeholder_type>(NULL);
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
built_in_datatype_def::create_complete_type(
		const count_ptr<const const_param_expr_list>& p, 
		const footprint& top) const {
	// nothing, built-in types have no footprint manater
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
meta_type_tag_enum
built_in_datatype_def::get_meta_type(void) const {
	if (this == &bool_traits::built_in_definition) {
		// return bool_traits::type_tag_enum_value;
		return META_TYPE_BOOL;
	}
	else if (this == &int_traits::built_in_definition) {
		// return int_traits::type_tag_enum_value;
		return META_TYPE_INT;
	} else {
		// no other built-in definitions at this time
		THROW_EXIT;
		return META_TYPE_NONE;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if BUILTIN_CHANNEL_FOOTPRINTS
/**
	Should we check for int-size limit?  (32 vs. 64)?
 */
void
built_in_datatype_def::count_channel_member(
		fundamental_channel_footprint& f) const {
	if (this == &bool_traits::built_in_definition) {
		++f.size<bool_tag>();
	}
	else if (this == &int_traits::built_in_definition) {
		++f.size<int_tag>();
	} else {
		// no other built-in definitions at this time
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
built_in_datatype_def::dump_channel_field_iterate(ostream& o, 
		channel_data_reader& r) const {
	if (this == &bool_traits::built_in_definition) {
		o << size_t(*r.iter_ref<bool_tag>()++);
	}
	else if (this == &int_traits::built_in_definition) {
		o << *r.iter_ref<int_tag>()++;
	} else {
		THROW_EXIT;
	}
	return o;
}
#endif

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

	We REALLY need to overhaul the definition/type system...
 */
void
built_in_datatype_def::write_object(
		const persistent_object_manager& m, ostream& f) const {
	STACKTRACE_PERSISTENT("built_in_data::write_object()");
	static const port_formals_manager port_formals;
	static const CHP::action_sequence fake_chp;
	static const footprint_manager fake_footprint_map;
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
	fake_footprint_map.write_object_base(m, f);
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
#if REQUIRE_DEFINITION_EXPORT
	mark_export();		// does one ever typedef param types??
#endif
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
meta_type_tag_enum
built_in_param_def::get_meta_type(void) const {
	if (this == &pbool_traits::built_in_definition)
		return META_TYPE_PBOOL;
	if (this == &pint_traits::built_in_definition)
		return META_TYPE_PINT;
	if (this == &preal_traits::built_in_definition)
		return META_TYPE_PREAL;
	else	return META_TYPE_NONE;
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
never_ptr<const scopespace>
built_in_param_def::get_scopespace(void) const {
	return never_ptr<const scopespace>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
good_bool
built_in_param_def::certify_port_actuals(
		const checked_refs_type& cr) const {
	ICE_NEVER_CALL(cerr);
	return good_bool(false);
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
	declared in "Object/traits/{pint,pbool}_traits.h",
	initialized in "Object/traits/class_traits_types.cc".
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
/**
	TODO: sort enum members alphabetically?
 */
ostream&
enum_datatype_def::dump(ostream& o) const {
	STACKTRACE_DUMP(__PRETTY_FUNCTION__);
	what(o) << ": " << key;
	if (defined) {
		INDENT_SECTION(o);
		o << endl << auto_indent << "{ ";
		used_id_map_type::const_iterator i(used_id_map.begin());
		const used_id_map_type::const_iterator e(used_id_map.end());
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
	return definition_base::get_qualified_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
enum_datatype_def::dump_qualified_name(ostream& o, const dump_flags& df) const {
	return definition_base::dump_qualified_name(o, df);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
enum_datatype_def::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Is there really anything in this scopespace?
 */
never_ptr<const scopespace>
enum_datatype_def::get_scopespace(void) const {
	return never_ptr<const scopespace>(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const datatype_definition_base>
enum_datatype_def::resolve_canonical_datatype_definition(void) const {
	return never_ptr<const datatype_definition_base>(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
good_bool
enum_datatype_def::certify_port_actuals(
		const checked_refs_type& cr) const {
	cerr << "Enums do not have ports." << endl;
	return good_bool(false);
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
#if PROCESS_DEFINITION_IS_NAMESPACE
		probe(scopespace::lookup_local(em));
#else
		probe(scopespace::lookup_member(em));
#endif
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
enum_datatype_def::create_complete_type(
		const count_ptr<const const_param_expr_list>& p, 
		const footprint& top) const {
	// nothing, doesn't have a footprint manager
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
meta_type_tag_enum
enum_datatype_def::get_meta_type(void) const {
//	return class_traits<enum_tag>::type_tag_enum_value;
	return META_TYPE_ENUM;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if BUILTIN_CHANNEL_FOOTPRINTS
/**
	Should we check for int-size limit?  (32 vs. 64)?
 */
void
enum_datatype_def::count_channel_member(
		fundamental_channel_footprint& f) const {
	++f.size<enum_tag>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
enum_datatype_def::dump_channel_field_iterate(ostream& o, 
		channel_data_reader& r) const {
	return o << *r.iter_ref<enum_tag>()++;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively collects reachable pointers and register them
	with the persistent object manager.  
 */
void
enum_datatype_def::collect_transient_info(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
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
	STACKTRACE_PERSISTENT_VERBOSE;
	write_string(f, key);
	m.write_pointer(f, parent);
	definition_base::write_object_base(m, f);

	// can't use scopespace because enum_member is not persistent
//	scopespace::write_object_base(m, f);
	{
		const size_t s = used_id_map.size();
		write_value(f, s);
		used_id_map_type::const_iterator i(used_id_map.begin());
		const used_id_map_type::const_iterator e(used_id_map.end());
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
	STACKTRACE_PERSISTENT_VERBOSE;
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
/**
	TODO: dump members alphabetically.
 */
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
	o << auto_indent <<
		"In datatype definition \"" << key << "\", we have: {" << endl;
	{
		scopespace::dump_for_definitions(o);
		const expr_dump_context dc(this);
		o << auto_indent << "unroll sequence: " << endl;
		{	INDENT_SECTION(o);
			sequential_scope::dump(o, dc);
		}
		// CHP
		if (!set_chp.empty()) {
			o << auto_indent << "set-CHP:" << endl;
			INDENT_SECTION(o);
			set_chp.dump(o << auto_indent, dc) << endl;
		}
		if (!get_chp.empty()) {
			o << auto_indent << "get-CHP:" << endl;
			INDENT_SECTION(o);
			get_chp.dump(o << auto_indent, dc) << endl;
		}
		if (footprint_map.size()) {
			footprint_map.dump(o << auto_indent, dc) << endl;
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
	return datatype_definition_base::get_qualified_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
user_def_datatype::dump_qualified_name(ostream& o, const dump_flags& df) const {
	return definition_base::dump_qualified_name(o, df);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
user_def_datatype::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
user_def_datatype::get_scopespace(void) const {
	return never_ptr<const scopespace>(this);
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
user_def_datatype::lookup_member(const string& id) const {
#if PROCESS_DEFINITION_IS_NAMESPACE
	return scopespace::lookup_local(id);
#else
	return scopespace::lookup_member(id);
#endif
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
never_ptr<const definition_base::instance_placeholder_type>
user_def_datatype::add_port_formal(
		const count_ptr<instantiation_statement_base>& f, 
		const token_identifier& id) {
	typedef	never_ptr<const instance_placeholder_type>	return_type;
	NEVER_NULL(f);
	INVARIANT(f.is_a<data_instantiation_statement>());
	// check and make sure identifier wasn't repeated in formal list!
	{
	const never_ptr<const object>
		probe(lookup_member(id));
	if (probe) {
		probe->what(cerr << id << " already taken as a ") << " ERROR!";
		return return_type(NULL);
	}
	}

	const return_type pf(add_instance(f, id, false)
		.is_a<const instance_placeholder_type>());
		// false -- formals are never conditional
	NEVER_NULL(pf);
	INVARIANT(pf->get_name() == id);

	// since we already checked used_id_map, there cannot be a repeat
	// in the port_formals_list!
	port_formals.add_port_formal(pf);
	INVARIANT(lookup_port_formal(id));
	return pf;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const port_formals_manager>
user_def_datatype::get_port_formals_manager(void) const {
	return never_ptr<const port_formals_manager>(&port_formals);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
good_bool
user_def_datatype::certify_port_actuals(
		const checked_refs_type& cr) const {
	return port_formals.certify_port_actuals(cr);
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
/**
	Has a footprint now!
	definition copied verbatim from process_definition::get_footprint
 */
const footprint&
user_def_datatype::get_footprint(
		const count_ptr<const const_param_expr_list>& p) const {
	STACKTRACE_VERBOSE;
	// will create one, if necessary
	// new footprints will not be unrolled until explicitly done
	return *footprint_map.lookup(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	definition copied verbatim from process_definition::register_complete_type
 */
void
user_def_datatype::register_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	STACKTRACE_VERBOSE;
	footprint_map.insert(p, *this);	// uses mutability of footprint_map
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	definition copied verbatim from process_definition::unroll_complete_type
 */
good_bool
user_def_datatype::__unroll_complete_type(
		const count_ptr<const const_param_expr_list>& p, 
		const footprint& top) const {
	STACKTRACE_VERBOSE;
#if ENABLE_DATASTRUCTS
if (defined) {
	footprint* const f = &footprint_map[p];
	{
		const canonical_type_base canonical_params(p);
		const template_actuals
			canonical_actuals(canonical_params.get_template_params(
				template_formals.num_strict_formals()));
		const canonical_user_def_data_type
			cpt(make_canonical_type(canonical_actuals));
		const unroll_context c(f, &top);
		// no parent b/c doing away with lookup of globals, 
		// when we do, need to chain the context with parent...
		if (!template_formals.unroll_formal_parameters(
				c, canonical_actuals).good) {
			cerr << "ERROR: unrolling template formals." << endl;
			return good_bool(false);
		}
#if 0 && ENABLE_STACKTRACE
		STACKTRACE_INDENT << "new context c @ " << &c << endl;
		c.dump(cerr) << endl;
#endif
		if (sequential_scope::unroll(c).good) {
		} else {
			// already have partial error message
			// cpt.dump(cerr << "Instantiated from ") << endl;
			return good_bool(false);
		}
	}
	return good_bool(true);
} else {
	cerr << "ERROR: cannot unroll incomplete data type " <<
			get_qualified_name() << endl;
	// parent should print: "instantiated from here"
	// could print parameters too
	return good_bool(false);
}
#else
	FINISH_ME_EXIT(Fang);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	definition copied verbatim from process_definition::create_complete_type
	TODO: create send/recv CHP after creating dependent types.  
 */
good_bool
user_def_datatype::create_complete_type(
		const count_ptr<const const_param_expr_list>& p, 
		const footprint& top) const {
	STACKTRACE_VERBOSE;
#if ENABLE_DATASTRUCTS
if (defined) {
	footprint* f = &footprint_map[p];
	// will automatically unroll first if not already unrolled
	if (!f->is_created()) {
		if (!__unroll_complete_type(p, top).good) {
			return good_bool(false);
		}
		const canonical_type_base canonical_params(p);
		const template_actuals
			canonical_actuals(canonical_params.get_template_params(
				template_formals.num_strict_formals()));
		const canonical_user_def_data_type
			cpt(make_canonical_type(canonical_actuals));
		const unroll_context c(f, &top);
		// no parent b/c doing away with lookup of globals, 
		// when we do, need to chain the context with parent...
		if (!template_formals.unroll_formal_parameters(
				c, canonical_actuals).good) {
			cerr << "ERROR: unrolling template formals." << endl;
			return good_bool(false);
		}
		// this replays internal aliases of all instances in this scope
		if (!f->create_dependent_types(top).good) {
			// error message
			return good_bool(false);
		}
		// TODO: CHP -- override unroll_lang()
	}
	return good_bool(true);
} else {
	cerr << "ERROR: cannot create incomplete data type " <<
			get_qualified_name() << endl;
	// parent should print: "instantiated from here"
	return good_bool(false);
}
#else
	FINISH_ME(Fang);
	return good_bool(false);
#endif	// ENABLE_DATASTRUCTS
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not supposed to be called.
	No user-defined data types allowed in built-in channel specifications
	... yet.
 */
meta_type_tag_enum
user_def_datatype::get_meta_type(void) const {
#if 0
	ICE_NEVER_CALL(cerr);
	return META_TYPE_NONE;
#else
	return META_TYPE_STRUCT;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if BUILTIN_CHANNEL_FOOTPRINTS
/**
	Should we check for int-size limit?  (32 vs. 64)?
 */
void
user_def_datatype::count_channel_member(
		fundamental_channel_footprint& f) const {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
user_def_datatype::dump_channel_field_iterate(ostream& o, 
		channel_data_reader&) const {
	ICE_NEVER_CALL(cerr);
	return o;
}
#endif

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
	footprint_map.collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not recursive, manager will call this once.  
	NOTE: this must be kept consistent with 
	built_in_datatype_def::write_object, 
	for the sake of faking and intercepting built-in definitions.
 */
void
user_def_datatype::write_object(
		const persistent_object_manager& m, ostream& f) const {
	STACKTRACE_PERSISTENT("user_def_datatype::write_object()");
	write_string(f, key);
	m.write_pointer(f, parent);
	definition_base::write_object_base(m, f);
	scopespace::write_object_base(m, f);
	m.write_pointer(f, base_type);
	port_formals.write_object_base(m, f);
	// connections and assignments
	sequential_scope::write_object_base(m, f);
	// body
	set_chp.write_object_base(m, f);
	get_chp.write_object_base(m, f);
	footprint_map.write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
user_def_datatype::load_object(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("user_def_datatype::load_object()");
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, parent);
	definition_base::load_object_base(m, f);
	scopespace::load_object_base(m, f);
	m.read_pointer(f, base_type);
	port_formals.load_object_base(m, f);
	// connections and assignments
	sequential_scope::load_object_base(m, f);
	// body
	set_chp.load_object_base(m, f);
	get_chp.load_object_base(m, f);
	footprint_map.load_object_base(m, f, *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
user_def_datatype::load_used_id_map_object(excl_ptr<persistent>& o) {
	if (o.is_a<placeholder_base_type>()) {
		excl_ptr<placeholder_base_type>
			icbp = o.is_a_xfer<placeholder_base_type>();
		add_instance(icbp);
		INVARIANT(!icbp);
	} else {
		o->what(cerr << "TODO: define method for adding ")
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
never_ptr<const scopespace>
datatype_definition_alias::get_scopespace(void) const {
	return base->get_base_datatype_def()->get_scopespace();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const port_formals_manager>
datatype_definition_alias::get_port_formals_manager(void) const {
	return base->get_base_datatype_def()->get_port_formals_manager();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
good_bool
datatype_definition_alias::certify_port_actuals(
		const checked_refs_type& cr) const {
	return base->get_base_datatype_def()->certify_port_actuals(cr);
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
datatype_definition_alias::create_complete_type(
		const count_ptr<const const_param_expr_list>& p, 
		const footprint& top) const {
	ICE_NEVER_CALL(cerr);
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should never be called, as canonical type are supposed to
	resolve typedef aliases.  
 */
meta_type_tag_enum
datatype_definition_alias::get_meta_type(void) const {
	ICE_NEVER_CALL(cerr);
//	return META_TYPE_NONE;
	return base->get_base_datatype_def()->get_meta_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if BUILTIN_CHANNEL_FOOTPRINTS
/**
	Should we check for int-size limit?  (32 vs. 64)?
 */
void
datatype_definition_alias::count_channel_member(
		fundamental_channel_footprint& f) const {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
datatype_definition_alias::dump_channel_field_iterate(ostream& o, 
		channel_data_reader&) const {
	ICE_NEVER_CALL(cerr);
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively collects reachable pointers and register them
	with the persistent object manager.  
 */
void
datatype_definition_alias::collect_transient_info(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
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
	STACKTRACE_PERSISTENT_VERBOSE;
	if (o.is_a<placeholder_base_type>()) {
		excl_ptr<placeholder_base_type>
			icbp = o.is_a_xfer<placeholder_base_type>();
		add_instance(icbp);
		INVARIANT(!icbp);
	} else {
		o->what(cerr << "TODO: define method for adding ")
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
		scope_parent_type(),
		sequential_scope(), 
#if !PROCESS_DEFINITION_IS_NAMESPACE
		key(), 
		parent(), 
#endif
		meta_type(META_TYPE_PROCESS),	// don't care
		port_formals(), 
		prs(), rte(), chp(), 
		footprint_map() {
	// no null check: because of partial reconstruction
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("this @ " << this << endl);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This constructor is reserved for the module.  
	NOTE: module is always nullary (template)
 */
process_definition::process_definition(const string& s) :
		definition_base(), 
		process_definition_base(),
#if PROCESS_DEFINITION_IS_NAMESPACE
		scope_parent_type(s),
#else
		scope_parent_type(),
#endif
		sequential_scope(), 
#if !PROCESS_DEFINITION_IS_NAMESPACE
		key(s), 
		parent(), 
#endif
		meta_type(META_TYPE_PROCESS),	// top-type is a process
		port_formals(), 
		prs(), rte(), chp(), 
		footprint_map(0, *this) {
	STACKTRACE_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructor for a process definition symbol table entry.  
	TODO: when is the footprint_map's arity set? after adding 
		template-formal parameter.  
 */
process_definition::process_definition(
		const never_ptr<const name_space> o, 
		const string& s, 
		const meta_type_tag_enum t) :
		definition_base(), 
		process_definition_base(),
#if PROCESS_DEFINITION_IS_NAMESPACE
		scope_parent_type(s, o),
#else
		scope_parent_type(),
#endif
		sequential_scope(), 
#if !PROCESS_DEFINITION_IS_NAMESPACE
		key(s), 
		parent(o), 
#endif
		meta_type(t), 
		port_formals(), 
		prs(), rte(), chp(), 
		footprint_map() {
	STACKTRACE_VERBOSE;
	// fill me in...
	NEVER_NULL(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_definition::~process_definition() {
	STACKTRACE_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_definition::what(ostream& o) const {
	return o << "process-definition";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
meta_type_tag_enum
process_definition::get_meta_type(void) const {
#if 0
	// not true because process definitions now mask as
	// channel and datastruct definitions
	return META_TYPE_PROCESS;
#else
	return meta_type;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const process_definition&
process_definition::get_canonical_proc_def(void) const {
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Spill contents of the used_id_map.
	\param o the output stream.
	\return the same output stream.
	TODO: sort used_id_map entries alphabetically... at least in dumping
 */
ostream&
process_definition::dump(ostream& o) const {
	STACKTRACE_DUMP(__PRETTY_FUNCTION__);
	definition_base::dump(o);	// dump template signature first
	INDENT_SECTION(o);	
	// now dump ports
	port_formals.dump(o) << endl;
	// now dump rest of contents
	o << auto_indent <<
		"In definition \"" << key << "\", we have: {" << endl;
	{	// begin indent level
		scope_parent_type::dump_for_definitions(o);
		if (defined) {
			const expr_dump_context dc(this);
			o << auto_indent << "unroll sequence: " << endl;
			{	INDENT_SECTION(o);
				sequential_scope::dump(o, dc);
			}
			// RTE
			if (!rte.empty()) {
				o << auto_indent << "rte:" << endl;
				INDENT_SECTION(o);
				const RTE::assignment_dump_context rdc(*this);
				rte.dump(o, rdc);	// << endl;
			}
			// PRS
			if (!prs.empty()) {
				o << auto_indent << "prs:" << endl;
				INDENT_SECTION(o);
				const PRS::rule_dump_context rdc(*this);
				prs.dump(o, rdc);	// << endl;
			}
			// CHP
			if (!chp.empty()) {
				o << auto_indent << "chp:" << endl;
				INDENT_SECTION(o);
				chp.dump(o << auto_indent, dc) << endl;
			}
			// SPEC
			if (!spec.empty()) {
				o << auto_indent << "spec:" << endl;
				INDENT_SECTION(o);
				const PRS::rule_dump_context rdc(*this);
				spec.dump(o, rdc);	// << endl;
			}
			// subinstances
			if (footprint_map.size()) {
				footprint_map.dump(
					o << auto_indent, dc) << endl;
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
	return definition_base::get_qualified_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_definition::dump_qualified_name(ostream& o,
		const dump_flags& df) const {
	return definition_base::dump_qualified_name(o, df);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
process_definition::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
process_definition::get_scopespace(void) const {
	return never_ptr<const scopespace>(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_definition::commit_arity(void) {
	footprint_map.set_arity(template_formals.arity(), *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const object>
process_definition::lookup_member(const string& s) const {
#if PROCESS_DEFINITION_IS_NAMESPACE
	return scopespace::lookup_local(s);	// not scope_parent_type?
#else
	return scopespace::lookup_member(s);	// not scope_parent_type?
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const port_formals_manager>
process_definition::get_port_formals_manager(void) const {
	return never_ptr<const port_formals_manager>(&port_formals);
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
	STACKTRACE_VERBOSE;
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
never_ptr<const definition_base::instance_placeholder_type>
process_definition::add_port_formal(
		const count_ptr<instantiation_statement_base>& f, 
		const token_identifier& id) {
	typedef	never_ptr<const instance_placeholder_type>	return_type;
	NEVER_NULL(f);
	// check and make sure identifier wasn't repeated in formal list!
	{
	const never_ptr<const object>
		probe(lookup_member(id));
	if (probe) {
		probe->what(cerr << id << " already taken as a ") << " ERROR!";
		return return_type(NULL);
	}
	}

	const return_type pf(add_instance(f, id, false)
		.is_a<const instance_placeholder_type>());
		// false -- formals are never conditional
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
const footprint*
process_definition::lookup_footprint(
		const count_ptr<const const_param_expr_list>& p) const {
	STACKTRACE_VERBOSE;
	return footprint_map.lookup(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This asserts that the sought footprint is found.  
 */
const footprint&
process_definition::get_footprint(
		const count_ptr<const const_param_expr_list>& p) const {
	STACKTRACE_VERBOSE;
	const footprint* f = lookup_footprint(p);
	NEVER_NULL(f);
	return *f;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre the arity of the footprint_manager must be set.  
 */
void
process_definition::register_complete_type(
		const count_ptr<const const_param_expr_list>& p) const {
	STACKTRACE_VERBOSE;
	footprint_map.insert(p, *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: this can be used to unroll a process definition
	into a top-level footprint.  
	NOTE: this is also intended for a top-level module to invoke.  
 */
good_bool
process_definition::__unroll_complete_type(
		const count_ptr<const const_param_expr_list>& p, 
		footprint& f, 
		const footprint& top) const {
	// unroll using the footprint manager
	STACKTRACE_VERBOSE;
	{
		const canonical_type_base canonical_params(p);
		const template_actuals
			canonical_actuals(canonical_params.get_template_params(
				template_formals.num_strict_formals()));
		const unroll_context c(&f, &top);
		// no parent b/c doing away with lookup of globals, 
		// when we do, need to chain the context with parent...
		if (!template_formals.unroll_formal_parameters(
				c, canonical_actuals).good) {
			cerr << "ERROR: unrolling template formals." << endl;
			return good_bool(false);
		}
		if (sequential_scope::unroll(c).good) {
			// NOTE: nothing can be done with production rules
			// until nodes have been assigned local ID numbers
			// in the create phase, in create_complete_type, below.
		if (meta_type == META_TYPE_PROCESS) {
			// Automatically collect implicit supply ports
			if (!f.connect_implicit_ports(c).good) {
				// TODO: error message
				return good_bool(false);
			}
			// This must be done after unrolling, but before
			// and IDs are assigned by creating
		}
		} else {
		// already have partial error message
			// cpt.dump(cerr << "Instantiated from ") << endl;
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Process the language bodies.
 */
good_bool
process_definition::unroll_lang(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	footprint& f(c.get_target_footprint());
	// after all aliases have been successfully assigned local IDs
	// then process the PRS and CHP bodies
	if ((meta_type == META_TYPE_PROCESS) && !rte.unroll(c).good) {
		// already have error message
		return good_bool(false);
	}
	if ((meta_type == META_TYPE_PROCESS) && !prs.unroll(c).good) {
		// already have error message
		return good_bool(false);
	}
	if (!spec.unroll(c).good) {
		// already have error message
		return good_bool(false);
	}
	// CHP unrolling also checks channel connectivity now
	if ((meta_type == META_TYPE_PROCESS) && !chp.unroll(c).good) {
		// already have error message
		return good_bool(false);
	}
	// allocate local CHP event pool
	f.allocate_chp_events();
	// need to propagate flags after connectivity processing!
	f.synchronize_alias_flags();
	// after RTE processed and subprocess evaluated
	f.reconstruct_local_atomic_update_graph();
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("in process_definition::unroll_lang()");
	const port_alias_tracker& st(f.get_scope_alias_tracker());
	st.dump(cerr) << endl;
#endif
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create the definition footprint for a complete process_type.  
	This can be invoked as a top-level module.  
	\pre this definition is completely defined.  
 */
good_bool
process_definition::__create_complete_type(
		const count_ptr<const const_param_expr_list>& p, 
		footprint& f, 
		const footprint& top) const {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("Creating process type: " <<
		get_qualified_name());
	if (p) {
		p->dump(STACKTRACE_STREAM << '<',
			expr_dump_context::default_value) << '>';
	}
	STACKTRACE_STREAM << endl;
#endif
try {
	const footprint::create_lock LOCK(f);	// will catch recursion error
	// will automatically unroll first if not already unrolled
	if (!f.is_created()) {
		STACKTRACE_INDENT_PRINT("unrolling type definition." << endl);
		const unroll_context c(&f, &top);
		// why not pass context?
		if (!__unroll_complete_type(p, f, top).good) {
			// already have type error message
			return good_bool(false);
		}
		// no need to re-unroll formal parameters, 
		// already expanded in footprint
		// this replays internal aliases of all instances in this scope
		// doesn't use context? what if dependent on global parameter?
		// probably need to pass it in!
		if (!f.create_dependent_types(c).good) {
			// error message
		if (parent) {
			cerr << "Error creating process type: " <<
				get_qualified_name();
			if (p) {
				p->dump(cerr << '<',
					expr_dump_context::default_value)
					<< '>';
			}
			cerr << endl;
		} // suppress this message for the top-level module
			return good_bool(false);
		}
#if 1
		if (!unroll_lang(c).good) {
			return good_bool(false);
		}
#endif
		// check channel producer/consumer connectivity:
		// since alias sets were computed before connectivity
		// we may need to re-synchronize...
		// however, skip some checks for top-level instantiations
		const error_count ec(f.connection_diagnostics(&top == &f));
		// f.mark_created();	// ?
		// count warnings
		if (f.warnings() || ec.errors) {
			if (ec.errors) {
				cerr << "Errors found (" << ec.errors << ")";
			}
			if (f.warnings()) {
				if (ec.errors) cerr << ", ";
				cerr << "Warnings found (" << f.warnings() << ")";
			}
			cerr << " while creating complete type ";
			if (&f == &top) {
				cerr << "<top-level>";
			} else {
				f.dump_type(cerr);
			}
			cerr << "." << endl;
			if (ec.errors) {
				return good_bool(false);
			}
		}
		// construct one-time port_template subinstances
		f.get_port_template();	// cache it (optional)
		// could also let it get cached on-demand
#if ENABLE_STACKTRACE
		f.dump_with_collections(cerr << "Final footprint: " << endl);
#endif
	} else {
		STACKTRACE_INDENT_PRINT("already created" << endl);
	}
	return good_bool(true);
} catch (...) {
if (parent) {
	cerr << "Error creating type: " << get_qualified_name();
	if (p) {
		p->dump(cerr << '<', expr_dump_context::default_value) << '>';
	}
	cerr << endl;
}
	// else don't print name for top-level module
	throw;	// re-throw
	return good_bool(false);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
process_definition::create_complete_type(
		const count_ptr<const const_param_expr_list>& p, 
		const footprint& top) const {
	STACKTRACE_VERBOSE;
if (defined) {
	footprint* const f = footprint_map.lookup(p);
	if (f) {
		return __create_complete_type(p, *f, top);
	} else {
		cerr << "ERROR: looking up " << get_qualified_name();
		if (p) {
			p->dump(cerr << " with parameters: ");
		}
		cerr << endl;
		return good_bool(false);
	}
} else {
	cerr << "ERROR: cannot create undefined process " <<
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
process_definition::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
#if 0
	cerr << "registering definition: " << key << endl;
#endif
	// no need to visit template formals, port formals, separately, 
	// b/c they're all registered in the used_id_map.  
	scope_parent_type::collect_transient_info_base(m);
	sequential_scope::collect_transient_info_base(m);
#if 0
	port_formals.collect_transient_info_base(m);	// is a NO-OP, actually
	// pointers already covered by scopespace::collect
#endif
	// PRS
	prs.collect_transient_info_base(m);
	rte.collect_transient_info_base(m);
	chp.collect_transient_info_base(m);
	spec.collect_transient_info_base(m);
	footprint_map.collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_definition::collect_transient_info(persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not recursive, manager will call this once.  
 */
void
process_definition::write_object_base(
		const persistent_object_manager& m, ostream& f) const {
	STACKTRACE_PERSISTENT_VERBOSE;
#if !PROCESS_DEFINITION_IS_NAMESPACE
	write_string(f, key);
	m.write_pointer(f, parent);
#endif
	definition_base::write_object_base(m, f);
	write_value(f, meta_type);
	port_formals.write_object_base(m, f);
#if PROCESS_DEFINITION_IS_NAMESPACE
	scope_parent_type::write_object(m, f);
#else
	scope_parent_type::write_object_base(m, f);
#endif
	// connections and assignments
	sequential_scope::write_object_base(m, f);
// if (meta_type == META_TYPE_PROCESS) {
	// PRS
	prs.write_object_base(m, f);
	rte.write_object_base(m, f);
	chp.write_object_base(m, f);
// }
	spec.write_object_base(m, f);
	footprint_map.write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_definition::write_object(
		const persistent_object_manager& m, ostream& f) const {
	write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_definition::load_object_base(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT_VERBOSE;
	STACKTRACE_INDENT_PRINT("this @ " << this << endl);
#if ENABLE_STACKTRACE
	what(STACKTRACE_INDENT_PRINT("what: ")) << endl;
#endif
	
#if !PROCESS_DEFINITION_IS_NAMESPACE
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, parent);
#endif
	definition_base::load_object_base(m, f);
	read_value(f, meta_type);
	port_formals.load_object_base(m, f);
#if PROCESS_DEFINITION_IS_NAMESPACE
	scope_parent_type::load_object(m, f);
#else
	scope_parent_type::load_object_base(m, f);
#endif
	// connections and assignments
	sequential_scope::load_object_base(m, f);
// if (meta_type == META_TYPE_PROCESS) {
	// PRS
	prs.load_object_base(m, f);
	rte.load_object_base(m, f);
	chp.load_object_base(m, f);
// }
	spec.load_object_base(m, f);
	footprint_map.load_object_base(m, f, *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_definition::load_object(
		const persistent_object_manager& m, istream& f) {
	load_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !PROCESS_DEFINITION_IS_NAMESPACE
void
process_definition::load_used_id_map_object(excl_ptr<persistent>& o) {
	STACKTRACE_PERSISTENT_VERBOSE;
	if (o.is_a<placeholder_base_type>()) {
		excl_ptr<placeholder_base_type>
			icbp = o.is_a_xfer<placeholder_base_type>();
		add_instance(icbp);
		INVARIANT(!icbp);
	} else {
		o->what(cerr << "TODO: define method for adding ")
			<< " back to process definition." << endl;
	}
}
#endif

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
/**
	TODO: replace many of the below functions with a base class call
	to this method instead.
 */
const process_definition&
process_definition_alias::get_canonical_proc_def(void) const {
	return base->get_base_proc_def()->get_canonical_proc_def();
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
never_ptr<const scopespace>
process_definition_alias::get_scopespace(void) const {
	return base->get_base_proc_def()->get_scopespace();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
meta_type_tag_enum
process_definition_alias::get_meta_type(void) const {
	return base->get_base_proc_def()->get_meta_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const fundamental_type_reference>
process_definition_alias::get_base_type_ref(void) const {
	return base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const port_formals_manager>
process_definition_alias::get_port_formals_manager(void) const {
	return base->get_base_proc_def()->get_port_formals_manager();
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
good_bool
process_definition_alias::certify_port_actuals(
		const checked_refs_type& cr) const {
	return base->get_base_proc_def()->certify_port_actuals(cr);
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
	STACKTRACE_VERBOSE;
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
	STACKTRACE_VERBOSE;
	const template_actuals& ba(base->get_template_params());
	const template_actuals
		ta(ba.transform_template_actuals(a, template_formals));
	return base->get_base_proc_def()->make_canonical_fundamental_type_reference(ta);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively collects reachable pointers and register them
	with the persistent object manager.  
 */
void
process_definition_alias::collect_transient_info(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
#if 0
	cerr << "registering alias: " << key << endl;
#endif
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
	STACKTRACE_PERSISTENT_VERBOSE;
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
	STACKTRACE_PERSISTENT_VERBOSE;
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
	STACKTRACE_PERSISTENT_VERBOSE;
#if 0
	cerr << "WARNING: didn't expect to call "
		"process_definition_alias::load_used_id_map_object()." << endl;
#endif
	if (o.is_a<placeholder_base_type>()) {
		excl_ptr<placeholder_base_type>
			icbp = o.is_a_xfer<placeholder_base_type>();
		add_instance(icbp);
		INVARIANT(!icbp);
	} else {
		o->what(cerr << "TODO: define method for adding ")
			<< " back to process typedef." << endl;
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

