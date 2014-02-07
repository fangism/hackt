/**
 *	\file "Object/persistent_type_hash.h"
 *	This contains hash_key codes for all serializable, 
 *	persistent classes in the HAC language.  
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
 *	TODO: optionally, use gperf to generate a fast perfect hash.
 *
 *	$Id: persistent_type_hash.h,v 1.34 2011/03/23 00:36:09 fang Exp $
 */

#ifndef	__HAC_OBJECT_PERSISTENT_TYPE_HASH_H__
#define	__HAC_OBJECT_PERSISTENT_TYPE_HASH_H__
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

// for canonical built-in channel types
#define	CANONICAL_FUNDAMENTAL_CHANNEL_TYPE_TYPE_KEY		"fndchntp"

// for heap-allocated footprints
#define	GENERIC_FOOTPRINT_TYPE_KEY				"footprnt"

// Type references:
#define	PROCESS_TYPE_REFERENCE_TYPE_KEY				"proctprf"
#define	BLTIN_CHANNEL_TYPE_REFERENCE_TYPE_KEY			"bchntprf"
#define	USER_CHANNEL_TYPE_REFERENCE_TYPE_KEY			"uchntprf"
#define	DATA_TYPE_REFERENCE_TYPE_KEY				"datatprf"
	// no need for param type reference, or built-in data type reference

// instance and value placeholders
#define	PROCESS_INSTANCE_PLACEHOLDER_TYPE_KEY			"procplch"
#define	CHANNEL_INSTANCE_PLACEHOLDER_TYPE_KEY			"chanplch"
#define	STRUCT_INSTANCE_PLACEHOLDER_TYPE_KEY			"dstrplch"
#define	ENUM_INSTANCE_PLACEHOLDER_TYPE_KEY			"enumplch"
#define	DBOOL_INSTANCE_PLACEHOLDER_TYPE_KEY			"dboolplh"
#define	DINT_INSTANCE_PLACEHOLDER_TYPE_KEY			"dintplch"
#define	NODE_INSTANCE_PLACEHOLDER_TYPE_KEY			"nodeplch"
#define	PBOOL_VALUE_PLACEHOLDER_TYPE_KEY			"pboolplh"
#define	PINT_VALUE_PLACEHOLDER_TYPE_KEY				"pintplch"
#define	PREAL_VALUE_PLACEHOLDER_TYPE_KEY			"prealplh"
#define	PSTRING_VALUE_PLACEHOLDER_TYPE_KEY			"pstrplch"

#if 0
// OBSOLETE: after footprints took charge of managing instance collections
// Unique instances, post-unrolling:
// Instantiations, collections (non-unique):
#define	PROCESS_INSTANCE_COLLECTION_TYPE_KEY			"proccoll"
#define	CHANNEL_INSTANCE_COLLECTION_TYPE_KEY			"chancoll"
#define	STRUCT_INSTANCE_COLLECTION_TYPE_KEY			"dstrcoll"
#define	ENUM_INSTANCE_COLLECTION_TYPE_KEY			"enumcoll"
#define	DBOOL_INSTANCE_COLLECTION_TYPE_KEY			"dboolcol"
#define	DINT_INSTANCE_COLLECTION_TYPE_KEY			"dintcoll"
#define	PBOOL_INSTANCE_COLLECTION_TYPE_KEY			"pboolcol"
#define	PINT_INSTANCE_COLLECTION_TYPE_KEY			"pintcoll"
#define	PREAL_INSTANCE_COLLECTION_TYPE_KEY			"prealcol"

// dense port collections
#define	PROCESS_PORT_FORMAL_ARRAY_TYPE_KEY			"procpfar"
#define	CHANNEL_PORT_FORMAL_ARRAY_TYPE_KEY			"chanpfar"
#define	STRUCT_PORT_FORMAL_ARRAY_TYPE_KEY			"dstrpfar"
#define	ENUM_PORT_FORMAL_ARRAY_TYPE_KEY				"enumpfar"
#define	DBOOL_PORT_FORMAL_ARRAY_TYPE_KEY			"dboolpfa"
#define	DINT_PORT_FORMAL_ARRAY_TYPE_KEY				"dintpfar"

