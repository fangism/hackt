/*
	"art.yy"
	AsynchRonous Tools (.art)

	to do: find out how to unwind the stack for accurate error reporting
	walk through yyssp, yyvsp... in y.tab.cc
	state stack pointer and value stack pointer?

*/

%{
#include "art_parser.h"

using namespace std;
using namespace ART::parser;

extern	int yylex(void);
extern "C" {
	int yyparse(void);
	void yyerror(const char* msg) {
		cerr << "parse error: " << msg << endl;
	/* line and col information? */
		exit(1);
	}
}


%}

%union {
// lexer members
	long long_val;
	double double_val;
	string* text;
// parser members
}


%token	ID
%token	FLOAT
%token	INT
%token	STRING
%token	LBRACE RBRACE LPAREN RPAREN LBRACKET RBRACKET
%token	LT GT				// angle brackets
%token	LE GE EQUAL NOTEQUAL
%token	SEMICOLON COMMA COLON THICKBAR MEMBER SCOPE
%token	ASSIGN COLONASSIGN RANGE
%token	IMPLIES RARROW
%token	BEGINLOOP BEGINPROB ENDPROB
%token	PLUS MINUS STAR DIVIDE PERCENT
%token	BANG QUERY
%token	DEFINEOP
%token	TILDE AND PIPE LOGICAL_AND LOGICAL_OR XOR
%token	INSERT EXTRACT

// keywords
%token	NAMESPACE
%token	OPEN AS
%token	CHP HSE PRS
%token	SKIP ELSE LOG
%token	DEFINE DEFPROC DEFCHAN DEFTYPE
%token	INT_TYPE BOOL_TYPE PINT_TYPE PBOOL_TYPE
%token	SET GET SEND RECV
%token	CHANNEL
%token	TRUE FALSE

%start	body
%%
//------------------------------------------------------------------------
//	Grammar -- re-written to be LALR(1)
//------------------------------------------------------------------------

// top level syntax
body
	: body definition
	| definition
	| body SEMICOLON basic_item
	| basic_item
	;

basic_item
	: namespace_management
	| instance_item
	;

// namespace management
namespace_management
	: NAMESPACE ID LBRACE body RBRACE
	| NAMESPACE ID 
	| OPEN id_expr AS ID
	| OPEN id_expr 
	// ever close namespace?
	;

// Process, datatype, and channel definition.
definition
	: defproc 
	| deftype 
	| defchan 
	;


//------------------------------------------------------------------------
//	Process
//------------------------------------------------------------------------

def_or_proc
	: DEFINE
	| DEFPROC
	;

defproc
	// using <> to follow C+ template parameters
	: def_or_proc ID
	  optional_template_param_list_in_angles
	  LPAREN optional_port_formal_list RPAREN
	  LBRACE definition_body RBRACE
	;


// Meta (template) language parameters

optional_template_param_list_in_angles
	: LT template_param_list GT
	|
	;

template_param_list
	: template_param_list SEMICOLON template_param
	| template_param
	;

template_param
	// changing to C-style formal parameters, allowing comma-lists
	// is there any need for user-defined types in template argument?
	: base_template_type template_formal_id_list
	;

template_formal_id_list
	: template_formal_id_list COMMA template_formal_id
	| template_formal_id
	;

template_formal_id
	: formal_id optional_range_list_in_brackets
	;

optional_port_formal_list
	: port_formal_list
	|
	;

// port parameters
port_formal_list
	// would rather use COMMA-delimiter, but wth...
	: port_formal_list SEMICOLON port_formal
	| port_formal
	;

port_formal
	// must switch to C-style formals, eliminate id_list
	: type_id formal_id port_formal_id_list
	;

port_formal_id_list
	: port_formal_id_list COMMA port_formal_id
	| port_formal_id
	;

port_formal_id
	: formal_id optional_range_list_in_brackets
	;

type_id
	: id_expr optional_postfix_expr_list_in_angles
		// for userdef or chan type, and templating
	| base_chan_type
	| base_data_type
	;

formal_id
	: ID
	;

