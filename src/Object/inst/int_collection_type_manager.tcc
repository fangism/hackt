/**
	\file "Object/inst/int_collection_type_manager.tcc"
	Template class for instance_collection's type manager.  
	$Id: int_collection_type_manager.tcc,v 1.11 2006/10/18 01:19:34 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INT_COLLECTION_TYPE_MANAGER_TCC__
#define	__HAC_OBJECT_INST_INT_COLLECTION_TYPE_MANAGER_TCC__

#include <iostream>
#include "Object/inst/int_collection_type_manager.h"
#include "Object/traits/int_traits.h"
#include "Object/type/data_type_reference.h"
#include "Object/unroll/instantiation_statement.h"
#include "Object/unroll/datatype_instantiation_statement.h"
#include "Object/expr/pint_const.h"
#include "util/persistent_object_manager.h"
#include "util/stacktrace.h"

namespace HAC {
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
		return os << traits_type::tag_name << '<' <<
			c.type_parameter << '>';
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
#if !USE_INSTANCE_PLACEHOLDERS
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
#if USE_INSTANCE_PLACEHOLDERS
		return i.get_placeholder()->
			get_initial_instantiation_statement().
				template is_a<const data_type_reference>();
#else
		const never_ptr<const data_instantiation_statement>
			first(i.get_initial_instantiation_statement());
		return first->get_type_ref()
			.template is_a<const data_type_reference>();
#endif
	}
}
#endif

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
INT_COLLECTION_TYPE_MANAGER_CLASS::check_type(
		const instance_collection_parameter_type& tp) const {
	INVARIANT(this->type_parameter);
	return bad_bool(tp != this->type_parameter);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param t type must be resolved constant.
	\pre first time called for the collection.  
	NOTE: when the deep copy of the instance collection is made, 
		the collection parameter type could've been copied over.  
 */
INT_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
good_bool
INT_COLLECTION_TYPE_MANAGER_CLASS::commit_type_first_time(
		const instance_collection_parameter_type& tp) {
	// INVARIANT(!this->type_parameter);
	if (tp > 0) {
		this->type_parameter = tp;
		return good_bool(true);
	} else {
		cerr << "Error: int<> must be instantiated with a "
			"positive width, but got: " << tp << endl;
		return good_bool(false);
	}
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
		t(tp->make_canonical_fundamental_type_reference()
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
}	// end namespace HAC

#undef INT_COLLECTION_TYPE_MANAGER_TEMPLATE_SIGNATURE
#undef INT_COLLECTION_TYPE_MANAGER_CLASS

#endif	// __HAC_OBJECT_INST_INT_COLLECTION_TYPE_MANAGER_TCC__