// dense port collections
#define	PROCESS_PORT_ACTUAL_COLLECTION_TYPE_KEY			"procpacc"
#define	CHANNEL_PORT_ACTUAL_COLLECTION_TYPE_KEY			"chanpacc"
#define	STRUCT_PORT_ACTUAL_COLLECTION_TYPE_KEY			"dstrpacc"
#define	ENUM_PORT_ACTUAL_COLLECTION_TYPE_KEY			"enumpacc"
#define	DBOOL_PORT_ACTUAL_COLLECTION_TYPE_KEY			"dboolpac"
#define	DINT_PORT_ACTUAL_COLLECTION_TYPE_KEY			"dintpacc"
#endif

// Unique instances, post-unrolling:
#define	UNIQUE_PROCESS_INSTANCE_TYPE_KEY			"procunis"

// Instance references:
	// simple instance reference may contain multidimensional indices
#define	SIMPLE_PROCESS_META_INSTANCE_REFERENCE_TYPE_KEY		"sprcmref"
#define	SIMPLE_CHANNEL_META_INSTANCE_REFERENCE_TYPE_KEY		"schnmref"
#define	SIMPLE_DBOOL_META_INSTANCE_REFERENCE_TYPE_KEY		"sdbmiref"
#define	SIMPLE_DINT_META_INSTANCE_REFERENCE_TYPE_KEY		"sdimiref"
#define	SIMPLE_ENUM_META_INSTANCE_REFERENCE_TYPE_KEY		"sdemiref"
#define	SIMPLE_STRUCT_META_INSTANCE_REFERENCE_TYPE_KEY		"sdsmiref"
#define	SIMPLE_NODE_META_INSTANCE_REFERENCE_TYPE_KEY		"spndiref"
#define	SIMPLE_PBOOL_META_VALUE_REFERENCE_TYPE_KEY		"spbmvref"
#define	SIMPLE_PINT_META_VALUE_REFERENCE_TYPE_KEY		"spimvref"
#define	SIMPLE_PREAL_META_VALUE_REFERENCE_TYPE_KEY		"sprmvref"
#define	SIMPLE_PSTRING_META_VALUE_REFERENCE_TYPE_KEY		"spsmvref"

	// simple non-meta instance references
#define	SIMPLE_PROCESS_NONMETA_INSTANCE_REFERENCE_TYPE_KEY	"sprcnref"
#define	SIMPLE_CHANNEL_NONMETA_INSTANCE_REFERENCE_TYPE_KEY	"schnnref"
#define	SIMPLE_DBOOL_NONMETA_INSTANCE_REFERENCE_TYPE_KEY	"sdbnref"
#define	SIMPLE_DINT_NONMETA_INSTANCE_REFERENCE_TYPE_KEY		"sdinref"
#define	SIMPLE_ENUM_NONMETA_INSTANCE_REFERENCE_TYPE_KEY		"sdenref"
#define	SIMPLE_STRUCT_NONMETA_INSTANCE_REFERENCE_TYPE_KEY	"sdsnref"
#define	SIMPLE_PBOOL_NONMETA_INSTANCE_REFERENCE_TYPE_KEY	"spbnref"
#define	SIMPLE_PINT_NONMETA_INSTANCE_REFERENCE_TYPE_KEY		"spinref"
#define	SIMPLE_PREAL_NONMETA_INSTANCE_REFERENCE_TYPE_KEY	"sprnref"
#define	SIMPLE_PSTRING_NONMETA_INSTANCE_REFERENCE_TYPE_KEY	"spsnref"
	// member non-meta instance references
