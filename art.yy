/*
	"art.yy"
	AsynchRonous Tools (.art)

	note: this is not the same language as that found in lib/art.cy
	but is very close.  Differences are mostly syntactic and not semantic.  

	note: ancient versions of yacc reject // end-of-line comments
*/

%{
#include <iostream>

#include "art_parser.h"			// should be first
#include "art_parser_prs.h"
#include "art_parser_hse.h"
#include "art_parser_chp.h"
#include "y.output.h"			// auto-generated state strings! :)

using namespace std;
using namespace ART::parser;

extern	int yylex(void);		// ancient compiler rejects
extern	int at_eof(void);		// from "art.l"
extern "C" {
	int yyparse(void);		// parser routine to call
	void yyerror(const char* msg);	// ancient compiler rejects
//	void yyerror(char* msg);	// replace with this if necessary
}

/*
useful typedefs are defined in art_parser.h
macros: d = delimiter, n = node, b = begin, e = end, l = list
*/

/**
	documenting yacc's internal tables:
	The extern declarations are needed so the compiler doesn't complain
	about uninitialized values.  
	They are actually defined in the same generated file y.tab.cc.
	The definitions are not actually used.  
 */
namespace yacc {

/**
	The values of this table correspond to the reduction rule as
	enumerated in order of appearance in the grammar file.  
	The table is indexed by the production rule number.  
 */
extern const short yylhs[];

/**
	The value of this table correspond to the number of symbols
	on the right-hand-side of a production.  
	This number is used to determine the number of symbols to 
	pop off of the yyss symbol stack.  
	Tha table is indexed by the production rule number.  
 */
extern const short yylen[];

/**
	This table is used to determine whether the parser is in a state
	to reduce (as the first action to check in yyloop).  
	If the value is zero, then the parser continues without reducing, 
	otherwise it jumps to yyreduce.  
	This table is indexed by yystate, which is declared as a
	local int in yyparse().  
 */
extern const short yydefred[];

/**
	Still figuring this one out...
	the state to jump to if yyn is a valid state, 
	This table is indexed by yym, yylhs[yyn].
 */
extern const short yydgoto[];

/**
	Values are used to determine whether parser is in a valid
	state for shifting.  
	This table is indexed by yystate and *yyssp (only for error recovery).  
 */
extern const short yysindex[];

/**
	Values are used to determine whether parser is in a valid 
	state for reducing.  
	This table is indexed by yystate.  
 */
extern const short yyrindex[];

/**
	Values are used to determine whether parser is in a valid 
	state for goto.  
	This table is indexed by yystate.  
 */
extern const short yygindex[];

/**
	State transition table for updating yyn, indexed by yyn.  
 */
extern const short yytable[];

/**
	Values correspond to enumerated tokens, 
	indexed by yyn, and compared to yychar for consistency.  
 */
extern const short yycheck[];

/**
	The string names with which tokens, both terminal and nonterminal,
	were defined.  The indices of the table correspond to either 
	single characters' value or the automatic enumerations of 
	token symbols, yychar.  
	Value of NULL implies that no string is associated
	with a particular index.  
 */
extern const char* const yyname[];

/**
	The string representations of the production rules, shown as
	left-hand-side : right-hand-side.  
	This table is indexed by the rule number, enumerated in the order
	of appearance in the grammar file.  
 */
extern const char* const yyrule[];

};	// end namespace yacc

%}

%union {
/**
	Use this universal symbol type for both lexer and parser.  
	The reason we stick to a single abstract type as opposed to 
	a union is so that in error handling, we don't have to keep track
	of token tags to figure out which union member is actually in
	a particular symbol stack entry when we dump the stack.  
	(see yyerror() for details)
	We leverage polymorphism for clean, modular error reporting.  
	Since all return types are abstract nodes, 
	a consequence of this choice is that we perform some run-time
	sanity type checks in the constructors for the various classes.  
	This keeps the art.yy grammar file as clean as possible.  
	Let the constructors bear the burden.  
 */
	ART::parser::node*	n;
/***
	It is not safe to refer to the node* n member of the union
	even if all of the below members of the union are 
	somehow derived from node, because their virtual tables differ.  
	Instead one must write a wrapper to properly convert
	pointers with their virtual tables.  
	How do we know what union member it is?
	A just question.  We walk the state stack pointer.  
	This is done using yacc-union-type.awk.  
***/
	terminal*		_terminal;
	token_keyword*		_token_keyword;
	token_string*		_token_string;
	token_char*		_token_char;
	token_int*		_token_int;
	token_bool*		_token_bool;
	token_float*		_token_float;
	token_identifier*	_token_identifier;
	token_quoted_string*	_token_quoted_string;
	token_type*		_token_type;

	root_body*		_root_body;
	namespace_body*		_namespace_body;
	using_namespace*	_using_namespace;
	process_prototype*	_process_prototype;
	process_def*		_process_def;
	def_type_id*		_def_type_id;
	type_id*		_type_id;
	port_formal_decl_list*	_port_formal_decl_list;
	port_formal_decl*	_port_formal_decl;
	port_formal_id_list*	_port_formal_id_list;
	port_formal_id*		_port_formal_id;
	template_formal_decl_list*	_template_formal_decl_list;
	template_formal_decl*	_template_formal_decl;
	template_formal_id_list*	_template_formal_id_list;
	template_formal_id*	_template_formal_id;
	data_type_base*		_data_type_base;
	base_data_type_list*	_base_data_type_list;
	user_data_type_prototype*	_user_data_type_prototype;
	user_data_type_def*	_user_data_type_def;
	chan_type*		_chan_type;
	user_chan_type_prototype*	_user_chan_type_prototype;
	user_chan_type_def*	_user_chan_type_def;
	data_param_list*	_data_param_list;
	instance_declaration*	_instance_declaration;
	declaration_id_list*	_declaration_id_list;
	declaration_base*	_declaration_base;
	declaration_array*	_declaration_array;
	definition_body*	_definition_body;
	loop_instantiation*	_loop_instantiation;
	guarded_definition_body_list*	_guarded_definition_body_list;
	guarded_definition_body*	_guarded_definition_body;
	actuals_connection*	_actuals_connection;
	alias_assign*		_alias_assign;

	expr*			_expr;
	paren_expr*		_paren_expr;
	id_expr*		_id_expr;

/*** not used
	prefix_expr*		_prefix_expr;
	index_expr*		_index_expr;
	member_expr*		_member_expr;
	arith_expr*		_arith_expr;
	relational_expr*	_relational_expr;
	logical_expr*		_logical_expr;
***/
	assign_stmt*		_assign_stmt;
	incdec_stmt*		_incdec_stmt;
	expr_list*		_expr_list;
	range*			_range;
	range_list*		_range_list;

	CHP::body*		_chp_body;
	CHP::stmt_list*		_chp_stmt_list;
	CHP::skip*		_chp_skip;
	CHP::log*		_chp_log;
	CHP::loop*		_chp_loop;
	CHP::do_until*		_chp_do_until;
	CHP::wait*		_chp_wait;
	CHP::nondet_selection*	_chp_nondet_selection;
	CHP::det_selection*	_chp_det_selection;
	CHP::guarded_command*	_chp_guarded_command;
	CHP::else_clause*	_chp_else_clause;
	CHP::comm_list*		_chp_comm_list;
	CHP::send*		_chp_send;
	CHP::receive*		_chp_receive;

	HSE::body*		_hse_body;
	HSE::stmt_list*		_hse_stmt_list;
	HSE::skip*		_hse_skip;
	HSE::loop*		_hse_loop;
	HSE::do_until*		_hse_do_until;
	HSE::wait*		_hse_wait;
	HSE::nondet_selection*	_hse_nondet_selection;
	HSE::det_selection*	_hse_det_selection;
	HSE::guarded_command*	_hse_guarded_command;
	HSE::else_clause*	_hse_else_clause;

	PRS::body*		_prs_body;
	PRS::rule_list*		_prs_rule_list;
	PRS::rule*		_prs_rule;

}

