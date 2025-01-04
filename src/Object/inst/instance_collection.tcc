/**
	\file "Object/inst/instance_collection.tcc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file originally came from 
		"Object/art_object_instance_collection.tcc"
		in a previous life.  
	$Id: instance_collection.tcc,v 1.55 2011/02/08 22:32:48 fang Exp $
	TODO: trim includes
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_COLLECTION_TCC__
#define	__HAC_OBJECT_INST_INSTANCE_COLLECTION_TCC__

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// overridable debug switches

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include <exception>
#include <iostream>
#include <algorithm>
#include <functional>
#include <vector>
#include <list>

// experimental: suppressing automatic template instantiation
#include "Object/common/extern_templates.hh"

#include "Object/inst/instance_collection.hh"
#include "Object/inst/instance_array.hh"
#include "Object/inst/instance_scalar.hh"
#include "Object/inst/instance_placeholder.hh"
// #include "Object/inst/alias_actuals.tcc"
#include "Object/inst/alias_actuals.hh"
#include "Object/inst/subinstance_manager.tcc"
#include "Object/inst/instance_pool.tcc"
#include "Object/inst/internal_aliases_policy.hh"
#include "Object/inst/port_alias_tracker.tcc"
#include "Object/expr/const_index.hh"
#include "Object/expr/const_range.hh"
#include "Object/expr/const_param_expr_list.hh"		// for debug only
#include "Object/expr/const_index_list.hh"
#include "Object/expr/const_range_list.hh"
#include "Object/def/definition_base.hh"
#include "Object/common/namespace.hh"
#include "Object/type/canonical_type.hh"
#include "Object/ref/meta_instance_reference_subtypes.hh"
#include "Object/ref/nonmeta_instance_reference_subtypes.hh"
#include "Object/ref/simple_nonmeta_instance_reference.hh"
#include "Object/ref/simple_meta_instance_reference.hh"
#include "Object/unroll/instantiation_statement_base.hh"
#include "Object/def/footprint.hh"
#include "Object/global_entry.tcc"
#include "Object/unroll/instantiation_statement.hh"
#include "Object/inst/sparse_collection.tcc"
#include "Object/inst/element_key_dumper.hh"
#include "Object/inst/port_formal_array.tcc"
#include "Object/inst/port_actual_collection.tcc"
#include "Object/inst/collection_traits.hh"
#include "Object/inst/collection_pool.tcc"		// for lookup_index
#if PROCESS_DEFINITION_IS_NAMESPACE
#include "Object/module.hh"
#endif
#include "common/ICE.hh"

#include "util/multikey_assoc.tcc"
#include "util/packed_array.tcc"
#include "util/memory/count_ptr.tcc"

#include "util/persistent_object_manager.tcc"
#include "util/indent.hh"
#include "util/what.hh"
#include "util/stacktrace.hh"
#include "util/static_trace.hh"
#if __cplusplus < 201103L
#include "util/compose.hh"
#include "util/binders.hh"
#include "util/dereference.hh"
#endif

#if ENABLE_STACKTRACE
#include <iterator>
#endif

//=============================================================================

namespace HAC {
namespace entity {
using std::string;
#if __cplusplus < 201103L
using std::_Select1st;
using std::for_each;
USING_UTIL_COMPOSE
using util::dereference;
using std::mem_fun_ref;
using util::bind2nd_argval;
#endif
#include "util/using_ostream.hh"
using util::multikey_generator;
using util::multikey;
using util::value_writer;
using util::value_reader;
using util::write_value;
using util::read_value;
using util::indent;
using util::auto_indent;
using util::persistent_traits;

//=============================================================================
// class collection_interface method definitions

COLLECTION_INTERFACE_TEMPLATE_SIGNATURE
void
COLLECTION_INTERFACE_CLASS::write_local_pointer(
		const footprint& f, ostream& o) const {
	const unsigned char t = traits_type::type_tag_enum_value;
	write_value(o, t);
	this->write_pointer(o,
		f.template get_instance_collection_pool_bundle<Tag>());
}

//=============================================================================
// class instance_collection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
INSTANCE_COLLECTION_CLASS::instance_collection() :
		parent_type(), 
		collection_type_manager_parent_type(), 
		footprint_ref(NULL), 
		source_placeholder(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
INSTANCE_COLLECTION_CLASS::instance_collection(const footprint& f, 
			const instance_placeholder_ptr_type p) :
		parent_type(), 
		collection_type_manager_parent_type(), 
		footprint_ref(&f), 
		source_placeholder(p) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
INSTANCE_COLLECTION_CLASS::~instance_collection() {
	STACKTRACE_DTOR("~instance_collection<>()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
const INSTANCE_COLLECTION_CLASS&
INSTANCE_COLLECTION_CLASS::get_canonical_collection(void) const {
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only implemented by instance_scalar and port_formal_array
	subclasses, which are legal port structures.
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
never_ptr<physical_instance_collection>
INSTANCE_COLLECTION_CLASS::deep_copy(footprint&) const {
	ICE_NEVER_CALL(cerr);
	return never_ptr<physical_instance_collection>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print qualified name using the footprint back-reference 
	with complete type parameters.
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
ostream&
INSTANCE_COLLECTION_CLASS::dump_hierarchical_name(ostream& o, 
		const dump_flags& df) const {
	// copied from instance_placeholder_base::dump_qualified_name()
	const instance_placeholder_base::owner_ptr_type
		owner(source_placeholder->get_owner());
#if PROCESS_DEFINITION_IS_NAMESPACE
if (!owner.is_a<const module>()) {
#endif
	const never_ptr<const name_space>
		n(owner.template is_a<const name_space>());
	if (n) {
		if (!n->is_global_namespace()) {
			if (owner.is_a<const definition_base>()) {
#if PROCESS_DEFINITION_IS_NAMESPACE
				if (df.show_definition_owner)
#endif
					footprint_ref->dump_type(o) << "::";
			} else if (df.show_namespace_owner) {
				n->dump_qualified_name(o, df) << "::";
			}
		}
	} else if (owner->dump_include_parent(df)) {
		footprint_ref->dump_type(o) << "::";
		// if not owned by namespace!
	}
#if PROCESS_DEFINITION_IS_NAMESPACE
}
#endif
	o << source_placeholder->get_name();
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This needs to be specialized with a functor...
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
ostream&
INSTANCE_COLLECTION_CLASS::type_dump(ostream& o) const {
	typename collection_type_manager_parent_type::dumper dump_it(o);
	return dump_it(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Covariant return of a virtual function.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
never_ptr<const physical_instance_placeholder>
INSTANCE_COLLECTION_CLASS::get_placeholder_base(void) const {
	return this->source_placeholder;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the type-reference given by the first instantiation
	statement (which may be predicated).  
	This is not guaranteed to be the *final* type of the collection.  
	TODO: Perhaps rename this to make a clearer distinction?
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<const fundamental_type_reference>
INSTANCE_COLLECTION_CLASS::get_unresolved_type_ref(void) const {
	NEVER_NULL(this->source_placeholder);
	return this->source_placeholder->get_unresolved_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Must match exact type, i.e. be connectibly type equivalent.  
	Includes relaxed parameters, if applicable.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
bool
INSTANCE_COLLECTION_CLASS::must_be_collectibly_type_equivalent(
		const this_type& c) const {
	return collection_type_manager_parent_type::
		must_be_collectibly_type_equivalent(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets the type of the collection during the first instantiation
	of any of its members.  
	Called from instantiation_statement.
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
good_bool
INSTANCE_COLLECTION_CLASS::establish_collection_type(
		const instance_collection_parameter_type& t) {
	return collection_type_manager_parent_type::commit_type_first_time(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
bool
INSTANCE_COLLECTION_CLASS::has_relaxed_type(void) const {
	return collection_type_manager_parent_type::is_relaxed_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	During unroll phase, this commits the type of the collection.  
	\param t the data integer type reference, containing width, 
		must already be resolved to a const_param_expr_list.  
	\return false on success, true on error.  
	\post the integer width is fixed for the rest of the program.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
bad_bool
INSTANCE_COLLECTION_CLASS::check_established_type(
		const instance_collection_parameter_type& t) const {
	// functor, specialized for each class
	return collection_type_manager_parent_type::check_type(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need to return a legitmate reference to a parameter list!
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
never_ptr<const const_param_expr_list>
INSTANCE_COLLECTION_CLASS::get_actual_param_list(void) const {
	STACKTRACE("instance_collection::get_actual_param_list()");
	return never_ptr<const const_param_expr_list>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This prefixes a footprint-translated pointer with a reference
	to the external footprint.  
	Called by port_actual_collection.  
	TODO: consider an abbreviated form just uses a port_formal 
	positional number.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::write_external_pointer(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, this->footprint_ref);
	const collection_pool_bundle_type&
		pb(this->footprint_ref->
			template get_instance_collection_pool_bundle<Tag>());
	this->write_pointer(o, pb);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Counterpart to instance_collection::write_external_pointer()
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
never_ptr<const INSTANCE_COLLECTION_CLASS>
INSTANCE_COLLECTION_CLASS::read_external_pointer(
		const persistent_object_manager& m, istream& i) {
	never_ptr<footprint> ext_fp;
	m.read_pointer(i, ext_fp);
	NEVER_NULL(ext_fp);
	m.load_object_once(&*ext_fp);	// must load before looking up!
	const never_ptr<const parent_type>
		p(ext_fp->template get_instance_collection_pool_bundle<Tag>()
		.read_pointer(i));
	NEVER_NULL(p);
	const never_ptr<const this_type>
		ret(p.template is_a<const this_type>());
	NEVER_NULL(ret);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	initial_instantiation_statement_ptr is permitted to be NULL
	for instance collections that belong to footprints.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_VERBOSE;
//	STACKTRACE_PERSISTENT("instance_collection<Tag>::collect_base()");
	STACKTRACE_INDENT_PRINT("this @ " << this << endl);
	// don't bother visiting parent footprint?
	collection_type_manager_parent_type::collect_transient_info_base(m);
	if (this->source_placeholder) {
		source_placeholder->collect_transient_info(m);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_PERSISTENT("instance_collection<Tag>::write_base()");
	// owning footprint? -- responsibility of owner footprint
	// to restores back-references
	collection_type_manager_parent_type::write_object_base(m, o);
	m.write_pointer(o, this->source_placeholder);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param f is the owning footprint to re-establish back-reference.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::load_object_base(
		const footprint& f, 
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT("instance_collection<Tag>::load_base()");
	// owning footprint? re-establish back-reference
	this->footprint_ref = never_ptr<const footprint>(&f);
	collection_type_manager_parent_type::load_object_base(m, i);
	m.read_pointer(i, this->source_placeholder);
	// TODO: need to load in advance to make the key available
	// what about placeholder's parent namespaces???
	NEVER_NULL(this->source_placeholder);
	m.load_object_once(const_cast<instance_placeholder_type*>(
		&*this->source_placeholder));
}

//=============================================================================
// class array method definitions

INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::instance_array() : 
		parent_type(), 
		collection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::instance_array(const this_type& t) : 
		parent_type(t), 
		collection() {		// don't copy
	INVARIANT(t.collection.empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::instance_array(const footprint& f, 
		const instance_placeholder_ptr_type p) :
		parent_type(f, p), 
		collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::~instance_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
bool
INSTANCE_ARRAY_CLASS::is_partially_unrolled(void) const {
	return !collection.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return false.  Sparse collections can never be formal.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
bool
INSTANCE_ARRAY_CLASS::is_formal(void) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ARRAY_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre the alias a MUST belong to this collection!
		Will assert fail if this is not the case.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ARRAY_CLASS::dump_element_key(ostream& o,
		const instance_alias_info_type& a) const {
	const key_type& k(this->collection.lookup_key(a));
	return element_key_dumper<D>()(o, k);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre the alias a MUST belong to this collection!
		Will assert fail if this is not the case.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ARRAY_CLASS::dump_element_key(ostream& o, const size_t i) const {
	const key_type& k(this->collection.lookup_key(i));
	return element_key_dumper<D>()(o, k);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the key corresponding to the referenced element.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
multikey_index_type
INSTANCE_ARRAY_CLASS::lookup_key(const instance_alias_info_type& a) const {
	// NB: specialized to scalar pint_value_type for D == 1!
	return multikey<D,pint_value_type>(this->collection.lookup_key(a));
	// convert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the key corresponding to the referenced element.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
multikey_index_type
INSTANCE_ARRAY_CLASS::lookup_key(const size_t i) const {
	// NB: specialized to scalar pint_value_type for D == 1!
	return multikey<D,pint_value_type>(this->collection.lookup_key(i));
	// convert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the internal unique nonzero ID number corresponding
	to the alias argument, zero to signal not found.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
size_t
INSTANCE_ARRAY_CLASS::lookup_index(const instance_alias_info_type& a) const {
	return this->collection.lookup_index(a);	// already 1-based
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return 0 if not found.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
size_t
INSTANCE_ARRAY_CLASS::lookup_index(const multikey_index_type& k) const {
	INVARIANT(k.dimensions() == D);
	return this->collection.lookup_index(key_type(k));
	// already 1-based
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
size_t
INSTANCE_ARRAY_CLASS::collection_size(void) const {
	return this->collection.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return reference to element in this collection with the 
		same corresponding key as the alias argument in
		its parent collection.  
	Will assert fail if not found.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
typename INSTANCE_ARRAY_CLASS::instance_alias_info_type&
INSTANCE_ARRAY_CLASS::get_corresponding_element(
		const collection_interface_type& p, 
		const instance_alias_info_type& a) {
	const this_type& t(IS_A(const this_type&, p));	// assert dynamic cast
	const key_type& k(t.collection.lookup_key(a));
	return this->collection[k];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dumps all instances recursively, including subinstances (ports).  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ARRAY_CLASS::dump_unrolled_instances(ostream& o,
		const dump_flags& df) const {
#if 0
	o << "[dump flags: " << (df.show_definition_owner ? "(def) " : " ") <<
		(df.show_namespace_owner ? "(ns) " : " ") <<
		(df.show_leading_scope ? "(::)]" : "]");
#endif
#if __cplusplus >= 201103L
        typename parent_type::key_dumper d(o, df);
        for (const auto& elem : this->collection) {
          d(elem);
        }
#else
	for_each(this->collection.begin(), this->collection.end(),
		typename parent_type::key_dumper(o, df));
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Is smart enough to distinguish between scalar (keyless)
	instance dumping and indexed dumping. 
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE 
ostream&
INSTANCE_COLLECTION_CLASS::key_dumper::operator () (
		const instance_alias_info_type& p) {
if (p.container) {
	const size_t dim = p.container->get_dimensions();
	if (dim)
		p.dump_key(os << auto_indent);
	if (p.container->get_canonical_collection().has_relaxed_type())
		p.dump_actuals(os);
	os << " = ";
	const instance_alias_info_type* peek = p.peek();
	NEVER_NULL(peek);
	peek->dump_hierarchical_name(os, df);
	if (p.instance_index)
		os << " (" << p.instance_index << ')';
	p.dump_attributes(os);	// attributes of *this* alias only (p)
	p.dump_ports(os << ' ', df);
	if (dim)
		os << endl;
} else {
	// this is possible only if dumping in mid-construction
	// should only ever happen while debuygging
	os << "???";
}
	return os;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates integer parameters at the specified indices.
	\param actuals the (optional) relaxed template arguments, 
		which is only applicable to processes.  
	\param i fully-specified range of indices to instantiate.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ARRAY_CLASS::instantiate_indices(const const_range_list& ranges, 
		const unroll_context& c) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("this = " << this << endl);
	if (!ranges.is_valid()) {
		ranges.dump(cerr << "ERROR: invalid instantiation range list: ",
			expr_dump_context::default_value) << endl;
		return good_bool(false);
	}
	// for process only (or anything with relaxed typing)
	if (!collection_type_manager_parent_type::
			complete_type_definition_footprint(
				instance_relaxed_actuals_type(NULL)).good) {
		return good_bool(false);
	}
	// now iterate through, unrolling one at a time...
	// stop as soon as there is a conflict
	// later: factor this out into common helper class
	multikey_generator<D, pint_value_type> key_gen;
#if ENABLE_STACKTRACE
	ranges.dump(STACKTRACE_INDENT << "range: ",
		expr_dump_context::default_value) << endl;
#endif
	ranges.make_multikey_generator(key_gen);
	key_gen.initialize();
	bool err = false;
	do {
		instance_alias_info_type* const new_elem =
			collection.insert(key_gen, instance_alias_info_type());
		if (new_elem) {
			// ALERT: shouldn't relaxed actuals be attached
			// before calling recursive instantiate?
			// only so if ports ever depend on relaxed parameters.  
			// We've established that they do not, see NOTES.  
			try {
			new_elem->instantiate(never_ptr<const this_type>(this),
				c.as_target_footprint());
			// can throw!
			} catch (...) {
				err = true;
			}
		} else {
			// found one that already exists!
			// more detailed message, please!
			cerr << "ERROR: Index " << key_gen << " of ";
			what(cerr) << ' ' <<
				this->source_placeholder->get_qualified_name() <<
				" already instantiated!" << endl;
			err = true;
		}
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	return good_bool(!err);
}	// end method instantiate_indices

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns local instance ids to all aliases. 
	\pre all aliases have been played (internal and external)
		and union-find structures are final.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ARRAY_CLASS::allocate_local_instance_ids(footprint& f) {
	STACKTRACE_VERBOSE;
	iterator i(collection.begin());
	const iterator e(collection.end());
	for ( ; i!=e; i++) {
		if (!const_cast<element_type&>(*i).assign_local_instance_id(f))
			return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        Expands indices which may be under-specified into explicit
        indices for the implicit subslice, if it is densely packed.
        Depends on the current state of the collection.
        \param l is list of indices, which may be under-specified,
                or even empty.
        \return fully-specified index list, or empty list if there is error.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
const_index_list
INSTANCE_ARRAY_CLASS::resolve_indices(const const_index_list& l) const {
	typedef	util::multikey_assoc_compact_helper<D,pint_value_type>
						compact_helper_type;
	STACKTRACE_VERBOSE;
	const size_t l_size = l.size();
	if (D == l_size) {
		// already fully specified
		return l;
	}
	// convert indices to pair of list of multikeys
	if (!l_size) {
		return const_index_list(l, compact_helper_type::is_compact(
				this->collection.get_key_index_map()));
	}
	// else construct slice
	list<pint_value_type> lower_list, upper_list;
#if __cplusplus >= 201103L
        for (const auto& index : l) {
          lower_list.push_back(index->lower_bound());
          upper_list.push_back(index->upper_bound());
        }
#else
	transform(l.begin(), l.end(), back_inserter(lower_list),
		unary_compose(
			mem_fun_ref(&const_index::lower_bound),
			dereference<const_index_list::value_type>()
		)
	);
	transform(l.begin(), l.end(), back_inserter(upper_list),
		unary_compose(
			mem_fun_ref(&const_index::upper_bound),
			dereference<const_index_list::value_type>()
		)
	);
#endif
	return const_index_list(l,
		compact_helper_type::is_compact_slice(
			this->collection.get_key_index_map(), 
			lower_list, upper_list));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Associative lookup using native key type.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
never_ptr<typename INSTANCE_ARRAY_CLASS::element_type>
INSTANCE_ARRAY_CLASS::operator [] (const key_type& index) const {
	typedef	never_ptr<element_type>		ptr_return_type;
	const const_iterator
		it(this->collection.find_iterator(index));
	if (it == this->collection.end()) {
		this->type_dump(
			cerr << "ERROR: reference to uninstantiated ") << " "
				<< this->source_placeholder->get_qualified_name()
				<< " at index: " << index << endl;
		return ptr_return_type(NULL);
	}
	const element_type& b(*it);
	if (b.valid()) {
		// unfortunately, this cast is necessary
		// safe because we know b is not a reference to a temporary
		return ptr_return_type(const_cast<element_type*>(&b));
	} else {
		// remove the blank we added?
		// not necessary, but could keep the collection "clean"
		this->type_dump(
			cerr << "ERROR: reference to uninstantiated ") << " "
				<< this->source_placeholder->get_qualified_name()
				<< " at index: " << index << endl;
		return ptr_return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return valid instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
typename INSTANCE_ARRAY_CLASS::instance_alias_info_ptr_type
INSTANCE_ARRAY_CLASS::lookup_instance(const multikey_index_type& i) const {
	INVARIANT(D == i.dimensions());
	const key_type index(i);
	STACKTRACE_INDENT_PRINT("i = " << i << endl);
	STACKTRACE_INDENT_PRINT("index = " << index << endl);
	return (*this)[index];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param l list in which to accumulate instance references.
	\param r the ranges, must be valid, and fully resolved.
	\return false on error, e.g. if value doesn't exist or
		is uninitialized; true on success.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
bool
INSTANCE_ARRAY_CLASS::lookup_instance_collection(
		std::list<instance_alias_info_ptr_type>& l,
		const const_range_list& r) const {
	INVARIANT(!r.empty());
	key_generator_type key_gen;
	r.make_multikey_generator(key_gen);
	key_gen.initialize();
	bool ret = true;
	do {
		const const_iterator
			it(this->collection.find_iterator(key_gen));
		if (it == collection.end()) {
			this->type_dump(
				cerr << "FATAL: reference to uninstantiated ")
					<< " index " << key_gen << endl;
			l.push_back(instance_alias_info_ptr_type(NULL));
			ret = false;
		} else {
		const element_type& pi(*it);
		// pi MUST be valid if it belongs to an array
		if (pi.valid()) {
			l.push_back(instance_alias_info_ptr_type(
				const_cast<element_type*>(&pi)));
		} else {
			this->type_dump(
				cerr << "FATAL: reference to uninstantiated ")
					<< " index " << key_gen << endl;
			l.push_back(instance_alias_info_ptr_type(NULL));
			ret = false;
		}
		}
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Packs resolved range of aliases into a collection.  
	\pre array a must already be sized properly, do not resize here.  
	\return true on error, else false.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
bad_bool
INSTANCE_ARRAY_CLASS::unroll_aliases(const multikey_index_type& l, 
		const multikey_index_type& u, alias_collection_type& a) const {
	typedef	typename alias_collection_type::iterator
						alias_collection_iterator;
	STACKTRACE_VERBOSE;
	const key_type lower(l);	// this will assert dimension match!
	const key_type upper(u);	// this will assert dimension match!
	key_generator_type key_gen(lower, upper);
	key_gen.initialize();
	bool ret = false;
	alias_collection_iterator a_iter(a.begin());
	const const_iterator collection_end(this->collection.end());
	// maybe INVARIANT(sizes == iterations)
	do {
		// really is a monotonic incremental search, 
		// don't need log(N) lookup each time, fix later...
		const const_iterator
			it(this->collection.find_iterator(key_gen));
		if (it == collection_end) {
			this->type_dump(
			cerr << "FATAL: reference to uninstantiated ") <<
				" index " << key_gen << endl;
			*a_iter = never_ptr<element_type>(NULL);
			ret = true;
		} else {
			const element_type& pi(*it);
			*a_iter = never_ptr<element_type>(
				const_cast<element_type*>(&pi));
		}
		++a_iter;
		key_gen++;
	} while (key_gen != key_gen.lower_corner);
	INVARIANT(a_iter == a.end());
	return bad_bool(ret);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Grab all aliases in a flat collection.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::get_all_aliases(
		std::vector<const_instance_alias_info_ptr_type>& aliases) const {
	aliases.reserve(this->collection_size());
	const_iterator i(this->collection.begin()), 
		e(this->collection.end());
	for ( ; i!=e; ++i) {
		aliases.push_back(const_instance_alias_info_ptr_type(&*i));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively aliases public ports between two instance collections.  
	\param p subinstance collection to connect to this.  
	\pre this has identical type to p
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ARRAY_CLASS::connect_port_aliases_recursive(
		physical_instance_collection& p, 
		target_context& c) {
	STACKTRACE_VERBOSE;
	this_type& t(IS_A(this_type&, p));	// assert dynamic_cast
	INVARIANT(this->collection.size() == t.collection.size());
	iterator i(this->collection.begin());
	iterator j(t.collection.begin());
	const iterator e(this->collection.end());
	for ( ; i!=e; ++i, ++j) {
		// unfortunately, set iterators only return const refs
		// we only intend to modify the value without modifying the key
		element_type& ii(const_cast<element_type&>(
			AS_A(const element_type&, *i)));
		element_type& jj(const_cast<element_type&>(
			AS_A(const element_type&, *j)));
		// possibly redundant port type checking is unnecessary
		if (!element_type::checked_connect_port(ii, jj, c).good) {
			// error message?
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is called to replay port aliases when instantiating
	a process.  The port aliases do not need to be rechecked.
	\param p subinstance collection to connect to this.  
	\pre this has identical type to p
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::reconnect_port_aliases_recursive(
		physical_instance_collection& p) {
	STACKTRACE_VERBOSE;
	this_type& t(IS_A(this_type&, p));	// assert dynamic_cast
	INVARIANT(this->collection.size() == t.collection.size());
	iterator i(this->collection.begin());
	iterator j(t.collection.begin());
	const iterator e(this->collection.end());
	for ( ; i!=e; ++i, ++j) {
		// unfortunately, set iterators only return const refs
		// we only intend to modify the value without modifying the key
		element_type& ii(const_cast<element_type&>(
			AS_A(const element_type&, *i)));
		element_type& jj(const_cast<element_type&>(
			AS_A(const element_type&, *j)));
		// possibly redundant port type checking is unnecessary
		element_type::replay_connect_port(ii, jj);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Walks the entire collection and create definition footprints of
	constituent types.  
	The call to internal_alias_policy::connect also replays the 
	internal aliases for each complete type to each instance of that type.  
	(Consider renaming: create_dependent_types_and_replay_internal_aliases.)
	TODO: optimize with specialization for non-recursive types.  
		(low priority)
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ARRAY_CLASS::create_dependent_types(const footprint& top) {
	STACKTRACE_VERBOSE;
	iterator i(this->collection.begin());
	const iterator e(this->collection.end());
if (i == e) {
	// no instances in this collection were instantiated (conditional)
	return good_bool(true);
}
if (this->has_relaxed_type()) {
	STACKTRACE_INDENT_PRINT("collection has relaxed type" << endl);
	for ( ; i!=e; ++i) {
	// not canonical alias
	if (i->get_relaxed_actuals()) {
	// then postpone until relaxed template parameters are bound
	// can this be taken out of loop?
		STACKTRACE_INDENT_PRINT("have relaxed actuals: ");
#if ENABLE_STACKTRACE
		i->get_relaxed_actuals()->dump(cerr,
			expr_dump_context::default_value);
		cerr << endl;
#endif
		if (!element_type::create_dependent_types(*i, top).good)
			return good_bool(false);
		element_type& ii(const_cast<element_type&>(
			AS_A(const element_type&, *i)));
		// this call will deduce the canonical type from ii
		if (!internal_alias_policy::connect(ii).good) {
			return good_bool(false);
		}
	}
	}
} else {
	STACKTRACE_INDENT_PRINT("collection has strict type" << endl);
	// type of container is already strict, 
	// evaluate it once and re-use it when replaying internal aliases
	const typename parent_type::instance_collection_parameter_type
		t(collection_type_manager_parent_type::__get_raw_type());
	if (!this->create_definition_footprint(t, top).good) {
		return good_bool(false);
	}
	for ( ; i!=e; ++i) {
		element_type& ii(const_cast<element_type&>(
			AS_A(const element_type&, *i)));
		if (!internal_alias_policy::connect(ii, t).good) {
			return good_bool(false);
		}
	}
}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre already called create_dependent_types.
	NOTE: this should not be const-qualified, because we allow
	the port_alias_tracker to modify and update the aliases.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::collect_port_aliases(port_alias_tracker& t) const {
	STACKTRACE_VERBOSE;
#if 0
	const_iterator i(this->collection.begin());
	const const_iterator e(this->collection.end());
	for ( ; i!=e; i++) {
		// fix-remove const casting...
		element_type& ii(const_cast<element_type&>(*i));
		INVARIANT(ii.instance_index);
		// 0 is not an acceptable index
		t.template get_id_map<Tag>()[ii.instance_index]
			.push_back(never_ptr<element_type>(&ii));
#if RECURSE_COLLECT_ALIASES
		ii.collect_port_aliases(t);
#else
		// no need to recurse because pool_manager visits
		// every instance collection already
#endif
	}
#else
#if __cplusplus >= 201103L
        typename parent_type::scope_alias_collector c(t);
        for (const auto& elem : collection) {
          c(elem);
        }
#else
	// mmm... functional
	for_each(collection.begin(), collection.end(), 
		typename parent_type::scope_alias_collector(t));
#endif
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads a key from binary stream then returns a reference to the 
	indexed instance alias.  
	The index was (conditionally) written in
	instance_alias_info::write_next_connection().  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
typename INSTANCE_ARRAY_CLASS::instance_alias_info_type&
INSTANCE_ARRAY_CLASS::load_reference(istream& i) {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::load_reference()");
	size_t index;		// 1-indexed
	read_value(i, index);
	INVARIANT(index);
	element_type* const e = this->collection.find(index);
	NEVER_NULL(e);
	return *e;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor for collecting scope aliases into alias sets
	in the port_alias_tracker.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::scope_alias_collector::operator ()
		(const instance_alias_info_type& a) {
	typedef	instance_alias_info_type	element_type;
	// fix-remove const casting...
	// needed for the alias_reference_set pointers :X
	element_type& ii(const_cast<element_type&>(a));
	INVARIANT(ii.instance_index);
	// 0 is not an acceptable index
	// the following statement was split up to avoid ICE-ing g++-3.4.0
	typename port_alias_tracker_base<Tag>::map_type&
		tm(tracker.template get_id_map<Tag>());
	tm[ii.instance_index].push_back(never_ptr<element_type>(&ii));
#if RECURSE_COLLECT_ALIASES
	ii.collect_port_aliases(tracker);
#else
	// no need to recurse because pool_manager visits
	// every instance collection already
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Going to need some sort of element_reader counterpart.
	\param e is a reference to a INSTANCE_ALIAS_CLASS.
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::element_collector::operator ()
		(const instance_alias_info_type& e) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::element_collector::operator()");
	e.collect_transient_info_base(pom);
	// postpone connection writing until next phase
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Going to need some sort of element_reader counterpart.
	\param e is a reference to a INSTANCE_ALIAS_CLASS.
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::element_writer::operator () (
		const instance_alias_info_type& e) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::element_writer::operator()");
	// elements don't come with keys anymore, keys are managed separately
	e.write_object_base(fp, pom, os);
	// postpone connection writing until next phase
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This was greatly simplified after separating key from values
	in the sparse collection.  
	This must perfectly complement element_writer::operator().
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::element_loader::operator () (
		instance_alias_info_type& e) {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::element_loader::operator()");
	e.container = this->back_ref;
	e.load_object_base(this->fp, this->pom, this->is);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::connection_writer::operator() (
		const instance_alias_info_type& e) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::connection_writer::operator()");
	const instance_alias_info_type* const next(e.peek());
	NEVER_NULL(next);
	if (next != &e) {
		write_value<char>(os, 1);
		next->write_next_connection(pom, os);
	} else {
		write_value<char>(os, 0);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Must complement connection_writer::operator().
	const_cast is an unfortunate consequence of set only
	returning const references and const iterators, where we intend
	the non-key part of the object to me mutable.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::connection_loader::operator() (
		instance_alias_info_type& elem) {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::connection_loader::operator()");
	char c;
	read_value(this->is, c);
	if (c) {
		// lookup the instance in the collection referenced
		// and connect them
		elem.load_next_connection(this->pom, this->is);
	} else {
		INVARIANT(elem.peek() == &elem);
	}
	// else just leave it pointing to itself, 
	// which was how it was constructed
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::finalize_substructure_aliases(
		const unroll_context& c) {
	iterator i(this->collection.begin());
	const iterator e(this->collection.end());
	for ( ; i!=e; ++i) {
		// should synchronize relaxed template parameters
		// instantiate and re-connect ports recursively as needed
		i->finalize_find(c);
		// catch/rethrow exception?
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Applies direction flags to all aliases in collection, 
	applicable only to channel types.  
	\param f the direction flags to set.  
	\return good if successful.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ARRAY_CLASS::set_alias_connection_flags(
		const connection_flags_type f) {
#if __cplusplus >= 201103L
        typename element_type::connection_flag_setter s(f);
        for (auto& elem : this->collection) {
          s(elem);
        }
        return s.status;
#else
	return for_each(this->collection.begin(), this->collection.end(), 
		typename element_type::connection_flag_setter(f)).status;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For now, sparse collections never referenced as formals, 
	so this should never be called.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::instantiate_actuals_from_formals(
		port_actuals_type&,
		target_context&) const {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visitor.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::accept(alias_visitor& v) const {
	for_each(this->collection.begin(), this->collection.end(),
		bind2nd_argval(mem_fun_ref(&element_type::accept), v));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::collect_transients()");
	parent_type::collect_transient_info_base(m);
	for_each(this->collection.begin(), this->collection.end(), 
		typename parent_type::element_collector(m));
	// optimization for later: factor this out into a policy
	// so that collections without pointers to collect
	// may be skipped.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::write_pointer(ostream& o, 
		const collection_pool_bundle_type& pb) const {
	const unsigned char e = collection_traits<this_type>::ENUM_VALUE;
	write_value(o, e);
	const collection_index_entry::index_type index =
		pb.template get_collection_pool<this_type>()
			.lookup_index(*this);
	write_value(o, index);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is a sparse collection, thus, we need to write out keys along 
	with values.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::write_object(const footprint& fp, 
		const persistent_object_manager& m, ostream& f) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::write_object()");
	parent_type::write_object_base(m, f);
	// need to know how many members to expect
	const size_t s = this->collection.size();
	write_value(f, s);
	// to preserve key to index mapping, we must write out keys
	// by order of index, which may not be sorted w.r.t. keys.  
{
	// sparse_collections' public indices are 1-indexed
	// write sparse keys in order of index
	size_t i = 1;
	value_writer<key_type> write_key(f);
	for ( ; i<=s; ++i) {
		const key_type& k(collection.lookup_key(i));
		write_key(k);
	}
}
	const const_iterator
		b(this->collection.begin()), e(this->collection.end());
	for_each(b, e, typename parent_type::element_writer(fp, m, f));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only writes out connections.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::write_connections(const collection_pool_bundle_type& m, 
		ostream& f) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::write_connections()");
	for_each(collection.begin(), collection.end(), 
		typename parent_type::connection_writer(m, f));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: this also re-registers with the footprint's collection map.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::load_object(footprint& fp, 
		const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::load_object()");
	parent_type::load_object_base(fp, m, f);
	fp.register_collection_map_entry(
		this->source_placeholder->get_footprint_key(), 
		lookup_collection_pool_index_entry(
		fp.template get_instance_collection_pool_bundle<Tag>()
			.template get_collection_pool<this_type>(), *this));
	// procedure:
	// 1) load all instantiated indices *without* their connections
	//      let them start out pointing to themselves.  
	// 2) each element contains information to reconstruct, 
	//      we need temporary local storage for it.
	size_t _collection_size;
	read_value(f, _collection_size);
{
	size_t i = 0;
	// populate collection by key and index first, with default initialized
	// element values.  
	key_type temp_key;
	value_reader<key_type> read_key(f);
	for ( ; i < _collection_size; ++i) {
		read_key(temp_key);
		const element_type* const v =
			collection.insert(temp_key, element_type());
		// keys will automatically be reassociated with correct
		// index, because they are added in the exact same order
		// as before
		INVARIANT(v);	// must succeed
	}
}
	// now can we load connections at the same time?
	const iterator b(collection.begin()), e(collection.end());
	for_each(b, e, typename parent_type::element_loader(
			fp, m, f, never_ptr<const this_type>(this)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only loads in connections
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::load_connections(const collection_pool_bundle_type& m, 
		istream& f) {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::load_connections()");
	for_each(collection.begin(), collection.end(), 
		typename parent_type::connection_loader(m, f));
}

//=============================================================================
// class array method definitions (specialized)

INSTANCE_SCALAR_TEMPLATE_SIGNATURE
INSTANCE_SCALAR_CLASS::instance_array() : 
		parent_type(), 
		the_instance() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
INSTANCE_SCALAR_CLASS::instance_array(const footprint& f, 
		const instance_placeholder_ptr_type p) :
		parent_type(f, p), 
		the_instance() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
INSTANCE_SCALAR_CLASS::~instance_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
bool
INSTANCE_SCALAR_CLASS::is_partially_unrolled(void) const {
	return this->the_instance.valid();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if the originating placeholder was formal.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
bool
INSTANCE_SCALAR_CLASS::is_formal(void) const {
	return this->source_placeholder->is_port_formal();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
ostream&
INSTANCE_SCALAR_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param a the scalar alias contained.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
ostream&
INSTANCE_SCALAR_CLASS::dump_element_key(ostream& o,
		const instance_alias_info_type& a) const {
	INVARIANT(&a == &this->the_instance);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i public 1-based index, must be 1!
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
ostream&
INSTANCE_SCALAR_CLASS::dump_element_key(ostream& o,
		const size_t i) const {
	INVARIANT(i == 1);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return an empty multikey index because scalars aren't indexed.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
multikey_index_type
INSTANCE_SCALAR_CLASS::lookup_key(const instance_alias_info_type&) const {
	return multikey_index_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return an empty multikey index because scalars aren't indexed.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
multikey_index_type
INSTANCE_SCALAR_CLASS::lookup_key(const size_t) const {
	return multikey_index_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should never be called, as scalar aliases are not mapped
	to any collection's indices.  
	\return 0.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
size_t
INSTANCE_SCALAR_CLASS::lookup_index(const instance_alias_info_type& a) const {
	INVARIANT(&this->the_instance == &a);	// must be back-reference
	return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param k multikey_index, which should always be empty (0 dimensions).
	\return 1 always (1-based index).  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
size_t
INSTANCE_SCALAR_CLASS::lookup_index(const multikey_index_type& k) const {
	INVARIANT(!k.size());
	return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
size_t
INSTANCE_SCALAR_CLASS::collection_size(void) const {
	return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the corresponding instance referenced by an alias
		in a different collection (same population).  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
typename INSTANCE_SCALAR_CLASS::instance_alias_info_type&
INSTANCE_SCALAR_CLASS::get_corresponding_element(
		const collection_interface_type& p, 
		const instance_alias_info_type& a) {
	const this_type& t(IS_A(const this_type&, p));
	INVARIANT(&t.the_instance == &a);
	return this->the_instance;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
ostream&
INSTANCE_SCALAR_CLASS::dump_unrolled_instances(ostream& o,
		const dump_flags& df) const {
if (this->the_instance.container) {
	// no auto-indent, continued on same line
	// see physical_instance_collection::dump for reason why
	typename parent_type::key_dumper(o, df)(this->the_instance);
} else {
	// this only happens when dumping the collection before
	// it is complete.
	o << "[null container]";
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiate port actuals from a template of formals.
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
never_ptr<physical_instance_collection>
INSTANCE_SCALAR_CLASS::deep_copy(footprint& tf) const {
	collection_pool_bundle_type&
		pool(tf.template get_instance_collection_pool_bundle<Tag>());
	port_actuals_type* ret = pool.allocate_port_collection();
	NEVER_NULL(ret);
	// placement construct
	new (ret) port_actuals_type(never_ptr<const this_type>(this));
	instantiate_actuals_from_formals(*ret, tf);
	return never_ptr<physical_instance_collection>(ret);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates the_instance of integer datatype.
	Ideally, the error should never trigger because
	re-instantiation / redeclaration of a scalar instance
	is easily detected (and actually detected) during the compile phase.
	TODO: attaching of relaxed actuals should be policy-dependent!
	TODO: check template parameter constraints upon instantiation!
		Consider looking for SPEC-assert directives?
	\param i indices must be NULL because this is not an array.
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
good_bool
INSTANCE_SCALAR_CLASS::instantiate_indices(
		const const_range_list& r, 
		const unroll_context& c) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("this = " << this << endl);
	INVARIANT(r.empty());
	if (this->the_instance.valid()) {
		// should never happen, but just in case...
		this->type_dump(cerr << "ERROR: Scalar ") <<
			" already instantiated!" << endl;
		return good_bool(false);
	}
	// here we need an explicit instantiation (recursive)
	try {
	this->the_instance.instantiate(never_ptr<const this_type>(this),
		c.as_target_footprint());
	} catch (...) {
		return good_bool(false);
	}
	// for process only (or anything with relaxed typing)
	if (!collection_type_manager_parent_type::
			complete_type_definition_footprint(
				instance_relaxed_actuals_type(NULL)).good
			) {
		return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns local instance ids to all aliases. 
	\pre all aliases have been played (internal and external)
		and union-find structures are final.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
good_bool
INSTANCE_SCALAR_CLASS::allocate_local_instance_ids(footprint& f) {
	STACKTRACE_VERBOSE;
if (this->the_instance.valid()) {
	return good_bool(this->the_instance.assign_local_instance_id(f) != 0);
} else {
	// not instantiated due to conditional
	return good_bool(true);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This specialization isn't ever supposed to be called.
	\param l is list of indices, which may be under-specified,
		or even empty.
	\return empty index list, always.
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
const_index_list
INSTANCE_SCALAR_CLASS::resolve_indices(const const_index_list&) const {
	cerr << "WARNING: instance_array<Tag,0>::resolve_indices(const_index_list) "
		"always returns an empty list!" << endl;
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return valid instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
typename INSTANCE_SCALAR_CLASS::instance_alias_info_ptr_type
INSTANCE_SCALAR_CLASS::lookup_instance(const multikey_index_type&) const {
	typedef	typename INSTANCE_SCALAR_CLASS::instance_alias_info_ptr_type
						ptr_return_type;
	if (!this->the_instance.valid()) {
		this->type_dump(cerr << "ERROR: Reference to uninstantiated ")
			<< "!" << endl;
		return ptr_return_type(NULL);
	} else	return ptr_return_type(
		const_cast<instance_alias_info_type*>(
			&static_cast<const instance_alias_info_type&>(
				this->the_instance)));
	// ok to return non-const reference to the type, 
	// perhaps it should be declared mutable?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should never be called.  
	\return false to signal error.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
bool
INSTANCE_SCALAR_CLASS::lookup_instance_collection(
		std::list<instance_alias_info_ptr_type>&,
		const const_range_list& r) const {
	cerr << "WARNING: instance_array<Tag,0>::lookup_instance_collection(...) "
		"should never be called." << endl;
	INVARIANT(r.empty());
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::get_all_aliases(
		std::vector<const_instance_alias_info_ptr_type>& aliases) const {
	aliases.push_back(
		const_instance_alias_info_ptr_type(&this->the_instance));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Upper and lower bound arguments unused.  
	\return true on error, false on success.
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
bad_bool
INSTANCE_SCALAR_CLASS::unroll_aliases(const multikey_index_type&, 
		const multikey_index_type&, alias_collection_type& a) const {
	STACKTRACE_VERBOSE;
	if (this->the_instance.valid()) {
		*(a.begin()) = instance_alias_info_ptr_type(
			const_cast<instance_alias_info_type*>(
				&static_cast<const instance_alias_info_type&>(
					this->the_instance)));
		return bad_bool(false);
	} else {
		this->type_dump(cerr << "ERROR: Reference to uninstantiated ")
			<< "!" << endl;
		return bad_bool(true);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively aliases public ports between two instance collections.  
	\param p subinstance collection to connect to this.  
	\pre this has identical type to p
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
good_bool
INSTANCE_SCALAR_CLASS::connect_port_aliases_recursive(
		physical_instance_collection& p,
		target_context& c) {
	STACKTRACE_VERBOSE;
	this_type& t(IS_A(this_type&, p));	// assert dynamic_cast
	return instance_type::checked_connect_port(
		this->the_instance, t.the_instance, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For replaying established port aliases.
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::reconnect_port_aliases_recursive(
		physical_instance_collection& p) {
	STACKTRACE_VERBOSE;
	this_type& t(IS_A(this_type&, p));	// assert dynamic_cast
	return instance_type::replay_connect_port(
		this->the_instance, t.the_instance);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively creates footprints of complete types bottom-up.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
good_bool
INSTANCE_SCALAR_CLASS::create_dependent_types(const footprint& top) {
	STACKTRACE_VERBOSE;
if (!this->the_instance.valid()) {
	// uninstantiated scalar because of conditional
	return good_bool(true);
}
if (this->has_relaxed_type()) {
	// then postpone until relaxed template parameters are bound
	return good_bool(true);
} else {
	const typename parent_type::instance_collection_parameter_type
		t(collection_type_manager_parent_type::__get_raw_type());
	if (!this->create_definition_footprint(t, top).good) {
		return good_bool(false);
	}
}
	if (!internal_alias_policy::connect(
			const_cast<instance_type&>(this->the_instance)).good) {
		return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: redefine this method as non-const.
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::collect_port_aliases(port_alias_tracker& t) const {
if (this->the_instance.valid()) {
#if 0
	INVARIANT(this->the_instance.instance_index);
	// 0 is not an acceptable index
	t.template get_id_map<Tag>()[this->the_instance.instance_index]
		.push_back(never_ptr<instance_type>(
			&const_cast<instance_type&>(this->the_instance)));
#if RECURSE_COLLECT_ALIASES
	this->the_instance.collect_port_aliases(t);
#else
	// no need to recurse because pool_manager visits
	// every instance collection already
#endif
#else
	// could just declare anonymous temporary functor, whatever...
	typename parent_type::scope_alias_collector collect_it(t);
	collect_it(this->the_instance);
#endif
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
typename INSTANCE_SCALAR_CLASS::instance_alias_info_type&
INSTANCE_SCALAR_CLASS::load_reference(istream&) {
	STACKTRACE_PERSISTENT("instance_scalar::load_reference()");
	// no key to read!
	return this->the_instance;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::finalize_substructure_aliases(
		const unroll_context& c) {
	this->the_instance.finalize_find(c);
	// catch/rethrow exception?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
good_bool
INSTANCE_SCALAR_CLASS::set_alias_connection_flags(
		const connection_flags_type f) {
	return this->the_instance.set_connection_flags(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This initializes the scalar instance by propagating formal
	connection information to the actual.  
	This forward local information from callee to caller.  
	\throw general exception if creating dependent types fails.  
		TODO: better error handling.
	\pre dependent types are already created (and errors rejected)
		Is currently done in canonical_type::unroll_port_instances.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::instantiate_actuals_from_formals(
		port_actuals_type& p,
		target_context& c) const {
	INVARIANT(p.collection_size() == 1);
#if 0
	if (!create_dependent_types(*c.get_top_footprint()).good) {
		// error message, already have?
		THROW_EXIT;
	}
#endif
	// only one element to instantiate
	p.begin()->instantiate_actual_from_formal(
		never_ptr<const port_actuals_type>(&p), c, this->the_instance);
	// propagate actuals from formal to actual
	// propagate direction connection information from formal to actual
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::accept(alias_visitor& v) const {
	this->the_instance.accept(v);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("instance_scalar::collect_transients()");
	parent_type::collect_transient_info_base(m);
	this->the_instance.check(this);
	this->the_instance.collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Counterpart is footprint::read_pointer, 
	and collection_pool_bundle::read_pointer.
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::write_pointer(ostream& o, 
		const collection_pool_bundle_type& pb) const {
	const unsigned char e = collection_traits<this_type>::ENUM_VALUE;
	write_value(o, e);
	const collection_index_entry::index_type index = 
		pb.template get_collection_pool<this_type>()
			.lookup_index(*this);
	write_value(o, index);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::write_object(const footprint& fp, 
		const persistent_object_manager& m, ostream& f) const {
	STACKTRACE_PERSISTENT("instance_scalar::write_object()");
	parent_type::write_object_base(m, f);
	typename parent_type::element_writer(fp, m, f)(this->the_instance);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::write_connections(
		const collection_pool_bundle_type& m, 
		ostream& f) const {
	STACKTRACE_PERSISTENT("instance_scalar::write_connections()");
	typename parent_type::connection_writer(m, f)(this->the_instance);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: this also re-registers with the footprint's collection map.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::load_object(footprint& fp, 
		const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("instance_scalar::load_object()");
	parent_type::load_object_base(fp, m, f);
	fp.register_collection_map_entry(
		this->source_placeholder->get_footprint_key(), 
		lookup_collection_pool_index_entry(
		fp.template get_instance_collection_pool_bundle<Tag>()
			.template get_collection_pool<this_type>(), *this));
	typename parent_type::element_loader(
		fp, m, f, never_ptr<const this_type>(this))(this->the_instance);
	this->the_instance.check(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::load_connections(
		const collection_pool_bundle_type& m, 
		istream& f) {
	STACKTRACE_PERSISTENT("instance_scalar::load_connections()");
	typename parent_type::connection_loader(m, f)(this->the_instance);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_COLLECTION_TCC__

