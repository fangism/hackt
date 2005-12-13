/**
	\file "Object/object_fwd.h"
	Forward declarations for all HAC::entity classes and typedefs.
	$Id: object_fwd.h,v 1.2 2005/12/13 04:15:16 fang Exp $
	This file used to be:
	Id: art_object_fwd.h,v 1.18.20.1 2005/12/11 00:45:13 fang Exp
 */

#ifndef	__HAC_OBJECT_OBJECT_FWD_H__
#define	__HAC_OBJECT_OBJECT_FWD_H__

#include "util/size_t.h"
#include "Object/traits/classification_tags.h"
#include "Object/traits/class_traits_fwd.h"
#include "Object/expr/types.h"
#include "Object/type/canonical_type_fwd.h"

namespace HAC {
namespace entity {

// TODO: organize into groups by where full declarations are found
	typedef	class_traits<bool_tag>		bool_traits;
	typedef	class_traits<int_tag>		int_traits;
	typedef	class_traits<enum_tag>		enum_traits;
	typedef	class_traits<process_tag>	process_traits;
	typedef	class_traits<channel_tag>	channel_traits;
	typedef	class_traits<pbool_tag>		pbool_traits;
	typedef	class_traits<pint_tag>		pint_traits;

	class module;
	class object;
	class scopespace;
	class sequential_scope;
	class name_space;
	class built_in_datatype_def;
	class type_reference_base;
	template <class> class canonical_type; // "Object/type/canonical_type.h"
	class fundamental_type_reference;
	class simple_meta_instance_reference_base;
	class simple_nonmeta_instance_reference_base;
	class instance_collection_base;
	class physical_instance_collection;
	class meta_instance_reference_base;
	class definition_base;
	class user_def_chan;
	class user_def_datatype;
	class channel_definition_base;
	class channel_type_reference_base;
	class builtin_channel_type_reference;
	class channel_type_reference;
	class datatype_definition_base;
	class enum_datatype_def;
	class data_type_reference;
	class datatype_instance_collection;
	class simple_datatype_meta_instance_reference_base;
	class process_definition_base;
	class process_definition;
	class process_type_reference;
	class built_in_param_def;
	class built_in_channel_def;
	class typedef_base;

	// from "Object/ref/*nonmeta_instance_reference*.h"
	// note there are generic (non-meta) abstract base classes from which
	// the meta-versions are derived.
	class nonmeta_instance_reference_base;
	class channel_instance_reference_base;
	class process_instance_reference_base;
	class datatype_instance_reference_base;
	class param_instance_reference_base;
	class int_instance_reference_base;
	class bool_instance_reference_base;
	class enum_instance_reference_base;
	class struct_instance_reference_base;
	class pint_instance_reference_base;
	class pbool_instance_reference_base;

	template <class>
	class meta_instance_reference;

	typedef	meta_instance_reference<channel_tag>
		channel_meta_instance_reference_base;
	typedef	meta_instance_reference<process_tag>
		process_meta_instance_reference_base;
	typedef	meta_instance_reference<int_tag>
		int_meta_instance_reference_base;
	typedef	meta_instance_reference<bool_tag>
		bool_meta_instance_reference_base;
	typedef	meta_instance_reference<enum_tag>
		enum_meta_instance_reference_base;
	typedef	meta_instance_reference<datastruct_tag>
		struct_meta_instance_reference_base;
	// base classes for meta_value_references
	typedef	meta_instance_reference<pbool_tag>
		pbool_meta_instance_reference_base;
	typedef	meta_instance_reference<int_tag>
		pint_meta_instance_reference_base;

	template <class>
	class simple_meta_instance_reference;
	template <class>
	class simple_nonmeta_instance_reference;
	template <class>
	class simple_nonmeta_value_reference;

	typedef	simple_nonmeta_instance_reference<channel_tag>
		simple_channel_nonmeta_instance_reference;
	typedef	simple_nonmeta_instance_reference<process_tag>
		simple_process_nonmeta_instance_reference;

#if 1
	// value reference this?
	typedef	simple_nonmeta_instance_reference<datatype_tag>
		simple_datatype_nonmeta_instance_reference;
#endif
	// data: use value reference
	typedef	simple_nonmeta_value_reference<int_tag>
		simple_int_nonmeta_instance_reference;
	typedef	simple_nonmeta_value_reference<bool_tag>
		simple_bool_nonmeta_instance_reference;
	typedef	simple_nonmeta_value_reference<enum_tag>
		simple_enum_nonmeta_instance_reference;
	typedef	simple_nonmeta_value_reference<datastruct_tag>
		simple_datastruct_nonmeta_instance_reference;
	typedef	simple_nonmeta_value_reference<pbool_tag>
		simple_pbool_nonmeta_instance_reference;
	typedef	simple_nonmeta_value_reference<pint_tag>
		simple_pint_nonmeta_instance_reference;

	template <class>
	class instance_collection;

	template <class, size_t>
	class instance_array;
	template <class, size_t>
	class value_array;

	// subclasses of datatype_instance_collection
	typedef	instance_collection<bool_tag>
		bool_instance_collection;
	typedef	instance_collection<int_tag>
		int_instance_collection;
	typedef	instance_collection<enum_tag>
		enum_instance_collection;
	typedef	instance_collection<datastruct_tag>
		struct_instance_collection;
	typedef	instance_collection<channel_tag>
		channel_instance_collection;
	typedef	instance_collection<process_tag>
		process_instance_collection;

