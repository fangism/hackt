/**
	\file "Object/ref/instance_reference.cc"
	Class instantiations for the meta_instance_reference family of objects.
	Thie file was reincarnated from "Object/art_object_inst_ref.cc".
 	$Id: instance_reference.cc,v 1.10.10.1.2.5 2006/02/18 21:47:31 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_INSTANCE_REFERENCE_CC__
#define	__HAC_OBJECT_REF_INSTANCE_REFERENCE_CC__

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <list>

#include "Object/type/fundamental_type_reference.h"
#include "Object/type/canonical_type.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/param_value_collection.h"
#include "Object/common/namespace.h"
#include "Object/common/dump_flags.h"
#include "Object/ref/simple_meta_instance_reference.tcc"
#include "Object/ref/simple_nonmeta_instance_reference.tcc"
#include "Object/ref/member_meta_instance_reference.tcc"
#include "Object/ref/aggregate_meta_instance_reference_base.h"
#include "Object/expr/const_range.h"
#include "Object/expr/dynamic_meta_index_list.h"
#include "Object/expr/dynamic_meta_range_list.h"
#include "Object/expr/nonmeta_index_list.h"
#include "Object/unroll/instantiation_statement_base.h"
#include "Object/unroll/loop_scope.h"
#include "Object/unroll/alias_connection.h"
#include "Object/persistent_type_hash.h"
#include "Object/traits/proc_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/general_collection_type_manager.h"
#include "Object/ref/meta_instance_reference_subtypes.tcc"
#include "Object/unroll/port_connection_base.h"

// introduced by using canonical_types
#include "Object/def/user_def_datatype.h"
#include "Object/def/user_def_chan.h"
#include "Object/def/process_definition.h"
#include "Object/type/canonical_generic_chan_type.h"

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
SPECIALIZE_UTIL_WHAT(HAC::entity::process_member_meta_instance_reference, 
		"process-member-inst-ref")
SPECIALIZE_UTIL_WHAT(HAC::entity::channel_member_meta_instance_reference, 
		"channel-member-inst-ref")

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
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::process_member_meta_instance_reference, 
		MEMBER_PROCESS_INSTANCE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::channel_member_meta_instance_reference, 
		MEMBER_CHANNEL_INSTANCE_REFERENCE_TYPE_KEY, 0)
}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::multidimensional_sparse_set_traits;
using util::multidimensional_sparse_set;
using util::write_value;
using util::read_value;
using util::persistent_traits;

//=============================================================================
// class meta_instance_reference_base method definitions

/**
	Wrapped interface to constructing type-specific alias connections.  
 */
excl_ptr<aliases_connection_base>
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
#if 0
	return i->make_aggregate_meta_instance_reference_private();
#else
	FINISH_ME(Fang);
	return count_ptr<aggregate_meta_instance_reference_base>(NULL);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<port_connection_base>
meta_instance_reference_base::make_port_connection(
		const count_ptr<const meta_instance_reference_base>& r) {
	return r->make_port_connection_private(r);
}

//=============================================================================
// class simple_meta_instance_reference_base method definitions

/**
	Private empty constructor.
 */
simple_meta_instance_reference_base::simple_meta_instance_reference_base() :
		array_indices(NULL) {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
simple_meta_instance_reference_base::~simple_meta_instance_reference_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just print the index portion of the reference.  
 */
ostream&
simple_meta_instance_reference_base::dump_indices(ostream& o, 
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
simple_meta_instance_reference_base::attach_indices(
		excl_ptr<index_list_type>& i) {
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
	Common persistence visitor for all subclasses.  
 */
void
simple_meta_instance_reference_base::collect_transient_info_base(
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
simple_meta_instance_reference_base::write_object_base(
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
simple_meta_instance_reference_base::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, array_indices);
	// must load the indices early?
	if (array_indices)
		m.load_object_once(array_indices);
}

//=============================================================================
// class simple_nonmeta_instance_reference_base method definitions

simple_nonmeta_instance_reference_base::simple_nonmeta_instance_reference_base(
		) : parent_type(), array_indices() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
simple_nonmeta_instance_reference_base::simple_nonmeta_instance_reference_base(
		excl_ptr<index_list_type>& i) :
		parent_type(), array_indices(i) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
simple_nonmeta_instance_reference_base::~simple_nonmeta_instance_reference_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
simple_nonmeta_instance_reference_base::dimensions(void) const {
	size_t dim = get_inst_base()->get_dimensions();
	if (array_indices) {
		const size_t c = array_indices->dimensions_collapsed();
		INVARIANT(c <= dim);
		return dim -c;
	}
	else return dim;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Temporary: un meta-checked.  
 */
good_bool
simple_nonmeta_instance_reference_base::attach_indices(
		excl_ptr<index_list_type>& i) {
	INVARIANT(!array_indices);
	NEVER_NULL(i);

	// dimension-check:
	const never_ptr<const instance_collection_base>
		inst_base(get_inst_base());
	// number of indices must be <= dimension of instance collection.  
	const size_t max_dim = dimensions();    // depends on indices
	if (i->size() > max_dim) {
		cerr << "ERROR: instance collection " << inst_base->get_name()
			<< " is " << max_dim << "-dimensional, and thus, "
			"cannot be indexed " << i->size() <<
			"-dimensionally!  ";
			// caller will say where
		return good_bool(false);
	} 
	// else proceed...

	// allow under-specified dimensions?  
	// NOT for nonmeta instance references, or ALL or NONE
	// TODO: enforce this, modifying the above check

	// TODO: limited static range checking?
	// only if indices are ALL meta values

	array_indices = i;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Improved dump, ues contex flags to modify and format dump.  
 */
ostream&
simple_nonmeta_instance_reference_base::dump(ostream& o, 
		const expr_dump_context& c) const {
#if 0
	if (c.include_type_info)
		what(o) << " ";
#endif
	// modify flags for this?
	// depend on c.enclosing_scope?
	const never_ptr<const instance_collection_base>
		ib(get_inst_base());
	ib->dump_hierarchical_name(o, dump_flags::no_definition_owner);
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
#if 0
PHASE IN later...
// class collective_meta_instance_reference method definitions

collective_meta_instance_reference::collective_meta_instance_reference(
		never_ptr<const meta_instance_reference_base> b, 
		const param_expr* l, const param_expr* r) :
		meta_instance_reference_base(), 
		lower_index(never_ptr<const param_expr>(l)),
		upper_index(never_ptr<const param_expr>(r)) {
}

collective_meta_instance_reference::~collective_meta_instance_reference() {
}

ostream&
collective_meta_instance_reference::what(ostream& o) const {
	return o << "collective-inst-ref";
}

ostream&
collective_meta_instance_reference::dump(ostream& o) const {
	return what(o);
}

string
collective_meta_instance_reference::hash_string(void) const {
	string ret(base_array->hash_string());
	ret += "[";
	ret += lower_index->hash_string();
	if (upper_index) {
		ret += "..";
		ret += upper_index->hash_string();
	}
	ret += "]";
	return ret;
}
#endif

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

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_INSTANCE_REFERENCE_CC__

