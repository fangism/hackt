/*
	"art.yy"
	AsynchRonous Tools (.art)

	to do: find out how to unwind the stack for accurate error reporting
	walk through yyssp, yyvsp... in y.tab.cc
	state stack pointer and value stack pointer?

*/

%{
#include "art_parser.h"
#include "art_parser_prs.h"
#include "art_parser_hse.h"
// #include "art_parser_chp.h"

using namespace std;
using namespace ART::parser;

extern	int yylex(void);
extern "C" {
	int yyparse(void);			// parser routine to call
	void yyerror(const char* msg);		// defined below
}

// useful typedefs are defined in art_parser.h
// macros: d = delimiter, n = node, b = begin, e = end, l = list

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
	ART::parser::node* n;
// let the various constructors perform optional dynamic type-cast checks
}

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


%token	LBRACE RBRACE LPAREN RPAREN LBRACKET RBRACKET
%token	LT GT				// angle brackets
%token	SEMICOLON COMMA COLON MEMBER 
%token	ASSIGN
%token	PLUS MINUS STAR DIVIDE PERCENT
%token	BANG QUERY
%token	TILDE AND PIPE XOR
*/
%type	<n>	'{' '}' '[' ']' '(' ')' '<' '>'
%type	<n>	',' '.' ';' ':'
%type	<n>	'=' '+' '-' '*' '/' '%'
%type	<n>	'!' '?' '~' '&' '|' '^'

/*
	the following tokens are defined below because they consist of
	2 or more characters
*/

%token	<n>	ID
%token	<n>	FLOAT
%token	<n>	INT
%token	<n>	STRING

%token	<n>	LE GE EQUAL NOTEQUAL
%token	<n>	THICKBAR SCOPE RANGE
%token	<n>	IMPLIES RARROW
%token	<n>	BEGINLOOP BEGINPROB ENDPROB
%token	<n>	DEFINEOP
%token	<n>	LOGICAL_AND LOGICAL_OR
%token	<n>	INSERT EXTRACT
%token	<n>	PLUSPLUS MINUSMINUS

// keywords
%token	<n>	NAMESPACE
%token	<n>	OPEN AS
%token	<n>	CHP_LANG HSE_LANG PRS_LANG
%token	<n>	SKIP ELSE LOG
%token	<n>	DEFINE DEFPROC DEFCHAN DEFTYPE
%token	<n>	INT_TYPE BOOL_TYPE PINT_TYPE PBOOL_TYPE
%token	<n>	SET GET SEND RECV
%token	<n>	CHANNEL
%token	<n>	BOOL_TRUE BOOL_FALSE

// non-terminals
%type	<n>	top_root body basic_item namespace_management
%type	<n>	definition def_or_proc defproc def_type_id
%type	<n>	optional_template_formal_decl_list_in_angles
%type	<n>	template_formal_decl_list template_formal_decl
%type	<n>	template_formal_id_list template_formal_id
%type	<n>	optional_port_formal_decl_list_in_parens port_formal_decl_list
%type	<n>	port_formal_decl port_formal_id_list port_formal_id
%type	<n>	type_id base_template_type
//%type	<n>	formal_id
%type	<n>	base_chan_type chan_or_port
%type	<n>	base_data_type_list_in_parens
%type	<n>	base_data_type_list base_data_type
%type	<n>	deftype defchan
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
%type	<n>	chp_body_item chp_loop chp_do_until chp_selection
%type	<n>	chp_nondet_guarded_command_list
%type	<n>	chp_unmatched_det_guarded_command_list
%type	<n>	chp_matched_det_guarded_command_list
%type	<n>	chp_guarded_command chp_else_clause chp_assignment
%type	<n>	chp_comm_list chp_comm_action chp_send chp_recv
%type	<n>	hse_body full_hse_body_item_list full_hse_body_item
%type	<n>	hse_body_item hse_loop hse_do_until hse_selection
%type	<n>	hse_guarded_command hse_else_clause
%type	<n>	hse_nondet_guarded_command_list
%type	<n>	hse_matched_det_guarded_command_list
%type	<n>	hse_unmatched_det_guarded_command_list
%type	<n>	hse_assignment
%type	<n>	prs_body single_prs prs_arrow dir prs_expr
%type	<n>	paren_expr expr
//%type	<n>	primary_expr
%type	<n>	literal id_expr qualified_id
%type	<n>	member_index_expr_list member_index_expr unary_expr
%type	<n>	multiplicative_expr additive_expr shift_expr
%type	<n>	relational_equality_expr and_expr
%type	<n>	exclusive_or_expr inclusive_or_expr
%type	<n>	logical_and_expr logical_or_expr
%type	<n>	assignment_stmt
//%type	<n>	conditional_expr optional_expr_in_braces
%type	<n>	optional_member_index_expr_list_in_angles
%type	<n>	member_index_expr_list_in_angles
%type	<n>	member_index_expr_list_in_parens
%type	<n>	expr_list_in_parens expr_list
%type	<n>	optional_range_list_in_brackets range_list_in_brackets
%type	<n>	range_list range


