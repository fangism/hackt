/**
	\file "art_object_classification_details.h"
	Traits and policy classes for instances.  
	$Id: art_object_classification_details.h,v 1.1.4.2 2005/02/24 01:03:13 fang Exp $
 */

#ifndef	__ART_OBJECT_CLASSIFICATION_DETAILS_H__
#define	__ART_OBJECT_CLASSIFICATION_DETAILS_H__

#include "art_object_fwd.h"
#include "art_object_classification_tags.h"
#include "packed_array_fwd.h"
#include "ring_node_fwd.h"

namespace ART {
namespace entity {
using util::packed_array_generic;
using util::ring_node_derived;

/**
	Default definition is empty, must specialize for each class.  
	The definitions contained herein serve no purpose but to 
	document the design pattern used.  
	A few default patterns are provided.  
	The typedefs defined in specalizations of this are intended
	to describe the relationship between various classes.  
	This serves as a centralized location for determining
	implementation details.  
 */
template <class Tag>
struct class_traits {
	/**
		The type of uniquely created object with possible aliases
	 */
	typedef	void				instance_type;

	/**
		Unique alias placeholder pointing to a instance.
		This is most likely a base type from which 
		dimension-specific subclasses are derived.  
	 */
	typedef	void				instance_alias_type;
	/// a non-owned, modifiable pointer reference to instance_alias_type
	typedef	never_ptr<instance_alias_type>	instance_alias_ptr_type;

	/**
		This is the base type of the instance collection, 
		the home location of the instance_aliases.  
		This class will be sub-typed into dimension-specific
		arrays and a scalar instance.  
	 */
	typedef	void				instance_collection_type;
	typedef	void				instance_collection_parent_type;

	/**
		Before unrolling, this is the simple kind of 
		reference object that will be unrolled into
		(possibly) a collection of instance_aliases.  
	 */
	typedef	void				instance_reference_type;
	typedef	void				instance_reference_parent_type;

	/**
		This sub-type is for member references that also
		resolve to the same kind of instance aliases.  
	 */
	typedef	void				member_instance_reference_type;

	/**
		Instantiation statement type.
	 */
	typedef	void				instantiation_statement_type;

	/**
		This is the type of aggregate object that contains
		(pointers) to aliases resolved by unrolling references.  
		These are passed around during unroll time to
		manipulate references and connections.  
	 */
	typedef	packed_array_generic<pint_value_type, instance_alias_type>
						alias_collection_type;

	/**
		This type represent a connection statement to alias
		instance_aliases.  
	 */
	typedef	void				alias_connection_type;
	typedef	void				alias_connection_parent_type;

	/**
		An optional parameter to distinguish between
		subtypes of the same base, or variations of one type.  
		This may be an empty class, which opens opportunities
		for the Empty Base Class Optimization (EBCO).  
	 */
	typedef	void				instance_parameter_type;

	/**
		The type reference class used to distinguish types
		of the instances referenced.  
	 */
	typedef	void				type_ref_type;
	typedef	void				type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;

};	// end struct class_traits

//=============================================================================
// some useful forward declarations, because I can't find a better place...

class bool_instance_alias_info;
class int_instance_alias_info;
class enum_instance_alias_info;
class datastruct_instance_alias_info;
class channel_instance_alias_info;
class process_instance_alias_info;

typedef ring_node_derived<bool_instance_alias_info>
						bool_instance_alias_base;
typedef ring_node_derived<int_instance_alias_info>
						int_instance_alias_base;
typedef ring_node_derived<enum_instance_alias_info>
						enum_instance_alias_base;
typedef ring_node_derived<datastruct_instance_alias_info>
						struct_instance_alias_base;

class chan_instance_alias;	// temporary
class proc_instance_alias;	// temporary
typedef ring_node_derived<channel_instance_alias_info>
						channel_instance_alias_base;
typedef ring_node_derived<process_instance_alias_info>
						process_instance_alias_base;

//=============================================================================
template <>
struct class_traits<int_tag> {
	typedef	int_instance			instance_type;
	typedef	int_instance_alias_base		instance_alias_type;
	typedef	never_ptr<instance_alias_type>
						instance_alias_ptr_type;
	typedef	int_instance_collection		instance_collection_type;
	typedef	datatype_instance_collection	instance_collection_parent_type;
	typedef	int_instance_reference		instance_reference_type;
	typedef	datatype_instance_reference	instance_reference_parent_type;
	typedef	int_member_instance_reference	member_instance_reference_type;
	typedef	packed_array_generic<pint_value_type, instance_alias_ptr_type>
						alias_collection_type;
	typedef	int_alias_connection		alias_connection_type;
	typedef	data_alias_connection_base	alias_connection_parent_type;
	typedef	pint_value_type			instance_parameter_type;
	typedef	data_type_reference		type_ref_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
};	// end struct class_traits<int_tag>

//-----------------------------------------------------------------------------
template <>
struct class_traits<bool_tag> {
	typedef	bool_instance			instance_type;
	typedef	bool_instance_alias_base	instance_alias_type;
	typedef	never_ptr<instance_alias_type>	instance_alias_ptr_type;
	typedef	bool_instance_collection	instance_collection_type;
	typedef	datatype_instance_collection	instance_collection_parent_type;
	typedef	bool_instance_reference		instance_reference_type;
	typedef	datatype_instance_reference	instance_reference_parent_type;
	typedef	bool_member_instance_reference	member_instance_reference_type;
	typedef	packed_array_generic<pint_value_type, instance_alias_ptr_type>
						alias_collection_type;
	typedef	bool_alias_connection		alias_connection_type;
	typedef	data_alias_connection_base	alias_connection_parent_type;
	typedef	void				instance_parameter_type;
	typedef	data_type_reference		type_ref_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
};	// end struct class_traits<bool_tag>

//-----------------------------------------------------------------------------
template <>
struct class_traits<enum_tag> {
	typedef	enum_instance			instance_type;

