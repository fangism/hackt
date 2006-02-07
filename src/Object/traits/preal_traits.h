/**
	\file "Object/traits/preal_traits.h"
	Traits and policies for parameter floats.  
	This file used to be "Object/art_object_preal_traits.h".
	$Id: preal_traits.h,v 1.1.2.2 2006/02/07 02:57:58 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_PREAL_TRAITS_H__
#define	__HAC_OBJECT_TRAITS_PREAL_TRAITS_H__

#include "Object/traits/class_traits.h"

namespace HAC {
namespace entity {
//-----------------------------------------------------------------------------
/**
	Note: we're punting on non-parameter reals.
	NOTE: could defer non-parameter traits to corresponding datatype tag.
 */
template <>
struct class_traits<preal_tag> {
	typedef	preal_tag			tag_type;
	static const char			tag_name[];
	static const char			value_type_name[];
	typedef	preal_instance			instance_type;
	typedef	preal_value_type		value_type;
	typedef	real_value_type			data_value_type;

	typedef	preal_instance_collection	instance_collection_generic_type;
	typedef	instance_collection_generic_type
						value_collection_generic_type;
	typedef	param_value_collection		value_collection_parent_type;
	template <size_t D>
	struct value_array {
		typedef	entity::value_array<tag_type,D>	type;
	};
	typedef	preal_const			const_expr_type;
	typedef	preal_const_collection		const_collection_type;

	typedef	param_instantiation_statement_base
					instantiation_statement_parent_type;
	typedef	preal_instantiation_statement
					instantiation_statement_type;

	// define this elsewhere, in "traits/inst_stmt_chan.h"
	class instantiation_statement_type_ref_base;

#if 1
	// not officially supported!
	typedef	preal_instance_reference_base	
					nonmeta_instance_reference_base_type;
	// this will have a different template base
	typedef	simple_preal_nonmeta_instance_reference
					simple_nonmeta_instance_reference_type;
#else
	struct nonmeta_instance_reference_base_type :
		public nonmeta_instance_reference_base {
	};
	/**
		Dummy type that doesn't do anything until we officially
		support real nonmeta values.  
	 */
	struct simple_nonmeta_instance_reference_type :
			public nonmeta_instance_reference_base_type {
		/**
			Dummy constructor, required by the
			value_collection<>::make_nonmeta_instance_reference().
		 */
		template <class T>
		simple_nonmeta_instance_reference_type(const T&) { }
	};
#endif
	typedef	simple_preal_meta_instance_reference
					simple_meta_instance_reference_type;
	typedef	preal_meta_instance_reference_base
				meta_instance_reference_parent_type;
	typedef	real_expr			data_expr_base_type;
	typedef	preal_expr			expr_base_type;
	typedef const_param			const_collection_parent_type;

	typedef	never_ptr<preal_instance>	value_reference_ptr_type;
	typedef	packed_array_generic<preal_value_type, value_reference_ptr_type>
						value_reference_collection_type;
	typedef	preal_expression_assignment	expression_assignment_type;
	typedef	param_expression_assignment	expression_assignment_parent_type;
	enum { assignment_chunk_size = 32 };

	typedef	void			value_collection_parameter_type;
	typedef	null_parameter_type	instance_collection_parameter_type;

	class instantiation_statement_type_ref_base;

	typedef	param_type_reference		type_ref_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;

	/**
		Replacement for built-in type pointer.  
		Initialized in "Object/traits/class_traits_types.cc"
	 */
	static const type_ref_ptr_type		built_in_type_ptr;
private:
	static const built_in_param_def		built_in_definition;
};	// end struct class_traits<preal_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_PREAL_TRAITS_H__

