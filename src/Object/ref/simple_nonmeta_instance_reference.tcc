/**
	\file "Object/ref/simple_nonmeta_instance_reference.tcc"
	This file was "Object/art_object_nonmeta_inst_ref.tcc"
		in a previous life.  
	$Id: simple_nonmeta_instance_reference.tcc,v 1.6 2006/03/16 03:49:31 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_NONMETA_INSTANCE_REFERENCE_TCC__
#define	__HAC_OBJECT_REF_SIMPLE_NONMETA_INSTANCE_REFERENCE_TCC__

#include <iostream>
#include "Object/ref/simple_nonmeta_instance_reference.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/common/dump_flags.h"
#include "Object/expr/nonmeta_index_list.h"
#include "util/what.h"
#include "util/persistent_object_manager.tcc"

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
		common_base_type(), parent_type(), inst_collection_ref() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::simple_nonmeta_instance_reference(
		const instance_collection_ptr_type i) :
		common_base_type(), parent_type(), inst_collection_ref(i) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::~simple_nonmeta_instance_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
typename SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::instance_collection_ptr_type
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::get_inst_base_subtype(void) const {
	return inst_collection_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<const instance_collection_base>
SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS::get_inst_base(void) const {
	return inst_collection_ref;
}

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
		this->inst_collection_ref->dump_hierarchical_name(o,
			dump_flags::no_definition_owner);
	} else {
		this->inst_collection_ref->dump_hierarchical_name(o,
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

