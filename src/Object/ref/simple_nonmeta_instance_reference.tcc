/**
	\file "Object/ref/simple_nonmeta_instance_reference.tcc"
	This file was "Object/art_object_nonmeta_inst_ref.tcc"
		in a previous life.  
	$Id: simple_nonmeta_instance_reference.tcc,v 1.12 2007/01/21 05:59:36 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_NONMETA_INSTANCE_REFERENCE_TCC__
#define	__HAC_OBJECT_REF_SIMPLE_NONMETA_INSTANCE_REFERENCE_TCC__

#include <iostream>
#include <vector>
#include <algorithm>

#include "Object/ref/simple_nonmeta_instance_reference.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/expr_visitor.h"
#include "Object/common/dump_flags.h"
#include "Object/expr/nonmeta_index_list.h"
#include "Object/unroll/unroll_context.h"
#include "Object/global_entry.h"
#include "Object/ref/nonmeta_ref_implementation.tcc"
#include "util/what.h"
#include "util/persistent_object_manager.tcc"

#if ENABLE_STACKTRACE
#include <iterator>		// for ostream_iterator
#endif

// might as well include this, will be needed
#include "Object/ref/nonmeta_instance_reference_subtypes.h"

namespace HAC {
namespace entity {
using util::persistent_traits;
#include "util/using_ostream.h"

//=============================================================================
// class simple_nonmeta_instance_reference method definitions

/// private empty constructor
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::simple_nonmeta_instance_reference() :
		common_base_type(), parent_type(),
		inst_collection_ref() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::simple_nonmeta_instance_reference(
		const instance_placeholder_ptr_type i) :
		common_base_type(), parent_type(),
		inst_collection_ref(i) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::~simple_nonmeta_instance_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// inlined
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
typename SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::instance_placeholder_ptr_type
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::get_inst_base_subtype(void) const {
	return inst_collection_ref;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
size_t
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::dimensions(void) const {
	size_t dim = this->inst_collection_ref->get_dimensions();
	if (array_indices) {
		const size_t c = this->array_indices->dimensions_collapsed();
		INVARIANT(c <= dim);
		return dim -c;
	}
	else return dim;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::dump(ostream& o, 
		const expr_dump_context& c) const {
	if (c.include_type_info)
		this->what(o) << " ";
	NEVER_NULL(this->inst_collection_ref);
	if (c.enclosing_scope) {
		this->inst_collection_ref->dump_qualified_name(o,
			dump_flags::no_definition_owner);
	} else {
		this->inst_collection_ref->dump_qualified_name(o,
			dump_flags::default_value);
	}
	return simple_nonmeta_instance_reference_base::dump_indices(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Tentative: no nonscalar references allowed in the nonmeta language.
 */
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::attach_indices(
	excl_ptr<index_list_type>& i) {
	INVARIANT(!this->array_indices);
	NEVER_NULL(i);

	// dimension-check:
	// number of indices must be <= dimension of instance collection.  
	const size_t max_dim =
		this->inst_collection_ref->get_dimensions();
	if (i->size() != max_dim) {
		cerr << "ERROR: instance collection " <<
			this->inst_collection_ref->get_name()
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

	this->array_indices = i;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ripped from simple_nonmeta_value_reference::unroll_resolve_copy().
 */
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS>
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::unroll_resolve_copy(
		const unroll_context& c,
		const count_ptr<const this_type>& p) const {
	INVARIANT(p == this);
	if (this->array_indices) {
		// resolve the indices
		count_ptr<index_list_type>
			resolved_indices(this->array_indices
				->unroll_resolve_copy(c));
		if (!resolved_indices) {
			cerr << "Error resolving nonmeta value reference\'s "
				"indices." << endl;
			return count_ptr<this_type>(NULL);
		}
		if (std::equal(resolved_indices->begin(),
				resolved_indices->end(),
				this->array_indices->begin())) {
			// then resolution changed nothing, return this-copy
			return p;
		} else {
			excl_ptr<index_list_type>
				ri(resolved_indices.exclusive_release());
			count_ptr<this_type>
				ret(new this_type(this->inst_collection_ref));
			ret->attach_indices(ri);
			INVARIANT(!ri);		// transferred ownership
			return ret;
		}
	} else {
		// is scalar reference (cannot be implicit indices!)
		// therefore, just return this copy!
		return p;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This looks up a nonmeta reference and returns a list of all 
	globally allocated indices that MAY be referenced.
	The list is precise if the indices are resolved to
	compile-time constants (meta-parameters).  
	Called by CHPSIM::DependenceSetCollector::visit().
	MAINTAINENCE: Don't forget to update "simple_nonmeta_value_reference"
		__lookup_may_..._impl(), which copied from here...
	\param fp the *local* footprint to lookup.
	\param sm the top-level state-manager with global allocation
		information, and footprint frames.  
 */
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::lookup_may_reference_global_indices(
		const global_entry_context& c, 
		vector<size_t>& indices) const {
	STACKTRACE_VERBOSE;
	return __nonmeta_instance_lookup_may_reference_indices_impl(
		*this, c, indices, Tag());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Looks up a run-time scalar reference.
 */
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
size_t
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::lookup_nonmeta_global_index(
		const nonmeta_context_base& c) const {
	return __nonmeta_instance_global_lookup_impl(*this, c, Tag());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	common_base_type::collect_transient_info_base(m);
	inst_collection_ref->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::write_object(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, inst_collection_ref);
	common_base_type::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::load_object(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, inst_collection_ref);
	// need to load inst_collection_ref?
	common_base_type::load_object_base(m, i);
}

//=============================================================================
}	// end namespace entity
}	// end namepace HAC

#endif	// __HAC_OBJECT_REF_SIMPLE_NONMETA_INSTANCE_REFERENCE_TCC__