//------------------------------------------------------------------------
//	base types
//------------------------------------------------------------------------
// need to distinguish formals/declarations from instantiations w/o context

// template type
base_template_type
	: PINT_TYPE  // integer parameter
	| PBOOL_TYPE // Boolean parameter
	;

// channel type: channel, inport, outport
base_chan_type
	: chan_or_port LPAREN base_data_type_list RPAREN
	// eliminate defaulting? (to int?)
//	| chan_or_port
	;

chan_or_port
	: CHANNEL		// a channel
	| CHANNEL BANG		// an output port
	| CHANNEL QUERY		// an input port
	;

base_data_type_list
	: base_data_type_list COMMA base_data_type
	| base_data_type
	;

// actual data: int<width> or bool
base_data_type
	// ever need user-defined types?
	// optional parens get confused with template-parameters
	// going to use angle brackets <> in the template-fashion
	: INT_TYPE postfix_expr_list_in_angles
		// really, only one expr (INT) allowed inside
	| INT_TYPE
	| BOOL_TYPE
	;

// definition types
deftype
	: DEFTYPE ID DEFINEOP base_data_type 
	  LPAREN data_param_list RPAREN
	  LBRACE
		SET LBRACE chp_body RBRACE
		GET LBRACE chp_body RBRACE
	  RBRACE
	;

defchan
       : DEFCHAN ID DEFINEOP base_chan_type 
         LPAREN data_param_list RPAREN
	 LBRACE
		SEND LBRACE chp_body RBRACE
		RECV LBRACE chp_body RBRACE
	 RBRACE
	;

data_param_list
	// like declarations in formals list
	// consider using COLON, similar to C-style...
	: data_param_list SEMICOLON data_param
	| data_param
	;

data_param
	// forseen problem: array brackets are with data_type
	// but to follow C-style, we want the arrays to go with identifiers
	// thinking of forbidding list, restricting to single
	// semicolon-delimited declarations
	: base_data_type ID optional_range_list_in_brackets
	;


//--- definition_body ---

definition_body
	: instance_item definition_body
	| instance_item
	| language_body definition_body
	| language_body
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
	: instance_declaration SEMICOLON	// declaration: single or array
	| instance_connection SEMICOLON		// single connection
	// loop instantiation
	| LPAREN SEMICOLON ID COLON range COLON definition_body RPAREN
	// conditional instantiation
	| LBRACKET guarded_definition_body_list RBRACKET
	;

instance_declaration
	// type template is included in type_id, and is part of the type
	: type_id declaration_id_list
	;

declaration_id_list
	: declaration_id_list COMMA declaration_id_item
	| declaration_id_item
	;

declaration_id_item
	// array declaration: forbid connection, must connect later
	: ID range_list_in_brackets
	// single instance declaration without connection
	| ID
	// single instance declaration with connection
	| ID connection_actuals_list
	;

instance_connection
	// taking a declared array or single instance and connecting ports
	// are brackets part of the array/membership chain?
	: postfix_expr connection_actuals_list
		// can this first id be scoped and/or membered?
	| postfix_expr ASSIGN postfix_expr
	;

connection_actuals_list
	: postfix_expr_list_in_parens
	;

guarded_definition_body_list
	: guarded_definition_body_list THICKBAR guarded_definition_body
	| guarded_definition_body
	;

guarded_definition_body
	: expr RARROW definition_body
	;


//------------------------------------------------------------------------
//	Supported Languages
//------------------------------------------------------------------------

language_body
	: CHP LBRACE chp_body RBRACE
	| HSE LBRACE hse_body RBRACE
	| PRS LBRACE prs_body RBRACE
//	| STACK LBRACE stack_body RBRACE
//	and more...
	;

//--- Language: CHP ---

chp_body
	: full_chp_body_item_list
	;

full_chp_body_item_list
	: full_chp_body_item_list SEMICOLON full_chp_body_item
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
	| chp_selection
	| chp_assignment
	| chp_comm_list
	| SKIP
	| LOG expr_list_in_parens
	;

