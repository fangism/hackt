/**
	\file "Object/ref/member_meta_instance_reference.tcc"
	Method definitions for the meta_instance_reference family of objects.
	This file was reincarnated from "Object/art_object_member_inst_ref.tcc"
 	$Id: member_meta_instance_reference.tcc,v 1.21.8.1 2006/12/12 10:18:15 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_MEMBER_META_INSTANCE_REFERENCE_TCC__
#define	__HAC_OBJECT_REF_MEMBER_META_INSTANCE_REFERENCE_TCC__

#include <iostream>
#include "util/what.h"
#include "util/persistent_object_manager.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/inst/physical_instance_placeholder.h"
#include "Object/ref/member_meta_instance_reference.h"
#include "Object/ref/inst_ref_implementation.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/expr_visitor.h"
#include "Object/def/footprint.h"
#include "Object/unroll/unroll_context.h"
#include "Object/global_entry.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/stacktrace.h"
#include "common/ICE.h"

//=============================================================================
namespace HAC {
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
		const instance_placeholder_ptr_type m) :
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
	Dump brief, but dump parent instances too, 
	just omit the ultimate owner definition.  
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
MEMBER_INSTANCE_REFERENCE_CLASS::dump(ostream& o, 
		const expr_dump_context& c) const {
	base_inst_ref->dump(o, c);
	return parent_type::dump(o << '.', c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resolves the parent (super) instance first.  
	NOTE: this should NOT be used to lookup private members
		because they are not allocated util the alloc phase.  
		This is still unroll/create time.  
	\return pointer to modifiable instance collection, 
		belonging to the footprint being unrolled.  
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
typename MEMBER_INSTANCE_REFERENCE_CLASS::parent_member_ptr_type
MEMBER_INSTANCE_REFERENCE_CLASS::resolve_parent_member_helper(
		const unroll_context& c) const {
	typedef	parent_member_ptr_type			return_type;
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	this->dump(STACKTRACE_INDENT << "ref: ",
		expr_dump_context::default_value) << endl;
	STACKTRACE_INDENT_PRINT("c\'s target footprint:" << endl);
	c.dump(cerr) << endl;
#endif
	// this also include member_meta_instance_references
	const base_inst_type& _parent_inst_ref(*this->base_inst_ref);
	if (_parent_inst_ref.dimensions()){
		cerr << "ERROR: parent instance reference of a "
			"member reference must be scalar." << endl <<
			"However, non-scalar member-parent references "
			"may be introduced in the future, bug Fang about it."
			<< endl;
		return return_type(NULL);
	}
	const never_ptr<substructure_alias>
		parent_struct(
			_parent_inst_ref.unroll_scalar_substructure_reference(c));
	if (!parent_struct) {
		_parent_inst_ref.dump(
			cerr << "ERROR resolving member reference parent ", 
			expr_dump_context::default_value) << endl;
		return return_type(NULL);
	}
	// assert dynamic cast
	const physical_instance_placeholder&
		phys_inst(IS_A(const physical_instance_placeholder&, 
			*this->get_inst_base()));
	const never_ptr<instance_collection_base>
		resolved_instance(
			parent_struct->lookup_port_instance(phys_inst));
	if (!resolved_instance) {
		cerr << "ERROR resolving port instance." << endl;
		return return_type(NULL);
	}
	const return_type
		inst_base(resolved_instance
			.template is_a<collection_interface_type>());
	// expecting the right type!
	INVARIANT(inst_base);
#if ENABLE_STACKTRACE
	inst_base->dump(STACKTRACE_INDENT << "inst_base: ", 
		dump_flags::default_value) << endl;
#endif
	return inst_base;
}	// end method resolve_parent_member_helper

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recurse through parent instances first.  
	This can be used to lookup private members.  
	TODO: re-use most of resolve_parent_member_helper, 
		up until the lookup part?
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
size_t
MEMBER_INSTANCE_REFERENCE_CLASS::lookup_globally_allocated_index(
		const state_manager& sm, const footprint& top) const {
	STACKTRACE_VERBOSE;
	const base_inst_type& _parent_inst_ref(*this->base_inst_ref);
	if (_parent_inst_ref.dimensions()) {
		// error message copied from above
		cerr << "ERROR: parent instance reference of a "
			"member reference must be scalar." << endl <<
			"However, non-scalar member-parent references "
			"may be introduced in the future, bug Fang about it."
			<< endl;
		return 0;
	}
	const footprint_frame* const fpf =
		_parent_inst_ref.lookup_footprint_frame(sm, top);
	if (!fpf) {
		// TODO: better error message
		cerr << "Failure resolving parent instance reference" << endl;
		return 0;
	}
	const footprint* const fp = fpf->_footprint;
	NEVER_NULL(fp);
	const string& member_name(this->get_inst_base()->get_name());
	const footprint::instance_collection_ptr_type fi((*fp)[member_name]);
	if (!fi) {
		cerr << "No instance member named \'" <<  member_name <<
			"\' found." << endl;
		return 0;
	}
	// if found, we assert-cast its type: same as what we used to lookup
	const collection_interface_type&
		pi(IS_A(const collection_interface_type&, *fi));
	// if not, we have a serious inconsistency (throw bad_cast)
	// now we can get the local frame offset from an index
	// lookup with this collection.  

	// now need to compute the offset into the corresponding 
	// footprint_frame_map
	// we look for the local alias to get the local offset!
	const unroll_context uc(&top, &top);
	const instance_alias_info_ptr_type
		local_alias(__unroll_generic_scalar_reference_no_lookup(
			pi, this->array_indices, uc));
	if (!local_alias) {
		// TODO: better error message
		cerr << "Error resolving member instance alias." << endl;
		return 0;
	}
	const size_t ind = local_alias->instance_index;
	INVARIANT(ind);
	const footprint_frame_map_type& ffm(fpf->template get_frame_map<Tag>());
	// this lookup returns a globally allocated index
	return ffm[ind -1];	// 0-indexed offset
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
MEMBER_INSTANCE_REFERENCE_CLASS::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

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
MEMBER_INSTANCE_REFERENCE_CLASS::unroll_references_packed(
		const unroll_context& c, alias_collection_type& a) const {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	this->dump(STACKTRACE_INDENT << "ref: ",
		expr_dump_context::default_value) << endl;
	STACKTRACE_INDENT << "&c = " << &c << endl;
	STACKTRACE_INDENT << "c\'s target footprint:" << endl;
	c.dump(cerr) << endl;
#endif
	const parent_member_ptr_type
		inst_base(resolve_parent_member_helper(c));
	if (!inst_base) {
		// already have error message
		return bad_bool(true);
	}
	// do not look up the member using the footprint!
	// only the ultimate parent of the reference should use the footprint
	// copy the unroll_context *except* for the footprint pointer
	const unroll_context cc(c.make_member_context());

	// DO NOT CALL simple_meta_instance_reference::unroll_references_packed_helper
	// because that performs an additional lookup in the context
	// which is NOT necessary, because the member was already resolved.  
	// instead, call unroll_references_packed_helper_no_lookup(...)
	return unroll_references_packed_helper_no_lookup(
		cc, *inst_base, this->array_indices, a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
typename MEMBER_INSTANCE_REFERENCE_CLASS::instance_alias_info_ptr_type
MEMBER_INSTANCE_REFERENCE_CLASS::unroll_generic_scalar_reference(
		const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	const parent_member_ptr_type
		inst_base(resolve_parent_member_helper(c));
	if (!inst_base) {
		// already have error message
		return instance_alias_info_ptr_type(NULL);
	}
	const unroll_context cc(c.make_member_context());
	// The following call should NOT be doing extra lookup! (pass false)
	return __unroll_generic_scalar_reference_no_lookup(
			*inst_base, this->array_indices, cc);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return a hierarchical instance alias.  
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<substructure_alias>
MEMBER_INSTANCE_REFERENCE_CLASS::unroll_scalar_substructure_reference(
		const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	const parent_member_ptr_type
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
	// The following should NOT be doing extra lookup! (pass false)
	return parent_type::substructure_implementation_policy::
		template unroll_generic_scalar_substructure_reference<Tag>(
			*inst_base, this->array_indices, cc);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Using this instance-reference's globally allocated index, 
	just indexes into state_manager's global pool
	to find the footprint_frame.
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
const footprint_frame*
MEMBER_INSTANCE_REFERENCE_CLASS::lookup_footprint_frame(
		const state_manager& sm, const footprint& top) const {
	STACKTRACE_VERBOSE;
	return parent_type::substructure_implementation_policy::
		template member_lookup_footprint_frame<Tag>(*this, sm, top);
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
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_MEMBER_META_INSTANCE_REFERENCE_TCC__

