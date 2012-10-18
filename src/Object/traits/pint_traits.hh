/**
	\file "Object/traits/pint_traits.hh"
	Traits and policies for parameter integers.  
	This file used to be "Object/art_object_pint_traits.h".
	$Id: pint_traits.hh,v 1.18 2009/09/14 21:17:07 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_PINT_TRAITS_H__
#define	__HAC_OBJECT_TRAITS_PINT_TRAITS_H__

#include "Object/traits/class_traits.hh"

namespace HAC {
namespace entity {
//-----------------------------------------------------------------------------
template <>
struct class_traits<pint_tag> {
	template <class Tag>
	struct rebind {	typedef	class_traits<Tag>	type; };

	typedef	pint_tag			tag_type;
	typedef	int_tag				nonmeta_tag_type;
	static const char			tag_name[];
	static const char			value_type_name[];
	enum { type_tag_enum_value = META_TYPE_PINT };
	typedef	pint_instance			instance_type;
	typedef	pint_value_type			value_type;
	typedef	int_value_type			data_value_type;
	enum {		is_nonmeta_data_lvalue = false		};

	typedef	pint_instance_collection	instance_collection_generic_type;
	typedef	param_value_placeholder		value_placeholder_parent_type;
	typedef	pint_value_placeholder		instance_placeholder_type;
	typedef	instance_collection_generic_type
						value_collection_generic_type;
	typedef	param_value_collection		value_collection_parent_type;
	template <size_t D>
	struct value_array {
		typedef	entity::value_array<tag_type,D>	type;
	};
	typedef	pint_const			const_expr_type;
	typedef	pint_const_collection		const_collection_type;

	typedef	instantiation_statement_base
					instantiation_statement_parent_type;
	typedef	pint_instantiation_statement
					instantiation_statement_type;

	// define this elsewhere, in "traits/inst_stmt_chan.hh"
	class instantiation_statement_type_ref_base;

	// this will have a different template base
	typedef	data_nonmeta_instance_reference
					nonmeta_instance_reference_base_type;
	typedef	simple_pint_nonmeta_instance_reference
					simple_nonmeta_instance_reference_type;
	typedef	simple_pint_meta_value_reference
					simple_meta_value_reference_type;
	typedef	pint_meta_value_reference_base
				meta_value_reference_parent_type;
	typedef	int_expr			data_expr_base_type;
	typedef	pint_expr			expr_base_type;
	typedef const_param			const_collection_parent_type;

	typedef	never_ptr<pint_instance>	value_reference_ptr_type;
	typedef	packed_array_generic<pint_value_type, value_reference_ptr_type>
						value_reference_collection_type;
	typedef	pint_expression_assignment	expression_assignment_type;
	typedef	param_expression_assignment	expression_assignment_parent_type;
	enum { assignment_chunk_size = 32 };

	typedef	void			value_collection_parameter_type;
	typedef	null_parameter_type	instance_collection_parameter_type;

	typedef	param_type_reference		type_ref_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
	// typedef	canonical_generic_datatype	resolved_type_ref_type;

	/**
		Replacement for built-in type pointer.  
		Initialized in "Object/traits/class_traits_types.cc"
	 */
	static const type_ref_ptr_type		built_in_type_ptr;
	static const built_in_param_def		built_in_definition;
};	// end struct class_traits<pint_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_PINT_TRAITS_H__

