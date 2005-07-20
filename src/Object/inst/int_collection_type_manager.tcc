/**
	\file "Object/inst/int_collection_type_manager.tcc"
	Template class for instance_collection's type manager.  
	$Id: int_collection_type_manager.tcc,v 1.2 2005/07/20 21:00:52 fang Exp $
 */

#ifndef	__OBJECT_INST_INT_COLLECTION_TYPE_MANAGER_TCC__
#define	__OBJECT_INST_INT_COLLECTION_TYPE_MANAGER_TCC__

#include <iostream>
#include "Object/art_object_fwd.h"
#include "Object/inst/int_collection_type_manager.h"
#include "Object/traits/int_traits.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_inst_stmt.h"
#include "Object/art_object_inst_stmt_data.h"
#include "Object/expr/pint_const.h"
#include "util/persistent_object_manager.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;

#define INT_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE		\
template <class Tag>

#define	INT_COLLECTION_TYPE_MANAGER_CLASS				\
int_collection_type_manager<Tag>

//=============================================================================
/**
	Pretty-print functor.  
 */
INT_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
struct INT_COLLECTION_TYPE_MANAGER_CLASS::dumper {
	ostream& os;

	dumper(ostream& o) : os(o) { }

	ostream&
	operator () (const instance_collection_generic_type& c) {
		return os << class_traits<Tag>::tag_name << '<' <<
			c.type_parameter << ">^" << c.get_dimensions();
	}
};	// end struct dumper

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INT_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
void
INT_COLLECTION_TYPE_MANAGER_CLASS::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	write_value(o, this->type_parameter);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INT_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
void
INT_COLLECTION_TYPE_MANAGER_CLASS::load_object_base(
		const persistent_object_manager& m, istream& i) {
	read_value(i, this->type_parameter);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INT_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
typename INT_COLLECTION_TYPE_MANAGER_CLASS::type_ref_ptr_type
INT_COLLECTION_TYPE_MANAGER_CLASS::get_type(
		const instance_collection_generic_type& i) const {
	if (this->type_parameter) {
		return type_ref_ptr_type(
			data_type_reference::make_quick_int_type_ref(
				this->type_parameter));
	} else {
		// not yet unrolled... need to extract from
		// first instantiation statement.
		// extract as in pulling teeth...
		// TODO: subtype versions of the following calls
		const never_ptr<const data_instantiation_statement>
			first(i.index_collection.front()
			.template is_a<const data_instantiation_statement>());
		return first->get_type_ref()
			.template is_a<const data_type_reference>();
	}
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
INT_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
bad_bool
INT_COLLECTION_TYPE_MANAGER_CLASS::commit_type(
		const type_ref_ptr_type& tp) const {
	const pint_value_type new_width = get_int_width(tp);
	INVARIANT(this->type_parameter);
	return bad_bool(new_width != this->type_parameter);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param t type must be resolved constant.
	\pre first time called for the collection.  
 */
INT_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
void
INT_COLLECTION_TYPE_MANAGER_CLASS::commit_type_first_time(
		const type_ref_ptr_type& tp) {
	INVARIANT(!this->type_parameter);
	INVARIANT(tp->is_resolved());
	INVARIANT(tp->is_canonical());
	this->type_parameter = get_int_width(tp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Utility function for extracting the integer width from
	a generic data type reference.  
	TODO: if we sub-type data-types, this will be phased out
 */
INT_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
pint_value_type
INT_COLLECTION_TYPE_MANAGER_CLASS::get_int_width(
		const type_ref_ptr_type& tp) {
	const type_ref_ptr_type
		t(tp->make_canonical_type_reference()
			.template is_a<const data_type_reference>());
	INVARIANT(t->get_base_datatype_def()
		->resolve_canonical_datatype_definition() ==
			&int_traits::built_in_definition);

	const count_ptr<const param_expr_list>
		params(t->get_template_params().get_strict_args());
	NEVER_NULL(params);
	// extract first and only parameter, the integer width
	const count_ptr<const const_param_expr_list>
		cparams(params.template is_a<const const_param_expr_list>());

	NEVER_NULL(cparams);
	INVARIANT(cparams->size() == 1);
	const count_ptr<const const_param>&
		param1(cparams->front());
	NEVER_NULL(param1);
	const count_ptr<const pint_const>
		pwidth(param1.template is_a<const pint_const>());
	NEVER_NULL(pwidth);
	return pwidth->static_constant_value();
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#undef INT_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
#undef INT_COLLECTION_TYPE_MANAGER_CLASS

#endif	// __OBJECT_INST_INT_COLLECTION_TYPE_MANAGER_TCC__