#define	MEMBER_PROCESS_NONMETA_INSTANCE_REFERENCE_TYPE_KEY	"mprcnref"
#define	MEMBER_CHANNEL_NONMETA_INSTANCE_REFERENCE_TYPE_KEY	"mchnnref"
#define	MEMBER_DBOOL_NONMETA_INSTANCE_REFERENCE_TYPE_KEY	"mdbnref"
#define	MEMBER_DINT_NONMETA_INSTANCE_REFERENCE_TYPE_KEY		"mdinref"
#define	MEMBER_ENUM_NONMETA_INSTANCE_REFERENCE_TYPE_KEY		"mdenref"
#define	MEMBER_STRUCT_NONMETA_INSTANCE_REFERENCE_TYPE_KEY	"mdsnref"
/**
#define	MEMBER_PBOOL_NONMETA_VALUE_REFERENCE_TYPE_KEY		"mpbnref"
#define	MEMBER_PINT_NONMETA_VALUE_REFERENCE_TYPE_KEY		"mpinref"
#define	MEMBER_PREAL_NONMETA_VALUE_REFERENCE_TYPE_KEY		"mprnref"
#define	MEMBER_PSTRING_NONMETA_VALUE_REFERENCE_TYPE_KEY		"mpsnref"
**/

	// aggregates are complex compositions / concatenations of arrays
#define	AGGREGATE_PROCESS_META_INSTANCE_REFERENCE_TYPE_KEY	"aprcmref"
#define	AGGREGATE_CHANNEL_META_INSTANCE_REFERENCE_TYPE_KEY	"achnmref"
#define	AGGREGATE_DBOOL_META_INSTANCE_REFERENCE_TYPE_KEY	"adbmiref"
#define	AGGREGATE_DINT_META_INSTANCE_REFERENCE_TYPE_KEY		"adimiref"
#define	AGGREGATE_ENUM_META_INSTANCE_REFERENCE_TYPE_KEY		"ademiref"
#define	AGGREGATE_STRUCT_META_INSTANCE_REFERENCE_TYPE_KEY	"adsmiref"
#define	AGGREGATE_PBOOL_META_VALUE_REFERENCE_TYPE_KEY		"apbmvref"
#define	AGGREGATE_PINT_META_VALUE_REFERENCE_TYPE_KEY		"apimvref"
#define	AGGREGATE_PREAL_META_VALUE_REFERENCE_TYPE_KEY		"aprmvref"
#define	AGGREGATE_PSTRING_META_VALUE_REFERENCE_TYPE_KEY		"apsmvref"

	// member references of the form x.y, (x may be indexed / member ref.)
	// compile-time member meta references
#define	MEMBER_PROCESS_INSTANCE_REFERENCE_TYPE_KEY		"mprocref"
#define	MEMBER_CHANNEL_INSTANCE_REFERENCE_TYPE_KEY		"mchanref"
#define	MEMBER_DBOOL_INSTANCE_REFERENCE_TYPE_KEY		"mdbolref"
#define	MEMBER_DINT_INSTANCE_REFERENCE_TYPE_KEY			"mdintref"
#define	MEMBER_ENUM_INSTANCE_REFERENCE_TYPE_KEY			"menumref"
#define	MEMBER_STRUCT_INSTANCE_REFERENCE_TYPE_KEY		"mstrtref"
	// no such thing as param member instance reference... yet

// Expressions:
	// Note that the above pbool/pint instance references are 
	// also classified as expressions.  
	// Many expression classes come in two flavors: const, dynamic
	//	const -- resolved to a compile time constant
	//	dynamic -- everything else
	// When in doubt, it is safe to use the dynamic flavor.  

// value scalars
#define	CONST_PBOOL_TYPE_KEY					"cnstpb__"
#define	CONST_PINT_TYPE_KEY					"cnstpi__"
#define	CONST_PREAL_TYPE_KEY					"cnstpr__"
#define	CONST_PSTRING_TYPE_KEY					"cnstps__"
// value collections
#define	CONST_PBOOL_COLLECTION_TYPE_KEY				"cnstpbc"
#define	CONST_PINT_COLLECTION_TYPE_KEY				"cnstpic"
#define	CONST_PREAL_COLLECTION_TYPE_KEY				"cnstprc"
#define	CONST_PSTRING_COLLECTION_TYPE_KEY			"cnstpsc"