%{
/* automatically generated function to resolve parser symbol type
	on the yy value stack, base on yy state stack transitions
 */
extern	node* yy_union_resolve(const YYSTYPE& u, const short i, const short j);
extern	node* yy_union_lookup(const YYSTYPE& u, const int c);
%}

/*
	The lexer returns newly allocated nodes FOR ALL TOKENS, 
	even ones that are just symbols.  
	If you don't want to use a returned symbol, delete it!
	We are keeping the memory and performance overhead of the 
	front-end for now, for the sake of precise error reporting.  
	If performance is a concern, consider writing new allocators
	(using memory pools) to replace the default.  

	The following single characters are legitimate tokens:
	][(){}<>*%/=:;|!?~&^.,+-

	(can just copy these into a lex declaration, enclosed in [])

	note on character classes from grep's man page:
		Most metacharacters  lose  their  special  meaning  inside
		lists.  To include a literal ] place it first in the list.
		Similarly, to include a literal ^ place  it  anywhere  but
		first.  Finally, to include a literal - place it last.


%token	<_token_char>	LBRACE RBRACE LPAREN RPAREN LBRACKET RBRACKET
%token	<_token_char>	LT GT				// angle brackets
%token	<_token_char>	SEMICOLON COMMA COLON MEMBER 
%token	<_token_char>	ASSIGN
%token	<_token_char>	PLUS MINUS STAR DIVIDE PERCENT
%token	<_token_char>	BANG QUERY
%token	<_token_char>	TILDE AND PIPE XOR
*/
/* change these to _token_char */
%type	<_token_char>	'{' '}' '[' ']' '(' ')' '<' '>'
%type	<_token_char>	',' '.' ';' ':'
%type	<_token_char>	'=' '+' '-' '*' '/' '%'
%type	<_token_char>	'!' '?' '~' '&' '|' '^'

/*
	the following tokens are defined below because they consist of
	2 or more characters
*/

%token	<_token_identifier>	ID
%token	<_token_float>		FLOAT
%token	<_token_int>		INT
%token	<_token_quoted_string>	STRING

/* _token_string */
%token	<_token_string>		LE GE EQUAL NOTEQUAL
%token	<_token_string>		THICKBAR SCOPE RANGE
%token	<_token_string>		IMPLIES RARROW
%token	<_token_string>		BEGINLOOP BEGINPROB ENDPROB
%token	<_token_string>		DEFINEOP
%token	<_token_string>		LOGICAL_AND LOGICAL_OR
%token	<_token_string>		INSERT EXTRACT
%token	<_token_string>		PLUSPLUS MINUSMINUS

/* _token_keyword */
%token	<_token_keyword>	NAMESPACE
%token	<_token_keyword>	OPEN AS
%token	<_token_keyword>	CHP_LANG HSE_LANG PRS_LANG
%token	<_token_keyword>	SKIP ELSE LOG
%token	<_token_keyword>	DEFINE DEFPROC DEFCHAN DEFTYPE
%token	<_token_keyword>	SET GET SEND RECV
%token	<_token_keyword>	CHANNEL

%token	<_token_bool>		BOOL_TRUE BOOL_FALSE

/* _token_type */
%token	<_token_type>		INT_TYPE BOOL_TYPE PINT_TYPE PBOOL_TYPE