%start	top_root
%%
//------------------------------------------------------------------------
//	Grammar -- re-written to be LALR(1)
//------------------------------------------------------------------------

// top level syntax
top_root
	: body
	// allow empty file
	| { $$ = NULL; }
	;

body
	: body definition { $$ = root_body_append($1, NULL, $2); }
	| definition { $$ = new root_body($1); }
	| body basic_item { $$ = root_body_append($1, NULL, $2); }
	| basic_item { $$ = new root_body($1); }
	;

basic_item
// namespace_management already includes semicolon where needed
	: namespace_management
// instance_item already includes semicolon where needed
	| instance_item
	;

// namespace management
namespace_management
	// C++ style classes/namespaces require semicolon
	: NAMESPACE ID '{' top_root '}' ';'
		{ $$ = new namespace_body($1, $2, $3, $4, $5, $6); }
	// or C++ style: using namespace blah;
	| OPEN id_expr AS ID ';'
		{ $$ = new using_namespace($1, $2, $3, $4, $5); }
	| OPEN id_expr ';'
		{ $$ = new using_namespace($1, $2, $3); }
	// ever close namespace?
	;

// Process, datatype, and channel definition.
definition
	// default actions
	: defproc 
	| deftype 			// not done yet
	| defchan 			// not done yet
	;


//------------------------------------------------------------------------
//	Process
//------------------------------------------------------------------------

def_or_proc
	// default actions
	: DEFINE
	| DEFPROC
	;

defproc
	// using <> to follow C+ template parameters
	: def_or_proc def_type_id
	  optional_port_formal_decl_list_in_parens
	  '{' definition_body '}'
	{ $$ = new process_def($1, $2, $3, 
		definition_body_wrap($4, $5, $6));
	}
	;

optional_port_formal_decl_list_in_parens
	// note: the parens are NOT optional!
	: '(' port_formal_decl_list ')'
		{ $$ = port_formal_decl_list_wrap($1, $2, $3); }
	| '(' ')'
		{ $$ = (new port_formal_decl_list(NULL))->wrap($1, $2); }
	;

def_type_id
	: ID optional_template_formal_decl_list_in_angles
		{ $$ = new def_type_id($1, $2); }
	;

// Meta (template) language parameters

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
	// changing to C-style formal parameters, allowing comma-lists
	// is there any need for user-defined types in template argument?
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

// port parameters
port_formal_decl_list
	// would rather use ','-delimiter, but wth...
	: port_formal_decl_list ';' port_formal_decl
		{ $$ = port_formal_decl_list_append($1, $2, $3); }
	| port_formal_decl
		{ $$ = new port_formal_decl_list($1); }
	;

port_formal_decl
	// must switch to C-style formals, eliminate id_list
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
		// for userdef or chan type, and templating
		{ $$ = new type_id($1, $2); }
	| base_chan_type
	| base_data_type
	;

//------------------------------------------------------------------------
//	base types
//------------------------------------------------------------------------

// template type
base_template_type
	// default actions
	: PINT_TYPE 		// integer parameter
	| PBOOL_TYPE		// boolean parameter
	;