#define	CONST_RANGE_TYPE_KEY					"cnstrng"
#define	DYNAMIC_RANGE_TYPE_KEY					"dynrng"

#define	CONST_PARAM_EXPR_LIST_TYPE_KEY				"cprmlst"
#define	DYNAMIC_PARAM_EXPR_LIST_TYPE_KEY			"dprmlst"

	// "index" may be single pint or a range
#define	CONST_INDEX_LIST_TYPE_KEY				"cindlst"
#define	DYNAMIC_INDEX_LIST_TYPE_KEY				"dindlst"
#define	CONST_RANGE_LIST_TYPE_KEY				"crnglst"
#define	DYNAMIC_RANGE_LIST_TYPE_KEY				"drnglst"

	// symbolic expressions on meta-values
#define	PARAM_DEFINED_EXPR_TYPE_KEY				"pdefined"
#define	PINT_UNARY_EXPR_TYPE_KEY				"pintunry"
#define	PBOOL_UNARY_EXPR_TYPE_KEY				"pbolunry"
#define	PREAL_UNARY_EXPR_TYPE_KEY				"prelunry"
#define	PINT_ARITH_EXPR_TYPE_KEY				"piarthex"
#define	PINT_RELATIONAL_EXPR_TYPE_KEY				"pireltex"
#define	PREAL_ARITH_EXPR_TYPE_KEY				"prarthex"
#define	PREAL_RELATIONAL_EXPR_TYPE_KEY				"prreltex"
#define	PBOOL_LOGICAL_EXPR_TYPE_KEY				"pblgcexp"
#define	PINT_ARITH_LOOP_EXPR_TYPE_KEY				"piarthlp"
#define	PBOOL_LOGICAL_LOOP_EXPR_TYPE_KEY			"pblogclp"
#define	PREAL_ARITH_LOOP_EXPR_TYPE_KEY				"prarthlp"
#define	PINT_META_FUNCTION_CALL_TYPE_KEY			"pimfunex"
#define	PBOOL_META_FUNCTION_CALL_TYPE_KEY			"pbmfunex"
#define	PREAL_META_FUNCTION_CALL_TYPE_KEY			"prmfunex"
#define	PSTRING_RELATIONAL_EXPR_TYPE_KEY			"psreltex"
#define	PSTRING_META_FUNCTION_CALL_TYPE_KEY			"psmfunex"

	// convert expressions
#define	CONVERT_PINT_TO_PREAL_EXPR_TYPE_KEY			"cnvtpipr"
	// run-time cast expressions
#define	BOOL_RETURN_CAST_EXPR_TYPE_KEY				"bretcast"
#define	INT_RETURN_CAST_EXPR_TYPE_KEY				"iretcast"
#define	REAL_RETURN_CAST_EXPR_TYPE_KEY				"rretcast"
#define	STRING_RETURN_CAST_EXPR_TYPE_KEY			"sretcast"

// nonmeta expressions
#define	NONMETA_INT_ARITH_EXPR_TYPE_KEY				"narithex"
#define	NONMETA_INT_RELATIONAL_EXPR_TYPE_KEY			"nrelatex"
#define	NONMETA_BOOL_LOGICAL_EXPR_TYPE_KEY			"nlogicex"
#define	NONMETA_INT_NEGATION_EXPR_TYPE_KEY			"nnegatex"
#define	NONMETA_BOOL_NEGATION_EXPR_TYPE_KEY			"nbnotex"
#define	NONMETA_INDEX_LIST_TYPE_KEY				"nindlist"
#define	NONMETA_EXPR_LIST_TYPE_KEY				"nexplist"
#define	NONMETA_RANGE_TYPE_KEY					"nmetarng"
#define	NONMETA_RANGE_LIST_TYPE_KEY				"nrnglist"
#define	NONMETA_INT_ARITH_LOOP_EXPR_TYPE_KEY			"niarlpex"
#define	NONMETA_BOOL_LOGICAL_LOOP_EXPR_TYPE_KEY			"nlgclpex"
#define	NONMETA_REAL_ARITH_LOOP_EXPR_TYPE_KEY			"nrarlpex"
#define	NONMETA_CHANNEL_PROBE_EXPR_TYPE_KEY			"chanprob"
#define	NONMETA_FUNCTION_CALL_EXPR_TYPE_KEY			"nfuncexp"

