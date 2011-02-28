/**
	\file "Object/ref/instance_reference.cc"
	Class instantiations for the meta_instance_reference family of objects.
	Thie file was reincarnated from "Object/art_object_inst_ref.cc".
 	$Id: instance_reference.cc,v 1.25 2011/02/28 09:37:44 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_INSTANCE_REFERENCE_CC__
#define	__HAC_OBJECT_REF_INSTANCE_REFERENCE_CC__

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <list>
#include <algorithm>

#include "Object/ref/meta_instance_reference_subtypes.tcc"
#include "Object/ref/simple_meta_instance_reference.tcc"
#include "Object/ref/simple_nonmeta_instance_reference.tcc"
#include "Object/ref/member_meta_instance_reference.tcc"
#if NONMETA_MEMBER_REFERENCES
#include "Object/ref/member_nonmeta_instance_reference.tcc"
#endif
#include "Object/ref/aggregate_meta_instance_reference.tcc"
#include "Object/type/fundamental_type_reference.h"
#include "Object/type/canonical_type.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/inst/channel_instance_collection.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/param_value_collection.h"
#include "Object/common/namespace.h"
#include "Object/common/dump_flags.h"
#include "Object/expr/const_range.h"
#include "Object/expr/dynamic_meta_index_list.h"
#include "Object/expr/dynamic_meta_range_list.h"
#include "Object/expr/nonmeta_index_list.h"
#include "Object/expr/pstring_expr.h"
#include "Object/unroll/instantiation_statement_base.h"
#include "Object/unroll/loop_scope.h"
#include "Object/unroll/alias_connection.h"
#include "Object/persistent_type_hash.h"
#include "Object/traits/instance_traits.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/general_collection_type_manager.h"
#include "Object/unroll/port_connection_base.h"

// introduced by using canonical_types
#include "Object/def/user_def_datatype.h"
#include "Object/def/user_def_chan.h"
#include "Object/def/process_definition.h"
#include "Object/type/canonical_generic_chan_type.h"
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "Object/global_channel_entry.h"
#endif

#include "common/TODO.h"

#include "util/persistent_object_manager.tcc"
#include "util/multidimensional_sparse_set.tcc"

//=============================================================================
// specializations

namespace util {
SPECIALIZE_UTIL_WHAT(HAC::entity::simple_process_meta_instance_reference, 
		"process-inst-ref")
SPECIALIZE_UTIL_WHAT(HAC::entity::simple_channel_meta_instance_reference, 
		"channel-inst-ref")
SPECIALIZE_UTIL_WHAT(HAC::entity::simple_process_nonmeta_instance_reference, 
		"process-nonmeta-inst-ref")
SPECIALIZE_UTIL_WHAT(HAC::entity::simple_channel_nonmeta_instance_reference, 
		"channel-nonmeta-inst-ref")
#if NONMETA_MEMBER_REFERENCES
SPECIALIZE_UTIL_WHAT(HAC::entity::member_process_nonmeta_instance_reference, 
		"process-nonmeta-member-ref")
SPECIALIZE_UTIL_WHAT(HAC::entity::member_channel_nonmeta_instance_reference, 
		"channel-nonmeta-member-ref")
#endif
SPECIALIZE_UTIL_WHAT(HAC::entity::process_member_meta_instance_reference, 
		"process-member-inst-ref")
SPECIALIZE_UTIL_WHAT(HAC::entity::channel_member_meta_instance_reference, 
		"channel-member-inst-ref")
SPECIALIZE_UTIL_WHAT(HAC::entity::aggregate_process_meta_instance_reference, 
		"process-agg.-inst-ref")
SPECIALIZE_UTIL_WHAT(HAC::entity::aggregate_channel_meta_instance_reference, 
		"channel-agg.-inst-ref")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_process_meta_instance_reference, 
		SIMPLE_PROCESS_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_channel_meta_instance_reference, 
		SIMPLE_CHANNEL_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_process_nonmeta_instance_reference, 
		SIMPLE_PROCESS_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_channel_nonmeta_instance_reference, 
		SIMPLE_CHANNEL_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
#if NONMETA_MEMBER_REFERENCES
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::member_process_nonmeta_instance_reference, 
		MEMBER_PROCESS_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::member_channel_nonmeta_instance_reference, 
		MEMBER_CHANNEL_NONMETA_INSTANCE_REFERENCE_TYPE_KEY, 0)
#endif
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::process_member_meta_instance_reference, 
		MEMBER_PROCESS_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::channel_member_meta_instance_reference, 
		MEMBER_CHANNEL_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::aggregate_process_meta_instance_reference, 
		AGGREGATE_PROCESS_META_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::aggregate_channel_meta_instance_reference, 
		AGGREGATE_CHANNEL_META_INSTANCE_REFERENCE_TYPE_KEY, 0)

namespace memory {
// explicit template instantiations
// needed for -O3
template class count_ptr<HAC::entity::meta_instance_reference_base>;
}
}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using std::equal;
using util::multidimensional_sparse_set_traits;
using util::multidimensional_sparse_set;
using util::write_value;
using util::read_value;
using util::persistent_traits;

//=============================================================================
// class meta_instance_reference_base method definitions

meta_instance_reference_base::~meta_instance_reference_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped interface to constructing type-specific alias connections.  
 */
