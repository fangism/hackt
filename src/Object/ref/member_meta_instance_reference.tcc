/**
	\file "Object/ref/member_meta_instance_reference.tcc"
	Method definitions for the meta_instance_reference family of objects.
	This file was reincarnated from "Object/art_object_member_inst_ref.tcc"
 	$Id: member_meta_instance_reference.tcc,v 1.3.2.1 2005/09/14 05:40:09 fang Exp $
 */

#ifndef	__OBJECT_REF_MEMBER_META_INSTANCE_REFERENCE_TCC__
#define	__OBJECT_REF_MEMBER_META_INSTANCE_REFERENCE_TCC__

#include <iostream>
#include "util/what.h"
#include "util/persistent_object_manager.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/ref/member_meta_instance_reference.h"
#include "Object/ref/inst_ref_implementation.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/unroll/unroll_context.h"
#include "util/memory/count_ptr.tcc"
#include "util/stacktrace.h"

//=============================================================================
namespace ART {
namespace entity {
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;
using util::persistent_traits;

//=============================================================================
// class member_meta_instance_reference method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
MEMBER_INSTANCE_REFERENCE_CLASS::member_meta_instance_reference() :
		parent_type(), base_inst_ref() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
MEMBER_INSTANCE_REFERENCE_CLASS::member_meta_instance_reference(
		const base_inst_ptr_type& b, 
		const instance_collection_ptr_type m) :
		parent_type(m), base_inst_ref(b) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
MEMBER_INSTANCE_REFERENCE_CLASS::~member_meta_instance_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
MEMBER_INSTANCE_REFERENCE_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resolves the parent (super) instance first.  
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<typename MEMBER_INSTANCE_REFERENCE_CLASS::instance_collection_generic_type>
MEMBER_INSTANCE_REFERENCE_CLASS::resolve_parent_member_helper(
		const unroll_context& c) const {
	typedef	count_ptr<instance_collection_generic_type>	return_type;
	STACKTRACE_VERBOSE;
	// this also include member_meta_instance_references
	const base_inst_type& _parent_inst_ref(
		IS_A(const simple_meta_instance_reference_base&, 
			*this->base_inst_ref));
	if (_parent_inst_ref.dimensions()) {
		cerr << "ERROR: parent instance reference of a "
			"member reference must be scalar." << endl <<
			"However, non-scalar member-parent references "
			"may be introduced in the future, bug Fang about it."
			<< endl;
		return return_type(NULL);
	}
	const never_ptr<substructure_alias>
		parent_struct(
			_parent_inst_ref.unroll_generic_scalar_reference(c));
	if (!parent_struct) {
		_parent_inst_ref.dump(
			cerr << "ERROR resolving member reference parent ")
			<< endl;
		return return_type(NULL);
	}
	const count_ptr<instance_collection_base>
		resolved_instance(parent_struct->lookup_port_instance(
			// assert dynamic cast
			IS_A(const physical_instance_collection&, 
				*this->get_inst_base())
		));
	if (!resolved_instance) {
		cerr << "ERROR resolving port instance." << endl;
		return return_type(NULL);
	}
	const return_type
		inst_base(resolved_instance
			.template is_a<instance_collection_generic_type>());
	// expecting the right type!
	INVARIANT(inst_base);
	return inst_base;
}	// end method resolve_parent_member_helper

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resolves a member instance reference.
	Procedure:
	\return error status.  
	TODO: far future, support for non-scalar parents.  
		That will allow effective construction of higher (>4)
		dimensional arrays!
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
MEMBER_INSTANCE_REFERENCE_CLASS::unroll_references(
		const unroll_context& c, alias_collection_type& a) const {
	STACKTRACE_VERBOSE;
	const count_ptr<instance_collection_generic_type>
		inst_base(resolve_parent_member_helper(c));
	if (!inst_base) {
		// already have error message
		return bad_bool(true);
	}
	// do not look up the member using the footprint!
	// only the ultimate parent of the reference should use the footprint
	// copy the unroll_context *except* for the footprint pointer
	const unroll_context cc(c.make_member_context());
	return unroll_references_helper(cc, *inst_base, this->array_indices, a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return a hierarchical instance alias.  
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<substructure_alias>
MEMBER_INSTANCE_REFERENCE_CLASS::unroll_generic_scalar_reference(
		const unroll_context& c) const {
	typedef	simple_meta_instance_reference_implementation<
			class_traits<Tag>::has_substructure>
				substructure_implementation_policy;
	STACKTRACE_VERBOSE;
	const count_ptr<instance_collection_generic_type>
		inst_base(resolve_parent_member_helper(c));
	if (!inst_base) {
		// already have error message
		return never_ptr<substructure_alias>(NULL);
	}
	const unroll_context cc(c.make_member_context());
	// FYI: the above line was added too fix case process/107,109
	// should remove the footprint from the context
	// only the ultimate parent of the reference should use the footprint
	// copy the unroll_context *except* for the footprint pointer
	return substructure_implementation_policy::
		template unroll_generic_scalar_reference<Tag>(
			*inst_base, this->array_indices, cc);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits children nodes and register pointers to object manager
	for serialization.  
	\param m the persistent object manager.
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
MEMBER_INSTANCE_REFERENCE_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	parent_type::collect_transient_info_base(m);
	NEVER_NULL(base_inst_ref);
	base_inst_ref->collect_transient_info(m);
	// instantiation_state has no pointers
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes the instance reference to output stream, translating
	pointers to indices as it goes along.  
	Note: the instantiation base must be written before the
		state information, for reconstruction purposes.  
	\param m the persistent object manager.  
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
MEMBER_INSTANCE_REFERENCE_CLASS::write_object(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, base_inst_ref);
	parent_type::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Loads the instance reference from an input stream, translating
	indices to pointers.  
	Note: the instantiation base must be loaded before the
		state information, because the instantiation state
		depends on the instantiation base being complete.  
	\param m the persistent object manager.  
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
MEMBER_INSTANCE_REFERENCE_CLASS::load_object(
		const persistent_object_manager& m, istream& f) {
	m.read_pointer(f, base_inst_ref);
	NEVER_NULL(base_inst_ref);
	m.load_object_once(const_cast<base_inst_type*>(&*base_inst_ref));
	parent_type::load_object_base(m, f);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_REF_MEMBER_META_INSTANCE_REFERENCE_TCC__

