/**
 *	\file "art_object_type_hash.h"
 *	This contains hash_key codes for all serializable, 
 *	persistent classes in the ART language.  
 *	This file needs to be kept consistent with the Cyclone implementation
 *	for libart (not libart++).  
 *	There are just string #define's, their order is irrelevant.  
 *
 *	The string definitions herein must follow two simple criteria:
 *	1) They need to be unique.
 *	2) They must be no longer than 8 chars (will fail an assert check).  
 *
 *	Use these enumerations to lookup which function to call
 *	to reconstruct an object from a binary file stream.
 *	Only concrete classes need to register with this.
 *	This enumeration imitates indexing into a globally visible 
 *	virtual table for persistent objects whose vptr's are transient
 *	and specific to each module.  
 *	As a convention, all enumerations are suffixed with _TYPE_KEY.  
 *
 *	$Id: art_object_type_hash.h,v 1.5 2005/02/27 22:54:18 fang Exp $
 */

#ifndef	__ART_OBJECT_TYPE_HASH_H__
#define	__ART_OBJECT_TYPE_HASH_H__
//=============================================================================

// macro name							hash string

// Modules:
#define	MODULE_TYPE_KEY						"module"

// Namespaces:
#define	NAMESPACE_TYPE_KEY					"namespc"

// Definitions:
#define	PROCESS_DEFINITION_TYPE_KEY				"procdefn"
#define	PROCESS_TYPEDEF_TYPE_KEY				"proctpdf"

#define	USER_DEF_CHAN_DEFINITION_TYPE_KEY			"chandefn"
#define	CHANNEL_TYPEDEF_TYPE_KEY				"chantpdf"

#define	USER_DEF_DATA_DEFINITION_TYPE_KEY			"datadefn"
#define	ENUM_DEFINITION_TYPE_KEY				"enumdefn"
#define	DATA_TYPEDEF_TYPE_KEY					"datatpdf"
//	no built-in definition types, like params

// Type references:
#define	PROCESS_TYPE_REFERENCE_TYPE_KEY				"proctprf"
#define	CHANNEL_TYPE_REFERENCE_TYPE_KEY				"chantprf"
#define	DATA_TYPE_REFERENCE_TYPE_KEY				"datatprf"
	// no need for param type reference, or built-in data type reference

// Instantiations, collections (non-unique):
#define	PROCESS_INSTANCE_COLLECTION_TYPE_KEY			"proccoll"
#define	CHANNEL_INSTANCE_COLLECTION_TYPE_KEY			"chancoll"
#define	STRUCT_INSTANCE_COLLECTION_TYPE_KEY			"dstrcoll"
#define	ENUM_INSTANCE_COLLECTION_TYPE_KEY			"enumcoll"
#define	DBOOL_INSTANCE_COLLECTION_TYPE_KEY			"dboolcol"
#define	DINT_INSTANCE_COLLECTION_TYPE_KEY			"dintcoll"
#define	PBOOL_INSTANCE_COLLECTION_TYPE_KEY			"pboolcol"
#define	PINT_INSTANCE_COLLECTION_TYPE_KEY			"pintcoll"

// Unique instances, post-unrolling:
#define	UNIQUE_PROCESS_INSTANCE_TYPE_KEY			"procunis"

// Instance references:
	// simple instance reference may contain multidimensional indices
#define	SIMPLE_PROCESS_INSTANCE_REFERENCE_TYPE_KEY		"sprocref"
#define	SIMPLE_CHANNEL_INSTANCE_REFERENCE_TYPE_KEY		"schanref"
#define	SIMPLE_DBOOL_INSTANCE_REFERENCE_TYPE_KEY		"sdbref"
#define	SIMPLE_DINT_INSTANCE_REFERENCE_TYPE_KEY			"sdiref"
#define	SIMPLE_ENUM_INSTANCE_REFERENCE_TYPE_KEY			"sderef"
#define	SIMPLE_STRUCT_INSTANCE_REFERENCE_TYPE_KEY		"sdsref"
#define	SIMPLE_PBOOL_INSTANCE_REFERENCE_TYPE_KEY		"spbref"
#define	SIMPLE_PINT_INSTANCE_REFERENCE_TYPE_KEY			"spiref"

	// aggregates are complex compositions / concatenations of arrays
#define	AGGREGATE_PROCESS_INSTANCE_REFERENCE_TYPE_KEY		"aprocref"
#define	AGGREGATE_CHANNEL_INSTANCE_REFERENCE_TYPE_KEY		"achanref"
#define	AGGREGATE_DBOOL_INSTANCE_REFERENCE_TYPE_KEY		"adbref"
#define	AGGREGATE_DINT_INSTANCE_REFERENCE_TYPE_KEY		"adiref"
#define	AGGREGATE_PBOOL_INSTANCE_REFERENCE_TYPE_KEY		"apbref"
#define	AGGREGATE_PINT_INSTANCE_REFERENCE_TYPE_KEY		"apiref"

	// member references of the form x.y, (x may be indexed / member ref.)
