/**
	\file "art_object_fwd.h"
	Forward declarations for all ART::entity classes and typedefs.
	$Id: art_object_fwd.h,v 1.7.12.3.2.3 2005/03/11 01:05:28 fang Exp $
 */

#ifndef	__ART_OBJECT_FWD_H__
#define	__ART_OBJECT_FWD_H__

#include <cstddef>			// for size_t

#include "memory/pointer_classes_fwd.h"
#include "STL/deque_fwd.h"
#include "art_object_classification_tags.h"

namespace ART {
namespace entity {
	USING_DEQUE
	using namespace util::memory;

// TODO: organize into groups by where full declarations are found

	template <class>
	struct class_traits;

	class module;
	class object;
	class scopespace;
	class sequential_scope;
	class name_space;
	class built_in_datatype_def;
	class fundamental_type_reference;
	class simple_instance_reference;
	class instance_collection_base;
	class physical_instance_collection;
	class instance_reference_base;
	class definition_base;
	class user_def_chan;
	class user_def_type;
	class channel_definition_base;
	class channel_type_reference;
	class datatype_definition_base;
	class enum_datatype_def;
	class data_type_reference;
	class datatype_instance_collection;
	class datatype_instance_reference;
	class process_definition_base;
	class process_definition;
	class process_type_reference;
	class built_in_param_def;
	class typedef_base;

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

	template <class>
	class instance_reference;

	typedef	instance_reference<channel_tag>
		channel_instance_reference;
	typedef	instance_reference<process_tag>
		process_instance_reference;
	typedef	instance_reference<bool_tag>
		bool_instance_reference;
	typedef	instance_reference<int_tag>
		int_instance_reference;
	typedef	instance_reference<enum_tag>
		enum_instance_reference;
	typedef	instance_reference<datastruct_tag>
		datastruct_instance_reference;

	template <class>
	class member_instance_reference;

	typedef	member_instance_reference<process_tag>
		process_member_instance_reference;
	typedef	member_instance_reference<channel_tag>
		channel_member_instance_reference;
	typedef	member_instance_reference<bool_tag>
		bool_member_instance_reference;
	typedef	member_instance_reference<int_tag>
		int_member_instance_reference;
	typedef	member_instance_reference<enum_tag>
		enum_member_instance_reference;
	typedef	member_instance_reference<datastruct_tag>
		datastruct_member_instance_reference;

	class param_type_reference;
	class pbool_type_reference;
	class pint_type_reference;

	template <class>
	class value_collection;
	class param_instance_collection;
	typedef	value_collection<pint_tag>
		pint_instance_collection;
	typedef	value_collection<pbool_tag>
		pbool_instance_collection;

	class pint_instance;
	class pbool_instance;
	class int_instance;
	class bool_instance;
	class enum_instance;
	class struct_instance;
	class channel_instance;
	class process_instance;

	template <class>		class instance_alias_info;
	template <class, size_t>	class instance_alias;

	class instance_management_base;
	class param_expression_assignment;

	// defined in "art_object_connect.h"
	class instance_reference_connection;
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
	class instantiation_statement_base;
	class param_instantiation_statement;

	class channel_instantiation_statement;
	class process_instantiation_statement;
	class data_instantiation_statement;
#if 0
	class pbool_instantiation_statement;
	class pint_instantiation_statement;
#else
	typedef	instantiation_statement<pbool_tag>
		pbool_instantiation_statement;
	typedef	instantiation_statement<pint_tag>
		pint_instantiation_statement;
#if 0
	typedef	instantiation_statement<datatype_tag>
		data_instantiation_statement;
	typedef	instantiation_statement<channel_tag>
		channel_instantiation_statement;
	typedef	instantiation_statement<process_tag>
		process_instantiation_statement;
#endif
#endif

// expressions and family
	class param_instance_reference;
	template <class>
	class value_reference;
	typedef	value_reference<pint_tag>
		pint_instance_reference;
	typedef	value_reference<pbool_tag>
		pbool_instance_reference;

	class param_expr;
	class const_param;
	class param_expr_list;
	class const_param_expr_list;
	class dynamic_param_expr_list;
	class index_expr;               // BEWARE also in ART::parser!
	class const_index;
	class pbool_expr;
	class pint_expr;
	class pint_const;
	class pbool_const;
	class param_unary_expr;
	class pint_unary_expr;
	class pbool_unary_expr;
	class param_binary_expr;
	class arith_expr;
	class relational_expr;
	class logical_expr;
	class range_expr;
	class pint_range;
	class const_range;
	class range_expr_list;
	class const_range_list;
	class dynamic_range_list;
//	class unconditional_range_list;
//	class conditional_range_list;
//	class loop_range_list;
	class index_list;
	class const_index_list;
	class dynamic_index_list;

	template <class>
	class const_collection;
	typedef	const_collection<pint_tag>
		pint_const_collection;
	typedef	const_collection<pbool_tag>
		pbool_const_collection;

	/**
		The global integer-type for parameter integers.  
		This may have to be changed to int32 in the future, 
		for 64b portability...
	 */
	typedef	long		pint_value_type;

	/**
		The global boolean-type for parameter integers.  
	 */
	typedef	bool		pbool_value_type;

	class instance_collection_stack_item;

	/**
		Value type of this needs to be more general
		to accommodate loop and conditional scopes?
	 */
	// try to convert this to excl_ptr or sticky_ptr...
	typedef count_ptr<const range_expr_list>
			index_collection_item_ptr_type;

	/**
		UPDATE: now contains reference to instantiation_statements, 
		which *contain* the index/range expressions.  

		We keep track of the state of instance collections at
		various program points with this container.

		Eventually work with sub-types only?
	 */
	typedef DEFAULT_DEQUE(never_ptr<const instantiation_statement_base>)
			index_collection_type;

	class unroll_context;

}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_FWD_H__