// channel type: channel, inport, outport, and data types
base_chan_type
	// eliminate defaulting? (to int?), use <template> style?
	: chan_or_port base_data_type_list_in_parens
		{ $$ = chan_type_attach_data_types($1, $2); }
	;

chan_or_port
	: CHANNEL		// a channel
		{ $$ = new chan_type($1); }
	| CHANNEL '!'		// an output port
		{ $$ = new chan_type($1, $2); }
	| CHANNEL '?'		// an input port
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

// actual data: int<width> or bool
base_data_type
	// ever need user-defined types?
	// optional parens get confused with template-parameters
	// going to use angle brackets <> in the template-fashion
	: INT_TYPE '<' INT '>'
		{ $$ = new data_type_base($1, $2, $3, $4); }
	| INT_TYPE
		{ $$ = new data_type_base($1); }
	| BOOL_TYPE
		{ $$ = new data_type_base($1); }
	;

// TO DO
// definition types
deftype
	: DEFTYPE ID DEFINEOP base_data_type 
          data_param_list_in_parens
	  '{'
		SET '{' chp_body '}'
		GET '{' chp_body '}'
	  '}'
	;

defchan
       : DEFCHAN ID DEFINEOP base_chan_type 
         data_param_list_in_parens
	 '{'
		SEND '{' chp_body '}'
		RECV '{' chp_body '}'
	 '}'
	;

data_param_list_in_parens
	: '(' data_param_list ')'
		{ $$ = data_param_list_wrap($1, $2, $3); }
	;

data_param_list
	// like declarations in formals list
	// consider using ':', similar to C-style...
	: data_param_list ';' data_param
		{ $$ = data_param_list_append($1, $2, $3); }
	| data_param
		{ $$ = new data_param_list($1); }
	;

data_param
	// forseen problem: array brackets are with data_type
	// but to follow C-style, we want the arrays to go with identifiers
	// thinking of forbidding list, restricting to single
	// semicolon-delimited declarations
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


//--- definition_body ---

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

// considering splitting declarations from connection, e.g.
//		// declare first
// myprocesstype<template-params> foo[N,M];		// may be ranges as well
// foo[i,j](port-actuals);	// inside some for-loop, presumably
//		// then sparse instantiation connects and assigns
//
// myprocesstype bar(port-actuals);		// for single instance and decl
// myprocesstype<X,Y> foo;			// declare without connection
// foo(port-actuals);				// then connect

instance_item
	: instance_declaration			// declaration: single or array
	| instance_connection			// connection of ports
	| instance_alias			// aliasing connection
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
	// type template is included in type_id, and is part of the type
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
	// array declaration: forbid connection, must connect later
	: ID range_list_in_brackets
		{ $$ = new declaration_array($1, $2); }
	// single instance declaration without connection
	| ID
		{ $$ = new declaration_base($1); }
	// single instance declaration with connection
	| ID connection_actuals_list
		{ $$ = new actuals_connection($1, $2); }
	// alias or assignment (not just member_id expression?)
	| ID '=' expr
		{ $$ = new alias_assign($1, $2, $3); }
	;

instance_connection
	// taking a declared array or single instance and connecting ports
	// are brackets part of the array/membership chain?
	: member_index_expr connection_actuals_list ';'
		// can this first id be scoped and/or membered?
		{ $$ = new actuals_connection($1, $2, $3); }
	;

instance_alias
	// aliasing syntax, or data types is value assignment (general expr?)
	// type check this, of course
//	: member_index_expr '=' member_index_expr ';'
	: member_index_expr '=' expr ';'
		{ $$ = new alias_assign($1, $2, $3, $4); }
	;

// this rule is sort of redundant, oh well...
connection_actuals_list
	: member_index_expr_list_in_parens
	;

guarded_definition_body_list
	: guarded_definition_body_list THICKBAR guarded_definition_body
		{ $$ = guarded_definition_body_list_append($1, $2, $3); }
	| guarded_definition_body
		{ $$ = new guarded_definition_body_list($1); }
	;

// any else clause?

guarded_definition_body
	: expr RARROW definition_body
		{ $$ = new guarded_definition_body($1, $2, $3); }
	;


//------------------------------------------------------------------------
//	Supported Languages
//------------------------------------------------------------------------

