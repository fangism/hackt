/**
	\file "Object/inst/instance_collection.cc"
	Method definitions for instance collection classes.
	This file was originally "Object/art_object_instance.cc"
		in a previous (long) life.  
 	$Id: instance_collection.cc,v 1.25.4.2 2006/11/01 07:52:28 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_COLLECTION_CC__
#define	__HAC_OBJECT_INST_INSTANCE_COLLECTION_CC__

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS		(0 && ENABLE_STACKTRACE)


#include <iostream>
#include <algorithm>

#include "Object/def/definition_base.h"
#include "Object/def/footprint.h"
#include "Object/def/user_def_datatype.h"
#include "Object/type/fundamental_type_reference.h"
#include "Object/inst/datatype_instance_collection.h"
#include "Object/inst/general_collection_type_manager.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/nonmeta_instance_reference_base.h"
#include "Object/unroll/instantiation_statement.h"
#include "Object/unroll/null_parameter_type.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/common/namespace.h"
#include "Object/persistent_type_hash.h"
#include "Object/inst/substructure_alias_base.h"
#include "common/TODO.h"
#include "common/ICE.h"

// the following are required by use of canonical_type<>
// see also the temporary hack in datatype_instance_collection
#include "Object/def/enum_datatype_def.h"
#include "Object/expr/pint_expr.h"
#include "Object/expr/const_param.h"
#include "Object/type/canonical_type.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/bool_instance_collection.h"
#include "Object/inst/int_instance_collection.h"
#include "Object/inst/enum_instance_collection.h"
#include "Object/inst/struct_instance_collection.h"
#include "Object/inst/param_value_collection.h"	// for dynamic_cast
#include "Object/common/dump_flags.h"
#include "Object/type/param_type_reference.h"	// for must_be_type_eq
#include "Object/inst/param_value_placeholder.h"
#include "Object/inst/datatype_instance_placeholder.h"

#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/compose.h"
#include "util/binders.h"
#include "util/dereference.h"
#include "util/indent.h"
#include "util/stacktrace.h"


//=============================================================================
namespace HAC {
namespace entity {
using namespace ADS;		// for composition functors
using util::dereference;
#include "util/using_ostream.h"
using util::bind2nd_argval_void;
using util::indent;
using util::auto_indent;
using util::write_string;
using util::read_string;
using util::write_value;
using util::read_value;

//=============================================================================
// class instance_collection_base method definitions

const never_ptr<const instance_collection_base>
instance_collection_base::null(NULL);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
instance_collection_base::~instance_collection_base() {
	STACKTRACE_DTOR("~instance_collection_base()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps the collection excluding the index collection 
	list from instantiation statements.  
 */
