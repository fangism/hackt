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
	int yyparse(void);			// parser routine to call
	void yyerror(const char* msg);		// defined below

}



%}

%union {
/// use this universal symbol type for both lexer and parser
	node* n;
// let the various constructors perform static type-cast checks
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
	][(){}<>*%/=:;|!?~&^.+-

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
%token	PLUS MINUS STAR DIVIDE
%token	BANG QUERY
%token	TILDE AND PIPE XOR

	the following tokens are defined below because they consist of
	2 or more characters
*/

%token	ID
%token	FLOAT
%token	INT
%token	STRING

%token	LE GE EQUAL NOTEQUAL
%token	THICKBAR SCOPE RANGE
%token	IMPLIES RARROW
%token	BEGINLOOP BEGINPROB ENDPROB
%token	DEFINEOP
%token	LOGICAL_AND LOGICAL_OR
%token	INSERT EXTRACT
%token	PLUSPLUS MINUSMINUS

// keywords
%token	NAMESPACE
%token	OPEN AS
%token	CHP HSE PRS
%token	SKIP ELSE LOG
%token	DEFINE DEFPROC DEFCHAN DEFTYPE
%token	INT_TYPE BOOL_TYPE PINT_TYPE PBOOL_TYPE
%token	SET GET SEND RECV
%token	CHANNEL
%token	BOOL_TRUE BOOL_FALSE

%start	top_root
%%
//------------------------------------------------------------------------
//	Grammar -- re-written to be LALR(1)
//------------------------------------------------------------------------

// top level syntax
top_root
	: body
	// allow empty file
	|
	;

body
	: body definition
	| definition
	| body ';' basic_item
	| basic_item
	;

basic_item
	: namespace_management
	| instance_item
	;

// namespace management
namespace_management
	: NAMESPACE ID '{' body '}'
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
	  '(' optional_port_formal_list ')'
	  '{' definition_body '}'
	;


// Meta (template) language parameters

optional_template_param_list_in_angles
	: '<' template_param_list '>'
	|
	;

template_param_list
	: template_param_list ';' template_param
	| template_param
	;

template_param
	// changing to C-style formal parameters, allowing comma-lists
	// is there any need for user-defined types in template argument?
	: base_template_type template_formal_id_list
	;

template_formal_id_list
	: template_formal_id_list ',' template_formal_id
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
	// would rather use ','-delimiter, but wth...
	: port_formal_list ';' port_formal
	| port_formal
	;

port_formal
	// must switch to C-style formals, eliminate id_list
	: type_id formal_id port_formal_id_list
	;

port_formal_id_list
	: port_formal_id_list ',' port_formal_id
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
	: chan_or_port '(' base_data_type_list ')'
	// eliminate defaulting? (to int?)
//	| chan_or_port
	;

chan_or_port
	: CHANNEL		// a channel
	| CHANNEL '!'		// an output port
	| CHANNEL '?'		// an input port
	;

base_data_type_list
	: base_data_type_list ',' base_data_type
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
	  '(' data_param_list ')'
	  '{'
		SET '{' chp_body '}'
		GET '{' chp_body '}'
	  '}'
	;

defchan
       : DEFCHAN ID DEFINEOP base_chan_type 
         '(' data_param_list ')'
	 '{'
		SEND '{' chp_body '}'
		RECV '{' chp_body '}'
	 '}'
	;

data_param_list
	// like declarations in formals list
	// consider using ':', similar to C-style...
	: data_param_list ';' data_param
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
	: instance_declaration ';'	// declaration: single or array
	| instance_connection ';'		// single connection
	// loop instantiation
	| '(' ';' ID ':' range ':' definition_body ')'
	// conditional instantiation
	| '[' guarded_definition_body_list ']'
	;

instance_declaration
	// type template is included in type_id, and is part of the type
	: type_id declaration_id_list
	;

declaration_id_list
	: declaration_id_list ',' declaration_id_item
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
	| postfix_expr '=' postfix_expr
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
	: CHP '{' chp_body '}'
	| HSE '{' hse_body '}'
	| PRS '{' prs_body '}'
//	| STACK '{' stack_body '}'
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
	| chp_selection
	| chp_assignment
	| chp_comm_list
	| SKIP
	| LOG expr_list_in_parens
	;

chp_loop
	// do-forever loop
	: BEGINLOOP chp_body ']'
	// do-until-all-guards-false
	| BEGINLOOP chp_det_guarded_command_list ']'
	;

chp_selection
	// wait for expr to become true
	: '[' expr ']'
	| '[' chp_det_guarded_command_list ']'
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
//	: postfix_expr '+'
//	| postfix_expr '-'
//	| postfix_expr COLONASSIGN expr
	: assignment_expr
	;

chp_comm_list
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
	: postfix_expr '!' expr_list_in_parens
//	| postfix_expr '!' expr
	;

chp_recv
	: postfix_expr '?' postfix_expr_list_in_parens
//	| postfix_expr '?' postfix_expr
	;

//--- Language: HSE ---

hse_body
	: full_hse_body_item_list
	;

full_hse_body_item_list
	: full_hse_body_item_list ';' full_hse_body_item
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
	: BEGINLOOP hse_body ']'
	| BEGINLOOP hse_det_guarded_command_list ']'
	;

hse_selection
	: '[' expr ']'
	| '[' hse_det_guarded_command_list ']'
	| '[' hse_nondet_guarded_command_list ']'
	;

hse_guarded_command
	: expr RARROW hse_body 
	;

hse_else_clause
	: THICKBAR ELSE RARROW hse_body
	;

hse_nondet_guarded_command_list
	: hse_nondet_guarded_command_list ':' hse_guarded_command
	| hse_guarded_command ':' hse_guarded_command
	;

hse_det_guarded_command_list
	: hse_det_guarded_command_list THICKBAR hse_guarded_command
	| hse_guarded_command THICKBAR hse_guarded_command
	| hse_guarded_command THICKBAR hse_else_clause
	;

hse_assignment
//	: postfix_expr '+'
//	| postfix_expr '-'
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
	;

primary_expr
	: literal
// split out, so postfix_expr doesn't recur, 
// now unary_expr must accept paren_expr
//	| '(' expr ')'		
	| id_expr
	;

literal
	: INT
	| STRING
	| BOOL_TRUE
	| BOOL_FALSE
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
	: postfix_expr_list ',' postfix_expr
	| postfix_expr
	;

// this is what we want for expression arguments, without operators
postfix_expr
	: primary_expr
	// array index
	// | postfix_expr '[' expr ']'
	| postfix_expr range_list_in_brackets
	// member (just ID?)
	| postfix_expr '.' id_expr
	// no function calls in expressions... yet
	;

// this rule makes problems because of expression cycles
// look for any possible way for expr_list to conflict with expr
unary_expr
	: postfix_expr
	| paren_expr
	// no prefix operations, moved to assignment
	| '-' unary_expr
	| '!' unary_expr
	| '~' unary_expr
	;

multiplicative_expr
	: unary_expr
	| multiplicative_expr '*' unary_expr
	| multiplicative_expr '/' unary_expr
	| multiplicative_expr '%' unary_expr
	;

additive_expr
	: multiplicative_expr
	| additive_expr '+' multiplicative_expr
	| additive_expr '-' multiplicative_expr
	;


shift_expr
	: additive_expr
	| shift_expr EXTRACT additive_expr
	| shift_expr INSERT additive_expr
	;

relational_equality_expr
	: shift_expr
	| shift_expr '<' shift_expr
	| shift_expr '>' shift_expr		// this makes s/r-conflict?
	| shift_expr LE shift_expr
	| shift_expr GE shift_expr
	| shift_expr EQUAL shift_expr
	| shift_expr NOTEQUAL shift_expr
// can't cascade relational_expr
//	| relational_expr GE shift_expr
	;

and_expr
	: relational_equality_expr
	| and_expr '&' relational_equality_expr
	;

exclusive_or_expr
	: and_expr
	| exclusive_or_expr '^' and_expr
	;

inclusive_or_expr
	: exclusive_or_expr
	| inclusive_or_expr '|' exclusive_or_expr
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
	| logical_or_expr '?' expr ':' conditional_expr
	;
**/

assignment_expr
//	: conditional_expr
	: logical_or_expr
//	| logical_or_expr '=' assignment_expr
	| postfix_expr '=' assignment_expr
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
	: '{' expr '}'
	|
	;
**/

optional_postfix_expr_list_in_angles
	: postfix_expr_list_in_angles
	|
	;

postfix_expr_list_in_angles
	: '<' postfix_expr_list '>'
	;

postfix_expr_list_in_parens
	: '(' postfix_expr_list ')'
	;

expr_list_in_parens
	: '(' expr_list ')'
	;

expr_list
	: expr_list ',' expr
	| expr
	;

//--- array declaration syntax -------------------------------------------

optional_range_list_in_brackets
	: range_list_in_brackets
	|
	;

range_list_in_brackets
	: '[' range_list ']'
	;

range_list
	: range_list ',' range
	| range
	;

range
	: expr RANGE expr
	| expr
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
	cerr << "here is the yy-state-stack:";
	for (s=yyss; s <= yyssp; s++)
		cerr << ' ' << *s;
	cerr << endl;
	cerr << "here is the yy-value-stack:" << endl;
	for (v=yyvs; v <= yyvsp; v++) {
		if (v && v->n) {
			v->n->what(cerr << '\t') << endl;
		} else {
			cerr << "\t(null) " << endl;
		}
	}
	assert(yylval.n);	// NULL check
	yylval.n->what(cerr << "received: ") << endl;
	cerr << "on line " << current.line << " col " << current.col << endl;
	// or throw exception
	exit(1);
}