// Sequential and control statements: (loops and conditionals)
// including sequential instantiations, assignments and connections:
	
#define	PROCESS_INSTANTIATION_STATEMENT_TYPE_KEY		"procinst"
#define	CHANNEL_INSTANTIATION_STATEMENT_TYPE_KEY		"chaninst"
#define	DATA_INSTANTIATION_STATEMENT_TYPE_KEY			"datainst"
#define	DBOOL_INSTANTIATION_STATEMENT_TYPE_KEY			"dboolins"
#define	DINT_INSTANTIATION_STATEMENT_TYPE_KEY			"dintinst"
#define	PBOOL_INSTANTIATION_STATEMENT_TYPE_KEY			"pboolins"
#define	PINT_INSTANTIATION_STATEMENT_TYPE_KEY			"pintinst"
#define	PREAL_INSTANTIATION_STATEMENT_TYPE_KEY			"prealins"
#define	PSTRING_INSTANTIATION_STATEMENT_TYPE_KEY		"pstrinst"

#define	PROCESS_TEMPLATE_TYPE_COMPLETION_TYPE_KEY		"proctpcp"

#define	PROCESS_INSTANCE_ATTRIBUTE_TYPE_KEY			"procattr"
#define	CHANNEL_INSTANCE_ATTRIBUTE_TYPE_KEY			"chanattr"
#define	DATA_INSTANCE_ATTRIBUTE_TYPE_KEY			"dataattr"
#define	DBOOL_INSTANCE_ATTRIBUTE_TYPE_KEY			"dboolatt"
#define	DINT_INSTANCE_ATTRIBUTE_TYPE_KEY			"dintattr"
#define	ENUM_INSTANCE_ATTRIBUTE_TYPE_KEY			"enumattr"

#define	PROCESS_DIRECTION_DECLARATION_TYPE_KEY			"procdird"
#define	CHANNEL_DIRECTION_DECLARATION_TYPE_KEY			"chandird"
#define	DATA_DIRECTION_DECLARATION_TYPE_KEY			"datadird"
#define	DBOOL_DIRECTION_DECLARATION_TYPE_KEY			"dbooldir"
#define	DINT_DIRECTION_DECLARATION_TYPE_KEY			"dintdird"
#define	ENUM_DIRECTION_DECLARATION_TYPE_KEY			"enumdird"

#define	PBOOL_EXPR_ASSIGNMENT_TYPE_KEY				"pbassign"
#define	PINT_EXPR_ASSIGNMENT_TYPE_KEY				"piassign"
#define	PREAL_EXPR_ASSIGNMENT_TYPE_KEY				"prassign"
#define	PSTRING_EXPR_ASSIGNMENT_TYPE_KEY			"psassign"

// will be obsolete if it is subclassed into the following
#define	ALIAS_CONNECTION_TYPE_KEY				"aliascon"

#define	DBOOL_ALIAS_CONNECTION_TYPE_KEY				"dbalias"
#define	DINT_ALIAS_CONNECTION_TYPE_KEY				"dialias"
#define	ENUM_ALIAS_CONNECTION_TYPE_KEY				"enmalias"
#define	STRUCT_ALIAS_CONNECTION_TYPE_KEY			"stralias"
#define	CHANNEL_ALIAS_CONNECTION_TYPE_KEY			"chnalias"
#define	PROCESS_ALIAS_CONNECTION_TYPE_KEY			"prcalias"