/* non-terminals */
%type	<n>	module top_root body body_item basic_item namespace_management
%type	<n>	definition defproc def_type_id
%type	<_token_keyword>	def_or_proc
%type	<n>	declaration declare_proc_proto
%type	<n>	declare_type_proto declare_chan_proto
%type	<n>	optional_template_formal_decl_list_in_angles
%type	<n>	template_formal_decl_list template_formal_decl
%type	<n>	template_formal_id_list template_formal_id
%type	<n>	optional_port_formal_decl_list_in_parens port_formal_decl_list
%type	<n>	port_formal_decl port_formal_id_list port_formal_id
%type	<n>	type_id base_template_type
/* %type	<n>	formal_id */
%type	<n>	base_chan_type chan_or_port
%type	<n>	base_data_type_list_in_parens
%type	<n>	base_data_type_list base_data_type
%type	<n>	deftype defchan
%type	<n>	set_body get_body send_body recv_body
%type	<n>	data_param_list data_param data_param_list_in_parens
%type	<n>	data_param_id_list data_param_id
%type	<n>	definition_body
%type	<n>	instance_item instance_declaration 
%type	<n>	loop_instantiation conditional_instantiation
%type	<n>	declaration_id_list declaration_id_item
%type	<n>	instance_connection instance_alias connection_actuals_list
%type	<n>	guarded_definition_body_list guarded_definition_body
%type	<n>	language_body
%type	<n>	chp_body full_chp_body_item_list full_chp_body_item
%type	<n>	chp_body_item chp_loop chp_do_until chp_selection chp_wait
%type	<n>	chp_nondet_guarded_command_list
%type	<n>	chp_unmatched_det_guarded_command_list
%type	<n>	chp_matched_det_guarded_command_list
%type	<n>	chp_guarded_command chp_else_clause chp_assignment
%type	<n>	chp_comm_list chp_comm_action chp_send chp_recv
%type	<n>	hse_body full_hse_body_item_list full_hse_body_item
%type	<n>	hse_body_item hse_loop hse_do_until hse_selection hse_wait
%type	<n>	hse_guarded_command hse_else_clause
%type	<n>	hse_nondet_guarded_command_list
%type	<n>	hse_matched_det_guarded_command_list
%type	<n>	hse_unmatched_det_guarded_command_list
%type	<n>	hse_assignment
%type	<n>	prs_body single_prs
%type	<_expr>	prs_expr
%type	<_token_string>	prs_arrow
%type	<_token_char>	dir
%type	<_expr>	paren_expr expr
/* %type	<n>	primary_expr */
%type	<_expr>	literal
%type	<_id_expr>	id_expr
%type	<_id_expr>	qualified_id absolute_id relative_id
%type	<_expr_list>	member_index_expr_list
%type	<_expr>	member_index_expr unary_expr
%type	<_expr>	multiplicative_expr additive_expr shift_expr
%type	<_expr>	relational_equality_expr and_expr
%type	<_expr>	exclusive_or_expr inclusive_or_expr
%type	<_expr>	logical_and_expr logical_or_expr
%type	<n>	assignment_stmt binary_assignment unary_assignment
/* %type	<n>	conditional_expr optional_expr_in_braces */
%type	<n>	optional_member_index_expr_list_in_angles
%type	<n>	member_index_expr_list_in_angles
%type	<n>	member_index_expr_list_in_parens
%type	<_expr_list>	expr_list_in_parens expr_list
%type	<_range_list>	optional_range_list_in_brackets range_list_in_brackets
%type	<_range_list>	range_list
%type	<_range>	range


%start	module
%%
/******************************************************************************
//	Grammar -- re-written to be LALR(1)
******************************************************************************/

/* top level syntax */

module
	: top_root /* END_OF_FILE (doesn't work they way I want yet...) */
		{ $$ = $1; }
	;

top_root
	: body
	/* allow empty file */
	| 
		{ $$ = NULL; }
	;

body
	: body body_item { $$ = root_body_append($1, NULL, $2); }
	| body_item { $$ = new root_body($1); }
	;

body_item
	: basic_item
	| definition
	| declaration
	;

basic_item
/* namespace_management already includes semicolon where needed */
/* proposed change: forbid nested namespacs, only allow in root_item */
	: namespace_management
/* instance_item already includes semicolon where needed */
	| instance_item
	;

namespace_management
	/* C++ style classes/namespaces require semicolon */
	: NAMESPACE ID '{' top_root '}' ';'
		{ $$ = new namespace_body($1, $2, $3, $4, $5, $6); }
	/* or C++ style: using namespace blah; */
	| OPEN id_expr AS ID ';'
		{ $$ = new using_namespace($1, $2, $3, $4, $5); }
	| OPEN id_expr ';'
		{ $$ = new using_namespace($1, $2, $3); }
	/* ever close namespace? */
	;

/* Process, datatype, and channel definition. */
definition
	: defproc 
	| deftype
	| defchan
	;

/* declaration prototypes, like forward declarations */
declaration
	: declare_proc_proto
	| declare_type_proto
	| declare_chan_proto
	;

/******************************************************************************
//	Process
******************************************************************************/

def_or_proc
	: DEFINE
	| DEFPROC
	;

declare_proc_proto
	: def_or_proc def_type_id
	  optional_port_formal_decl_list_in_parens ';'
		{ $$ = new process_prototype($1, $2, $3, $4); }
	;

defproc
	/* using <> to follow C+ template parameters in def_type_id */
	: def_or_proc def_type_id
	  optional_port_formal_decl_list_in_parens '{' definition_body '}'
	{ $$ = new process_def($1, $2, $3, 
		definition_body_wrap($4, $5, $6));
	}
	| def_or_proc def_type_id
	  optional_port_formal_decl_list_in_parens '{' '}'
	{ $$ = new process_def($1, $2, $3, 
		definition_body_wrap($4, new definition_body(), $5));
	}
	;

optional_port_formal_decl_list_in_parens
	/* note: the parens are NOT optional! */
	: '(' port_formal_decl_list ')'
		{ $$ = port_formal_decl_list_wrap($1, $2, $3); }
	| '(' ')'
		{ $$ = (new port_formal_decl_list(NULL))->wrap($1, $2); }
	;

def_type_id
	: ID optional_template_formal_decl_list_in_angles
		{ $$ = new def_type_id($1, $2); }
	;

/******** Meta (template) language parameters ********/

optional_template_formal_decl_list_in_angles
	: '<' template_formal_decl_list '>'
		{ $$ = template_formal_decl_list_wrap($1, $2, $3); }
	| { $$ = NULL; }
	;

template_formal_decl_list
	: template_formal_decl_list ';' template_formal_decl
		{ $$ = template_formal_decl_list_append($1, $2, $3); }
	| template_formal_decl
		{ $$ = new template_formal_decl_list($1); }
	;