chp_loop
	// do-forever loop
	: BEGINLOOP chp_body RBRACKET
	// do-until-all-guards-false
	| BEGINLOOP chp_det_guarded_command_list RBRACKET
	;

chp_selection
	// wait for expr to become true
	: LBRACKET expr RBRACKET
	| LBRACKET chp_det_guarded_command_list RBRACKET
	| LBRACKET chp_nondet_guarded_command_list RBRACKET

	// wtf is this?... probalistic selection for FT
//	| "%[" { chp_guarded_command ":" }** "]%"
	| BEGINPROB chp_nondet_guarded_command_list ENDPROB
	;

// note: these lists must have at least 2 clauses, will have to fix with "else"
chp_nondet_guarded_command_list
	: chp_nondet_guarded_command_list COLON chp_guarded_command
	| chp_guarded_command COLON chp_guarded_command
	// can't have else clause in non-deterministic selection?
	;

chp_det_guarded_command_list
	: chp_det_guarded_command_list THICKBAR chp_guarded_command
	| chp_guarded_command THICKBAR chp_guarded_command
	// matched case statement (with else)
	| chp_guarded_command THICKBAR chp_else_clause
	;

chp_guarded_command
	: expr RARROW chp_body
	;

chp_else_clause
	: THICKBAR ELSE RARROW chp_body
	;

// consider replacing with c-style statements and type-checking for chp
// if top-of-language-stack == chp, forbid x-type of statement/expression
chp_assignment
//	: postfix_expr PLUS
//	| postfix_expr MINUS
//	| postfix_expr COLONASSIGN expr
	: assignment_expr
	;

chp_comm_list
	: chp_comm_list COMMA chp_comm_action
	| chp_comm_action
	;

chp_comm_action
	: chp_send 
	| chp_recv
	;

chp_send
	// for now, require parens like function-call to
	// disambiguate between ( expr ) and ( expr_list )
	: postfix_expr BANG expr_list_in_parens
//	| postfix_expr BANG expr
	;

chp_recv
	: postfix_expr QUERY postfix_expr_list_in_parens
//	| postfix_expr QUERY postfix_expr
	;

//--- Language: HSE ---

hse_body
	: full_hse_body_item_list
	;

full_hse_body_item_list
	: full_hse_body_item_list SEMICOLON full_hse_body_item
	| full_hse_body_item
	;

full_hse_body_item
	// temporary removal
//	: optional_expr_in_braces hse_body_item optional_expr_in_braces
	: hse_body_item
	;

hse_body_item
	: hse_loop
	| hse_selection
	| hse_assignment
	| SKIP
	;

hse_loop
	: BEGINLOOP hse_body RBRACKET
	| BEGINLOOP hse_det_guarded_command_list RBRACKET
	;

hse_selection
	: LBRACKET expr RBRACKET
	| LBRACKET hse_det_guarded_command_list RBRACKET
	| LBRACKET hse_nondet_guarded_command_list RBRACKET
	;

hse_guarded_command
	: expr RARROW hse_body 
	;

hse_else_clause
	: THICKBAR ELSE RARROW hse_body
	;

hse_nondet_guarded_command_list
	: hse_nondet_guarded_command_list COLON hse_guarded_command
	| hse_guarded_command COLON hse_guarded_command
	;

hse_det_guarded_command_list
	: hse_det_guarded_command_list THICKBAR hse_guarded_command
	| hse_guarded_command THICKBAR hse_guarded_command
	| hse_guarded_command THICKBAR hse_else_clause
	;

hse_assignment
//	: postfix_expr PLUS
//	| postfix_expr MINUS
	: assignment_expr
	;

//--- Language: PRS ---
// to do: add support for overriding default connection to Vdd, GND
// for power/ground isolation, and other tricks

prs_body
	: prs_body single_prs 
	| single_prs
	;

single_prs
	: prs_expr prs_arrow postfix_expr dir
	;

prs_arrow
	// single pr
	: RARROW 
	// generates combinatorial inverse
	| IMPLIES
	// what about #> for c-element?
	;

dir
	: PLUS 
	| MINUS 
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
	: LPAREN expr RPAREN
	;

