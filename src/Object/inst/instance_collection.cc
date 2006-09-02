/**
	\file "Object/inst/instance_collection.cc"
	Method definitions for instance collection classes.
	This file was originally "Object/art_object_instance.cc"
		in a previous (long) life.  
 	$Id: instance_collection.cc,v 1.22.4.3 2006/09/02 00:45:57 fang Exp $
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
#if USE_INSTANCE_PLACEHOLDERS
#include "Object/inst/instance_placeholder_base.h"
#endif

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
#if !USE_INSTANCE_PLACEHOLDERS
/**
	Instantiation base constructor.  
	The first time an instance is declared, its dimensions are
	set by the array-dimension list, if provided, else 0.
	The first set of indices given will be pushed onto the 
	instance collection stack.  
	\param o the owning scope.  
	\param n the name of the instance (collection).
	\param d the number of dimensions of this collection ([0,4]).  
		WAS: initial collection of indices, already resolved 
		as param_expr's.  
 */
// inline
instance_collection_base::instance_collection_base(const scopespace& o, 
		const string& n, const size_t d) : 
		object(), owner(owner_ptr_type(&o)),
		key(n), 
		dimensions(d), 
		super_instance() {
}
#endif

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
	if (is_partially_unrolled()) {
		type_dump(o);		// pure virtual
		// if (dimensions)
#if USE_INSTANCE_PLACEHOLDERS
		o << '^' << get_dimensions();
#else
		o << '^' << dimensions;
#endif
	} else {
		const param_value_collection*
			p(IS_A(const param_value_collection*, this));
		if (p && p->is_loop_variable()) {
			// loop induction variables don't have unroll statements
			o << "(loop induction pint)";
		} else {
			const count_ptr<const fundamental_type_reference>
				t(get_type_ref());
			if (t)	t->dump(o);
			else	o << "(not unrolled yet)";
		}
	}
#if USE_INSTANCE_PLACEHOLDERS
	return o << ' ' << get_name();
#else
	return o << ' ' << key;
#endif
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
#if 0
	// but we need a version for unrolled and resolved parameters.  
	if (is_partially_unrolled()) {
		type_dump(o);		// pure virtual
	} else {
		// this dump is appropriate for pre-unrolled, unresolved dumping
		// get_type_ref just grabs the type of the first statement
		get_type_ref()->dump(o);
	}
	o << ' ' << key;
#else
	dump_collection_only(o);
#endif
#if USE_INSTANCE_PLACEHOLDERS
	const size_t dimensions = get_dimensions();