// obsolete, in favor of subtypes
// #define	PORT_CONNECTION_TYPE_KEY			"portconn"
#define	PROCESS_PORT_CONNECTION_TYPE_KEY			"prcprtcn"
#define	CHANNEL_PORT_CONNECTION_TYPE_KEY			"chnprtcn"
#define	STRUCT_PORT_CONNECTION_TYPE_KEY				"dstprtcn"
#define	IMPLICIT_PORT_OVERRIDE_TYPE_KEY				"iprtovrd"
#define	PORT_SCOPE_TYPE_KEY					"portscop"
#define	LOOP_SCOPE_TYPE_KEY					"loopscop"
#define	CONDITIONAL_SCOPE_TYPE_KEY				"condscop"

// Language-specifics: (PRS, CHP, ...)
#define	PRS_PULLUP_TYPE_KEY					"PRSpulup"
#define	PRS_PULLDN_TYPE_KEY					"PRSpuldn"
#define	PRS_RULE_LOOP_TYPE_KEY					"PRSrloop"
#define	PRS_NESTED_RULES_TYPE_KEY				"PRSrnest"
#define	PRS_SUBCKT_RULES_TYPE_KEY				"PRSrsubc"
#define	PRS_RULE_COND_TYPE_KEY					"PRSrcond"
#define	PRS_AND_TYPE_KEY					"PRSand"
#define	PRS_OR_TYPE_KEY						"PRSor"
#define	PRS_AND_LOOP_TYPE_KEY					"PRSandlp"
#define	PRS_OR_LOOP_TYPE_KEY					"PRSorlp"
#define	PRS_NOT_TYPE_KEY					"PRSnot"
#define	PRS_LITERAL_TYPE_KEY					"PRSlitrl"
#define	PRS_MACRO_TYPE_KEY					"PRSmacro"

#define	RTE_ASSIGNMENT_TYPE_KEY					"RTEassgn"
#define	RTE_ASSIGNMENT_LOOP_TYPE_KEY				"RTEaloop"
#define	RTE_NESTED_ASSIGNMENTS_TYPE_KEY				"RTEanest"
#define	RTE_ASSIGNMENT_COND_TYPE_KEY				"RTEacond"
#define	RTE_BINOP_TYPE_KEY					"RTEbinop"
#define	RTE_BINOP_LOOP_TYPE_KEY					"RTEoplop"
#define	RTE_NOT_TYPE_KEY					"RTEnot--"
#define	RTE_LITERAL_TYPE_KEY					"RTElitrl"

#define	CHP_SEQUENCE_TYPE_KEY					"CHPseqnc"
#define	CHP_CONCURRENT_TYPE_KEY					"CHPconcr"
#define	CHP_GUARDED_ACTION_TYPE_KEY				"CHPguard"
#define	CHP_DET_TYPE_KEY					"CHPdetsl"
#define	CHP_NONDET_TYPE_KEY					"CHPndtsl"
#define	CHP_PROB_TYPE_KEY					"CHPprbsl"
#define	CHP_SELECT_LOOP_TYPE_KEY				"CHPlopsl"
#define	CHP_ACTION_LOOP_TYPE_KEY				"CHPlopac"
#define	CHP_ASSIGNMENT_TYPE_KEY					"CHPassgn"
#define	CHP_WAIT_TYPE_KEY					"CHPwait"
#define	CHP_SEND_TYPE_KEY					"CHPsend"
#define	CHP_RECEIVE_TYPE_KEY					"CHPrecv"
#define	CHP_FOREVER_LOOP_TYPE_KEY				"CHPloop"
#define	CHP_DO_WHILE_TYPE_KEY					"CHPdowhl"
#define	CHP_FUNCTION_CALL_TYPE_KEY				"CHPfcall"

#define	SPEC_DIRECTIVE_TYPE_KEY					"SPCdrctv"
#define	SPEC_PROC_DIRECTIVE_TYPE_KEY				"SPCprdrc"
#define	SPEC_INVARIANT_TYPE_KEY					"SPCinvar"
#define	SPEC_DIRECTIVE_LOOP_TYPE_KEY				"SPCdloop"
#define	SPEC_DIRECTIVE_CONDITIONAL_TYPE_KEY			"SPCdcond"
	// more class constants here...

//=============================================================================
#endif	// __HAC_OBJECT_PERSISTENT_TYPE_HASH_H__