#define	MEMBER_PROCESS_INSTANCE_REFERENCE_TYPE_KEY		"mprocref"
#define	MEMBER_CHANNEL_INSTANCE_REFERENCE_TYPE_KEY		"mchanref"
#define	MEMBER_DBOOL_INSTANCE_REFERENCE_TYPE_KEY		"mdbolref"
#define	MEMBER_DINT_INSTANCE_REFERENCE_TYPE_KEY			"mdintref"
#define	MEMBER_ENUM_INSTANCE_REFERENCE_TYPE_KEY			"menumref"
#define	MEMBER_STRUCT_INSTANCE_REFERENCE_TYPE_KEY		"mstrtref"
	// no such thing as param member instance reference

// Expressions:
	// Note that the above pbool/pint instance references are 
	// also classified as expressions.  
	// Many expression classes come in two flavors: const, dynamic
	//	const -- resolved to a compile time constant
	//	dynamic -- everything else
	// When in doubt, it is safe to use the dynamic flavor.  

#define	CONST_PBOOL_TYPE_KEY					"cnstpb"
#define	CONST_PINT_TYPE_KEY					"cnstpi"
#define	CONST_PBOOL_COLLECTION_TYPE_KEY				"cnstpbc"
#define	CONST_PINT_COLLECTION_TYPE_KEY				"cnstpic"

#define	CONST_RANGE_TYPE_KEY					"cnstrng"
#define	DYNAMIC_RANGE_TYPE_KEY					"dynrng"

#define	CONST_PARAM_EXPR_LIST_TYPE_KEY				"cprmlst"
#define	DYNAMIC_PARAM_EXPR_LIST_TYPE_KEY			"dprmlst"

	// "index" may be single pint or a range
#define	CONST_INDEX_LIST_TYPE_KEY				"cindlst"
#define	DYNAMIC_INDEX_LIST_TYPE_KEY				"dindlst"
#define	CONST_RANGE_LIST_TYPE_KEY				"crnglst"
#define	DYNAMIC_RANGE_LIST_TYPE_KEY				"drnglst"

	// symbolic expressions
#define	PINT_UNARY_EXPR_TYPE_KEY				"intunary"
#define	PBOOL_UNARY_EXPR_TYPE_KEY				"boolunry"
#define	ARITH_EXPR_TYPE_KEY					"arithexp"
#define	RELATIONAL_EXPR_TYPE_KEY				"relatexp"
#define	LOGICAL_EXPR_TYPE_KEY					"logicexp"

// Sequential and control statements: (loops and conditionals)
// including sequential instantiations, assignments and connections:
	
#define	PROCESS_INSTANTIATION_STATEMENT_TYPE_KEY		"procinst"
#define	CHANNEL_INSTANTIATION_STATEMENT_TYPE_KEY		"chaninst"
#define	DATA_INSTANTIATION_STATEMENT_TYPE_KEY			"datainst"
#define	DBOOL_INSTANTIATION_STATEMENT_TYPE_KEY			"dboolins"
#define	DINT_INSTANTIATION_STATEMENT_TYPE_KEY			"dintinst"
#define	PBOOL_INSTANTIATION_STATEMENT_TYPE_KEY			"pboolins"
#define	PINT_INSTANTIATION_STATEMENT_TYPE_KEY			"pintinst"

#define	PBOOL_EXPR_ASSIGNMENT_TYPE_KEY				"pbassign"
#define	PINT_EXPR_ASSIGNMENT_TYPE_KEY				"piassign"

// will be obsolete if it is subclassed into the following
#define	ALIAS_CONNECTION_TYPE_KEY				"aliascon"

#define	DBOOL_ALIAS_CONNECTION_TYPE_KEY				"dbalias"
#define	DINT_ALIAS_CONNECTION_TYPE_KEY				"dialias"
#define	ENUM_ALIAS_CONNECTION_TYPE_KEY				"enmalias"
#define	STRUCT_ALIAS_CONNECTION_TYPE_KEY			"stralias"
#define	CHANNEL_ALIAS_CONNECTION_TYPE_KEY			"chnalias"
#define	PROCESS_ALIAS_CONNECTION_TYPE_KEY			"prcalias"

#define	PORT_CONNECTION_TYPE_KEY				"portconn"
#define	LOOP_SCOPE_TYPE_KEY					"loopscop"
#define	CONDITIONAL_SCOPE_TYPE_KEY				"condscop"

// Language-specifics: (PRS, CHP, ...)

	// more class constants here...

//=============================================================================
#endif	// __ART_OBJECT_TYPE_HASH_H__

