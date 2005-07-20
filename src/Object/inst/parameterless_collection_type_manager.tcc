/**
	\file "Object/inst/parameterless_collection_type_manager.tcc"
	Template class for instance_collection's type manager.  
	$Id: parameterless_collection_type_manager.tcc,v 1.2 2005/07/20 21:00:53 fang Exp $
 */

#ifndef	__OBJECT_INST_PARAMETERLESS_COLLECTION_TYPE_MANAGER_TCC__
#define	__OBJECT_INST_PARAMETERLESS_COLLECTION_TYPE_MANAGER_TCC__

#include "Object/inst/parameterless_collection_type_manager.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_definition_data.h"
#include <iostream>
#include "util/persistent_object_manager.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"

#define PARAMETERLESS_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE	\
template <class Tag>

#define	PARAMETERLESS_COLLECTION_TYPE_MANAGER_CLASS			\
parameterless_collection_type_manager<Tag>

//=============================================================================
/**
	Pretty-print functor.  
 */
PARAMETERLESS_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
struct PARAMETERLESS_COLLECTION_TYPE_MANAGER_CLASS::dumper {
	ostream& os;

	dumper(ostream& o) : os(o) { }

	ostream&
	operator () (const instance_collection_generic_type& c) {
		return os << class_traits<Tag>::tag_name << ' ' <<
			c.type_parameter->get_qualified_name() <<
			'^' << c.get_dimensions();
	}
};	// end struct dumper

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PARAMETERLESS_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
void
PARAMETERLESS_COLLECTION_TYPE_MANAGER_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	if (this->type_parameter)
		this->type_parameter->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PARAMETERLESS_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
void
PARAMETERLESS_COLLECTION_TYPE_MANAGER_CLASS::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, this->type_parameter);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PARAMETERLESS_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
void
PARAMETERLESS_COLLECTION_TYPE_MANAGER_CLASS::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, this->type_parameter);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PARAMETERLESS_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
typename PARAMETERLESS_COLLECTION_TYPE_MANAGER_CLASS::type_ref_ptr_type
PARAMETERLESS_COLLECTION_TYPE_MANAGER_CLASS::get_type(
		const instance_collection_generic_type& e) const {
#if 0
	// can't to this, may be NULL because the collection's type
	// hasn't been set yet
	NEVER_NULL(this->type_parameter);	// NOT true
	return type_ref_ptr_type(
		new data_type_reference(this->type_parameter));
#else
	return type_ref_ptr_type(new data_type_reference(
		// want get_base_def_subtype!!!
		e.get_base_def()
		.template is_a<const datatype_definition_base>()));
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: called during connection checking.  
 */
PARAMETERLESS_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
bool
PARAMETERLESS_COLLECTION_TYPE_MANAGER_CLASS::must_match_type(
		const this_type& r) const {
	// must be typedef-resolved!
	return this->type_parameter == r.type_parameter;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	During unroll phase, this commits the type of the collection.  
	\param t the data integer type reference, containing width, 
		must already be resolved to a const_param_expr_list.  
	\return false on success, true on error.  
	\post the integer width is fixed for the rest of the program.  
	2005-07-07:
	TODO: given complete type t, can't distinguish between
		establishing strict type for the entire array
		vs. relaxed type with instance-specific 
		relaxed actuals!!!
		We choose to work around this in 
		instantiation_statement<>::unroll().
	NOW just type checks without committing.  
	TODO: RENAME
 */
PARAMETERLESS_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
bad_bool
PARAMETERLESS_COLLECTION_TYPE_MANAGER_CLASS::commit_type(
		const type_ref_ptr_type& t) const {
	INVARIANT(this->type_parameter);
	return bad_bool(this->type_parameter != t->get_base_def());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param t type must be resolved constant.
	\pre first time called for the collection.  
	TODO: handle typedef'd enums!
 */
PARAMETERLESS_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
void
PARAMETERLESS_COLLECTION_TYPE_MANAGER_CLASS::commit_type_first_time(
		const type_ref_ptr_type& t) {
	INVARIANT(!this->type_parameter);
#if 0
	INVARIANT(t->is_resolved());
	INVARIANT(t->is_canonical());
#endif
	this->type_parameter =
		t->get_base_def().template is_a<
			const typename class_traits<Tag>::
				instance_collection_parameter_type::
				element_type>();
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#undef PARAMETERLESS_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
#undef PARAMETERLESS_COLLECTION_TYPE_MANAGER_CLASS

#endif	// __OBJECT_INST_PARAMETERLESS_COLLECTION_TYPE_MANAGER_TCC__