language_body
	: CHP_LANG '{' chp_body '}'
//		{ $$ = new CHP::body($1, chp_stmt_list_wrap($2, $3, $4)); }
	| HSE_LANG '{' hse_body '}'
		{ $$ = new HSE::body($1, hse_stmt_list_wrap($2, $3, $4)); }
	| PRS_LANG '{' prs_body '}'
		{ $$ = new PRS::body($1, prs_rule_list_wrap($2, $3, $4)); }
//	| STACK_LANG '{' stack_body '}'
//		{ $$ = new stack::body($1, stack_rule_list_wrap($2, $3, $4)); }
//	and more...
	;

//--- Language: CHP ---

chp_body
	: full_chp_body_item_list
	;

full_chp_body_item_list
	: full_chp_body_item_list ';' full_chp_body_item
	| full_chp_body_item
	;

// make _this_ string together the pieces, rather than having the item
// add itself to a global list. If we do that, then everything will be
// properly scoped and the walk stack will store the correct state...

full_chp_body_item
	// expr_in_braces are assertions
//	: optional_expr_in_braces chp_body_item optional_expr_in_braces
	// temporarily simplify
	: chp_body_item
	;

// statement
chp_body_item
	: chp_loop
	| chp_do_until
	| chp_selection
	| chp_assignment
	| chp_comm_list
	| SKIP
	| LOG expr_list_in_parens
	;

chp_loop
	// do-forever loop
	: BEGINLOOP chp_body ']'
	;

chp_do_until
	// do-until-all-guards-false
	: BEGINLOOP chp_matched_det_guarded_command_list ']'
	;

chp_selection
	// wait for expr to become true
	: '[' expr ']'
	| '[' chp_matched_det_guarded_command_list ']'
	| '[' chp_nondet_guarded_command_list ']'

	// wtf is this?... probalistic selection for FT
//	| "%[" { chp_guarded_command ":" }** "]%"
	| BEGINPROB chp_nondet_guarded_command_list ENDPROB
	;

// note: these lists must have at least 2 clauses, will have to fix with "else"
chp_nondet_guarded_command_list
	: chp_nondet_guarded_command_list ':' chp_guarded_command
	| chp_guarded_command ':' chp_guarded_command
	// can't have else clause in non-deterministic selection?
	;

chp_matched_det_guarded_command_list
	: chp_unmatched_det_guarded_command_list THICKBAR chp_else_clause
	| chp_unmatched_det_guarded_command_list
	;

chp_unmatched_det_guarded_command_list
	: chp_unmatched_det_guarded_command_list THICKBAR chp_guarded_command
	| chp_guarded_command
	;

chp_guarded_command
	: expr RARROW chp_body
	;

chp_else_clause
	: ELSE RARROW chp_body
	;

// consider replacing with c-style statements and type-checking for chp
// if top-of-language-stack == chp, forbid x-type of statement/expression
chp_assignment
	: assignment_stmt
	;

chp_comm_list
	// gives comma-separated communications precedence
	: chp_comm_list ',' chp_comm_action
	| chp_comm_action
	;

chp_comm_action
	: chp_send 
	| chp_recv
	;

chp_send
	// for now, require parens like function-call to
	// disambiguate between ( expr ) and ( expr_list )
	: member_index_expr '!' expr_list_in_parens
//	| member_index_expr '!' expr
	;

chp_recv
	: member_index_expr '?' member_index_expr_list_in_parens
//	| member_index_expr '?' member_index_expr
	;

//--- Language: HSE ---

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
	// temporary removal of assertions
//	: optional_expr_in_braces hse_body_item optional_expr_in_braces
	: hse_body_item
	;

hse_body_item
	// returns an HSE::statement
	: hse_loop
	| hse_do_until
	| hse_selection
	| hse_assignment
	| SKIP { $$ = new HSE::skip($1); }
	;

hse_loop
	: BEGINLOOP hse_body ']'
		{ $$ = new HSE::loop(hse_stmt_list_wrap($1, $2, $3)); }
	;