ostream&
instance_collection_base::dump_collection_only(ostream& o) const {
	NEVER_NULL(this);
	type_dump(o);		// pure virtual
	// if (dimensions)
		o << '^' << get_dimensions();
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Forwarded call to dump.  
 */
ostream&
instance_collection_base::dump(ostream& o) const {
	return dump(o, dump_flags::default_value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance_collection_base::dump_base(ostream& o) const {
	return dump_base(o, dump_flags::default_value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overridden by param_value_collection.  
 */
ostream&
instance_collection_base::dump_base(ostream& o, const dump_flags& df) const {
	dump_collection_only(o);
	const size_t dimensions = get_dimensions();
	if (dimensions) {
		o << endl;
	} else {
		// the list contains exactly one instantiation statement
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
instance_collection_base::get_footprint_key(void) const {
	const never_ptr<const instance_placeholder_base>
		p(__get_placeholder_base());
	NEVER_NULL(p);
	return p->get_footprint_key();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance_collection_base::dump_hierarchical_name(ostream& o) const {
	STACKTRACE_VERBOSE;
	if (super_instance) {
		return super_instance->dump_hierarchical_name(o,
			dump_flags::default_value) << '.' << get_name();
	} else {
		const never_ptr<const instance_placeholder_base>
			p(__get_placeholder_base());
		NEVER_NULL(p);
		return p->dump_qualified_name(o, dump_flags::default_value);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance_collection_base::dump_hierarchical_name(ostream& o, 
		const dump_flags& df) const {
	STACKTRACE_VERBOSE;
	if (super_instance) {
		return super_instance->dump_hierarchical_name(o, df)
			<< '.' << get_name();
	} else {
		const never_ptr<const instance_placeholder_base>
			p(__get_placeholder_base());
		NEVER_NULL(p);
		return p->dump_qualified_name(o, df);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
instance_collection_base::hierarchical_depth(void) const {
	return super_instance ? super_instance->hierarchical_depth() +1 : 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Whether or not this instance is a reference to a collection
	local to a definition, else is a top-level (global).
 */
bool
instance_collection_base::is_local_to_definition(void) const {
	const owner_ptr_type owner(get_owner());
	return owner.is_a<const definition_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Upward recursive: find top-most super-instance and allocate 
	state from there top-down.  
 */
good_bool
instance_collection_base::create_super_instance(footprint& f) {
	// super-instance corresponds to a substructure alias
	// some traversal similar to dump_hierarchical_name.
	INVARIANT(super_instance);
	return good_bool(super_instance->allocate_state(f) != 0);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
void
instance_collection_base::collect_transient_info_base(
		persistent_object_manager& m) const {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Q: Where and when was the super_instance pointer tracked?
 */
void
instance_collection_base::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	// m.write_pointer(o, super_instance);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instance_collection_base::load_object_base(
		const persistent_object_manager& m, istream& i) {
	// m.read_pointer(i, super_instance);
}
#endif

//=============================================================================
// class physical_instance_collection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
physical_instance_collection::~physical_instance_collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_placeholder_base>
physical_instance_collection::__get_placeholder_base(void) const {
	return get_placeholder_base();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
physical_instance_collection::get_dimensions(void) const {
	return get_placeholder_base()->get_dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
physical_instance_collection::get_name(void) const {
	return get_placeholder_base()->get_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
physical_instance_collection::get_owner(void) const {
	return get_placeholder_base()->get_owner();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
physical_instance_collection::dump(ostream& o, const dump_flags& df) const {
#if 0
	o << "dump flags: " << (df.show_definition_owner ? "(def) " : " ") <<
		(df.show_namespace_owner ? "(ns) " : " ") <<
		(df.show_leading_scope ? "(::) " : " ") << endl << auto_indent;
#endif
	parent_type::dump_base(o, df);
	// it IS partially unrolled
		const size_t dimensions = get_dimensions();
		if (dimensions) {
			// then is an array
			INDENT_SECTION(o);
			o << auto_indent << "{" << endl;
			{
				// INDENT_SECTION macro not making unique IDs
				INDENT_SECTION(o);
				dump_unrolled_instances(o, df);
			}
			o << auto_indent << "}";        // << endl;
		} else {
			// else nothing to say, just one scalar instance
			dump_unrolled_instances(o, df);
		}
	return o;
}

//=============================================================================
// class param_value_collection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_value_collection::~param_value_collection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_placeholder_base>
param_value_collection::__get_placeholder_base(void) const {
	return get_placeholder_base();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
param_value_collection::get_dimensions(void) const {
	// alternative, defer to value_array
	return get_placeholder_base()->get_dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
param_value_collection::get_name(void) const {
	return get_placeholder_base()->get_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
param_value_collection::get_owner(void) const {
	return get_placeholder_base()->get_owner();
}

//=============================================================================
// class datatype_instance_collection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Private empty constructor.
 */
datatype_instance_collection::datatype_instance_collection() :
		parent_type() {
	// no assert
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_instance_collection::~datatype_instance_collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default action for request for resolved param list.  
	This is appropriate for collection types that have no 
	template parameters.  
	\return null list of parameters.  
 */
never_ptr<const const_param_expr_list>
datatype_instance_collection::get_actual_param_list(void) const {
	return never_ptr<const const_param_expr_list>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Temporary ugly hack.  :(
	TODO: implement for real.  
	REMARK: want to use a virtual function, but can't because
		children types expect different argument types!
	Possible to fake it...?
 */
good_bool
datatype_instance_collection::establish_collection_type(
		const instance_collection_parameter_type& p) {
{
	bool_instance_collection* const
		b(IS_A(bool_instance_collection*, this));
	if (b) {
		return b->establish_collection_type(
			bool_instance_collection::instance_collection_parameter_type());
	}
}{
	int_instance_collection* const
		i(IS_A(int_instance_collection*, this));
	if (i) {
		const canonical_type_base::const_param_list_ptr_type&
			pp(p.get_raw_template_params());
		NEVER_NULL(pp);
		INVARIANT(pp->size() == 1);
		const int_instance_collection::instance_collection_parameter_type
			w = IS_A(const pint_expr&, *pp->front())
				.static_constant_value();
		return i->establish_collection_type(w);
	}
}{
	enum_instance_collection* const
		e(IS_A(enum_instance_collection*, this));
	if (e) {
		const enum_instance_collection::instance_collection_parameter_type
			d = p.get_base_def().is_a<const enum_datatype_def>();
		return e->establish_collection_type(d);
	}
}{
	// TODO: user-def-structs
	struct_instance_collection* const
		e(IS_A(struct_instance_collection*, this));
	if (e) {
		return e->establish_collection_type(p);
	}
}
	ICE(cerr,
		cerr << "Unhandled case in this function." << endl;
	)
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bad_bool
datatype_instance_collection::check_established_type(
		const instance_collection_parameter_type& p) const {
{
	const bool_instance_collection* const
		b(IS_A(const bool_instance_collection*, this));
	if (b) {
		return b->check_established_type(
			bool_instance_collection::instance_collection_parameter_type());
	}
}{
	const int_instance_collection* const
		i(IS_A(const int_instance_collection*, this));
	if (i) {
		const canonical_type_base::const_param_list_ptr_type&
			pp(p.get_raw_template_params());
		NEVER_NULL(pp);
		INVARIANT(pp->size() == 1);
		const int_instance_collection::instance_collection_parameter_type
			w = IS_A(const pint_expr&, *pp->front())
				.static_constant_value();
		return i->check_established_type(w);
	}
}{
	const enum_instance_collection* const
		e(IS_A(const enum_instance_collection*, this));
	if (e) {
		const enum_instance_collection::instance_collection_parameter_type
			d = p.get_base_def().is_a<const enum_datatype_def>();
		return e->check_established_type(d);
	}
}{
	const struct_instance_collection* const
		e(IS_A(const struct_instance_collection*, this));
	if (e) {
		return e->check_established_type(p);
	}
}
	ICE(cerr,
		cerr << "Unhandled case in this function." << endl;
	)
	return bad_bool(true);
}

//=============================================================================
// class instance_placeholder_base method definitions

instance_placeholder_base::instance_placeholder_base(
		const scopespace& s, const string& k, const size_t d) :
		owner(&s), key(k), dimensions(d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
instance_placeholder_base::~instance_placeholder_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Do we want the leading "::" of the global scope?
 */
string
instance_placeholder_base::get_qualified_name(void) const {
#if 1
	if (owner && !owner->is_global_namespace())
#else
	if (owner)
#endif
	{
		return owner->get_qualified_name() + "::" +key;
	}
		// "::" should be the same as HAC::parser::scope
	else return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	We catch a special case: when we refer to induction variables, 
	we drop any qualifiers.  
 */
ostream&
instance_placeholder_base::dump_qualified_name(ostream& o, 
		const dump_flags& df) const {
#if 0
	o << "[dump flags: " << (df.show_definition_owner ? "(def) " : " ") <<
		(df.show_namespace_owner ? "(ns) " : " ") <<
		(df.show_leading_scope ? "(::)]" : "]");
#endif
if (owner) {
	const param_value_placeholder* const
		p(IS_A(const param_value_placeholder*, this));
	if (p && p->is_loop_variable()) {
		// nothing, just print the plain key
		// maybe '$' to indicate variable?
		o << '$';
	} else if (owner.is_a<const definition_base>() &&
			df.show_definition_owner) {
		owner->dump_qualified_name(o, df) << "::";
	} else if (owner.is_a<const name_space>() &&
			(df.show_namespace_owner &&
			!owner->is_global_namespace())) {
		owner->dump_qualified_name(o, df) << "::";
	}
}
	return o << key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Footprint key only needs qualifiers for instance placeholders
	belonging to non-global namespaces.  
 */
string
instance_placeholder_base::get_footprint_key(void) const {
	const never_ptr<const name_space> ns(owner.is_a<const name_space>());
	if (ns && ns->get_parent()) {
		// use this, now that leading global namespace is dropped
		return get_qualified_name();
//		return ns->get_qualified_name() + "::" + key;
	} else {
		// also covers loop-induction variables
		return key;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	This should really just be folded into children-class methods...
 */
ostream&
instance_placeholder_base::dump_base(ostream& o) const {
	get_type_ref()->dump(o);
	o << ' ' << key;
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance_placeholder_base::pair_dump(ostream& o) const {
	o << auto_indent << get_name() << " = ";
	return dump(o, dump_flags::no_owners) << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance_placeholder_base::pair_dump_top_level(ostream& o) const {
	o << auto_indent << get_name() << " = ";
	return dump(o, dump_flags::verbose) << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance_placeholder_base::dump(ostream& o) const {
	return dump_base(o, dump_flags::default_value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overridden by param_value_placeholder?
 */
ostream&
instance_placeholder_base::dump_base(ostream& o, const dump_flags& df) const {
	// dump_collection_only(o);
	get_unresolved_type_ref()->dump(o) << " " << get_qualified_name();
	if (dimensions) {
		o << "^" << dimensions;
	}
	// o << endl;
#if 0
	if (dimensions) {
		o << endl;
	} else {
		// the list contains exactly one instantiation statement
	}
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if formal expressions (in declarations)
	are provably equivalent, equating by positional parameters.  
 */
bool
instance_placeholder_base::formal_size_equivalent(const this_type& b) const {
	const index_collection_item_ptr_type
		ii(this->get_initial_instantiation_indices()),
		ji(b.get_initial_instantiation_indices());
	if (ii && ji) {
		return ii->must_be_formal_size_equivalent(*ji);
	} else 	return (!ii && !ji);
	// both NULL is ok too
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Return's the type's base definition.
	TODO: far future, types may be template parameters, 
		which makes the base definition argument-dependent.  
 */
never_ptr<const definition_base>
instance_placeholder_base::get_base_def(void) const {
	return get_unresolved_type_ref()->get_base_def();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't bother visting the owner because collecting
	is traversed top-down.  
 */
void
instance_placeholder_base::collect_transient_info_base(
		persistent_object_manager& m) const {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instance_placeholder_base::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, owner);
	write_string(o, key);
	write_value(o, dimensions);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instance_placeholder_base::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, owner);
	read_string(i, key);
	read_value(i, dimensions);
#if 1
	// need this to guarantee that hierarchical name is
	// available when re-constructing footprints' instance collections.  
	m.load_object_once(const_cast<scopespace*>(&*owner));
#endif
}

//=============================================================================
// class param_value_placeholder method definitions
// should this go in "param_value_collection.cc" ... whatever.

param_value_placeholder::param_value_placeholder(const size_t d) :
		parent_type(d) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_value_placeholder::param_value_placeholder(
		const scopespace& s, const string& k, const size_t d) :
		parent_type(s, k, d) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_value_placeholder::~param_value_placeholder() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Queries whether or not this is a template formal, by 
	checking its membership in the owner.  
	\return 0 (false) if is not a template formal, 
		otherwise returns the position (1-indexed)
		of the instance referenced, 
		useful for determining template parameter equivalence.  
	TODO: is there potential confusion here if the key shadows
		a declaration else where?
 */
size_t
param_value_placeholder::is_template_formal(void) const {
	const never_ptr<const definition_base>
		def(owner.is_a<const definition_base>());
	if (def)
		return def->lookup_template_formal_position(key);
	else {
		// owner is not a definition
		INVARIANT(owner.is_a<const name_space>());
		// is owned by a namespace, i.e. actually instantiated
		return 0;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if referenced placeholder belongs in the relaxed set
		of template formal parameters.  
 */
bool
param_value_placeholder::is_relaxed_template_formal(void) const {
	const never_ptr<const definition_base>
		def(owner.is_a<const definition_base>());
	if (def) {
		return def->probe_relaxed_template_formal(key);
	} else return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For two template formals to be equivalent, their
	type and size must match, names need not.  
	Currently allows comparison of parameter and non-parameter
	formal types.  
	Is conservative because parameters (in sizes) may be dynamic, 
	or collective.  
	TODO: this is only applicable to param_value_collection.  

	Origin: This was moved from 
	instance_collection_base::template_formal_equivalent(), and modified.
 */
bool
param_value_placeholder::template_formal_equivalent(const this_type& b) const {
	// first make sure base types are equivalent.  
	const count_ptr<const param_type_reference>
		t_type(get_param_type_ref());
	const count_ptr<const param_type_reference>
		b_type(b.get_param_type_ref());
	// used to be may_be_equivalent...
	if (!t_type->must_be_type_equivalent(*b_type)) {
		// then their instantiation types differ
		return false;
	}
	// then compare sizes and dimensionality
	return formal_size_equivalent(b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: just copied from param_value_collection.
	Checks for dimension and size equality between expression and 
	instantiation.  
	So far, only used by param_value_collection derivatives, 
		in the context of checking template formals.  
	May be useful else where for connections.  
	NOTE: this is conservative and need not be precise.  
	\return true if dimensions *may* match.  
 */
good_bool
param_value_placeholder::may_check_expression_dimensions(
		const param_expr& pe) const {
	STACKTRACE_VERBOSE;
	// MUST_BE_A(const param_value_collection*, this);
	// else is not an expression class!

	// dimensions() used to be a pure virtual method
	// problem when dimensions() is called during construction:
	// error: pure virtual method called (during construction)
	// this occurs during static construction of the global 
	// built in definition object: ind_def, which is templated
	// with int width.  
	// Solutions: 
	// 1) make an unsafe/unchecked constructor for this special case.
	// 2) add the template parameter after contruction is complete, 
	//      which is safe as long as no other global (outside of
	//      art_built_ins.cc) depends on it.
	// we choose 2 because it is a general solution.  
	const size_t pdim = pe.dimensions();
	if (dimensions != pdim) {
		// number of dimensions doesn't even match!
		// useful error message?
		return good_bool(false);
	}
	// dimensions match
	if (dimensions != 0) {
		/**
			We used to statically check dimensions for 
			early rejection, but the effort gave little benefit.  
		**/
		// be conservative
		return good_bool(true);
	} else {
		// dimensions == 0 means instantiation is a single instance.  
		// size may be zero b/c first statement hasn't been added yet
		return good_bool(pdim == 0);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: just copied from param_value_collection.
	Checks for dimension and size equality between expression and 
	instantiation.  
	So far, only used by param_value_collection derivatives, 
		in the context of checking template formals against actuals
		when unrolling instantiations.  
	May be useful else where for connections.  
	This should really onle be called suring the unroll phase, 
		when instance collections are 'certain'.  
	NOTE: instantiation indices may depend on template parameters
		so they need to be unroll resolved!
	TODO: what if template formal size depends on template parameter!
	\return true if dimensions *may* match.  
 */
good_bool
param_value_placeholder::must_check_expression_dimensions(
		const const_param& pe, const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	// MUST_BE_A(const param_value_collection*, this);
	// else is not an expression class!

	// dimensions() used to be a pure virtual method
	// problem when dimensions() is called during construction:
	// error: pure virtual method called (during construction)
	// this occurs during static construction of the global 
	// built in definition object: ind_def, which is templated
	// with int width.  
	// Solutions: 
	// 1) make an unsafe/unchecked constructor for this special case.
	// 2) add the template parameter after contruction is complete, 
	//      which is safe as long as no other global (outside of
	//      art_built_ins.cc) depends on it.
	// we choose 2 because it is a general solution.  
	if (dimensions != pe.dimensions()) {
		// number of dimensions doesn't even match!
		// useful error message?
		return good_bool(false);
	}
	// dimensions match
	if (dimensions != 0) {
#if 1
		// true for formal parameters
		// number of dimensions doesn't even match!
		// this is true only if parameters that check this
		// are template formals.  
		// not sure if this will be called by non-formals, will see...

		INVARIANT(pe.has_static_constant_dimensions());
		const const_range_list d(pe.static_constant_dimensions());

		// make sure sizes in each dimension
		const index_collection_item_ptr_type
			mrl(get_initial_instantiation_indices());
		NEVER_NULL(mrl);
		const count_ptr<const const_range_list>
			crl(mrl.is_a<const const_range_list>());
		if (crl) {
			// return good_bool(*crl == d);
			return good_bool(crl->is_size_equivalent(d));
		} else {
			// is dynamic, conservatively return false
			// we're in trouble for template-dependent expressions
			// need unroll parameters!
			const_range_list _r;
			if (!mrl->unroll_resolve_rvalues(_r, c).good) {
				// there was error resolving parameters!
				// should this ever happen???
			ICE(cerr, 
				cerr << "Error resolving instantiation range "
					"for checking expression dimensions!"
					<< endl;
			)
				return good_bool(false);
			}
			return good_bool(_r.is_size_equivalent(d));
		}
#else
		const const_index_list dummy;
		const const_index_list cil(resolve_indices(dummy)); // virtual
		if (cil.empty()) {
			cerr << "Error resolving indices of value collection."
				<< endl;
			// already have error message?
			// was unable to resolve dense collection
			return good_bool(false);
		}
		const const_range_list crl(cil.collapsed_dimension_ranges());
		return good_bool(crl.is_size_equivalent(d));
#endif
	} else {
		return good_bool(pe.dimensions() == 0);
	}
}	// end method param_value_collection::must_check_expression_dimensions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: we will allow this, once we open up nested types and values.
	OOOooo... template metaprogramming, here we come!
	1) Parameters cannot be in public ports.  
	2) Thus they cannot even be referenced.  
	3) This is just a placeholder that should never be called.  
 */
param_value_placeholder::member_inst_ref_ptr_type
param_value_placeholder::make_member_meta_instance_reference(
		const inst_ref_ptr_type& b) const {
	typedef	member_inst_ref_ptr_type	return_type;
	NEVER_NULL(b);
	cerr << "Referencing parameter members is strictly forbidden!" << endl;
	ICE_NEVER_CALL(cerr);
	return return_type(NULL);
}

//=============================================================================
// class physical_instance_placeholder method definitions

physical_instance_placeholder::physical_instance_placeholder(
		const scopespace& s, const string& k, const size_t d) :
		parent_type(s, k, d) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
physical_instance_placeholder::~physical_instance_placeholder() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - _
/**
	Really, this should be in physical_instance_placeholder.  

	Queries whether or not this is a port formal, by 
	checking its membership in the owner.  
	\return 1-indexed position into port list, else 0 if not found.
 */
size_t
physical_instance_placeholder::is_port_formal(void) const {
	const never_ptr<const definition_base>
		def(owner.is_a<const definition_base>());
	return def ? def->lookup_port_formal_position(*this) : 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Much like equivalence for template formals, except that
	names also need to match for port formals.  
	Rationale: need to be able to refer to the public ports
	of a prototype, which must correspond to those of the definition, 
	and vice versa.  
 */
bool
physical_instance_placeholder::port_formal_equivalent(
		const this_type& b) const {
	// first make sure base types are equivalent.  
	const count_ptr<const fundamental_type_reference>
		t_type(get_unresolved_type_ref());
	const count_ptr<const fundamental_type_reference>
		b_type(b.get_unresolved_type_ref());
	if (!t_type->may_be_connectibly_type_equivalent(*b_type)) {
		// then their instantiation types differ
		return false;
	}
	// then compare sizes and dimensionality
	if (!formal_size_equivalent(b))
		return false;
	// last, but not least, name must match
	return key == b.key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
physical_instance_placeholder::dump(ostream& o, const dump_flags& df) const {
	return parent_type::dump_base(o, df);
	// cannot possibly be unrolled
}

//=============================================================================
// class datatype_instance_placeholder method definitions

datatype_instance_placeholder::datatype_instance_placeholder(
		const scopespace& s, const string& k, const size_t d) :
		parent_type(s, k, d) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_instance_placeholder::~datatype_instance_placeholder() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_instance_collection*
datatype_instance_placeholder::make_collection(void) const {
	return IS_A(datatype_instance_collection*,
		make_instance_collection_footprint_copy());
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_COLLECTION_CC__