meta_instance_reference_base::alias_connection_ptr_type
meta_instance_reference_base::make_aliases_connection(
		const count_ptr<const this_type>& i) {
	NEVER_NULL(i);
	return i->make_aliases_connection_private();
	// have the option of adding first instance here...
	// ret->append_meta_instance_reference(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<aggregate_meta_instance_reference_base>
meta_instance_reference_base::make_aggregate_meta_instance_reference(
		const count_ptr<const this_type>& i) {
	NEVER_NULL(i);
	const count_ptr<aggregate_meta_instance_reference_base>
		ret(i->make_aggregate_meta_instance_reference_private());
	const good_bool g(ret->append_meta_instance_reference(i));
	INVARIANT(g.good);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
meta_instance_reference_base::port_connection_ptr_type
meta_instance_reference_base::make_port_connection(
		const count_ptr<const meta_instance_reference_base>& r) {
	return r->make_port_connection_private(r);
}

//=============================================================================
// class simple_meta_indexed_reference_base method definitions

/**
	Private empty constructor.
 */
simple_meta_indexed_reference_base::simple_meta_indexed_reference_base() :
		array_indices(NULL) {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
simple_meta_indexed_reference_base::simple_meta_indexed_reference_base(
		indices_ptr_arg_type i) :
		array_indices(i) {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
simple_meta_indexed_reference_base::~simple_meta_indexed_reference_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just print the index portion of the reference.  
 */
ostream&
simple_meta_indexed_reference_base::dump_indices(ostream& o, 
		const expr_dump_context& c) const {
	if (array_indices) {
		expr_dump_context dc(c);
		dc.include_type_info = false;
		array_indices->dump(o, dc);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds indices to instance reference.  
	Must check with number of number of dimensions.  
	(Should we allow under-specification of dimensions?
		i.e. x[i] of a 2-D array to be a 1D array reference.)
	Then referring to x is referring to the entire array of x.  

	Fancy: an indexed instance reference may be under-specified
	Referring to partial sub-arrays, e.g. x[i] where x is 2-dimensional.  
	In this case, when we check for static instance coverage, 
	
	\return true if successful, else false.  
 */
good_bool
simple_meta_indexed_reference_base::attach_indices(indices_ptr_arg_type i) {
	/**
		We used to perform static checks for index collisions, 
		but there was little benefit in catching early errors, 
		and the effort was not worthwhile.  
	**/
	array_indices = i;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Forwarded call to resolve indices (independent of collection).
	\returns null of there are no indices, or unrolling fails.  
	So don't use null to diagnose an error without checking
		this->array_indices first.  
 */
count_ptr<const const_index_list>
simple_meta_indexed_reference_base::unroll_resolve_indices(
		const unroll_context& c) const {
	typedef	count_ptr<const const_index_list>	return_type;
if (array_indices) {
	const return_type
		resolved_indices(meta_index_list::unroll_resolve_indices(
			array_indices, c));
	if (!resolved_indices) {
		cerr << "Error resolving meta indices: ";
		dump_indices(cerr, expr_dump_context::default_value) << endl;
		return return_type(NULL);
	}
	return resolved_indices;
} else {
	return return_type(NULL);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Common persistence visitor for all subclasses.  
 */
void
simple_meta_indexed_reference_base::collect_transient_info_base(
		persistent_object_manager& m) const {
	if (array_indices)
		array_indices->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param m the persistent object manager for pointer translation. 
	\param o the output stream to write binary.
	\pre o MUST BE the stream corresponding to this object.
 */
void
simple_meta_indexed_reference_base::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, array_indices);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note: the instantiation base must be loaded before the
		state information, because the instantiation state
		depends on the instantiation base being complete.
	\param m the persistent object manager for pointer translation. 
	\param i the input stream to read binary.
	\pre i MUST BE the stream corresponding to this object.
 */
void
simple_meta_indexed_reference_base::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, array_indices);
	// must load the indices early?
	if (array_indices) {
		m.load_object_once(
			const_cast<index_list_type*>(&*array_indices));
	}
}

//=============================================================================
// class nonmeta_instance_reference_base method definitions

nonmeta_instance_reference_base::~nonmeta_instance_reference_base() { }

//=============================================================================
// class simple_nonmeta_instance_reference_base method definitions

simple_nonmeta_instance_reference_base::simple_nonmeta_instance_reference_base(
		) : array_indices() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
simple_nonmeta_instance_reference_base::simple_nonmeta_instance_reference_base(
		excl_ptr<index_list_type>& i) :
		array_indices(i) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
simple_nonmeta_instance_reference_base::~simple_nonmeta_instance_reference_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Improved dump, uses context flags to modify and format dump.  
 */
ostream&
simple_nonmeta_instance_reference_base::dump_indices(ostream& o, 
		const expr_dump_context& c) const {
	if (array_indices) {
		array_indices->dump(o, c);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Common persistence visitor for all subclasses.  
 */
void
simple_nonmeta_instance_reference_base::collect_transient_info_base(
		persistent_object_manager& m) const {
	if (array_indices)
		array_indices->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param m the persistent object manager for pointer translation. 
	\param o the output stream to write binary.
	\pre o MUST BE the stream corresponding to this object.
 */
void
simple_nonmeta_instance_reference_base::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, array_indices);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note: the instantiation base must be loaded before the
		state information, because the instantiation state
		depends on the instantiation base being complete.
	\param m the persistent object manager for pointer translation. 
	\param i the input stream to read binary.
	\pre i MUST BE the stream corresponding to this object.
 */
void
simple_nonmeta_instance_reference_base::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, array_indices);
	// must load the 
	if (array_indices)
		m.load_object_once(array_indices);
}

//=============================================================================
// class aggregate_reference_collection_base method definitions

/**
	For subarray sizes to be concatenable, the trailing dimensions
	(after first) must match.  
 */
good_bool
aggregate_reference_collection_base::check_concatenable_subarray_sizes(
		const size_array_type& s) {
	typedef size_array_type::const_iterator	size_iterator;
	const size_iterator zb(s.begin()), ze(s.end());
	const multikey_index_type& head_size(*zb);
	size_iterator zi(zb+1);
	for ( ; zi!=ze; ++zi) {
		// compare *trailing* dimensions
		if (!equal(head_size.begin() +1, head_size.end(),
				zi->begin() +1)) {
			cerr << "Error: trailing dimensions mismatch between "
				"sub-collection 1 and " <<
				distance(zb, zi) +1 << "." << endl;
			cerr << "\tgot: " << head_size << " and: " <<
				*zi << endl;
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For subarray sizes to be constructible, all dimensions must match.  
 */
good_bool
aggregate_reference_collection_base::check_constructible_subarray_sizes(
		const size_array_type& s) {
	typedef size_array_type::const_iterator	size_iterator;
	const size_iterator zb(s.begin()), ze(s.end());
	const multikey_index_type& head_size(*zb);
	size_iterator zi(zb+1);
	for ( ; zi!=ze; ++zi) {
		// compare *all* dimensions
		if (!equal(head_size.begin(), head_size.end(),
				zi->begin())) {
			cerr << "Error: dimensions mismatch between "
				"sub-collection 1 and " <<
				distance(zb, zi) +1 << "." << endl;
			cerr << "\tgot: " << head_size << " and: " <<
				*zi << endl;
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
aggregate_reference_collection_base::check_subarray_sizes(
		const size_array_type& s) const {
	return _is_concatenation ? check_concatenable_subarray_sizes(s) :
		check_constructible_subarray_sizes(s);
}

//-----------------------------------------------------------------------------
// class aggregate_meta_instance_reference_base method definitions
aggregate_meta_instance_reference_base::
	~aggregate_meta_instance_reference_base() { }


//=============================================================================
// class process_meta_instance_reference method definitions
// replaced with meta_instance_reference template

//=============================================================================
// class channel_meta_instance_reference method definitions
// replaced with meta_instance_reference template

//=============================================================================
// explicit template instantiations

template class meta_instance_reference<channel_tag>;
template class meta_instance_reference<process_tag>;
template class simple_meta_instance_reference<channel_tag>;
template class simple_meta_instance_reference<process_tag>;
template class simple_nonmeta_instance_reference<channel_tag>;
template class simple_nonmeta_instance_reference<process_tag>;
template class member_meta_instance_reference<channel_tag>;
template class member_meta_instance_reference<process_tag>;
template class aggregate_meta_instance_reference<channel_tag>;
template class aggregate_meta_instance_reference<process_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_INSTANCE_REFERENCE_CC__