	typedef	simple_meta_instance_reference<channel_tag>
		simple_channel_meta_instance_reference;
	typedef	simple_meta_instance_reference<process_tag>
		simple_process_meta_instance_reference;
	typedef	simple_meta_instance_reference<bool_tag>
		simple_bool_meta_instance_reference;
	typedef	simple_meta_instance_reference<int_tag>
		simple_int_meta_instance_reference;
	typedef	simple_meta_instance_reference<enum_tag>
		simple_enum_meta_instance_reference;
	typedef	simple_meta_instance_reference<datastruct_tag>
		simple_datastruct_meta_instance_reference;

	template <class>
	class member_meta_instance_reference;

	typedef	member_meta_instance_reference<process_tag>
		process_member_meta_instance_reference;
	typedef	member_meta_instance_reference<channel_tag>
		channel_member_meta_instance_reference;
	typedef	member_meta_instance_reference<bool_tag>
		bool_member_meta_instance_reference;
	typedef	member_meta_instance_reference<int_tag>
		int_member_meta_instance_reference;
	typedef	member_meta_instance_reference<enum_tag>
		enum_member_meta_instance_reference;
	typedef	member_meta_instance_reference<datastruct_tag>
		datastruct_member_meta_instance_reference;

	class param_type_reference;
	class pbool_type_reference;
	class pint_type_reference;

	template <class>
	class value_collection;
	class param_value_collection;
	typedef	value_collection<pint_tag>
		pint_instance_collection;
	typedef	value_collection<pbool_tag>
		pbool_instance_collection;

	template <class>
	class state_instance;

	class pint_instance;	// should be value
	class pbool_instance;	// should be value
#if 0
	class int_instance;
	class bool_instance;
	class enum_instance;
	class struct_instance;
	class channel_instance;
	class process_instance;
#else
	typedef	state_instance<int_tag>		int_instance;
	typedef	state_instance<bool_tag>	bool_instance;
	typedef	state_instance<enum_tag>	enum_instance;
	typedef	state_instance<datastruct_tag>	struct_instance;
	typedef	state_instance<channel_tag>	channel_instance;
	typedef	state_instance<process_tag>	process_instance;
#endif

	template <class>		class instance_alias_info;
	template <class, size_t>	class instance_alias;

	class instance_management_base;
	class param_expression_assignment;
	template <class>
	class expression_assignment;
	typedef	expression_assignment<pbool_tag>
		pbool_expression_assignment;
	typedef	expression_assignment<pint_tag>
		pint_expression_assignment;

	// defined in "Object/unroll/*connection*.h"
	class meta_instance_reference_connection;
	class port_connection;
	class aliases_connection_base;
	class data_alias_connection_base;
	template <class>	class alias_connection;
	typedef alias_connection<int_tag>
		int_alias_connection;
	typedef alias_connection<bool_tag>
		bool_alias_connection;
	typedef alias_connection<enum_tag>
		enum_alias_connection;
	typedef alias_connection<datastruct_tag>
		datastruct_alias_connection;
	typedef alias_connection<channel_tag>
		channel_alias_connection;
	typedef alias_connection<process_tag>
		process_alias_connection;

	template <class>
	class instantiation_statement;
	template <class>
	class param_instantiation_statement;

	class instantiation_statement_base;
	class param_instantiation_statement_base;

	typedef	param_instantiation_statement<pbool_tag>
		pbool_instantiation_statement;
	typedef	param_instantiation_statement<pint_tag>
		pint_instantiation_statement;
	typedef	instantiation_statement<datatype_tag>
		data_instantiation_statement;
	typedef	instantiation_statement<channel_tag>
		channel_instantiation_statement;
	typedef	instantiation_statement<process_tag>
		process_instantiation_statement;

// expressions and family
	class simple_param_meta_value_reference;
	template <class>
	class simple_meta_value_reference;
	typedef	simple_meta_value_reference<pint_tag>
		simple_pint_meta_instance_reference;
	typedef	simple_meta_value_reference<pbool_tag>
		simple_pbool_meta_instance_reference;

	template <class>
	class simple_nonmeta_value_reference;
	typedef	simple_nonmeta_value_reference<int_tag>
		simple_int_nonmeta_value_reference;
	typedef	simple_nonmeta_value_reference<bool_tag>
		simple_bool_nonmeta_value_reference;
	typedef	simple_nonmeta_value_reference<enum_tag>
		simple_enum_nonmeta_value_reference;
	typedef	simple_nonmeta_value_reference<datastruct_tag>
		simple_struct_nonmeta_value_reference;

	// from "Object/expr/data_expr[_base].h"
	class data_expr;
	class bool_expr;
	class int_expr;
	class enum_expr;
	class struct_expr;
	class int_arith_expr;
	class int_range_expr;
	class nonmeta_index_expr_base;
	class nonmeta_range_expr_base;
	class nonmeta_index_list;
	class nonmeta_range_list;

	class param_expr;
	class const_param;
	class param_expr_list;
	class const_param_expr_list;
	class dynamic_param_expr_list;
	class meta_index_expr;
	class const_index;
	class pbool_expr;
	class pint_expr;
	class pint_const;
	class pbool_const;
	class param_unary_expr;
	class pint_unary_expr;
	class pbool_unary_expr;
	class param_binary_expr;
	class pint_arith_expr;
	class pint_relational_expr;
	class pbool_logical_expr;
	class meta_range_expr;
	class pint_range;
	class const_range;
	class meta_range_list;
	class const_range_list;
	class dynamic_meta_range_list;
//	class unconditional_range_list;
//	class conditional_range_list;
//	class loop_range_list;
	class meta_index_list;
	class const_index_list;
	class dynamic_meta_index_list;

	template <class>
	class const_collection;
	typedef	const_collection<pint_tag>
		pint_const_collection;
	typedef	const_collection<pbool_tag>
		pbool_const_collection;

	class instance_collection_stack_item;

	class unroll_context;

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_OBJECT_FWD_H__

