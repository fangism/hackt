/**
	\file "Object/inst/port_actual_collection.tcc"
	$Id: port_actual_collection.tcc,v 1.11.24.1 2010/01/22 23:41:37 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PORT_ACTUAL_COLLECTION_TCC__
#define	__HAC_OBJECT_INST_PORT_ACTUAL_COLLECTION_TCC__

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#endif

#include <iterator>
#include <iostream>
#include <algorithm>
#include "Object/inst/port_actual_collection.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/port_alias_tracker.h"
#include "Object/def/footprint.h"
#include "Object/port_context.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range_list.h"

#include "common/TODO.h"

#include "util/multikey.h"
#include "util/IO_utils.h"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.tcc"
#include "Object/inst/collection_traits.h"
#include "Object/inst/collection_pool.tcc"	// for lookup_index

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using std::distance;
using std::for_each;
using util::read_value;
using util::write_value;
using util::value_reader;
using util::value_writer;

//=============================================================================
// class port_actual_collection method definitions

PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
PORT_ACTUAL_COLLECTION_CLASS::port_actual_collection() :
		parent_type(), formal_collection(), value_array(0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: re-write function as something other than constructor
		to return instead of throw.
	\throw exception if there is an error in unrolling ports.  
	\pre if we're propagating connection information hierarchically, 
		then the formal collection type must already be successfully
		created.  We don't create here, to avoid exception handling
		in this recursive constructor.  
 */
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
PORT_ACTUAL_COLLECTION_CLASS::port_actual_collection(
		const formal_collection_ptr_type f, 
		const unroll_context& c) :
		parent_type(), 
		formal_collection(f), 
		value_array(f->collection_size()) {
	// pass the iterators (or this collection) to the formal collection 
	// for initialization of the connected state
	// this requires that summaries are already constructed
	// by the formal collection (requires create pass)
	this->formal_collection->instantiate_actuals_from_formals(*this, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
PORT_ACTUAL_COLLECTION_CLASS::~port_actual_collection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
ostream&
PORT_ACTUAL_COLLECTION_CLASS::what(ostream& o) const {
	return o << "port-actual-collection<" << traits_type::tag_name << ">";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
typename PORT_ACTUAL_COLLECTION_CLASS::iterator
PORT_ACTUAL_COLLECTION_CLASS::begin(void) {
	return &this->value_array[0];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
typename PORT_ACTUAL_COLLECTION_CLASS::const_iterator
PORT_ACTUAL_COLLECTION_CLASS::begin(void) const {
	return &this->value_array[0];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
typename PORT_ACTUAL_COLLECTION_CLASS::iterator
PORT_ACTUAL_COLLECTION_CLASS::end(void) {
	return &this->value_array[this->value_array.size()];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
typename PORT_ACTUAL_COLLECTION_CLASS::const_iterator
PORT_ACTUAL_COLLECTION_CLASS::end(void) const {
	return &this->value_array[this->value_array.size()];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
bool
PORT_ACTUAL_COLLECTION_CLASS::is_partially_unrolled(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return false.  By definition, these collections are actuals,
		not formals.  
 */
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
bool
PORT_ACTUAL_COLLECTION_CLASS::is_formal(void) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
const instance_collection<Tag>&
PORT_ACTUAL_COLLECTION_CLASS::get_canonical_collection(void) const {
	NEVER_NULL(this->formal_collection);
	return *this->formal_collection;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
 	\return 1-based index.  
 */
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
size_t
PORT_ACTUAL_COLLECTION_CLASS::lookup_index(
		const instance_alias_info_type& a) const {
//	STACKTRACE_VERBOSE;
	const size_t offset = distance(this->begin(), &a);
	INVARIANT(offset < this->value_array.size());
//	STACKTRACE_INDENT_PRINT("return " << offset +1);
	return offset +1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param a the alias belonging to THIS container, whose index
		is looked up and printed according to its corresponding
		index-position in the formal collection.  
 */
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
ostream&
PORT_ACTUAL_COLLECTION_CLASS::dump_element_key(ostream& o, 
		const instance_alias_info_type& a) const {
	NEVER_NULL(this->formal_collection);
	// internally all 0-based indices, no correction needed
	return this->formal_collection->dump_element_key(o, 
		this->lookup_index(a));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
ostream&
PORT_ACTUAL_COLLECTION_CLASS::dump_element_key(ostream& o, 
		const size_t i) const {
	NEVER_NULL(this->formal_collection);
	return this->formal_collection->dump_element_key(o, i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
multikey_index_type
PORT_ACTUAL_COLLECTION_CLASS::lookup_key(
		const instance_alias_info_type& a) const {
	STACKTRACE_VERBOSE;
	NEVER_NULL(this->formal_collection);
	// internally uses 1-based index
	return this->formal_collection->lookup_key(this->lookup_index(a));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
multikey_index_type
PORT_ACTUAL_COLLECTION_CLASS::lookup_key(const size_t i) const {
	STACKTRACE_VERBOSE;
	NEVER_NULL(this->formal_collection);
	return this->formal_collection->lookup_key(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
typename PORT_ACTUAL_COLLECTION_CLASS::instance_alias_info_type&
PORT_ACTUAL_COLLECTION_CLASS::get_corresponding_element(
		const collection_interface_type& p, 
		const instance_alias_info_type& a) {
	STACKTRACE_VERBOSE;
	const size_t offset = p.lookup_index(a);
	INVARIANT(offset);
	return this->value_array[offset -1];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
ostream&
PORT_ACTUAL_COLLECTION_CLASS::dump_unrolled_instances(ostream& o, 
		const dump_flags& df) const {
	for_each(this->begin(), this->end(),
		typename formal_collection_type::key_dumper(o, df));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
ostream&
PORT_ACTUAL_COLLECTION_CLASS::type_dump(ostream& o) const {
	return this->formal_collection->type_dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<const fundamental_type_reference>
PORT_ACTUAL_COLLECTION_CLASS::get_unresolved_type_ref(void) const {
//	STACKTRACE_VERBOSE;
	return this->formal_collection->get_unresolved_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
typename PORT_ACTUAL_COLLECTION_CLASS::instance_placeholder_ptr_type
PORT_ACTUAL_COLLECTION_CLASS::get_placeholder(void) const {
//	STACKTRACE_VERBOSE;
	return this->formal_collection->get_placeholder();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
never_ptr<const physical_instance_placeholder>
PORT_ACTUAL_COLLECTION_CLASS::get_placeholder_base(void) const {
//	STACKTRACE_VERBOSE;
	return this->formal_collection->get_placeholder_base();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This shouldn't have to resize, size should just match that
	of the referenced formal collection.  
	Should re-write code so this is not called, 
	e.g. not virtual function in collection_interface.  
 */
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
good_bool
PORT_ACTUAL_COLLECTION_CLASS::instantiate_indices(
		const const_range_list& /* ranges */, 
		const unroll_context&) {
#if 0
	INVARIANT(!this->value_array.size());
	const key_type k(ranges.resolve_sizes());
	this->value_array.resize(k);
#endif
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
good_bool
PORT_ACTUAL_COLLECTION_CLASS::allocate_local_instance_ids(footprint& f) {
	STACKTRACE_VERBOSE;
	iterator i(this->begin()), e(this->end());
	for ( ; i!=e; ++i) {
		if (!i->assign_local_instance_id(f)) {
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Just need to make sure indices are in range.
	Since this is a dense collection, should always succeed if
	input range is valid.  
 */
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
const_index_list
PORT_ACTUAL_COLLECTION_CLASS::resolve_indices(const const_index_list& l) const {
	STACKTRACE_VERBOSE;
	return this->formal_collection->resolve_indices(l);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// operator []

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
typename PORT_ACTUAL_COLLECTION_CLASS::instance_alias_info_ptr_type
PORT_ACTUAL_COLLECTION_CLASS::lookup_instance(
		const multikey_index_type& l) const {
	typedef	instance_alias_info_ptr_type		return_type;
	STACKTRACE_VERBOSE;
	const size_t offset = this->formal_collection->lookup_index(l);
	if (offset) {
		return return_type(&const_cast<element_type&>(
			this->value_array[offset -1]));
	} else {
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param l list in which to accumulate instance references.  
	\return false to signal error, true to signal success.  
 */
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
bool
PORT_ACTUAL_COLLECTION_CLASS::lookup_instance_collection(
		typename default_list<instance_alias_info_ptr_type>::type& /* l */,
		const const_range_list& /* r */) const {
	STACKTRACE_VERBOSE;
#if 0
	INVARIANT(!r.empty());
	key_generator_type key_gen(r.lower_multikey(), r.upper_multikey());
	if (!value_array.range_check(key_gen.lower_corner)
			|| !value_array.range_check(key_gen.upper_corner)) {
		this->type_dump(cerr << "FATAL: reference to ") <<
			" with out-of-bounds indices " <<
			key_gen.lower_corner << ":" << key_gen.upper_corner
			<< endl;
		return false;
	}
	key_gen.initialize();
	// TODO: consider a subroutine to grab entire subslice efficiently
	do {
		const instance_alias_info_ptr_type
			pi(&const_cast<instance_alias_info_type&>(
				value_array[key_gen]));
		NEVER_NULL(pi);
		INVARIANT(pi->valid());	// port must already be instantiated
		l.push_back(pi);
		key_gen++;
	} while (key_gen != key_gen.lower_corner);
	return true;
#else
	// TODO: finish me
	return false;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
bad_bool
PORT_ACTUAL_COLLECTION_CLASS::unroll_aliases(const multikey_index_type& l, 
		const multikey_index_type& u, alias_collection_type& a) const {
	typedef	typename alias_collection_type::key_type
						collection_key_type;
	typedef	typename alias_collection_type::iterator
						alias_collection_iterator;
	typedef	multikey_index_type 		key_type;
	typedef	key_type::generator_type	key_generator_type;
	STACKTRACE_VERBOSE;
	// const key_type lower(l), upper(u);
	// else we know that every key is valid (since whole range is valid)
	key_generator_type key_gen(l, u);
	key_gen.initialize();
	alias_collection_iterator a_iter(a.begin());
	bool ret = false;
	const size_t max = this->value_array.size();
	do {
		const size_t offset = 
			this->formal_collection->lookup_index(key_gen);
		// 1-based index
		if (!offset || offset > max) {
			this->formal_collection->type_dump(
				cerr << "FATAL: reference to uninstantiated ")
					<< " index " << key_gen << endl;
			ret = true;
		} else {
			const instance_alias_info_ptr_type
				pi(&const_cast<instance_alias_info_type&>(
					value_array[offset -1]));
			NEVER_NULL(pi);
			INVARIANT(pi->valid());
			// port must already be instantiated
			*a_iter = pi;
		}
		++a_iter;
		key_gen++;
	} while (key_gen != key_gen.lower_corner);
	INVARIANT(a_iter == a.end());
#if 0
	if (ret) {
		cerr << "Error referencing " <<
			this->formal_collection->get_hierarchical_name()
			<< "." << endl;
	}
#endif
	return bad_bool(ret);
}	// end method unroll_aliases

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
good_bool
PORT_ACTUAL_COLLECTION_CLASS::connect_port_aliases_recursive(
		physical_instance_collection& p, const unroll_context& c) {
	STACKTRACE_VERBOSE;
	this_type& t(IS_A(this_type&, p));	// assert dynamic_cast
	INVARIANT(this->value_array.size() == t.value_array.size());
	iterator i(this->begin());
	iterator j(t.begin());
	const iterator e(this->end());
	for ( ; i!=e; ++i, ++j) {
		// unfortunately, set iterators only return const refs
		// we only intend to modify the value without modifying the key
		element_type& ii(*i);
		element_type& jj(*j);
		// possibly redundant port type checking is unnecessary
		if (!instance_alias_info_type::checked_connect_port(
				ii, jj, c).good) {
			// error message?
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursive creation of dependent types through public ports
	of instance hierarchy.
 */
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
good_bool
PORT_ACTUAL_COLLECTION_CLASS::create_dependent_types(const footprint& top) {
	STACKTRACE_VERBOSE;
	iterator i(this->begin());
	const iterator e(this->end());
if (i == e) {
	// no instances in this collection were instantiated (conditional)
	return good_bool(true);
}
if (this->formal_collection->has_relaxed_type()) {
	for ( ; i!=e; i++) {
		if (!element_type::create_dependent_types(*i, top).good)
			return good_bool(false);
		element_type& ii(*i);
		// this call will deduce the canonical type from ii
		if (!internal_alias_policy::connect(ii).good) {
			return good_bool(false);
		}
	}
} else {
	// type of container is already strict, 
	// evaluate it once and re-use it when replaying internal aliases
	const typename formal_collection_type::instance_collection_parameter_type
		t(this->formal_collection->__get_raw_type());
	if (!formal_collection_type::create_definition_footprint(t, top).good) {
		return good_bool(false);
	}
	for ( ; i!=e; i++) {
		element_type& ii(*i);
		if (!internal_alias_policy::connect(ii, t).good) {
			return good_bool(false);
		}
	}
}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Really should be named collect_scope_aliases.
	TODO: rewrite functionally
 */
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
void
PORT_ACTUAL_COLLECTION_CLASS::collect_port_aliases(
		port_alias_tracker& t) const {
	STACKTRACE_VERBOSE;
#if 0
	// TODO fix const_cast
	const_iterator i(this->begin());
	const const_iterator e(this->end());
	for ( ; i!=e; i++) {
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
	// mmm... functional
	for_each(this->begin(), this->end(), 
		typename formal_collection_type::scope_alias_collector(t));
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need to distinguish between scalar and array, just like is
	done with instance_array and instance_scalar::load_reference.  
 */
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
typename PORT_ACTUAL_COLLECTION_CLASS::instance_alias_info_type&
PORT_ACTUAL_COLLECTION_CLASS::load_reference(istream& i) {
	STACKTRACE_VERBOSE;
if (this->get_dimensions()) {
	size_t index;		// 1-indexed
	read_value(i, index);
	INVARIANT(index);
	return *(this->begin() +(index -1));	// array-access
} else {
	// nothing to load!
	INVARIANT(this->value_array.size() == 1);
	return *this->begin();
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
void
PORT_ACTUAL_COLLECTION_CLASS::construct_port_context(
		port_collection_context& pcc, 
		const footprint_frame& ff) const {
	STACKTRACE_VERBOSE;
	const_iterator i(this->begin());
	const const_iterator e(this->end());
	pcc.resize(this->value_array.size());
	size_t j = 0;
	for ( ; i!=e; ++i, ++j) {
		i->construct_port_context(pcc, ff, j);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
void
PORT_ACTUAL_COLLECTION_CLASS::assign_footprint_frame(footprint_frame& ff, 
		const port_collection_context& pcc) const {
	STACKTRACE_VERBOSE;
	INVARIANT(this->value_array.size() == pcc.size());
	const_iterator i(this->begin());
	const const_iterator e(this->end());
	size_t j = 0;
	for ( ; i!=e; ++i, ++j) {
		i->assign_footprint_frame(ff, pcc, j);
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visitor.  
 */
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
void
PORT_ACTUAL_COLLECTION_CLASS::accept(alias_visitor& v) const {
	for_each(this->begin(), this->end(),
		bind2nd_argval(mem_fun_ref(&element_type::accept), v)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
void
PORT_ACTUAL_COLLECTION_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	// since this is a port collection, there MUST be a super-instance.
	NEVER_NULL(this->super_instance);
//	parent_type::collect_transient_info_base(m);	// pure virt.
	// pool collection manager will take care of it already.
	for_each(this->begin(), this->end(),
		bind2nd_argval(mem_fun_ref(
			&element_type::collect_transient_info_base), m)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
void
PORT_ACTUAL_COLLECTION_CLASS::write_pointer(ostream& o, 
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
	No need to write keys in linear array.  Very efficient.  
	\pre formal_collection's size must match that of this actual
		collection. 
 */
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
void
PORT_ACTUAL_COLLECTION_CLASS::write_object(const footprint& fp, 
		const persistent_object_manager& m, ostream& f) const {
	this->formal_collection->write_external_pointer(m, f);
	// size is deduced from the formal_collection
	const size_t k = this->formal_collection->collection_size();
#if ENABLE_STACKTRACE
	cerr << "collection-size = " << k << endl;
#endif
	INVARIANT(k == this->value_array.size());
	const const_iterator b(this->begin()), e(this->end());
	INVARIANT(k == size_t(distance(b, e)));
	for_each(b, e, typename formal_collection_type::element_writer(
			fp, m, f));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
void
PORT_ACTUAL_COLLECTION_CLASS::write_connections(
		const collection_pool_bundle_type& m, ostream& f) const {
	for_each(this->begin(), this->end(), 
		typename formal_collection_type::connection_writer(m, f));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Actual collections do not need to be registered with 
	the containing footprint; they have hierarchical names.
 */
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
void
PORT_ACTUAL_COLLECTION_CLASS::load_object(footprint& fp, 
		const persistent_object_manager& m, istream& f) {
#if ENABLE_STACKTRACE
	cerr << "this (port-actual-collection) @ " << this << endl;
#endif
	// counterpart: instance_collection::write_external_pointer()
	this->formal_collection =
		formal_collection_type::read_external_pointer(m, f);
	NEVER_NULL(this->formal_collection);
	// not necessary anymore
	const size_t k = this->formal_collection->collection_size();
#if ENABLE_STACKTRACE
	cerr << "collection-size = " << k << endl;
#endif
	this->value_array.resize(k);
	const iterator b(this->begin()), e(this->end());
	INVARIANT(k == size_t(distance(b, e)));
	for_each(b, e, typename formal_collection_type::element_loader(
			fp, m, f, never_ptr<const this_type>(this)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Connections are loaded in a separate phase.   
 */
PORT_ACTUAL_COLLECTION_TEMPLATE_SIGNATURE
void
PORT_ACTUAL_COLLECTION_CLASS::load_connections(
		const collection_pool_bundle_type& m, istream& f) {
	for_each(this->begin(), this->end(), 
		typename formal_collection_type::connection_loader(m, f));
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PORT_ACTUAL_COLLECTION_TCC__

