// "art_object_type_enum.h"

/*
 *	This file needs to be kept consistent with the Cyclone implementation
 *	for libart (not libart++).  
 *	To define this enumeration in a namespace, just #include this file
 *	withing a namespace foo { ... } declaration in the referencing file.  
 */

//=============================================================================
/**
 *	Use these enumerations to lookup which function to call
 *	to reconstruct an object from a binary file stream.
 *	Only concrete classes need to register with this.
 *	This enumeration imitates indexing into a globally visible 
 *	virtual table for persistent objects whose vptr's are transient
 *	and specific to each module.  
 *	As a convention, all enumerations are suffixed with _TYPE.  
 */
enum type_index_enum {
	NULL_TYPE = 0,			// reserved = 0
	// can also be used to denote end of object stream

// Modules:
	MODULE_TYPE, 

// Namespaces:
	NAMESPACE_TYPE,

// Definitions:
	PROCESS_DEFINITION_TYPE,	// = 3
	PROCESS_TYPEDEF_TYPE,

	USER_DEF_CHAN_DEFINITION_TYPE, 
	CHANNEL_TYPEDEF_TYPE, 

	USER_DEF_DATA_DEFINITION_TYPE, 
	ENUM_DEFINITION_TYPE, 
	DATA_TYPEDEF_TYPE, 
//	no built-in definition types, like params

// Type references:
	PROCESS_TYPE_REFERENCE_TYPE, 	// = 10
	CHANNEL_TYPE_REFERENCE_TYPE,
	DATA_TYPE_REFERENCE_TYPE, 
	// no need for param type reference

// Instantiations:
	PROCESS_INSTANCE_COLLECTION_TYPE, 	// = 13
	CHANNEL_INSTANCE_COLLECTION_TYPE, 
	DATA_INSTANCE_COLLECTION_TYPE, 
	PBOOL_INSTANCE_COLLECTION_TYPE, // parameter boolean, or collection
	PINT_INSTANCE_COLLECTION_TYPE, 	// parameter integer, or collection

// Instance references:
	// simple instance reference may contain multidimensional indices
	SIMPLE_PROCESS_INSTANCE_REFERENCE_TYPE, 	// = 18
	SIMPLE_CHANNEL_INSTANCE_REFERENCE_TYPE, 
	SIMPLE_DATA_INSTANCE_REFERENCE_TYPE, 
	SIMPLE_PBOOL_INSTANCE_REFERENCE_TYPE, 
	SIMPLE_PINT_INSTANCE_REFERENCE_TYPE, 

	// aggregates are complex compositions / concatenations of arrays
	AGGREGATE_PROCESS_INSTANCE_REFERENCE_TYPE, 	// = 23
	AGGREGATE_CHANNEL_INSTANCE_REFERENCE_TYPE, 
	AGGREGATE_DATA_INSTANCE_REFERENCE_TYPE, 
	AGGREGATE_PBOOL_INSTANCE_REFERENCE_TYPE, 
	AGGREGATE_PINT_INSTANCE_REFERENCE_TYPE, 

	// member references of the form x.y, (x may be indexed / member ref.)
	MEMBER_PROCESS_INSTANCE_REFERENCE_TYPE, 	// = 28
	MEMBER_CHANNEL_INSTANCE_REFERENCE_TYPE, 
	MEMBER_DATA_INSTANCE_REFERENCE_TYPE, 
	// no such thing as param member instance reference

// Expressions:
	// Note that the above pbool/pint instance references are 
	// also classified as expressions.  
	// Many expression classes come in two flavors: const, dynamic
	//	const -- resolved to a compile time constant
	//	dynamic -- everything else
	// When in doubt, it is safe to use the dynamic flavor.  

	CONST_PBOOL_TYPE, 		// constant pbool, = 31
	CONST_PINT_TYPE, 		// constant pint

	CONST_RANGE_TYPE, 		// [x..y] where x, y are constant ints
	DYNAMIC_RANGE_TYPE, 		// x,y may be other expressions

	CONST_PARAM_EXPR_LIST_TYPE, 	// generic expression list, (useful?)
	DYNAMIC_PARAM_EXPR_LIST_TYPE, 	// generic expression list, (useful?)

	// "index" may be single pint or a range
	CONST_INDEX_LIST_TYPE, 		// = 37
	DYNAMIC_INDEX_LIST_TYPE, 
	CONST_RANGE_LIST_TYPE, 
	DYNAMIC_RANGE_LIST_TYPE, 

	// symbolic expressions
	PINT_UNARY_EXPR_TYPE, 		// = 41
	PBOOL_UNARY_EXPR_TYPE, 
	ARITH_EXPR_TYPE, 
	RELATIONAL_EXPR_TYPE, 
	LOGICAL_EXPR_TYPE, 

// Sequential and control statements: (loops and conditionals)
// including sequential instantiations, assignments and connections:
	
	PROCESS_INSTANTIATION_STATEMENT_TYPE, 	// = 46
	CHANNEL_INSTANTIATION_STATEMENT_TYPE,
	DATA_INSTANTIATION_STATEMENT_TYPE,
	PBOOL_INSTANTIATION_STATEMENT_TYPE,
	PINT_INSTANTIATION_STATEMENT_TYPE,

	PARAM_EXPR_ASSIGNMENT_TYPE, 	// list of expressions equated
	ALIAS_CONNECTION_TYPE, 
	PORT_CONNECTION_TYPE, 
	LOOP_SCOPE_TYPE, 
	CONDITIONAL_SCOPE_TYPE, 

// Language-specifics: (PRS, CHP, ...)

	// more class constants here...

	MAX_TYPE_INDEX_ENUM		// reserved, use this as a size_t
};	// end enum type_index_enum

