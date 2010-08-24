/**
	\file "Object/ref/simple_meta_dummy_reference.cc"
	Method definitions for the meta_dummy_reference family of objects.
	This file was reincarnated from "Object/art_object_inst_ref.cc".
 	$Id: simple_meta_dummy_reference.tcc,v 1.7 2010/08/24 21:05:50 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_META_DUMMY_REFERENCE_TCC__
#define	__HAC_OBJECT_REF_SIMPLE_META_DUMMY_REFERENCE_TCC__

#include <iostream>
#include <vector>

#include "Object/ref/simple_meta_dummy_reference.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/expr_visitor.h"
#include "Object/common/dump_flags.h"
#include "Object/unroll/unroll_context.h"
#include "Object/def/footprint.h"
#include "Object/type/fundamental_type_reference.h"
#include "common/TODO.h"
#include "common/ICE.h"
#include "util/what.h"
#include "util/persistent_object_manager.tcc"
// #include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/aggregate_meta_instance_reference_base.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/ref/inst_ref_implementation.h"
#include "Object/unroll/aliases_connection_base.h"
#include "Object/unroll/port_connection_base.h"
#include "util/stacktrace.h"
#include "Object/inst/dummy_placeholder.h"
#include "util/memory/count_ptr.tcc"

namespace HAC {
namespace entity {
using std::vector;
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;
using util::persistent_traits;
using util::string_list;

//=============================================================================
// class simple_meta_dummy_reference method definitions

/**
	Private empty constructor.  
 */
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_META_DUMMY_REFERENCE_CLASS::simple_meta_dummy_reference() :
		common_base_type(), 
		parent_type(), inst_collection_ref() {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_META_DUMMY_REFERENCE_CLASS::simple_meta_dummy_reference(
		const instance_placeholder_ptr_type pi) :
		common_base_type(), 
		parent_type(), 
		inst_collection_ref(pi) {
	NEVER_NULL(inst_collection_ref);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Special ctor that attaches indices initially. 
	\param pi placeholder instance pointer.
	\param i indices.  
	\pre in this variation, we require dimensions to be exact
		and not underspecified with the indices, i.e. only
		allowed for scalar references (for now).  
 */
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_META_DUMMY_REFERENCE_CLASS::simple_meta_dummy_reference(
		const instance_placeholder_ptr_type pi, 
		indices_ptr_arg_type i) :
		common_base_type(i), 
		parent_type(), 
		inst_collection_ref(pi) {
	NEVER_NULL(inst_collection_ref);
	INVARIANT(!this->dimensions());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_META_DUMMY_REFERENCE_CLASS::~simple_meta_dummy_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<const instance_placeholder_base>
SIMPLE_META_DUMMY_REFERENCE_CLASS::get_inst_base(void) const {
	return inst_collection_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_META_DUMMY_REFERENCE_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just wrapped around common base class implmentation.  
	Ripped off of the old simple_meta_indexed_reference_base::dump()
 */
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_META_DUMMY_REFERENCE_CLASS::dump(ostream& o, 
		const expr_dump_context& c) const {
	if (c.include_type_info)
		this->what(o) << " ";
	NEVER_NULL(this->inst_collection_ref);
	if (c.parent_instance_name.size()) {
		o << c.parent_instance_name << ".";
		this->inst_collection_ref->dump_qualified_name(o,
			dump_flags::no_definition_owner);
	} else if (c.enclosing_scope) {
		this->inst_collection_ref->dump_qualified_name(o,
			dump_flags::no_definition_owner);
	} else {
		this->inst_collection_ref->dump_qualified_name(o,
			dump_flags::default_value);
	}
	return simple_meta_indexed_reference_base::dump_indices(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_META_DUMMY_REFERENCE_CLASS::dump_local(ostream& o) const {
	const expr_dump_context edc(&*this->get_inst_base()->get_owner());
	return dump(o, edc);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Simplified from simple_meta_indexed_reference_base::dump_type_size().
 */
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_META_DUMMY_REFERENCE_CLASS::dump_type_size(ostream& o) const {
	this->get_unresolved_type_ref()->dump(o);
	const size_t d = this->dimensions();
	if (d) {
		o << '{' << d << "-dim}";
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
size_t
SIMPLE_META_DUMMY_REFERENCE_CLASS::dimensions(void) const {
	size_t dim = this->inst_collection_ref->get_dimensions();
	if (array_indices) {
		const size_t c = this->array_indices->dimensions_collapsed();
		INVARIANT(c <= dim);
		return dim -c;
	} else	return dim;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<const definition_base>
SIMPLE_META_DUMMY_REFERENCE_CLASS::get_base_def(void) const {
	return this->inst_collection_ref->get_base_def();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const fundamental_type_reference>
SIMPLE_META_DUMMY_REFERENCE_CLASS::get_unresolved_type_ref(void) const {
	return this->inst_collection_ref->get_unresolved_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Copy-reduced from simple_meta_indexed_reference_base.
 */
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
good_bool
SIMPLE_META_DUMMY_REFERENCE_CLASS::attach_indices(indices_ptr_arg_type i) {
	INVARIANT(!array_indices);
	NEVER_NULL(i);
	// dimension-check:
	// number of indices must be <= dimension of instance collection.  
	const size_t max_dim = dimensions();    // depends on indices
	if (i->size() > max_dim) {
		cerr << "ERROR: instance collection " <<
			this->inst_collection_ref->get_name()
			<< " is " << max_dim << "-dimensional, and thus, "
			"cannot be indexed " << i->size() <<
			"-dimensionally!  ";
			// caller will say where
		return good_bool(false);
	}
	// no static dimension checking
	array_indices = i;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Internal nodes are not reference-able as physical instances.  
 */
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
global_indexed_reference
SIMPLE_META_DUMMY_REFERENCE_CLASS::lookup_top_level_reference(
		const global_entry_context&) const {
	ICE_NEVER_CALL(cerr);
	return global_indexed_reference(0, 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
good_bool
SIMPLE_META_DUMMY_REFERENCE_CLASS::lookup_top_level_references(
		const global_entry_context&,
		global_reference_array_type&) const {
	ICE_NEVER_CALL(cerr);
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRIVATE_MEMBER_REFERENCES
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
good_bool
SIMPLE_META_DUMMY_REFERENCE_CLASS::lookup_global_reference_indices(
		const global_entry_context&, 
		typed_index_array_reference&,
		const unroll_context*) const {
	ICE_NEVER_CALL(cerr);
	return good_bool(false);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const instance_management_base>
SIMPLE_META_DUMMY_REFERENCE_CLASS::create_instance_attribute(
		const count_ptr<const meta_instance_reference_base>&, 
		const generic_attribute_list_type&) const {
	ICE_NEVER_CALL(cerr);
	return count_ptr<const instance_management_base>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_DUMMY_REFERENCE_CLASS::accept(nonmeta_expr_visitor& v) const {
	ICE_NEVER_CALL(cerr);
//	v.visit(*this);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const SIMPLE_META_DUMMY_REFERENCE_CLASS>
SIMPLE_META_DUMMY_REFERENCE_CLASS::__unroll_resolve_copy(
		const unroll_context& c, 
		const count_ptr<const this_type>& p) const {
	typedef	count_ptr<this_type>	return_type;
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	if (this->array_indices) {
		const count_ptr<const const_index_list>
			resolved_indices(unroll_resolve_indices(c));
		if (!resolved_indices) {
			cerr << "Error resolving meta indices." << endl;
			return return_type(NULL);
		}
		const return_type
			ret(new this_type(this->inst_collection_ref));
		ret->attach_indices(resolved_indices);
		return ret;
	} else {
		return p;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Covariant wrapper.  
 */
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const meta_instance_reference_base>
SIMPLE_META_DUMMY_REFERENCE_CLASS::unroll_resolve_copy(
		const unroll_context& c, 
		const count_ptr<const meta_instance_reference_base>& p) const {
	return __unroll_resolve_copy(c, p.template is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called by member_dummy_reference::unroll_references_packed.
	This implementation should be policy-determined.  
	\return a single instance alias to a substructure_alias.
 */
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<substructure_alias>
SIMPLE_META_DUMMY_REFERENCE_CLASS::unroll_scalar_substructure_reference(
		const unroll_context&) const {
	STACKTRACE_VERBOSE;
	ICE_NEVER_CALL(cerr);
	return never_ptr<substructure_alias>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dummy types never belong in ports.  
 */
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
typename SIMPLE_META_DUMMY_REFERENCE_CLASS::port_connection_ptr_type
SIMPLE_META_DUMMY_REFERENCE_CLASS::make_port_connection_private(
		const count_ptr<const meta_instance_reference_base>& r) const {
	INVARIANT(r == this);
	ICE_NEVER_CALL(cerr);
	return port_connection_ptr_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 1
// placeholder functions hoisted from meta_instance_reference_subtype
// for the purpose of fulfilling pure virtual functions.  

SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
bool
SIMPLE_META_DUMMY_REFERENCE_CLASS::may_be_type_equivalent(
		const meta_instance_reference_base& r) const {
	return IS_A(const this_type*, &r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
bool
SIMPLE_META_DUMMY_REFERENCE_CLASS::must_be_type_equivalent(
		const meta_instance_reference_base& r) const {
	return IS_A(const this_type*, &r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
SIMPLE_META_DUMMY_REFERENCE_CLASS::connect_port(
		physical_instance_collection&, const unroll_context&) const {
	ICE_NEVER_CALL(cerr);
	return bad_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
meta_instance_reference_base::alias_connection_ptr_type
SIMPLE_META_DUMMY_REFERENCE_CLASS::make_aliases_connection_private(void) const {
	ICE_NEVER_CALL(cerr);
	return alias_connection_ptr_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dummy types are not aggregable.  
 */
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<aggregate_meta_instance_reference_base>
SIMPLE_META_DUMMY_REFERENCE_CLASS::make_aggregate_meta_instance_reference_private(
		void) const {
	ICE_NEVER_CALL(cerr);
	return count_ptr<aggregate_meta_instance_reference_base>(NULL);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_DUMMY_REFERENCE_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	common_base_type::collect_transient_info_base(m);
	inst_collection_ref->collect_transient_info(m);
	// instantiation_state has no pointers
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits children nodes and register pointers to object manager
	for serialization.  
	\param m the persistent object manager.
 */
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_DUMMY_REFERENCE_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	this->collect_transient_info_base(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Common functionality for this->write_object and 
	member_SIMPLE_META_DUMMY_REFERENCE_CLASS::write_object.
 */
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_DUMMY_REFERENCE_CLASS::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, inst_collection_ref);
	common_base_type::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes the instance reference to output stream, translating
	pointers to indices as it goes along.  
	Note: the instantiation base must be written before the
		state information, for reconstruction purposes.  
	\param m the persistent object manager.  
 */
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_DUMMY_REFERENCE_CLASS::write_object(
		const persistent_object_manager& m, ostream& f) const {
	this->write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Common functionality for this->load_object and
	member_SIMPLE_META_DUMMY_REFERENCE_CLASS::load_object.
 */
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_DUMMY_REFERENCE_CLASS::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, inst_collection_ref);
	NEVER_NULL(inst_collection_ref);
#if 0
	// necessary? nope, just let the object_manager do it
	m.load_object_once(const_cast<instance_collection_generic_type*>(
		&*inst_collection_ref));
#endif
	common_base_type::load_object_base(m, i);
}
// else already visited

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Loads the instance reference from an input stream, translating
	indices to pointers.  
	Note: the instantiation base must be loaded before the
		state information, because the instantiation state
		depends on the instantiation base being complete.  
	\param m the persistent object manager.  
 */
SIMPLE_META_DUMMY_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_DUMMY_REFERENCE_CLASS::load_object(
		const persistent_object_manager& m, istream& f) {
	this->load_object_base(m, f);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_SIMPLE_META_DUMMY_REFERENCE_TCC__