hse_do_until
	// keep entering loop until all guards false
	: BEGINLOOP hse_matched_det_guarded_command_list ']'
		{ $$ = new HSE::do_until(hse_det_selection_wrap($1, $2, $3)); }
	;

hse_selection
	: '[' expr ']'
		{ $$ = new HSE::wait($1, $2, $3); }
	| '[' hse_matched_det_guarded_command_list ']'
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

// this is wrong, else clause may appear at END only... fix me
hse_unmatched_det_guarded_command_list
	: hse_unmatched_det_guarded_command_list THICKBAR hse_guarded_command
		{ $$ = hse_det_selection_append($1, $2, $3); }
	| hse_guarded_command
		{ $$ = new HSE::det_selection($1); }
	;

hse_assignment
	: assignment_stmt
		{ $$ = new HSE::assignment(
			dynamic_cast<ART::parser::incdec_stmt*>($1)); }
	;

//--- Language: PRS ---
// to do: add support for overriding default connection to Vdd, GND
// for power/ground isolation, and other tricks, pass gating... <-> <+> <=>

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
	// generates combinatorial inverse
	| IMPLIES
	// what about #> for c-element?
	;

dir
	: '+' 
	| '-' 
	;

// end of PRS language

// want prs expr to be only ~, & and | expressions
prs_expr
	// for now, allow any expression and type-check for bool later
	: expr
	;

// eventually combine expressions together and type-check later

// need to define expr
// Expressions, expressions, expressions
// mostly ripped from ANSI C++ grammar

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
	// all default actions, all are expr subclasses
	: INT
	| FLOAT
	| STRING
	| BOOL_TRUE
	| BOOL_FALSE
	;

id_expr
	: ID
		{ $$ = new id_expr($1); }	// wrap in id_expr
	| qualified_id
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

// this is what we want for expression arguments, without operators
member_index_expr
//	: primary_expr
	: id_expr
	// array index
	| member_index_expr range_list_in_brackets
		{ $$ = new index_expr($1, $2); }
	| member_index_expr '.' id_expr
		{ $$ = new member_expr($1, $2, $3); }
	// no function calls in expressions... yet
	;

unary_expr
	: member_index_expr
	| literal
	| paren_expr
	// no prefix operations, moved to assignment
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
// can't cascade relational_expr
//	| relational_expr GE shift_expr
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
//	: conditional_expr		// not supported
//	: logical_or_expr		// not supported
	: member_index_expr '=' expr
		{ $$ = new assign_stmt($1, $2, $3); }
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
	| member_index_expr PLUSPLUS
		{ $$ = new incdec_stmt($1, $2); }
	| member_index_expr MINUSMINUS
		{ $$ = new incdec_stmt($1, $2); }
	;

// for simplicity, forbid the use of assignments as expressions, 
//	reserve them as statements only

// THE BASIC EXPRESSION
expr
//	: conditional_expr		// not supported
//	: assignment_stmt
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

//--- array declaration syntax -------------------------------------------

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

//----end array ----------------------------------------------------------

%%
// user code, if any

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
	short* yyss;		// state stack base
	short* yyssp;		// state stack pointer
	YYSTYPE* yyvs;		// value stack base
	YYSTYPE* yyvsp;		// value stack pointer
	YYSTYPE yylval;		// the last token received
*/
void yyerror(const char* msg) {
	short* s;
	YYSTYPE* v;
	// msg is going to be "syntax error" from y.tab.cc
	cerr << "parse error: " << msg << endl;
	// we've kept track of the position of every token
	cerr << "yy-state-stack:";
	for (s=yyss; s <= yyssp; s++)
		cerr << ' ' << *s;
	cerr << endl;

	cerr << "yy-value-stack:" << endl;
	// or just show the top N items on stack...
	for (v=yyvs; v <= yyvsp; v++) {
		if (v && v->n) {
			v->n->what(cerr << '\t') << " " 
				<< v->n->where() << endl;
		} else {
			cerr << "\t(null) " << endl;
		}
	}
	assert(yylval.n);	// NULL check
	yylval.n->what(cerr << "received: ") << " "
		<< yylval.n->where() << endl;

	// to do: list possible expected tokens based on state table
	
	// or throw exception
	exit(1);
}