template_formal_decl
/* changing to C-style formal parameters, allowing comma-lists
	is there any need for user-defined types in template argument? */
	: base_template_type template_formal_id_list
		{ $$ = new template_formal_decl($1, $2); }
	;

template_formal_id_list
	: template_formal_id_list ',' template_formal_id
		{ $$ = template_formal_id_list_append($1, $2, $3); }
	| template_formal_id
		{ $$ = new template_formal_id_list($1); }
	;

template_formal_id
	: ID optional_range_list_in_brackets
		{ $$ = new template_formal_id($1, $2); }
	;

port_formal_decl_list
	/* would rather use ','-delimiter, but wth... */
	: port_formal_decl_list ';' port_formal_decl
		{ $$ = port_formal_decl_list_append($1, $2, $3); }
	| port_formal_decl
		{ $$ = new port_formal_decl_list($1); }
	;

port_formal_decl
	/* must switch to C-style formals, eliminate id_list */
	: type_id port_formal_id_list
		{ $$ = new port_formal_decl($1, $2); }
	;

port_formal_id_list
	: port_formal_id_list ',' port_formal_id
		{ $$ = port_formal_id_list_append($1, $2, $3); }
	| port_formal_id
		{ $$ = new port_formal_id_list($1); }
	;

port_formal_id
	: ID optional_range_list_in_brackets
		{ $$ = new port_formal_id($1, $2); }
	;

type_id
	: id_expr optional_member_index_expr_list_in_angles
		/* for userdef or chan type, and templating */
		{ $$ = new type_id($1, $2); }
	| base_chan_type
	| base_data_type
	;

/******************************************************************************
//	base types
******************************************************************************/

base_template_type
	: PINT_TYPE 		/* integer parameter */
		{ $$ = new data_type_base($1); }
	| PBOOL_TYPE		/* boolean parameter */
		{ $$ = new data_type_base($1); }
	;

/* channel type: channel, inport, outport, and data types */
base_chan_type
	/* eliminate defaulting? (to int?), use <template> style? */
	: chan_or_port base_data_type_list_in_parens
		{ $$ = chan_type_attach_data_types($1, $2); }
	;

chan_or_port
	: CHANNEL		/* a channel */
		{ $$ = new chan_type($1); }
	| CHANNEL '!'		/* an output port */
		{ $$ = new chan_type($1, $2); }
	| CHANNEL '?'		/* an input port */
		{ $$ = new chan_type($1, $2); }
	;

base_data_type_list_in_parens
	: '(' base_data_type_list ')'
		{ $$ = base_data_type_list_wrap($1, $2, $3); }
	;

base_data_type_list
	: base_data_type_list ',' base_data_type
		{ $$ = base_data_type_list_append($1, $2, $3); }
	| base_data_type
		{ $$ = new base_data_type_list($1); }
	;

/* actual data: int<width> or bool */
base_data_type
/* ever need user-defined types? eventually...
	optional parens get confused with template-parameters
	going to use angle brackets <> in the template-fashion */
	: INT_TYPE '<' INT '>'
		{ $$ = new data_type_base($1, $2, $3, $4); }
	| INT_TYPE
		{ $$ = new data_type_base($1); }
	| BOOL_TYPE
		{ $$ = new data_type_base($1); }
	;

/* definition types */
declare_type_proto
	: DEFTYPE ID DEFINEOP base_data_type 
          data_param_list_in_parens ';'
		{ $$ = new user_data_type_prototype($1, $2, $3, $4, $5, $6); }
	;

deftype
	: DEFTYPE ID DEFINEOP base_data_type 
          data_param_list_in_parens
	  '{' set_body get_body '}'
		{ $$ = new user_data_type_def(
			$1, $2, $3, $4, $5, $6, $7, $8, $9); }
	;

set_body
	: SET '{' chp_body '}'
		{ $$ = new CHP::body($1, chp_stmt_list_wrap($2, $3, $4)); }
	;

get_body
	: GET '{' chp_body '}'
		{ $$ = new CHP::body($1, chp_stmt_list_wrap($2, $3, $4)); }
	;

declare_chan_proto
	: DEFCHAN ID DEFINEOP base_chan_type 
	  data_param_list_in_parens ';'
		{ $$ = new user_chan_type_prototype($1, $2, $3, $4, $5, $6); }
	;
	
defchan
	: DEFCHAN ID DEFINEOP base_chan_type 
          data_param_list_in_parens
	  '{' send_body recv_body '}'
		{ $$ = new user_chan_type_def(
			$1, $2, $3, $4, $5, $6, $7, $8, $9); }
	;

send_body
	: SEND '{' chp_body '}'
		{ $$ = new CHP::body($1, chp_stmt_list_wrap($2, $3, $4)); }
	;

recv_body
	: RECV '{' chp_body '}'
		{ $$ = new CHP::body($1, chp_stmt_list_wrap($2, $3, $4)); }
	;

data_param_list_in_parens
	: '(' data_param_list ')'
		{ $$ = data_param_list_wrap($1, $2, $3); }
	;

data_param_list
/* like declarations in formals list
	consider using ';', similar to C-style... */
	: data_param_list ';' data_param
		{ $$ = data_param_list_append($1, $2, $3); }
	| data_param
		{ $$ = new data_param_list($1); }
	;

data_param
/*
	forseen problem: array brackets are with data_type
	but to follow C-style, we want the arrays to go with identifiers
	thinking of forbidding list, restricting to single
	semicolon-delimited declarations
*/
	: base_data_type data_param_id_list
		{ $$ = new instance_declaration($1, $2); }
	;

data_param_id_list
	: data_param_id_list ',' data_param_id
		{ $$ = declaration_id_list_append($1, $2, $3); }
	| data_param_id
		{ $$ = new declaration_id_list($1); }
	;

data_param_id
	: ID optional_range_list_in_brackets
		{ $$ = ($2) ? new declaration_array($1, $2)
			: new declaration_base($1); }
	;