primary_expr
	: literal
// split out, so postfix_expr doesn't recur, 
// now unary_expr must accept paren_expr
//	| LPAREN expr RPAREN		
	| id_expr
	;

literal
	: INT
	| STRING
	| TRUE
	| FALSE
	;

id_expr
	: ID
	| qualified_id
	;

qualified_id
	: qualified_id SCOPE ID
	| ID SCOPE ID
	;

postfix_expr_list
	: postfix_expr_list COMMA postfix_expr
	| postfix_expr
	;

// this is what we want for expression arguments, without operators
postfix_expr
	: primary_expr
	// array index
	// | postfix_expr LBRACKET expr RBRACKET
	| postfix_expr range_list_in_brackets
	// member (just ID?)
	| postfix_expr MEMBER id_expr
	// no function calls in expressions... yet
	;

// this rule makes problems because of expression cycles
// look for any possible way for expr_list to conflict with expr
unary_expr
	: postfix_expr
	| paren_expr
	// no prefix operations, moved to assignment
	| MINUS unary_expr
	| BANG unary_expr
	| TILDE unary_expr
	;

multiplicative_expr
	: unary_expr
	| multiplicative_expr STAR unary_expr
	| multiplicative_expr DIVIDE unary_expr
	| multiplicative_expr PERCENT unary_expr
	;

additive_expr
	: multiplicative_expr
	| additive_expr PLUS multiplicative_expr
	| additive_expr MINUS multiplicative_expr
	;


shift_expr
	: additive_expr
	| shift_expr EXTRACT additive_expr
	| shift_expr INSERT additive_expr
	;

relational_equality_expr
	: shift_expr
	| shift_expr LT shift_expr
	| shift_expr GT shift_expr		// this makes s/r-conflict?
	| shift_expr LE shift_expr
	| shift_expr GE shift_expr
	| shift_expr EQUAL shift_expr
	| shift_expr NOTEQUAL shift_expr
// can't cascade relational_expr
//	| relational_expr GE shift_expr
	;

and_expr
	: relational_equality_expr
	| and_expr AND relational_equality_expr
	;

exclusive_or_expr
	: and_expr
	| exclusive_or_expr XOR and_expr
	;

inclusive_or_expr
	: exclusive_or_expr
	| inclusive_or_expr PIPE exclusive_or_expr
	;

logical_and_expr
	: inclusive_or_expr
	| logical_and_expr LOGICAL_AND inclusive_or_expr
	;

logical_or_expr
	: logical_and_expr
	| logical_or_expr LOGICAL_OR logical_and_expr
	;

/** forget conditional expressions for now
conditional_expr
	: logical_or_expr
	| logical_or_expr QUERY expr COLON conditional_expr
	;
**/

assignment_expr
//	: conditional_expr
	: logical_or_expr
//	| logical_or_expr ASSIGN assignment_expr
	| postfix_expr ASSIGN assignment_expr
	;

// forbid using assignment_expr?
//	assignment_operator: one of
//		=  *=  /=  %=   +=  -=  >>=  <<=  &=  ^=  |=  ++  --

// THE EXPRESSION
expr
//	: conditional_expr
	: assignment_expr
	;

/** temporarily not needed
optional_expr_in_braces
	: LBRACE expr RBRACE
	|
	;
**/

optional_postfix_expr_list_in_angles
	: postfix_expr_list_in_angles
	|
	;

postfix_expr_list_in_angles
	: LT postfix_expr_list GT
	;

postfix_expr_list_in_parens
	: LPAREN postfix_expr_list RPAREN
	;

expr_list_in_parens
	: LPAREN expr_list RPAREN
	;

expr_list
	: expr_list COMMA expr
	| expr
	;

//--- array declaration syntax -------------------------------------------

optional_range_list_in_brackets
	: range_list_in_brackets
	|
	;

range_list_in_brackets
	: LBRACKET range_list RBRACKET
	;

range_list
	: range_list COMMA range
	| range
	;

range
	: expr RANGE expr
	| expr
	;

//----end array ----------------------------------------------------------

%%
// user code, if any

