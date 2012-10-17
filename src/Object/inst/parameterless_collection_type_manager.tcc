/**
	\file "Object/inst/parameterless_collection_type_manager.tcc"
	Template class for instance_collection's type manager.  
	$Id: parameterless_collection_type_manager.tcc,v 1.13 2006/10/18 20:58:04 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PARAMETERLESS_COLLECTION_TYPE_MANAGER_TCC__
#define	__HAC_OBJECT_INST_PARAMETERLESS_COLLECTION_TYPE_MANAGER_TCC__

#include "Object/inst/parameterless_collection_type_manager.hh"
#include "Object/type/data_type_reference.hh"
#include "Object/def/enum_datatype_def.hh"
#include <iostream>
#include "util/persistent_object_manager.hh"

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"

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
		return os << traits_type::tag_name << ' ' <<
			c.type_parameter->get_qualified_name();
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
typename PARAMETERLESS_COLLECTION_TYPE_MANAGER_CLASS::resolved_type_ref_type
PARAMETERLESS_COLLECTION_TYPE_MANAGER_CLASS::get_resolved_canonical_type(void) const {
	return resolved_type_ref_type(this->type_parameter);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: called during connection checking.  
 */
PARAMETERLESS_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
bool
PARAMETERLESS_COLLECTION_TYPE_MANAGER_CLASS::
		must_be_collectibly_type_equivalent(const this_type& r) const {
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
PARAMETERLESS_COLLECTION_TYPE_MANAGER_CLASS::check_type(
		const instance_collection_parameter_type& t) const {
	INVARIANT(this->type_parameter);
	return bad_bool(this->type_parameter != t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param t type must be resolved constant.
	\pre first time called for the collection.  
	TODO: handle typedef'd enums!
 */
PARAMETERLESS_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
good_bool
PARAMETERLESS_COLLECTION_TYPE_MANAGER_CLASS::commit_type_first_time(
		const instance_collection_parameter_type& t) {
	NEVER_NULL(t);
	INVARIANT(!this->type_parameter);
	this->type_parameter = t;
	return good_bool(true);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#undef PARAMETERLESS_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
#undef PARAMETERLESS_COLLECTION_TYPE_MANAGER_CLASS

#endif	// __HAC_OBJECT_INST_PARAMETERLESS_COLLECTION_TYPE_MANAGER_TCC__