/* --- definition_body --- */
definition_body
	: definition_body instance_item
		{ $$ = definition_body_append($1, NULL, $2); }
	| instance_item
		{ $$ = new definition_body($1); }
	| definition_body language_body
		{ $$ = definition_body_append($1, NULL, $2); }
	| language_body
		{ $$ = new definition_body($1); }
	;

/*
// considering splitting declarations from connection, e.g.
//		// declare first
// myprocesstype<template-params> foo[N,M];		// may be ranges as well
// foo[i,j](port-actuals);	// inside some for-loop, presumably
//		// then sparse instantiation connects and assigns
//
// myprocesstype bar(port-actuals);		// for single instance and decl
// myprocesstype<X,Y> foo;			// declare without connection
// foo(port-actuals);				// then connect
*/

instance_item
	: instance_declaration			/* single or array */
	| instance_connection			/* connection of ports */
	| instance_alias			/* aliasing connection */
	| loop_instantiation
	| conditional_instantiation
	;

loop_instantiation
	: '(' ';' ID ':' range ':' definition_body ')'
		{ $$ = new loop_instantiation($1, $2, $3, $4, $5, $6, $7); }
	;

conditional_instantiation
	: '[' guarded_definition_body_list ']'
		{ $$ = new conditional_instantiation(
			guarded_definition_body_list_wrap($1, $2, $3)); }
	;

instance_declaration
	/* type template is included in type_id, and is part of the type */
	: type_id declaration_id_list ';'
		{ $$ = new instance_declaration($1, $2, $3); }
	;

declaration_id_list
	: declaration_id_list ',' declaration_id_item
		{ $$ = declaration_id_list_append($1, $2, $3); }
	| declaration_id_item
		{ $$ = new declaration_id_list($1); }
	;

declaration_id_item
	/* array declaration: forbid connection, must connect later */
	: ID range_list_in_brackets
		{ $$ = new declaration_array($1, $2); }
	/* single instance declaration without connection */
	| ID
		{ $$ = new declaration_base($1); }
	/* single instance declaration with connection */
	| ID connection_actuals_list
		{ $$ = new actuals_connection($1, $2); }
	/* alias or assignment (not just member_id expression?) */
	| ID '=' expr
		{ $$ = new alias_assign($1, $2, $3); }
	;

instance_connection
/* taking a declared array or single instance and connecting ports
	are brackets part of the array/membership chain? */
	: member_index_expr connection_actuals_list ';'
		/* can this first id be scoped and/or membered? */
		{ $$ = new actuals_connection($1, $2, $3); }
	;

instance_alias
/* aliasing syntax, or data types is value assignment (general expr?)
	type check this, of course */
/*	: member_index_expr '=' member_index_expr ';'	*/
	: member_index_expr '=' expr ';'
		{ $$ = new alias_assign($1, $2, $3, $4); }
	;

/* this rule is sort of redundant, oh well... */
connection_actuals_list
	: member_index_expr_list_in_parens
	;

guarded_definition_body_list
	: guarded_definition_body_list THICKBAR guarded_definition_body
		{ $$ = guarded_definition_body_list_append($1, $2, $3); }
	| guarded_definition_body
		{ $$ = new guarded_definition_body_list($1); }
	;

/* any else clause? */

guarded_definition_body
	: expr RARROW definition_body
		{ $$ = new guarded_definition_body($1, $2, $3); }
	;


/******************************************************************************
//	Supported Languages
******************************************************************************/

language_body
	: CHP_LANG '{' chp_body '}'
		{ $$ = new CHP::body($1, chp_stmt_list_wrap($2, $3, $4)); }
	| HSE_LANG '{' hse_body '}'
		{ $$ = new HSE::body($1, hse_stmt_list_wrap($2, $3, $4)); }
	| PRS_LANG '{' prs_body '}'
		{ $$ = new PRS::body($1, prs_rule_list_wrap($2, $3, $4)); }
/*
//	| STACK_LANG '{' stack_body '}'
//		{ $$ = new stack::body($1, stack_rule_list_wrap($2, $3, $4)); }
//	and more...
*/
	;

/* --- Language: CHP --- */

chp_body
	: full_chp_body_item_list
	;

full_chp_body_item_list
	: full_chp_body_item_list ';' full_chp_body_item
		{ $$ = chp_stmt_list_append($1, $2, $3); }
	| full_chp_body_item
		{ $$ = new CHP::stmt_list($1); }
	;

/*
// make _this_ string together the pieces, rather than having the item
// add itself to a global list. If we do that, then everything will be
// properly scoped and the walk stack will store the correct state...
*/

full_chp_body_item
	/* expr_in_braces are assertions */
/*
//	: optional_expr_in_braces chp_body_item optional_expr_in_braces
	// temporarily simplify
*/
	: chp_body_item
	;

chp_body_item
	: chp_loop
	| chp_do_until
	| chp_selection
	| chp_wait
	| chp_assignment
	| chp_comm_list
	| SKIP { $$ = new CHP::skip($1); }
	| LOG expr_list_in_parens
		{ $$ = new CHP::log($1, $2); }
	;

chp_loop
	/* do-forever loop */
	: BEGINLOOP chp_body ']'
		{ $$ = new CHP::loop(hse_stmt_list_wrap($1, $2, $3)); }
	;

chp_do_until
	/* do-until-all-guards-false */
	: BEGINLOOP chp_matched_det_guarded_command_list ']'
		{ $$ = new CHP::do_until(chp_det_selection_wrap($1, $2, $3)); }
	;

chp_wait
	/* wait for expr to become true */
	: '[' expr ']'
		{ $$ = new CHP::wait($1, $2, $3); }
	;

chp_selection
	: '[' chp_matched_det_guarded_command_list ']'
		{ $$ = chp_det_selection_wrap($1, $2, $3); }
	| '[' chp_nondet_guarded_command_list ']'
		{ $$ = chp_nondet_selection_wrap($1, $2, $3); }
/*
// wtf is this?... probalistic selection for FT
//	| "%[" { chp_guarded_command ":" }** "]%"
//	| BEGINPROB chp_nondet_guarded_command_list ENDPROB
*/
	;

