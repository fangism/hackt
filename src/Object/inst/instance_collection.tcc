/**
	\file "Object/inst/instance_collection.tcc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file originally came from 
		"Object/art_object_instance_collection.tcc"
		in a previous life.  
	$Id: instance_collection.tcc,v 1.37.2.5 2006/10/29 20:04:57 fang Exp $
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

// experimental: suppressing automatic template instantiation
#include "Object/common/extern_templates.h"

#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_array.h"
#include "Object/inst/instance_scalar.h"
#include "Object/inst/instance_placeholder.h"
#include "Object/inst/alias_actuals.tcc"
#include "Object/inst/subinstance_manager.tcc"
#include "Object/inst/instance_pool.tcc"
#include "Object/inst/internal_aliases_policy.h"
#include "Object/inst/port_alias_tracker.h"
#include "Object/expr/const_index.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_param_expr_list.h"		// for debug only
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range_list.h"
#include "Object/def/definition_base.h"
#include "Object/type/canonical_type.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/nonmeta_instance_reference_subtypes.h"
#include "Object/ref/simple_nonmeta_instance_reference.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/unroll/instantiation_statement_base.h"
#include "Object/def/footprint.h"
#include "Object/global_entry.h"
#include "Object/port_context.h"
#include "Object/unroll/instantiation_statement.h"
#include "Object/inst/sparse_collection.tcc"
#include "Object/inst/element_key_dumper.h"
#include "Object/inst/port_formal_array.tcc"
#if USE_COLLECTION_INTERFACES
#include "Object/inst/port_actual_collection.tcc"
#endif
#include "common/ICE.h"

#include "util/multikey_assoc.tcc"
#include "util/packed_array.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/memory/chunk_map_pool.tcc"

#include "util/persistent_object_manager.tcc"
#include "util/indent.h"
#include "util/what.h"
#include "util/stacktrace.h"
#include "util/static_trace.h"
#include "util/compose.h"
#include "util/binders.h"
#include "util/dereference.h"

#if ENABLE_STACKTRACE
#include <iterator>
#endif

//=============================================================================

namespace HAC {
namespace entity {
using std::string;
using std::_Select1st;
using std::for_each;
#include "util/using_ostream.h"
using util::multikey_generator;
USING_UTIL_COMPOSE
using util::dereference;
using std::mem_fun_ref;
using util::bind2nd_argval;
using util::multikey;
using util::value_writer;
using util::value_reader;
using util::write_value;
using util::read_value;
using util::indent;
using util::auto_indent;
using util::persistent_traits;

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
		source_placeholder(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
INSTANCE_COLLECTION_CLASS::instance_collection(
			const instance_placeholder_ptr_type p) :
		parent_type(), 
		collection_type_manager_parent_type(), 
		source_placeholder(p) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
INSTANCE_COLLECTION_CLASS::~instance_collection() {
	STACKTRACE_DTOR("~instance_collection<>()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if USE_COLLECTION_INTERFACES
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
const INSTANCE_COLLECTION_CLASS&
INSTANCE_COLLECTION_CLASS::get_canonical_collection(void) const {
	return *this;
}
#endif

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
	\param p the placeholder back-reference pointer, 
		from which dimensions are inferred.  
	\return newly constructed d-dimensional array.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
INSTANCE_COLLECTION_CLASS*
INSTANCE_COLLECTION_CLASS::make_array(const instance_placeholder_ptr_type p) {
	const size_t d = p->get_dimensions();
	switch(d) {
		case 0: return new instance_array<Tag,0>(p);
		case 1: return new instance_array<Tag,1>(p);
		case 2: return new instance_array<Tag,2>(p);
		case 3: return new instance_array<Tag,3>(p);
		case 4: return new instance_array<Tag,4>(p);
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param p the placeholder back-reference pointer, 
		from which dimensions are inferred.  
	\return newly constructed d-dimensional array.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
INSTANCE_COLLECTION_CLASS*
INSTANCE_COLLECTION_CLASS::make_port_formal_array(
		const instance_placeholder_ptr_type p) {
	const size_t d = p->get_dimensions();
	switch(d) {
		case 0: return new instance_array<Tag,0>(p);
		case 1:
		case 2:
		case 3:
		case 4:
			return new port_formal_array<Tag>(p);
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
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
	STACKTRACE_PERSISTENT("instance_collection<Tag>::collect_base()");
	parent_type::collect_transient_info_base(m);
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
	parent_type::write_object_base(m, o);
	collection_type_manager_parent_type::write_object_base(m, o);
	m.write_pointer(o, this->source_placeholder);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::load_object_base(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT("instance_collection<Tag>::load_base()");
	parent_type::load_object_base(m, i);
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

#if POOL_ALLOCATE_INSTANCE_COLLECTIONS
// The following macro calls are intended to be equivalent to:
// TEMPLATE_CHUNK_MAP_POOL_ROBUST_STATIC_DEFINITION(
//	INSTANCE_ARRAY_TEMPLATE_SIGNATURE, INSTANCE_ARRAY_CLASS)
// but the preprocessor is retarded about arguments with commas in them, 
// so we forced to expand this macro by hand.  :S

INSTANCE_ARRAY_TEMPLATE_SIGNATURE
__SELF_CHUNK_MAP_POOL_STATIC_INIT(EMPTY_ARG, typename, INSTANCE_ARRAY_CLASS)

INSTANCE_ARRAY_TEMPLATE_SIGNATURE
__CHUNK_MAP_POOL_ROBUST_STATIC_GET_POOL(EMPTY_ARG, typename, INSTANCE_ARRAY_CLASS)

INSTANCE_ARRAY_TEMPLATE_SIGNATURE
__CHUNK_MAP_POOL_ROBUST_OPERATOR_NEW(EMPTY_ARG, INSTANCE_ARRAY_CLASS)

INSTANCE_ARRAY_TEMPLATE_SIGNATURE
__CHUNK_MAP_POOL_ROBUST_OPERATOR_PLACEMENT_NEW(EMPTY_ARG, INSTANCE_ARRAY_CLASS)

INSTANCE_ARRAY_TEMPLATE_SIGNATURE
__CHUNK_MAP_POOL_ROBUST_OPERATOR_DELETE(EMPTY_ARG, INSTANCE_ARRAY_CLASS)
#endif	// POOL_ALLOCATE_INSTANCE_COLLECTIONS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::instance_array() : 
		parent_type(), 
		collection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::instance_array(const instance_placeholder_ptr_type p) :
		parent_type(p), collection() {
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
#if USE_COLLECTION_INTERFACES
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
#endif

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
#if USE_COLLECTION_INTERFACES
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
#endif

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
#if USE_COLLECTION_INTERFACES
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
#endif

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
	for_each(this->collection.begin(), this->collection.end(),
		typename parent_type::key_dumper(o, df));
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
	NEVER_NULL(p.container);
	const size_t dim = p.container->get_dimensions();
	if (dim)
		p.dump_key(os << auto_indent);
#if ALLOCATE_PORT_ACTUAL_COLLECTIONS
	if (p.container->get_canonical_collection().has_relaxed_type())
#else
	if (p.container->has_relaxed_type())
#endif
		p.dump_actuals(os);
	os << " = ";
	NEVER_NULL(p.peek());
	p.peek()->dump_hierarchical_name(os, df);
	if (p.instance_index)
		os << " (" << p.instance_index << ')';
	p.dump_ports(os << ' ', df);
	if (dim)
		os << endl;
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
		const instance_relaxed_actuals_type& actuals, 
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
			complete_type_definition_footprint(actuals).good) {
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
			// then insertion of new value was successful
			new_elem->instantiate(
				never_ptr<const this_type>(this), c);
			// set its relaxed actuals!!! (if appropriate)
			if (actuals) {
			const bool attached(new_elem->attach_actuals(actuals));
			if (!attached) {
				cerr << "ERROR: attaching relaxed actuals to "
					<< this->source_placeholder->get_qualified_name() <<
					key_gen << endl;
				err = true;
			}
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
		typename default_list<instance_alias_info_ptr_type>::type& l,
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
	typedef	typename alias_collection_type::key_type
						collection_key_type;
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
	Recursively aliases public ports between two instance collections.  
	\param p subinstance collection to connect to this.  
	\pre this has identical type to p
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ARRAY_CLASS::connect_port_aliases_recursive(
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
		if (!element_type::checked_connect_port(ii, jj).good) {
			// error message?
			return good_bool(false);
		}
	}
	return good_bool(true);
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
	for ( ; i!=e; i++) {
		if (!element_type::create_dependent_types(*i, top).good)
			return good_bool(false);
		element_type& ii(const_cast<element_type&>(
			AS_A(const element_type&, *i)));
		// this call will deduce the canonical type from ii
		if (!internal_alias_policy::connect(ii).good) {
			return good_bool(false);
		}
	}
} else {
	// type of container is already strict, 
	// evaluate it once and re-use it when replaying internal aliases
	const typename parent_type::instance_collection_parameter_type
		t(collection_type_manager_parent_type::__get_raw_type());
	if (!create_definition_footprint(t, top).good) {
		return good_bool(false);
	}
	for ( ; i!=e; i++) {
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
	const_iterator i(this->collection.begin());
	const const_iterator e(this->collection.end());
	for ( ; i!=e; i++) {
		// fix-remove const casting...
		element_type& ii(const_cast<element_type&>(*i));
		INVARIANT(ii.instance_index);
		// 0 is not an acceptable index
		t.template get_id_map<Tag>()[ii.instance_index]
			.push_back(never_ptr<element_type>(&ii));
		ii.collect_port_aliases(t);
	}
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
	e.write_object_base(pom, os);
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
	e.load_object_base(this->pom, this->is);
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
/**
	Translates port formal placeholders to actual global IDs.  
	\pre footprint_frame has already been constructed.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::construct_port_context(port_collection_context& pcc, 
		const footprint_frame& ff) const {
	STACKTRACE_VERBOSE;
	const_iterator i(this->collection.begin());
	const const_iterator e(this->collection.end());
	pcc.resize(this->collection.size());
	size_t j = 0;
	for ( ; i!=e; i++, j++) {
		i->construct_port_context(pcc, ff, j);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns...
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::assign_footprint_frame(footprint_frame& ff, 
		const port_collection_context& pcc) const {
	STACKTRACE_VERBOSE;
	INVARIANT(this->collection.size() == pcc.size());
	const_iterator i(this->collection.begin());
	const const_iterator e(this->collection.end());
	size_t j = 0;
	for ( ; i!=e; i++, j++) {
		i->assign_footprint_frame(ff, pcc, j);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visitor.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::accept(alias_visitor& v) const {
	for_each(this->collection.begin(), this->collection.end(),
		bind2nd_argval(mem_fun_ref(&element_type::accept), v)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key, D)) {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::collect_transients()");
	parent_type::collect_transient_info_base(m);
	for_each(this->collection.begin(), this->collection.end(), 
		typename parent_type::element_collector(m));
	// optimization for later: factor this out into a policy
	// so that collections without pointers to collect
	// may be skipped.
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is a sparse collection, thus, we need to write out keys along 
	with values.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::write_object()");
	parent_type::write_object_base(m, f);
	// need to know how many members to expect
	const size_t s = this->collection.size();
	write_value(f, s);
	// to preserve key to index mapping, we must write out keys
	// by order of index, which may not be sorted w.r.t. keys.  
{
	// sparse_collections' public indices are 1-indexed
	size_t i = 1;
	value_writer<key_type> write_key(f);
	for ( ; i<=s; ++i) {
		const key_type& k(collection.lookup_key(i));
		write_key(k);
	}
}
	const const_iterator
		b(this->collection.begin()), e(this->collection.end());
	for_each(b, e, typename parent_type::element_writer(m, f));
	for_each(b, e, typename parent_type::connection_writer(m, f));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::load_object(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::load_object()");
	parent_type::load_object_base(m, f);
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
	iterator i(b);
	// can now use element_loader() functor
	for ( ; i!=e; ++i) {
		element_type& v(*i);
		v.load_object_base(m, f);
	}
	for_each(b, e, typename parent_type::connection_loader(m, f));
}

//=============================================================================
// class array method definitions (specialized)

#if POOL_ALLOCATE_INSTANCE_COLLECTIONS
// The following macro calls are intended to be equivalent to:
// TEMPLATE_CHUNK_MAP_POOL_ROBUST_STATIC_DEFINITION(
//	INSTANCE_SCALAR_TEMPLATE_SIGNATURE, INSTANCE_SCALAR_CLASS)
// but the preprocessor is retarded about arguments with commas in them, 
// so we forced to expand this macro by hand.  :S

INSTANCE_SCALAR_TEMPLATE_SIGNATURE
__SELF_CHUNK_MAP_POOL_STATIC_INIT(EMPTY_ARG, typename, INSTANCE_SCALAR_CLASS)

INSTANCE_SCALAR_TEMPLATE_SIGNATURE
__CHUNK_MAP_POOL_ROBUST_STATIC_GET_POOL(EMPTY_ARG, typename, INSTANCE_SCALAR_CLASS)

INSTANCE_SCALAR_TEMPLATE_SIGNATURE
__CHUNK_MAP_POOL_ROBUST_OPERATOR_NEW(EMPTY_ARG, INSTANCE_SCALAR_CLASS)

INSTANCE_SCALAR_TEMPLATE_SIGNATURE
__CHUNK_MAP_POOL_ROBUST_OPERATOR_PLACEMENT_NEW(EMPTY_ARG, INSTANCE_SCALAR_CLASS)

INSTANCE_SCALAR_TEMPLATE_SIGNATURE
__CHUNK_MAP_POOL_ROBUST_OPERATOR_DELETE(EMPTY_ARG, INSTANCE_SCALAR_CLASS)
#endif	// POOL_ALLOCATE_INSTANCE_COLLECTIONS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
INSTANCE_SCALAR_CLASS::instance_array() : 
		parent_type(), 
		the_instance() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
INSTANCE_SCALAR_CLASS::instance_array(const instance_placeholder_ptr_type p) :
		parent_type(p), the_instance() {
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
#if USE_COLLECTION_INTERFACES
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return an empty multikey index because scalars aren't indexed.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
multikey_index_type
INSTANCE_SCALAR_CLASS::lookup_key(const instance_alias_info_type& a) const {
	return multikey_index_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if USE_COLLECTION_INTERFACES
/**
	\return an empty multikey index because scalars aren't indexed.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
multikey_index_type
INSTANCE_SCALAR_CLASS::lookup_key(const size_t i) const {
	return multikey_index_type();
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should never be called, as scalar aliases are not mapped
	to any collection's indices.  
	\return 0.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
size_t
INSTANCE_SCALAR_CLASS::lookup_index(const instance_alias_info_type& a) const {
#if ALLOCATE_PORT_ACTUAL_COLLECTIONS
	INVARIANT(&this->the_instance == &a);
	return 1;
#else
	ICE_NEVER_CALL(cerr);
	return 0;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if USE_COLLECTION_INTERFACES
/**
	\param k multikey_index, which should always be empty (0 dimensions).
	\return 1 always.  
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
#endif

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
#if 0
//	if (this->the_instance.container->is_complete_type()) {
#if ALLOCATE_PORT_ACTUAL_COLLECTIONS
	if (this->has_relaxed_type())	// meaning "the container"
#else
	if (this->the_instance.container->has_relaxed_type())
#endif
	{
		this->the_instance.dump_actuals(o);
	}
//	}
#if 0
	o << "[dump flags: " << (df.show_definition_owner ? "(def) " : " ") <<
		(df.show_namespace_owner ? "(ns) " : " ") <<
		(df.show_leading_scope ? "(::)]" : "]");
#endif
	this->the_instance.peek()->dump_hierarchical_name(o << " = ", df);
	if (this->the_instance.instance_index)
		o << " (" << this->the_instance.instance_index << ')';
	this->the_instance.dump_ports(o << ' ', df);
#else
	typename parent_type::key_dumper(o, df)(this->the_instance);
#endif
} else {
	// this only happens when dumping the collection before
	// it is complete.
	o << "[null container]";
}
	return o;
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
		const instance_relaxed_actuals_type& actuals, 
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
	this->the_instance.instantiate(never_ptr<const this_type>(this), c);
	// for process only (or anything with relaxed typing)
	if (!collection_type_manager_parent_type::
			complete_type_definition_footprint(actuals).good) {
		return good_bool(false);
	}
	const bool attached(actuals ?
		this->the_instance.attach_actuals(actuals) : true);
	if (!attached) {
		cerr << "ERROR: attaching relaxed actuals to scalar ";
		this->type_dump(cerr) << " " <<
			this->source_placeholder->get_qualified_name()
			<< endl;
	}
	return good_bool(attached);
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
INSTANCE_SCALAR_CLASS::resolve_indices(const const_index_list& l) const {
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
INSTANCE_SCALAR_CLASS::lookup_instance(const multikey_index_type& i) const {
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
		typename default_list<instance_alias_info_ptr_type>::type& l,
		const const_range_list& r) const {
	cerr << "WARNING: instance_array<Tag,0>::lookup_instance_collection(...) "
		"should never be called." << endl;
	INVARIANT(r.empty());
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true on error, false on success.
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
bad_bool
INSTANCE_SCALAR_CLASS::unroll_aliases(const multikey_index_type& l, 
		const multikey_index_type& u, alias_collection_type& a) const {
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
		physical_instance_collection& p) {
	STACKTRACE_VERBOSE;
	this_type& t(IS_A(this_type&, p));	// assert dynamic_cast
	return instance_type::checked_connect_port(
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
	if (!instance_type::create_dependent_types(
			this->the_instance, top).good) {
		return good_bool(false);
	}
} else {
	const typename parent_type::instance_collection_parameter_type
		t(collection_type_manager_parent_type::__get_raw_type());
	if (!create_definition_footprint(t, top).good) {
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
	INVARIANT(this->the_instance.instance_index);
	// 0 is not an acceptable index
	t.template get_id_map<Tag>()[this->the_instance.instance_index]
		.push_back(never_ptr<instance_type>(
			&const_cast<instance_type&>(this->the_instance)));
	this->the_instance.collect_port_aliases(t);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
typename INSTANCE_SCALAR_CLASS::instance_alias_info_type&
INSTANCE_SCALAR_CLASS::load_reference(istream& i) {
	STACKTRACE_PERSISTENT("instance_scalar::load_reference()");
	// no key to read!
	// const_cast: have to modify next pointers to re-establish connection, 
	// which is semantically allowed because we allow the alias pointers
	// to be mutable.  
//	return const_cast<instance_type&>(this->the_instance);
	return this->the_instance;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translates port formal placeholders to actual global IDs.  
	\pre footprint_frame has already been constructed.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::construct_port_context(port_collection_context& pcc, 
		const footprint_frame& ff) const {
	STACKTRACE_VERBOSE;
	pcc.resize(1);
	this->the_instance.construct_port_context(pcc, ff, 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::assign_footprint_frame(footprint_frame& ff,
		const port_collection_context& pcc) const {
	STACKTRACE_VERBOSE;
	INVARIANT(pcc.size() == 1);
	this->the_instance.assign_footprint_frame(ff, pcc, 0);
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
INSTANCE_SCALAR_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key, 0)) {
	STACKTRACE_PERSISTENT("instance_scalar::collect_transients()");
	parent_type::collect_transient_info_base(m);
	this->the_instance.check(this);
	this->the_instance.collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::write_object(
		const persistent_object_manager& m, ostream& f) const {
	STACKTRACE_PERSISTENT("instance_scalar::write_object()");
	parent_type::write_object_base(m, f);
	typename parent_type::element_writer(m, f)(this->the_instance);
	typename parent_type::connection_writer(m, f)(this->the_instance);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::load_object(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("instance_scalar::load_object()");
	parent_type::load_object_base(m, f);
	typename parent_type::element_loader(m, f)(this->the_instance);
	typename parent_type::connection_loader(m, f)(this->the_instance);
	this->the_instance.check(this);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_COLLECTION_TCC__