	// temporary
	typedef	enum_instance_alias		instance_alias_type;
//	typedef	enum_instance_alias_base	instance_alias_type;

	typedef	never_ptr<instance_alias_type>	instance_alias_ptr_type;
	typedef	enum_instance_collection	instance_collection_type;
	typedef	datatype_instance_collection	instance_collection_parent_type;
	typedef	enum_instance_reference		instance_reference_type;
	typedef	datatype_instance_reference	instance_reference_parent_type;
	typedef	enum_member_instance_reference	member_instance_reference_type;
	typedef	packed_array_generic<pint_value_type, instance_alias_ptr_type>
						alias_collection_type;
	typedef	enum_alias_connection		alias_connection_type;
	typedef	data_alias_connection_base	alias_connection_parent_type;
	typedef	void				instance_parameter_type;
	typedef	data_type_reference		type_ref_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
};	// end struct class_traits<enum_tag>

//-----------------------------------------------------------------------------
template <>
struct class_traits<datastruct_tag> {
	typedef	struct_instance			instance_type;

	// temporary
	typedef	struct_instance_alias		instance_alias_type;
//	typedef	struct_instance_alias_base	instance_alias_type;

	typedef	never_ptr<instance_alias_type>	instance_alias_ptr_type;
	typedef	struct_instance_collection	instance_collection_type;
	typedef	datatype_instance_collection	instance_collection_parent_type;
	typedef	datastruct_instance_reference	instance_reference_type;
	typedef	datatype_instance_reference	instance_reference_parent_type;
	typedef	datastruct_member_instance_reference
						member_instance_reference_type;
	typedef	packed_array_generic<pint_value_type, instance_alias_ptr_type>
						alias_collection_type;
	typedef	datastruct_alias_connection	alias_connection_type;
	typedef	data_alias_connection_base	alias_connection_parent_type;
	// need real type here!
	typedef	void				instance_parameter_type;
	typedef	data_type_reference		type_ref_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
};	// end struct class_traits<datastruct_tag>

//-----------------------------------------------------------------------------
template <>
struct class_traits<process_tag> {
	typedef	process_instance		instance_type;

	// temporary
//	typedef	process_instance_alias_base	instance_alias_type;
	typedef	proc_instance_alias		instance_alias_type;

	typedef	never_ptr<instance_alias_type>	instance_alias_ptr_type;
	typedef	process_instance_collection	instance_collection_type;
	typedef	instance_collection_base	instance_collection_parent_type;
	typedef	process_instance_reference	instance_reference_type;
	typedef	simple_instance_reference	instance_reference_parent_type;
	typedef	process_member_instance_reference
						member_instance_reference_type;
	typedef	packed_array_generic<pint_value_type, instance_alias_ptr_type>
						alias_collection_type;
	typedef	process_alias_connection	alias_connection_type;
	typedef	aliases_connection_base		alias_connection_parent_type;
	// need real type here!
	typedef	void				instance_parameter_type;
	typedef	process_type_reference		type_ref_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
};	// end struct class_traits<process_tag>

//-----------------------------------------------------------------------------
template <>
struct class_traits<channel_tag> {
	typedef	channel_instance		instance_type;

	// temporary
//	typedef	channel_instance_alias_base	instance_alias_type;
	typedef	chan_instance_alias		instance_alias_type;

	typedef	never_ptr<instance_alias_type>	instance_alias_ptr_type;
	typedef	channel_instance_collection	instance_collection_type;
	typedef	instance_collection_base	instance_collection_parent_type;
	typedef	channel_instance_reference	instance_reference_type;
	typedef	simple_instance_reference	instance_reference_parent_type;
	typedef	channel_member_instance_reference
						member_instance_reference_type;
	typedef	packed_array_generic<pint_value_type, instance_alias_ptr_type>
						alias_collection_type;
	typedef	channel_alias_connection	alias_connection_type;
	typedef	aliases_connection_base		alias_connection_parent_type;
	// need real type here!
	typedef	void				instance_parameter_type;
	typedef	channel_type_reference		type_ref_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
};	// end struct class_traits<channel_tag>

//-----------------------------------------------------------------------------
//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_CLASSIFICATION_DETAILS_H__