/*
note: these lists must have at least 2 clauses, will have to fix with "else"
*/
chp_nondet_guarded_command_list
	: chp_nondet_guarded_command_list ':' chp_guarded_command
		{ $$ = chp_nondet_selection_append($1, $2, $3); }
	| chp_guarded_command ':' chp_guarded_command
	/* can't have else clause in non-deterministic selection? */
		{ $$ = (new CHP::nondet_selection($1))->append($2, $3); }
	;

chp_matched_det_guarded_command_list
	: chp_unmatched_det_guarded_command_list THICKBAR chp_else_clause
		{ $$ = chp_det_selection_append($1, $2, $3); }
	| chp_unmatched_det_guarded_command_list
	;

chp_unmatched_det_guarded_command_list
	: chp_unmatched_det_guarded_command_list THICKBAR chp_guarded_command
		{ $$ = chp_det_selection_append($1, $2, $3); }
	| chp_guarded_command
		{ $$ = new CHP::det_selection($1); }
	;

chp_guarded_command
	: expr RARROW chp_body
		{ $$ = new CHP::guarded_command($1, $2, $3); }
	;

chp_else_clause
	: ELSE RARROW chp_body
		{ $$ = new CHP::else_clause($1, $2, $3); }
	;

/*
// consider replacing with c-style statements and type-checking for chp
// if top-of-language-stack == chp, forbid x-type of statement/expression
*/
chp_assignment
/* allow binary and unary assignments */
	: assignment_stmt
	;

chp_comm_list
	/* gives comma-separated communications precedence */
	: chp_comm_list ',' chp_comm_action
		{ $$ = chp_comm_list_append($1, $2, $3); }
	| chp_comm_action
		{ $$ = new CHP::comm_list($1); }
	;

chp_comm_action
	: chp_send 
	| chp_recv
	;

chp_send
/*
	// for now, require parens like function-call to
	// disambiguate between ( expr ) and ( expr_list )
*/
	: member_index_expr '!' expr_list_in_parens
		{ $$ = new CHP::send($1, $2, $3); }
	;

chp_recv
	/* parens are now required */
	: member_index_expr '?' member_index_expr_list_in_parens
		{ $$ = new CHP::receive($1, $2, $3); }
	;

/* --- Language: HSE --- */

hse_body
	: full_hse_body_item_list
	;

full_hse_body_item_list
	: full_hse_body_item_list ';' full_hse_body_item
		{ $$ = hse_stmt_list_append($1, $2, $3); }
	| full_hse_body_item
		{ $$ = new HSE::stmt_list($1); }
	;

full_hse_body_item
/*
	// temporary removal of assertions
//	: optional_expr_in_braces hse_body_item optional_expr_in_braces
//		{ $$ = $2; }
*/
	: hse_body_item
	;

hse_body_item
	/* returns an HSE::statement */
	: hse_loop
	| hse_do_until
	| hse_wait
	| hse_selection
	| hse_assignment
	| SKIP { $$ = new HSE::skip($1); }
	;

hse_loop
	: BEGINLOOP hse_body ']'
		{ $$ = new HSE::loop(hse_stmt_list_wrap($1, $2, $3)); }
	;

hse_do_until
	/* keep entering loop until all guards false */
	: BEGINLOOP hse_matched_det_guarded_command_list ']'
		{ $$ = new HSE::do_until(hse_det_selection_wrap($1, $2, $3)); }
	;

hse_wait
	: '[' expr ']'
		{ $$ = new HSE::wait($1, $2, $3); }
	;

hse_selection
	: '[' hse_matched_det_guarded_command_list ']'
		{ $$ = hse_nondet_selection_wrap($1, $2, $3); }
	| '[' hse_nondet_guarded_command_list ']'
		{ $$ = hse_det_selection_wrap($1, $2, $3); }
	;

hse_guarded_command
	: expr RARROW hse_body 
		{ $$ = new HSE::guarded_command($1, $2, $3); }
	;

hse_else_clause
	: ELSE RARROW hse_body
		{ $$ = new HSE::else_clause($1, $2, $3); }
	;

hse_nondet_guarded_command_list
	: hse_nondet_guarded_command_list ':' hse_guarded_command
		{ $$ = hse_nondet_selection_append($1, $2, $3); }
	| hse_guarded_command ':' hse_guarded_command
		{ $$ = (new HSE::nondet_selection($1))->append($2, $3); }
	;

hse_matched_det_guarded_command_list
	: hse_unmatched_det_guarded_command_list THICKBAR hse_else_clause
		{ $$ = hse_det_selection_append($1, $2, $3); }
	| hse_unmatched_det_guarded_command_list
	;

hse_unmatched_det_guarded_command_list
	: hse_unmatched_det_guarded_command_list THICKBAR hse_guarded_command
		{ $$ = hse_det_selection_append($1, $2, $3); }
	| hse_guarded_command
		{ $$ = new HSE::det_selection($1); }
	;

hse_assignment
/*
//	: assignment_stmt
// only allow ++ and -- assignments
*/
	: unary_assignment
		{ $$ = new HSE::assignment(
			IS_A(ART::parser::incdec_stmt*, $1)); }
	;

/*
//--- Language: PRS ---
// to do: add support for overriding default connection to Vdd, GND
// for power/ground isolation, and other tricks, pass gating... <-> <+> <=>
*/

prs_body
	: prs_body single_prs 
		{ $$ = prs_rule_list_append($1, NULL, $2); }
	| single_prs { $$ = new PRS::rule_list($1); }
	;

single_prs
	: prs_expr prs_arrow member_index_expr dir
		{ $$ = new PRS::rule($1, $2, $3, $4); }
	;

prs_arrow
	: RARROW 
	/* generates combinatorial inverse */
	| IMPLIES
	/* what about #> for c-element? */
	;

dir
	: '+' 
	| '-' 
	;

/* end of PRS language */

/* want prs expr to be only ~, & and | expressions */
/* for now, allow any expression and type-check for bool later */
prs_expr
	: expr
	;

/******************************************************************************
// Expressions, expressions, expressions
// mostly ripped from ANSI C++ grammar
******************************************************************************/
paren_expr
	: '(' expr ')'
		{ $$ = new paren_expr($1, $2, $3); }
	;