#endif
	if (dimensions) {
#if 0
		// TODO: get rid of this, and update tests
		// this was kept temporarily for the sake of easing
		// test difference analysis
		o << " with indices: {" << endl;
		o << auto_indent << '}' << endl;
#else
		o << endl;
#endif
	} else {
		// the list contains exactly one instantiation statement
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance_collection_base::pair_dump(ostream& o) const {
	o << auto_indent << get_name() << " = ";
	return dump(o, dump_flags::no_owners) << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance_collection_base::pair_dump_top_level(ostream& o) const {
	o << auto_indent << get_name() << " = ";
	return dump(o, dump_flags::verbose) << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
instance_collection_base::get_qualified_name(void) const {
#if USE_INSTANCE_PLACEHOLDERS
	const owner_ptr_type owner(get_owner());
	const string key(get_name());
#endif
	if (owner)
		return owner->get_qualified_name() + "::" +key;
		// "::" should be the same as HAC::parser::scope
	else return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	We catch a special case: when we refer to induction variables, 
	we drop any qualifiers.  
 */
ostream&
instance_collection_base::dump_qualified_name(ostream& o, 
		const dump_flags& df) const {
#if 0
	o << "[dump flags: " << (df.show_definition_owner ? "(def) " : " ") <<
		(df.show_namespace_owner ? "(ns) " : " ") <<
		(df.show_leading_scope ? "(::)]" : "]");
#endif
#if USE_INSTANCE_PLACEHOLDERS
	const owner_ptr_type owner(get_owner());
#endif
if (owner) {
	const param_value_collection* const
		p(IS_A(const param_value_collection*, this));
	if (p && p->is_loop_variable()) {
		// nothing, just print the plain key
		// maybe '$' to indicate variable?
		o << '$';
	} else if (owner.is_a<const definition_base>() &&
			df.show_definition_owner) {
		owner->dump_qualified_name(o, df) << "::";
	} else if (owner.is_a<const name_space>() &&
			(df.show_namespace_owner ||
			!owner->is_global_namespace())) {
		owner->dump_qualified_name(o, df) << "::";
	}
}
#if USE_INSTANCE_PLACEHOLDERS
	return o << get_name();
#else
	return o << key;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance_collection_base::dump_hierarchical_name(ostream& o) const {
	STACKTRACE_VERBOSE;
	if (super_instance) {
		return super_instance->dump_hierarchical_name(o,
			dump_flags::default_value) << '.' <<
#if USE_INSTANCE_PLACEHOLDERS
				get_name();
#else
				key;
#endif
	} else {
		return dump_qualified_name(o, dump_flags::default_value);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance_collection_base::dump_hierarchical_name(ostream& o, 
		const dump_flags& df) const {
	STACKTRACE_VERBOSE;
	if (super_instance) {
		return super_instance->dump_hierarchical_name(o, df)
			<< '.' << 
#if USE_INSTANCE_PLACEHOLDERS
			get_name();
#else
			key;
#endif
	} else {
		return dump_qualified_name(o, df);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
instance_collection_base::hierarchical_depth(void) const {
	return super_instance ? super_instance->hierarchical_depth() +1 : 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Return's the type's base definition.
 */
never_ptr<const definition_base>
instance_collection_base::get_base_def(void) const {
	return get_type_ref()->get_base_def();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !USE_INSTANCE_PLACEHOLDERS
/**
	Checks whether or not this is a relaxed template formal parameter.  
 */
bool
instance_collection_base::is_relaxed_template_formal(void) const {
#if USE_INSTANCE_PLACEHOLDERS
	const owner_ptr_type owner(get_owner());
#endif
	const never_ptr<const definition_base>
		def(owner.is_a<const definition_base>());
	if (def) {
#if USE_INSTANCE_PLACEHOLDERS
		return def->probe_relaxed_template_formal(get_name());
#else
		return def->probe_relaxed_template_formal(key);
#endif
	} else return false;
}

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
instance_collection_base::is_template_formal(void) const {
#if USE_INSTANCE_PLACEHOLDERS
	const owner_ptr_type owner(get_owner());
#endif
	const never_ptr<const definition_base>
		def(owner.is_a<const definition_base>());
	if (def)
#if USE_INSTANCE_PLACEHOLDERS
		return def->lookup_template_formal_position(get_name());
#else
		return def->lookup_template_formal_position(key);
#endif
	else {
		// owner is not a definition
		INVARIANT(owner.is_a<const name_space>());
		// is owned by a namespace, i.e. actually instantiated
		return 0;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Really, this should be in physical_instance_placeholder.  

	Queries whether or not this is a port formal, by 
	checking its membership in the owner.  
	\return 1-indexed position into port list, else 0 if not found.
 */
size_t
instance_collection_base::is_port_formal(void) const {
#if USE_INSTANCE_PLACEHOLDERS
	const owner_ptr_type owner(get_owner());
#endif
	const never_ptr<const definition_base>
		def(owner.is_a<const definition_base>());
	return def ? def->lookup_port_formal_position(*this) : 0;
}
#endif	// USE_INSTANCE_PLACEHOLDERS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return offset into definition's subinstance array that contains
		the member.  
	NOTE: this is called by subinstance_manager::lookup_member_instance,
		which attempts to lookup a subinstance-index for a 
		private member.  Private members, however, are not
		pre-allocated during the create phase, which makes
		this inappropriate for any use.  
		This is a dead-end, do not call this.  
		rewrite elsewhere (20060120).
 */
size_t
instance_collection_base::is_member_instance(void) const {
#if 0
	const never_ptr<const definition_base>
		def(owner.is_a<const definition_base>());
	return def ? def->lookup_member_position(*this) : 0;
#endif
	ICE(cerr, 
		cerr << "This code is never supposed to be called, "
			"refer to source code around here for the reason."
			<< endl;
	)
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Whether or not this instance is a reference to a collection
	local to a definition, else is a top-level (global).
 */
bool
instance_collection_base::is_local_to_definition(void) const {
#if USE_INSTANCE_PLACEHOLDERS
	const owner_ptr_type owner(get_owner());
#endif
	return owner.is_a<const definition_base>();
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
instance_collection_base::port_formal_equivalent(const this_type& b) const {
	// first make sure base types are equivalent.  
	const count_ptr<const fundamental_type_reference>
		t_type(get_type_ref());
	const count_ptr<const fundamental_type_reference>
		b_type(b.get_type_ref());
	if (!t_type->may_be_connectibly_type_equivalent(*b_type)) {
		// then their instantiation types differ
		return false;
	}
	// then compare sizes and dimensionality
	if (!formal_size_equivalent(b))
		return false;
	// last, but not least, name must match
#if USE_INSTANCE_PLACEHOLDERS
	return get_name() == b.get_name();
#else
	return key == b.get_name();
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just compares dimensionality and sizes of an instantiation
	in a template formal context.  
	This applies to both template formals and port formals.  
	Is conservative, not precise, in the case where one of the
	parameter sizes (dimension) is dynamic.  
	In this case, the initial instantiation is guaranteed to be
		unconditional because it is a template or port formal. 
	\param b the other template formal instantiation to compare against.  
	\return true if dimensionality and sizes are equal.  
	TODO: need to account for relaxed parameters of physical
		instance collections?
 */
bool
instance_collection_base::formal_size_equivalent(const this_type& b) const {
#if USE_INSTANCE_PLACEHOLDERS
	const size_t dimensions = get_dimensions();
	const size_t bdim = b.get_dimensions();
#else
	const size_t bdim = b.dimensions;
#endif
	if (dimensions != bdim) {
		// useful error message here: dimensions don't match
		return false;
	}
#if USE_INSTANCE_PLACEHOLDERS
	return this->__get_placeholder_base()->formal_size_equivalent(
			*b.__get_placeholder_base());
#else
	const index_collection_item_ptr_type
		ii(this->get_initial_instantiation_indices()),
		ji(b.get_initial_instantiation_indices());
	if (ii && ji) {
		return ii->must_be_formal_size_equivalent(*ji);
	} else 	return (!ii && !ji);
	// both NULL is ok too
#endif
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
#if USE_INSTANCE_PLACEHOLDERS
	// m.write_pointer(o, super_instance);
#else
	m.write_pointer(o, owner);
	write_string(o, key);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instance_collection_base::load_object_base(
		const persistent_object_manager& m, istream& i) {
#if USE_INSTANCE_PLACEHOLDERS
	// m.read_pointer(i, super_instance);
#else
	m.read_pointer(i, owner);
	read_string(i, const_cast<string&>(key));
#endif
}

//=============================================================================
// class physical_instance_collection method definitions

#if !USE_INSTANCE_PLACEHOLDERS
physical_instance_collection::physical_instance_collection(
		const scopespace& o, const string& n, const size_t d) :
		parent_type(o, n, d) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
physical_instance_collection::~physical_instance_collection() {
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
	if (is_partially_unrolled()) {
#if USE_INSTANCE_PLACEHOLDERS
		const size_t dimensions = get_dimensions();
#endif
		if (dimensions) {
			INDENT_SECTION(o);
			o << auto_indent << "unrolled indices: {" << endl;
			{
				// INDENT_SECTION macro not making unique IDs
				INDENT_SECTION(o);
				dump_unrolled_instances(o, df);
			}
			o << auto_indent << "}";        // << endl;
		} else {
			// else nothing to say, just one scalar instance
			dump_unrolled_instances(o << " (instantiated)", df);
		}
	}
	return o;
}

//=============================================================================
// class param_value_collection method definitions

#if 0
count_ptr<meta_instance_reference_base>
param_value_collection::make_meta_instance_reference(void) const {
	ICE_NEVER_CALL(cerr);
	return count_ptr<meta_instance_reference_base>(NULL);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !USE_INSTANCE_PLACEHOLDERS
count_ptr<nonmeta_instance_reference_base>
param_value_collection::make_nonmeta_instance_reference(void) const {
	ICE_NEVER_CALL(cerr);
	return count_ptr<nonmeta_instance_reference_base>(NULL);
}
#endif

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
#if !USE_INSTANCE_PLACEHOLDERS
datatype_instance_collection::datatype_instance_collection(
		const scopespace& o, const string& n, const size_t d) :
		parent_type(o, n, d) {
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
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_COLLECTION_CC__