/***
primary_expr
	// all default actions: $$ = $1;
	: literal
// split out, so member_index_expr doesn't recur, 
// now unary_expr must accept paren_expr
//	| '(' expr ')'
	| id_expr
	;
***/

literal
	/* all default actions, all are expr subclasses */
	: INT { $$ = $1; }
	| FLOAT { $$ = $1; }
	| STRING { $$ = $1; }
	| BOOL_TRUE { $$ = $1; }
	| BOOL_FALSE { $$ = $1; }
	;

id_expr
		/* for identfiers that need to be searched upwards */
	: relative_id
		/* for specifying unambiguous type from global scope */
	| absolute_id
	;

absolute_id
	: SCOPE relative_id
		{ $$ = IS_A(id_expr*, $2)->force_absolute($1); }
	;

relative_id
	: qualified_id
	| ID
		{ $$ = new id_expr($1); }	/* wrap in id_expr */
	;

qualified_id
	: qualified_id SCOPE ID
		{ $$ = id_expr_append($1, $2, $3); }
	| ID SCOPE ID
		{ $$ = (new id_expr($1))->append($2, $3); }
	;

member_index_expr_list
	: member_index_expr_list ',' member_index_expr
		{ $$ = expr_list_append($1, $2, $3); }
	| member_index_expr { $$ = new expr_list($1); }
	;

/* this is what we want for expression arguments, without operators */
member_index_expr
/*	: primary_expr */
	: id_expr { $$ = $1; }
	/* array index */
	| member_index_expr range_list_in_brackets
		{ $$ = new index_expr($1, $2); }
	| member_index_expr '.' id_expr
		{ $$ = new member_expr($1, $2, $3); }
	/*			or just ID? */
	/* no function calls in expressions... yet */
	;

unary_expr
	: member_index_expr
	| literal { $$ = $1; }
	| paren_expr { $$ = $1; }
	/* no prefix operations, moved to assignment */
	| '-' unary_expr
		{ $$ = new prefix_expr($1, $2); }
	| '!' unary_expr
		{ $$ = new prefix_expr($1, $2); }
	| '~' unary_expr
		{ $$ = new prefix_expr($1, $2); }
	;

multiplicative_expr
	: unary_expr
	| multiplicative_expr '*' unary_expr
		{ $$ = new arith_expr($1, $2, $3); }
	| multiplicative_expr '/' unary_expr
		{ $$ = new arith_expr($1, $2, $3); }
	| multiplicative_expr '%' unary_expr
		{ $$ = new arith_expr($1, $2, $3); }
	;

additive_expr
	: multiplicative_expr
	| additive_expr '+' multiplicative_expr
		{ $$ = new arith_expr($1, $2, $3); }
	| additive_expr '-' multiplicative_expr
		{ $$ = new arith_expr($1, $2, $3); }
	;


shift_expr
	: additive_expr
	| shift_expr EXTRACT additive_expr
		{ $$ = new arith_expr($1, $2, $3); }
	| shift_expr INSERT additive_expr
		{ $$ = new arith_expr($1, $2, $3); }
	;

relational_equality_expr
	: shift_expr
	| shift_expr '<' shift_expr
		{ $$ = new relational_expr($1, $2, $3); }
	| shift_expr '>' shift_expr
		{ $$ = new relational_expr($1, $2, $3); }
	| shift_expr LE shift_expr
		{ $$ = new relational_expr($1, $2, $3); }
	| shift_expr GE shift_expr
		{ $$ = new relational_expr($1, $2, $3); }
	| shift_expr EQUAL shift_expr
		{ $$ = new relational_expr($1, $2, $3); }
	| shift_expr NOTEQUAL shift_expr
		{ $$ = new relational_expr($1, $2, $3); }
/*
// can't cascade relational_expr
//	| relational_expr GE shift_expr
*/
	;

and_expr
	: relational_equality_expr
	| and_expr '&' relational_equality_expr
		{ $$ = new logical_expr($1, $2, $3); }
	;

exclusive_or_expr
	: and_expr
	| exclusive_or_expr '^' and_expr
		{ $$ = new logical_expr($1, $2, $3); }
	;

inclusive_or_expr
	: exclusive_or_expr
	| inclusive_or_expr '|' exclusive_or_expr
		{ $$ = new logical_expr($1, $2, $3); }
	;

logical_and_expr
	: inclusive_or_expr
	| logical_and_expr LOGICAL_AND inclusive_or_expr
		{ $$ = new logical_expr($1, $2, $3); }
	;

logical_or_expr
	: logical_and_expr
	| logical_or_expr LOGICAL_OR logical_and_expr
		{ $$ = new logical_expr($1, $2, $3); }
	;

/** forget conditional expressions for now
conditional_expr
	: logical_or_expr
	| logical_or_expr '?' expr ':' conditional_expr
	;
**/

assignment_stmt
	: binary_assignment
	| unary_assignment
	;

binary_assignment
/*
//	: conditional_expr		// not supported
//	: logical_or_expr		// not supported
*/
	: member_index_expr '=' expr
		{ $$ = new assign_stmt($1, $2, $3); }
/*
//	| member_index_expr STARASSIGN expr
//	| member_index_expr DIVIDEASSIGN expr
//	| member_index_expr PLUSASSIGN expr
//	| member_index_expr MINUSASSIGN expr
//	| member_index_expr PERCENTASSIGN expr
//	| member_index_expr RIGHTSHIFTASSIGN expr
//	| member_index_expr LEFTSHIFTASSIGN expr
//	| member_index_expr ANDMASK expr
//	| member_index_expr ORMASK expr
//	| member_index_expr XORMASK expr
*/
	;

unary_assignment
	: member_index_expr PLUSPLUS
		{ $$ = new incdec_stmt($1, $2); }
	| member_index_expr MINUSMINUS
		{ $$ = new incdec_stmt($1, $2); }
	;

/*
// for simplicity, forbid the use of assignments as expressions, 
//	reserve them as statements only
*/

/* THE BASIC EXPRESSION */
expr
/*
//	: conditional_expr		// not supported
//	: assignment_stmt
*/
	: logical_or_expr
	;

/** temporarily not needed
optional_expr_in_braces
	: '{' expr '}'
	|
	;
**/

optional_member_index_expr_list_in_angles
	: member_index_expr_list_in_angles { $$ = $1; }
	| { $$ = NULL; }
	;

member_index_expr_list_in_angles
	: '<' member_index_expr_list '>'
		{ $$ = expr_list_wrap($1, $2, $3); }
	;

member_index_expr_list_in_parens
	: '(' member_index_expr_list ')'
		{ $$ = expr_list_wrap($1, $2, $3); }
	;

expr_list_in_parens
	: '(' expr_list ')'
		{ $$ = expr_list_wrap($1, $2, $3); }
	;

expr_list
	: expr_list ',' expr 
		{ $$ = expr_list_append($1, $2, $3); }
	| expr { $$ = new expr_list($1); }
	;

/* --- array declaration syntax ------------------------------------------- */

optional_range_list_in_brackets
	: range_list_in_brackets { $$ = $1; }
	| { $$ = NULL; }
	;

range_list_in_brackets
	: '[' range_list ']'
		{ $$ = range_list_wrap($1, $2, $3); }
	;

range_list
	: range_list ',' range 
		{ $$ = range_list_append($1, $2, $3); }
	| range { $$ = new range_list($1); }
	;

range
	: expr RANGE expr 
		{ $$ = new range($1, $2, $3); }
	| expr { $$ = new range($1); }
	;

/* ----end array ---------------------------------------------------------- */

%%
/**
	The goal is to keep the grammar in this "art.yy" clean, and not
	litter the various productions with error handling cases.  
	When the parser fails to match any productions, we want it
	to report what went wrong, not just give the useless "syntax error"
	message.  Fortunately, all the information about the state of the 
	parser is available, if we know where to look.  
	This function attempts to unwind the parser's value and state stacks
	and report precisely where the error occured -- WITHOUT having
	to write productions that contain the special error token.  
 */
/*	as a reminder, these are the variables in the parser
	hint: look at how they are used in the various yydebug blocks.  
	COMPATIBILITY ISSUE: the code generated by GNU bison differs
	somewhat, which causes the following code to break.  
	TO DO: resolve this, perhaps by using flags

	short* yyss;		// state stack base
	short* yyssp;		// state stack pointer
	YYSTYPE* yyvs;		// value stack base
	YYSTYPE* yyvsp;		// value stack pointer
	YYSTYPE yylval;		// the last token received
*/
// void yyerror(char* msg)		// replace with this if necessary
void yyerror(const char* msg) { 	// ancient compiler rejects
	const short* s;
	const YYSTYPE* v;
	node* resolved_node = NULL;
	// msg is going to be "syntax error" from y.tab.cc
	//	very useless in general
	cerr << "parse error: " << msg << endl;
	// we've kept track of the position of every token
	cerr << "parser stacks:" << endl << "state\tvalue" << endl;
	for (s=yyss, v=yyvs; s <= yyssp && v <= yyvsp; s++, v++) {
		// how do we know which union member?
		// need to look at the state stack, and the transition
		// from the previous state
/*** before union resolution was introduced:
		if (v && v->n) {
			v->n->what(cerr << '\t') << " " 
				<< v->n->where();
		} else {
			cerr << "\t(null) ";
		}
***/

// after union resolution was introduced
		if (v) {
			resolved_node = yy_union_resolve(*v, *(s-1), *s);
			if (resolved_node)
				resolved_node->what(cerr << '\t') << " "
					<< resolved_node->where();
			else
				cerr << "\t(null) ";
		} else {
			cerr << "\t(null) ";
		}

		cerr << endl << *s;
	}
	// sanity check
	assert(s > yyssp && v > yyvsp);
	assert(resolved_node);	// NULL check
//	cerr << "received: ";
	if (at_eof()) {
		cerr << "\t" << yyname[0];	// "end-of-file"
	} else {
/***
//	before union resolution was necessary this was used, very simple
		(yylval.n->what(cerr << "\t") << " ") << yylval.n->where();
***/
/***
	PROBLEM: now, don't know how to resolve union member 
	of the offending token, because it won't correspond to any valid state.  
	*s is unknown, and yylval is unknown
	The state-stack gives us no futher information.  
	BUT we have access to yychar, from the lexer which tells us the last
	token type returned.  
		// CERTAIN DEATH on yy_union_resolve!
		resolved_node = yy_union_resolve(yylval, *(s-1), *s);
***/
		resolved_node = yy_union_lookup(yylval, yychar);
		assert(resolved_node);
		(resolved_node->what(cerr << "\t") << " ")
			<< resolved_node->where();
	}
	cerr << endl;

	// take current state off of top of stack and 
	// print out possible points in productions
	cerr << "in state " << *yyssp << ", possible rules are:" << endl;
	{	int i;
		assert(*yyssp < yynss);
		for (i=0; i < yysss[*yyssp].n; i++) {
			cerr << yysss[*yyssp].rule[i] << endl;
		}
	}

	// list possible expected tokens based on state table
	// code ripped off from YYDEBUG parts of y.tab.c
	cerr << "acceptable tokens are: " << endl;
	{
		int yychar;
		int yyn;
		for (yychar = 0; yychar <= YYMAXTOKEN; yychar++) {
			// try all terminal tokens
			if ((yyn = yysindex[*yyssp]) && 
					(yyn += yychar) >= 0 && 
					yyn <= YYTABLESIZE && 
					yycheck[yyn] == yychar)
				cerr << '\t' << yyname[yychar]
					<< " (shift)" << endl;
			else if ((yyn = yyrindex[*yyssp]) && 
					(yyn += yychar) >= 0 && 
					yyn <= YYTABLESIZE && 
					yycheck[yyn] == yychar)
				cerr << '\t' << yyname[yychar]
					<< " (reduce)" << endl;
		}
	}
	
	// or throw exception
	exit(1);
}

