/**
	\file "art++-parse.yy"
	Yacc-generated parser source for the ART++ language.  

	note: this is not the same language as that found in lib/art.cy
	but is very close.  Differences are mostly syntactic and not semantic.  

	note: ancient versions of yacc reject // end-of-line comments

	$Id: art++-parse.yy,v 1.16.4.3 2005/05/03 03:35:17 fang Exp $
 */

%{
#include <iostream>

#include "art_parser.h"			// should be first
#include "art++-parse.output.h"		// auto-generated state strings! :)
#include "using_ostream.h"

/** work-around for bison-1.875 and gcc-3.x, until bison is fixed **/
#if defined (__GNUC__) && (3 <= __GNUC__)
#define __attribute__(arglist)		/* empty */
#endif

using namespace ART::lexer;
using namespace ART::parser;

#if YYBISON
#include "memory/excl_ptr.h"
/**
	Work-around for bison.
	Bison doesn't give public access to yyval, so we are forced to
	pass it through a global variable.  
	Make you long for yacc, doesn't it?
	Defined as an excl_ptr, which means exclusive but transferrable
	ownership.  
 */
util::memory::excl_ptr<root_body> AST_root;
#endif

#define	WRAP_LIST(left, list, right)	list->wrap(left, right)

#define	DELETE_TOKEN(tok)		delete tok

// kind of wasteful...
#define	WRAP_ANGLE_LIST(left, list, right)				\
	const char lc = left->get_char();				\
	const char rc = right->get_char();				\
	WRAP_LIST(new node_position(&lc, left->leftmost()),		\
		list, new node_position(&rc, right->leftmost()));	\
	DELETE_TOKEN(left); DELETE_TOKEN(right)

#define	APPEND_LIST(list, delim, item)					\
	DELETE_TOKEN(delim); list->push_back(item)

/**
	Bogus namespace for documenting yacc's internal tables:
	The extern declarations are needed so the compiler doesn't complain
	about uninitialized values.  
	They are actually defined in the same generated file y.tab.cc.
	The definitions contained herein are not actually used.  
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

}	// end namespace yacc

%}

/**
	NOTE: to use the following union definition, which will be
	summarized in "art++-parse-prefix.h" (generated), 
	you will need to include "art_parser_fwd.h" first
	(with using namespace ART::parser;) to provide forward
	declarations of the union-members' types.  
 */
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
	node_position*		_node_position;
	keyword_position*	_keyword_position;
	token_keyword*		_token_keyword;
	token_string*		_token_string;
	token_char*		_token_char;
	token_int*		_token_int;
	token_bool*		_token_bool;
	token_float*		_token_float;
	token_identifier*	_token_identifier;
	token_quoted_string*	_token_quoted_string;
	token_datatype*		_token_datatype;
	token_bool_type*	_token_bool_type;
	token_int_type*		_token_int_type;
	token_paramtype*	_token_paramtype;
	token_pbool_type*	_token_pbool_type;
	token_pint_type*	_token_pint_type;
	token_else*		_token_else;

	root_body*		_root_body;
	root_item*		_root_item;
	namespace_body*		_namespace_body;
	using_namespace*	_using_namespace;
	namespace_id*		_namespace_id;
	typedef_alias*		_typedef_alias;
	ART::parser::definition*	_definition;
	def_body_item*		_def_body_item;
	language_body*		_language_body;
	prototype*		_prototype;
	process_prototype*	_process_prototype;
	process_def*		_process_def;
	type_base*		_type_base;
	concrete_type_ref*	_concrete_type_ref;
	type_id*		_type_id;
	port_formal_decl_list*	_port_formal_decl_list;
	port_formal_decl*	_port_formal_decl;
	port_formal_id_list*	_port_formal_id_list;
	port_formal_id*		_port_formal_id;
	template_formal_decl_list_pair*	_template_formal_decl_list_pair;
	template_formal_decl_list*	_template_formal_decl_list;
	template_formal_decl*	_template_formal_decl;
	template_formal_id_list*	_template_formal_id_list;
	template_formal_id*	_template_formal_id;
/*	data_type_base*		_data_type_base;	*/
	data_type_ref_list*	_data_type_ref_list;
	user_data_type_prototype*	_user_data_type_prototype;
	user_data_type_def*	_user_data_type_def;
	enum_prototype*		_enum_prototype;
	enum_member_list*	_enum_member_list;
	enum_def*		_enum_def;
	chan_type*		_chan_type;
	user_chan_type_prototype*	_user_chan_type_prototype;
	user_chan_type_def*	_user_chan_type_def;
	data_param_id*		_data_param_id;
	data_param_id_list*	_data_param_id_list;
	data_param_decl*	_data_param_decl;
	data_param_decl_list*	_data_param_decl_list;
	instance_management*	_instance_management;
	instance_base*		_instance_base;
	ART::parser::instance_array*	_instance_array;
	instance_declaration*	_instance_declaration;
	instance_id_list*	_instance_id_list;
	definition_body*	_definition_body;
	loop_instantiation*	_loop_instantiation;
	conditional_instantiation*	_conditional_instantiation;
	guarded_definition_body_list*	_guarded_definition_body_list;
	guarded_definition_body*	_guarded_definition_body;
	instance_connection*	_instance_connection;
	connection_statement*	_connection_statement;
	statement*		_statement;
	ART::parser::instance_alias*	_instance_alias;
	alias_list*		_alias_list;

	expr*			_expr;
/*	paren_expr*		_paren_expr;	*/
	qualified_id*		_qualified_id;
	id_expr*		_id_expr;

	ART::parser::index_expr*	_index_expr;
	member_expr*		_member_expr;
/*** not needed
	prefix_expr*		_prefix_expr;
	arith_expr*		_arith_expr;
	relational_expr*	_relational_expr;
	logical_expr*		_logical_expr;
***/
	assign_stmt*		_assign_stmt;
	incdec_stmt*		_incdec_stmt;
	expr_list*		_expr_list;
/** not used
	template_argument_list*	_template_argument_list;
	connection_argument_list*	_connection_argument_list;
**/
	range*			_range;
	range_list*		_range_list;
	dense_range_list*	_dense_range_list;
	array_concatenation*	_array_concatenation;
	loop_concatenation*	_loop_concatenation;
	array_construction*	_array_construction;
	type_completion_statement*	_type_completion_statement;
	type_completion_connection_statement*	_type_completion_connection_statement;

	CHP::body*		_chp_body;
	CHP::stmt_list*		_chp_stmt_list;
	CHP::statement*		_chp_stmt;
	CHP::skip*		_chp_skip;
	CHP::log*		_chp_log;
	CHP::loop*		_chp_loop;
	CHP::do_until*		_chp_do_until;
	CHP::wait*		_chp_wait;
	CHP::selection*		_chp_selection;
	CHP::nondet_selection*	_chp_nondet_selection;
	CHP::det_selection*	_chp_det_selection;
	CHP::guarded_command*	_chp_guarded_command;
	CHP::else_clause*	_chp_else_clause;
	CHP::comm_list*		_chp_comm_list;
	CHP::communication*	_chp_communication;
	CHP::send*		_chp_send;
	CHP::receive*		_chp_receive;
	CHP::assignment*	_chp_assignment;
	CHP::incdec_stmt*	_chp_incdec_stmt;

	HSE::body*		_hse_body;
	HSE::statement*		_hse_stmt;
	HSE::stmt_list*		_hse_stmt_list;
	HSE::skip*		_hse_skip;
	HSE::loop*		_hse_loop;
	HSE::do_until*		_hse_do_until;
	HSE::wait*		_hse_wait;
	HSE::selection*		_hse_selection;
	HSE::nondet_selection*	_hse_nondet_selection;
	HSE::det_selection*	_hse_det_selection;
	HSE::guarded_command*	_hse_guarded_command;
	HSE::else_clause*	_hse_else_clause;
	HSE::assignment*	_hse_assignment;

	PRS::body*		_prs_body;
	PRS::body_item*		_prs_body_item;
	PRS::rule_list*		_prs_rule_list;
	PRS::rule*		_prs_rule;
	PRS::loop*		_prs_loop;
}

%{

/* Had to move these declarations AFTER YYSTYPE (above union) was defined
 * for a particular "yyerror_bison_hack.awk" to work.  
 */
extern	int yylex(void);		// ancient compiler rejects

namespace ART {
namespace lexer {
extern	int at_eof(void);		// from "art++-lex.ll"
}
}
using ART::lexer::at_eof;

static void yyerror(const char* msg);	// ancient compiler rejects

/* automatically generated function to resolve parser symbol type
	on the yy value stack, base on yy state stack transitions
 */
extern	node* yy_union_resolve(const YYSTYPE& u, const short i, const short j);
extern	node* yy_union_lookup(const YYSTYPE& u, const int c);

static
void
yyfreestacks(const short* yyss, const short* yyssp, 
		const YYSTYPE* yyvs, const YYSTYPE* yyvsp, 
		const YYSTYPE yylval);
%}

/*
	A bogus token to catch the starting value of the token enumeration.
	Because yacc and different versions of bison disagree on this.  
	Usually either 257 or 258, POSIX wants 257 methinks, but this avoids
	second guessing.  
 */
%token	MINIMUM_BOGOSITY

/*
	The lexer returns newly allocated nodes FOR ALL TOKENS, 
	even ones that are just symbols.  
	If you don't want to use a returned symbol, delete it!
	We are keeping the memory and performance overhead of the 
	front-end for now, for the sake of precise error reporting.  
	If performance is a concern, consider writing new allocators
	(using memory pools) to replace the default.  

	The following single characters are legitimate tokens:
	][(){}<>*%/=:;|!?~&^.,#+-

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
/* change these to _node_position (was _token_char) */
%type	<_node_position>	'{' '}' '[' ']' '(' ')'
%type	<_token_char>		'<' '>'
%type	<_node_position>	',' '.' ';' ':' '=' '#'
	/* used as template list wrappers and as comparators */
%type	<_token_char>		'+' '-' '*' '/' '%'
%type	<_token_char>		'!' '?' '~' '&' '|' '^'

/*
	the following tokens are defined below because they consist of
	2 or more characters
*/

%token	<_token_identifier>	ID
%token	<_token_float>		FLOAT
%token	<_token_int>		INT
%token	<_token_quoted_string>	STRING

%token	<_node_position>	THICKBAR SCOPE RANGE
%token	<_node_position>	BEGINLOOP BEGINPROB ENDPROB
%token	<_node_position>	DEFINEOP

/* _token_string */
%token	<_token_string>		LE GE EQUAL NOTEQUAL
%token	<_token_string>		IMPLIES RARROW
%token	<_token_string>		LOGICAL_AND LOGICAL_OR
%token	<_token_string>		INSERT EXTRACT
%token	<_token_string>		PLUSPLUS MINUSMINUS

/* _token_keyword: covert most of these to _keyword_position */
%token	<_keyword_position>	NAMESPACE
%token	<_keyword_position>	OPEN AS
%token	<_keyword_position>	CHP_LANG HSE_LANG PRS_LANG
%token	<_keyword_position>	SKIP LOG
%token	<_keyword_position>	DEFINE DEFPROC DEFCHAN DEFTYPE
%token	<_keyword_position>	TYPEDEF
%token	<_keyword_position>	SET GET SEND RECV
%token	<_keyword_position>	CHANNEL
%token	<_keyword_position>	TEMPLATE
%token	<_keyword_position>	ENUM

/* linkage modifiers */
%token	<_token_keyword>	EXTERN STATIC EXPORT

%token	<_token_else>		ELSE

%token	<_token_bool>		BOOL_TRUE BOOL_FALSE

/* _token_type */
%token	<_token_int_type>	INT_TYPE
%token	<_token_bool_type>	BOOL_TYPE
%token	<_token_pint_type>	PINT_TYPE
%token	<_token_pbool_type>	PBOOL_TYPE

/*
	Special yacc/bison-independent hack to get the maximum token enum
	because different versions of them start at either 257 or 258, 
	and the #define number for them can change!
 */
%token	MAXIMUM_BOGOSITY

/* non-terminals */
%type	<_root_body>	module
%type	<_root_body>	top_root body 
%type	<_root_item>	body_item
%type	<_root_item>	namespace_item
%type	<_root_item>	namespace_management
%type	<_namespace_id>	namespace_id
%type	<_typedef_alias>	type_alias
%type	<_definition>	definition
%type	<_process_def>	defproc
%type	<_keyword_position>	def_or_proc
%type	<_prototype>	prototype_declaration
%type	<_process_prototype>	declare_proc_proto
%type	<_user_data_type_prototype>	declare_datatype_proto
%type	<_user_chan_type_prototype>	declare_chan_proto
%type	<_template_formal_decl_list>	template_formal_decl_list_in_angles template_formal_decl_list
%type	<_template_formal_decl_list>	template_formal_decl_list_optional_in_angles
%type	<_template_formal_decl_list_pair>	template_specification optional_template_specification
%type	<_template_formal_decl>	template_formal_decl
%type	<_template_formal_id_list>	template_formal_id_list
%type	<_template_formal_id>	template_formal_id
%type	<_port_formal_decl_list>	optional_port_formal_decl_list_in_parens port_formal_decl_list
%type	<_port_formal_decl>	port_formal_decl
%type	<_port_formal_id_list>	port_formal_id_list
%type	<_port_formal_id>	port_formal_id
%type	<_concrete_type_ref>	physical_type_ref
%type	<_concrete_type_ref>	data_type_ref
%type	<_concrete_type_ref>	type_id
/* %type	<_data_type_base>	base_param_type */
%type	<_token_paramtype>	base_param_type
/* %type	<n>	formal_id */
%type	<_chan_type>	base_chan_type chan_or_port
%type	<_data_type_ref_list>	data_type_ref_list_in_parens data_type_ref_list
%type	<_token_datatype>	base_data_type
%type	<_enum_prototype>	declare_enum
%type	<_enum_def>		defenum
%type	<_enum_member_list>	enum_member_list
%type	<_user_data_type_def>	defdatatype
%type	<_user_chan_type_def>	defchan
%type	<_chp_body>	set_body get_body send_body recv_body
%type	<_data_param_decl_list>	data_param_decl_list
%type	<_data_param_decl_list>	data_param_decl_list_in_parens
%type	<_data_param_decl>	data_param_decl
%type	<_data_param_id_list>	data_param_id_list
%type	<_data_param_id>	data_param_id
%type	<_definition_body>	definition_body optional_definition_body
%type	<_def_body_item>	definition_body_item
%type	<_instance_management>	instance_item
%type	<_instance_declaration>	type_instance_declaration 
%type	<_loop_instantiation>	loop_instantiation
%type	<_conditional_instantiation>	conditional_instantiation
%type	<_instance_id_list>	instance_id_list
%type	<_instance_base>	instance_id_item
%type	<_connection_statement>	connection_statement
%type	<_type_completion_statement>	instance_type_completion_statement
%type	<_type_completion_connection_statement>	instance_type_completion_connection_statement
/* %type	<_instance_alias>	instance_alias	*/
%type	<_alias_list>	rvalue_optional_alias_list
%type	<_alias_list>	alias_list
%type	<_expr_list>	connection_actuals_list
%type	<_guarded_definition_body_list>	guarded_definition_body_list
%type	<_guarded_definition_body>	guarded_definition_body
%type	<_language_body>	language_body
%type	<_chp_stmt_list>	chp_body
%type	<_chp_stmt_list>	full_chp_body_item_list
%type	<_chp_stmt>	full_chp_body_item chp_body_item
%type	<_chp_loop>	chp_loop
%type	<_chp_do_until>	chp_do_until
%type	<_chp_selection>	chp_selection
%type	<_chp_wait>	chp_wait
%type	<_chp_nondet_selection>	chp_nondet_guarded_command_list
%type	<_chp_det_selection>	chp_unmatched_det_guarded_command_list
%type	<_chp_det_selection>	chp_matched_det_guarded_command_list
%type	<_chp_guarded_command>	chp_guarded_command
%type	<_chp_else_clause>	chp_else_clause
/* %type	<_chp_assignment>	chp_assignment */
%type	<_chp_comm_list>	chp_comm_list
%type	<_chp_communication>	chp_comm_action
%type	<_chp_send>	chp_send
%type	<_chp_receive>	chp_recv
%type	<_hse_stmt_list>	hse_body
%type	<_hse_stmt_list>	full_hse_body_item_list
%type	<_hse_stmt>	full_hse_body_item hse_body_item
%type	<_hse_loop>	hse_loop
%type	<_hse_do_until>	hse_do_until
%type	<_hse_selection>	hse_selection
%type	<_hse_wait>	hse_wait
%type	<_hse_guarded_command>	hse_guarded_command
%type	<_hse_else_clause>	hse_else_clause
%type	<_hse_nondet_selection>	hse_nondet_guarded_command_list
%type	<_hse_det_selection>	hse_matched_det_guarded_command_list
%type	<_hse_det_selection>	hse_unmatched_det_guarded_command_list
%type	<_hse_assignment>	hse_assignment
%type	<_prs_rule_list>	prs_body
%type	<_prs_body_item>	prs_body_item
%type	<_prs_rule>	single_prs
%type	<_prs_loop>	prs_loop
%type	<_expr>	prs_expr prs_paren_expr prs_unary_expr
%type	<_expr> prs_not prs_and prs_or
%type	<_expr> prs_and_loop prs_or_loop
%type	<_token_string>	prs_arrow
%type	<_token_char>	dir
%type	<_expr>	paren_expr expr
/* %type	<n>	primary_expr */
%type	<_expr>	literal
%type	<_id_expr>	id_expr
%type	<_qualified_id>	qualified_id absolute_id relative_id
%type	<_expr_list>	member_index_expr_list
%type	<_expr_list>	shift_expr_optional_list shift_expr_optional_list_in_angles
%type	<_expr>	optional_member_index_expr
%type	<_expr> simple_expr
%type	<_expr>	member_index_expr unary_expr
%type	<_member_expr>	member_expr
%type	<_index_expr>	index_expr
%type	<_expr>	multiplicative_expr additive_expr
%type	<_expr> shift_expr optional_shift_expr
%type	<_expr>	relational_equality_expr and_expr
%type	<_expr>	exclusive_or_expr inclusive_or_expr
%type	<_expr>	logical_and_expr logical_or_expr
/* %type	<_statement>	assignment_stmt */
%type	<_assign_stmt>	binary_assignment
%type	<_incdec_stmt>	unary_assignment
/* %type	<n>	conditional_expr optional_expr_in_braces */
%type	<_expr_list>	optional_template_arguments_in_angles
/* %type	<_expr_list>	member_index_expr_list_in_angles */
%type	<_expr_list>	member_index_expr_list_in_parens
%type	<_expr_list>	expr_list_in_parens expr_list
/* %type	<_range_list>	optional_range_list_in_brackets */
/* %type	<_range_list>	range_list_in_brackets */
/* %type	<_range_list>	range_list */
%type	<_dense_range_list>	dense_range_list optional_dense_range_list
%type	<_range_list>	sparse_range_list
/* %type	<_range_list>	optional_sparse_range_list */
%type	<_expr>		bracketed_dense_range
%type	<_range>	bracketed_sparse_range
%type	<_range>	range
%type	<_expr>		complex_aggregate_reference
%type	<_expr>		optional_complex_aggregate_reference
%type	<_expr>		complex_expr_term
%type	<_array_concatenation>		array_concatenation
%type	<_loop_concatenation>		loop_concatenation
%type	<_array_construction>		array_construction
%type	<_expr_list>	complex_aggregate_reference_list

%start	module
%%
/******************************************************************************
//	Grammar -- re-written to be LALR(1)
******************************************************************************/

/* top level syntax */

module
	: top_root
		{
#if YYBISON
			AST_root = util::memory::excl_ptr<root_body>($1);
#else	// YACC
			$$ = $1;
#endif
		}
	;

top_root
	: body
	/* allow empty file */
	| 
		{ $$ = NULL; }
	;

body
	: body body_item
		{ $$ = $1; $1->push_back($2); }
	| body_item
		{ $$ = new root_body($1); }
	;

body_item
	: namespace_item { $$ = $1; }
	| definition { $$ = $1; }
	| prototype_declaration { $$ = $1; }
	;

namespace_item
/* namespace_management already includes semicolon where needed */
/* proposed change: forbid nested namespacs, only allow in root_item */
	: namespace_management { $$ = $1; }
/* instance_item already includes semicolon where needed */
	| instance_item { $$ = $1; }
	| type_alias { $$ = $1; }
	;

namespace_management
	/* C++ style classes require semicolon, but not afer namespace */
	/* really the NAMESPACE and OPEN keyword tokens may be discarded */
	: NAMESPACE ID '{' top_root '}'
		{ if (!$4)
			$4 = new root_body(NULL);
		  WRAP_LIST($3, $4, $5);
		  $$ = new namespace_body($1, $2, $4); }
	/* or C++ style: using namespace blah; */
	| OPEN namespace_id AS ID ';'
		{ $$ = new using_namespace($1, $2, $4);
		  DELETE_TOKEN($3); DELETE_TOKEN($5); }
	| OPEN namespace_id ';'
		{ $$ = new using_namespace($1, $2);
		  DELETE_TOKEN($3); }
	/* ever close namespace? */
	;

namespace_id
	: relative_id
		{ $$ = new namespace_id($1); }
	;

/* Process, datatype, and channel definition. */
definition
	: defproc { $$ = $1; }
	| defdatatype { $$ = $1; }
	| defchan { $$ = $1; }
	| defenum { $$ = $1; }
	;

/* declaration prototypes, like forward declarations */
prototype_declaration
	: declare_proc_proto { $$ = $1; }
	| declare_datatype_proto { $$ = $1; }
	| declare_chan_proto { $$ = $1; }
	| declare_enum { $$ = $1; }
	;

/** type_id is either physical_type_ref or base_param_type */
type_alias
/* C-style typedef, but allowing templates */
	: optional_template_specification TYPEDEF physical_type_ref ID ';'
		{ $$ = new typedef_alias($1, $2, $3, $4);
		  DELETE_TOKEN($5); }
/*	other proposal, use {deftype,defchan,defproc} new<> = old<> */
	;

template_specification
	: TEMPLATE template_formal_decl_list_in_angles
		/* too damn lazy to keep around keyword... */
		{ DELETE_TOKEN($1);
		  $$ = new template_formal_decl_list_pair($2, NULL); }
	| TEMPLATE template_formal_decl_list_optional_in_angles
	  template_formal_decl_list_in_angles
		/* second set of formals is for relaxed parameters */
		{ DELETE_TOKEN($1);
		  $$ = new template_formal_decl_list_pair($2, $3); }
	;

optional_template_specification
	: template_specification
	| { $$ = NULL; }
	;

/*** later...
optional_linkage_specification
	: EXTERN
	| STATIC
	| EXPORT
	|		{ $$ = NULL; }
	;
***/

/******************************************************************************
//	Process
******************************************************************************/

def_or_proc
	: DEFINE
	| DEFPROC
	;

declare_proc_proto
	: optional_template_specification def_or_proc ID
	  optional_port_formal_decl_list_in_parens ';'
		{ $$ = new process_prototype($1, $2, $3, $4);
		  DELETE_TOKEN($5); }
	;

defproc
	/* C++ style template declaration */
	: optional_template_specification def_or_proc ID
	  optional_port_formal_decl_list_in_parens
	  '{' optional_definition_body '}'
	/* optional_definition_body will always be valid, sometimes empty */
		{ WRAP_LIST($5, $6, $7);
		  $$ = new process_def($1, $2, $3, $4, $6);
		}
	;

optional_port_formal_decl_list_in_parens
	/* note: the parens are NOT optional! */
	: '(' port_formal_decl_list ')'
		{ $$ = $2; WRAP_LIST($1, $2, $3); }
	| '(' ')'
		{ $$ = new port_formal_decl_list(); WRAP_LIST($1, $$, $2); }
		/* empty, but wrapped */
	;

/***
// already captured by physical_type_ref
concrete_type_ref
	: type_id optional_template_arguments_in_angles
		{ $$ = new concrete_type_ref($1, $2); }
	;
***/

/******** Meta (template) language parameters ********/

template_formal_decl_list_in_angles
	: '<' template_formal_decl_list '>'
		{ $$ = $2; WRAP_ANGLE_LIST($1, $2, $3); }
	;

template_formal_decl_list_optional_in_angles
	: template_formal_decl_list_in_angles
		{ $$ = $1; }
	| '<' '>'
		{ $$ = new template_formal_decl_list();
		  WRAP_ANGLE_LIST($1, $$, $2); }
	;

/** OBSOLETE
optional_template_formal_decl_list_in_angles
	: template_formal_decl_list_in_angles
	| { $$ = NULL; }
	;
**/

template_formal_decl_list
	: template_formal_decl_list ';' template_formal_decl
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| template_formal_decl
		{ $$ = new template_formal_decl_list($1); }
	;

template_formal_decl
/* changing to C-style formal parameters, allowing comma-lists
	is there any need for user-defined types in template argument? */
	: base_param_type template_formal_id_list
		{ $$ = new template_formal_decl($1, $2); }
	;

template_formal_id_list
	: template_formal_id_list ',' template_formal_id
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| template_formal_id
		{ $$ = new template_formal_id_list($1); }
	;

template_formal_id
	/** update formal declarations: only allow dense arrays, no ranges */
	: ID optional_dense_range_list
		{ $$ = new template_formal_id($1, $2); }
/**
	from http://www.computing.surrey.ac.uk/research/dsrg/fog/CxxGrammar.y:
	The potential shift-reduce conflict on > is resolved by
	flattening part of the expression grammar to know when the 
	next > is template end or arithmetic >.
**/
	| ID optional_dense_range_list '=' shift_expr
		{ $$ = new template_formal_id($1, $2, $3, $4); }
/**
	We choose to force the user to disambiguate by placing parentheses
	around relational expressions, which covers arithmetic use of '>'.
	Notice that below, shift_expr is the highest expression
	before relational_expr.  
**/
	;

port_formal_decl_list
	/* would rather use ','-delimiter, but wth... */
	: port_formal_decl_list ';' port_formal_decl
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| port_formal_decl
		{ $$ = new port_formal_decl_list($1); }
	;

port_formal_decl
	/* must switch to C-style formals, eliminate id_list */
	: physical_type_ref port_formal_id_list
		{ $$ = new port_formal_decl($1, $2); }
	;

port_formal_id_list
	: port_formal_id_list ',' port_formal_id
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| port_formal_id
		{ $$ = new port_formal_id_list($1); }
	;

port_formal_id
	/** update port formals: only dense arrays allowed, no sparse ranges */
	: ID optional_dense_range_list
		{ $$ = new port_formal_id($1, $2); }
	;

physical_type_ref
	: relative_id optional_template_arguments_in_angles
		/* for userdef or chan type, and templating */
		{ $$ = new concrete_type_ref(new type_id($1), $2); }
	| absolute_id optional_template_arguments_in_angles
		/* for userdef or chan type, and templating */
		{ $$ = new concrete_type_ref(new type_id($1), $2); }
	| base_chan_type
		/* what would template channel type ref look like? */
		{ $$ = new concrete_type_ref($1, NULL); }
	| data_type_ref
		{ $$ = $1; }
	;

data_type_ref
	: base_data_type optional_template_arguments_in_angles
		{ $$ = new concrete_type_ref($1, $2); }
	;

type_id
	: physical_type_ref { $$ = $1; }
	| base_param_type
		{ $$ = new concrete_type_ref($1, NULL); }
		/* should parameter declarations be allowed 
			in loops and conditionals? rather not */
	;

/******************************************************************************
//	base types
******************************************************************************/

base_param_type
	: PINT_TYPE 		/* integer parameter */
		{ $$ = $1; }
	| PBOOL_TYPE		/* boolean parameter */
		{ $$ = $1; }
	;

/* channel type: channel, inport, outport, and data types */
base_chan_type
	/* eliminate defaulting? (to int?), use <template> style? */
	: chan_or_port data_type_ref_list_in_parens
		{ $$ = $1; $$->attach_data_types($2); }
	;

chan_or_port
	: CHANNEL		/* a channel */
		{ $$ = new chan_type($1); }
	| CHANNEL '!'		/* an output port */
		{ $$ = new chan_type($1, $2); }
	| CHANNEL '?'		/* an input port */
		{ $$ = new chan_type($1, $2); }
	;

data_type_ref_list_in_parens
	: '(' data_type_ref_list ')'
		{ $$ = $2; WRAP_LIST($1, $2, $3); }
	;

/* why only base data types? why not user-defined ones? */
data_type_ref_list
/*	: data_type_ref_list ',' base_data_type	*/
	: data_type_ref_list ',' data_type_ref
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
/*	| base_data_type	*/
	| data_type_ref
		{ $$ = new data_type_ref_list($1); }
	;

/* actual data: int<width> or bool */
base_data_type
/* ever need user-defined types? eventually...
	optional parens get confused with template-parameters
	going to use angle brackets <> in the template-fashion */
/* BAD: eliminate this special case, let type-checker handle */
/***
	: INT_TYPE '<' INT '>'
		{ $$ = new data_type_base($1, 
			(new expr_list($3))->wrap($2, $4));
		}
***/
	: INT_TYPE
		{ $$ = $1; }
	| BOOL_TYPE
		{ $$ = $1; }
	;

/* definition types */
declare_datatype_proto
	: optional_template_specification DEFTYPE ID DEFINEOP
/*	  base_data_type */
	  data_type_ref		/* base? */
          data_param_decl_list_in_parens ';'
		{ $$ = new user_data_type_prototype($1, $2, $3, $4, $5, $6);
		  DELETE_TOKEN($7); }
	;

defdatatype
	: optional_template_specification DEFTYPE ID DEFINEOP
/*	  base_data_type */
	  data_type_ref		/* base? */
          data_param_decl_list_in_parens
	  '{' set_body get_body '}'
		{ $$ = new user_data_type_def(
			$1, $2, $3, $4, $5, $6, $7, $8, $9, $10); }
	;

set_body
	: SET '{' chp_body '}'
		{ WRAP_LIST($2, $3, $4);
		  $$ = new CHP::body($1, $3); }
	;

get_body
	: GET '{' chp_body '}'
		{ WRAP_LIST($2, $3, $4);
		  $$ = new CHP::body($1, $3); }
	;

declare_enum
	: ENUM ID ';'
		{ $$ = new enum_prototype($1, $2); DELETE_TOKEN($3); }
	;

defenum
	: ENUM ID '{' enum_member_list '}'
		{ WRAP_LIST($3, $4, $5);
		  $$ = new enum_def($1, $2, $4); }
	;

enum_member_list
	: enum_member_list ',' ID
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| ID
		{ $$ = new enum_member_list($1); }
	;

/** 
	CHANNELS
**/

declare_chan_proto
	: optional_template_specification DEFCHAN ID DEFINEOP base_chan_type 
	  data_param_decl_list_in_parens ';'
		{ $$ = new user_chan_type_prototype($1, $2, $3, $4, $5, $6);
		  DELETE_TOKEN($7); }
	;
	
defchan
	: optional_template_specification DEFCHAN ID DEFINEOP base_chan_type 
          data_param_decl_list_in_parens
	  '{' send_body recv_body '}'
		{ $$ = new user_chan_type_def(
			$1, $2, $3, $4, $5, $6, $7, $8, $9, $10); }
	;

send_body
	: SEND '{' chp_body '}'
		{ WRAP_LIST($2, $3, $4);
		  $$ = new CHP::body($1, $3); }
	;

recv_body
	: RECV '{' chp_body '}'
		{ WRAP_LIST($2, $3, $4);
		  $$ = new CHP::body($1, $3); }
	;

data_param_decl_list_in_parens
	: '(' data_param_decl_list ')'
		{ $$ = $2; WRAP_LIST($1, $2, $3); }
	;

data_param_decl_list
/* like declarations in formals list
	consider using ';', similar to C-style... */
	: data_param_decl_list ';' data_param_decl
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| data_param_decl
		{ $$ = new data_param_decl_list($1); }
	;

data_param_decl
/*
	forseen problem: array brackets are with data_type
	but to follow C-style, we want the arrays to go with identifiers
	thinking of forbidding list, restricting to single
	semicolon-delimited declarations
*/
	: data_type_ref data_param_id_list
		{ $$ = new data_param_decl($1, $2); }
	;

data_param_id_list
	: data_param_id_list ',' data_param_id
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| data_param_id
		{ $$ = new data_param_id_list($1); }
	;

data_param_id
	/** really, this should be formal */
	: ID optional_dense_range_list
		{ $$ = new data_param_id($1, $2); }
/** archaic
		{ $$ = ($2) ? new instance_array($1, $2)
			: new instance_base($1); }
**/
	;


/* --- definition_body --- */
definition_body
	: definition_body definition_body_item
		{ $$ = $1; $$->push_back($2); }
	| definition_body_item
		{ $$ = new definition_body($1); }
	;

definition_body_item
	: instance_item { $$ = $1; }
	| language_body { $$ = $1; }
	| type_alias { $$ = $1; }
	;

optional_definition_body
	: definition_body
	| { $$ = new definition_body(); }
		/* returns empty definition body instead of NULL
			because it needs to be wrapped in braces */
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
	: type_instance_declaration { $$ = $1; }	/* single or array */
	| connection_statement { $$ = $1; }	/* connection of ports */
	| instance_type_completion_statement { $$ = $1; }
	| instance_type_completion_connection_statement { $$ = $1; }
	| alias_list '=' expr ';'
		{ $$ = $1; APPEND_LIST($1, $2, $3); DELETE_TOKEN($4); }
			/* alias connection */
	| loop_instantiation { $$ = $1; }
	| conditional_instantiation { $$ = $1; }
	;

loop_instantiation
	: '(' ';' ID ':' range ':' definition_body ')'
		{ $$ = new loop_instantiation($1, $3, $5, $7, $8);
		  DELETE_TOKEN($2); DELETE_TOKEN($4); DELETE_TOKEN($6); }
	;

conditional_instantiation
	: '[' guarded_definition_body_list ']'
		{ WRAP_LIST($1, $2, $3);
		  $$ = new conditional_instantiation($2); }
	;

type_instance_declaration
	/* type template is included in type_id, and is part of the type */
	: type_id instance_id_list ';'
		{ $$ = new instance_declaration($1, $2);
		  DELETE_TOKEN($3); }
	;

instance_id_list
	: instance_id_list ',' instance_id_item
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| instance_id_item
		{ $$ = new instance_id_list($1); }
	;

instance_id_item
	/* array declaration: forbid connection, must connect later */
	: ID sparse_range_list
		{ $$ = new instance_array($1, $2); }
	/* single instance declaration without connection */
	| ID
		{ $$ = new instance_base($1); }
	/* single instance declaration with connection */
	| ID connection_actuals_list
		{ $$ = new instance_connection($1, $2); }
	/* instance alias or parameter assignment */
	| ID '=' rvalue_optional_alias_list
		{ WRAP_LIST($2, $3, NULL);
		  $$ = new instance_alias($1, $3); }
	;

connection_statement
/* taking a declared array or single instance and connecting ports
	are brackets part of the array/membership chain? */
	: member_index_expr connection_actuals_list ';'
		/* can this first id be scoped and/or membered? */
		{ $$ = new connection_statement($1, $2);
		  DELETE_TOKEN($3); }
	;

/* completing the relaxed template arguments of an instance */
instance_type_completion_statement
	: index_expr shift_expr_optional_list_in_angles ';'
		{ $$ = new type_completion_statement($1, $2);
		  DELETE_TOKEN($3); }
	;

instance_type_completion_connection_statement
	: index_expr shift_expr_optional_list_in_angles 
		connection_actuals_list ';'
		{ $$ = new type_completion_connection_statement($1, $2, $3);
		  DELETE_TOKEN($4); }
	;

rvalue_optional_alias_list
/* note that expr can be just another member_index_expr */
	: alias_list '=' expr
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| expr
		{ $$ = new alias_list($1); }
	;

/* aliasing syntax, or data types is value assignment (general expr?)
	type check this, of course */
alias_list
	: alias_list '=' complex_aggregate_reference
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	/* to type-check: first term must contain only rvalues */
	| complex_aggregate_reference
		{ $$ = new alias_list($1); }
	;
/* used to be member_index_expr */

connection_actuals_list
	/* down-cast to more specific type?  internal to consumer */
/*	: member_index_expr_list_in_parens	*/
	: '(' complex_aggregate_reference_list ')'
		{ $$ = $2; WRAP_LIST($1, $2, $3); }
	;

guarded_definition_body_list
	: guarded_definition_body_list THICKBAR guarded_definition_body
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
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
		{ WRAP_LIST($2, $3, $4); $$ = new CHP::body($1, $3); }
	| HSE_LANG '{' hse_body '}'
		{ WRAP_LIST($2, $3, $4); $$ = new HSE::body($1, $3); }
	| PRS_LANG '{' prs_body '}'
		{ WRAP_LIST($2, $3, $4); $$ = new PRS::body($1, $3); }
	;

/* --- Language: CHP --- */

chp_body
	: full_chp_body_item_list { $$ = $1; }
	;

full_chp_body_item_list
	: full_chp_body_item_list ';' full_chp_body_item
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
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
	: chp_loop { $$ = $1; }
	| chp_do_until { $$ = $1; }
	| chp_selection { $$ = $1; }
	| chp_wait { $$ = $1; }
/*	| chp_assignment { $$ = $1; }	-- expanded */
	| binary_assignment { $$ = new CHP::assignment($1); }
	| unary_assignment { $$ = new CHP::incdec_stmt($1); }
	| chp_comm_list { $$ = $1; }
	| SKIP { $$ = new CHP::skip($1); }
	| LOG expr_list_in_parens
		{ $$ = new CHP::log($1, $2); }
	;

chp_loop
	/* do-forever loop */
	: BEGINLOOP chp_body ']'
		{ WRAP_LIST($1, $2, $3); $$ = new CHP::loop($2); }
	;

chp_do_until
	/* do-until-all-guards-false */
	: BEGINLOOP chp_matched_det_guarded_command_list ']'
		{ WRAP_LIST($1, $2, $3); $$ = new CHP::do_until($2); }
	;

chp_wait
	/* wait for expr to become true */
	: '[' expr ']'
		{ $$ = new CHP::wait($2);
		  DELETE_TOKEN($1); DELETE_TOKEN($3); }
	;

chp_selection
	: '[' chp_matched_det_guarded_command_list ']'
		{ $$ = $2; WRAP_LIST($1, $2, $3); }
	| '[' chp_nondet_guarded_command_list ']'
		{ $$ = $2; WRAP_LIST($1, $2, $3); }
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
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| chp_guarded_command ':' chp_guarded_command
	/* can't have else clause in non-deterministic selection? */
		{ $$ = new CHP::nondet_selection($1);
		  APPEND_LIST($$, $2, $3); }
	;

chp_matched_det_guarded_command_list
	: chp_unmatched_det_guarded_command_list THICKBAR chp_else_clause
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| chp_unmatched_det_guarded_command_list
	;

chp_unmatched_det_guarded_command_list
	: chp_unmatched_det_guarded_command_list THICKBAR chp_guarded_command
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
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
chp_assignment
// allow binary and unary assignments
// constructors re-wrap with transfer of ownership
	: binary_assignment { $$ = new CHP::assignment($1); }
	| unary_assignment { $$ = new CHP::bool_assign($1); }
	;
*/

chp_comm_list
	/* gives comma-separated communications precedence */
	: chp_comm_list ',' chp_comm_action
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| chp_comm_action
		{ $$ = new CHP::comm_list($1); }
	;

chp_comm_action
	: chp_send { $$ = $1; }
	| chp_recv { $$ = $1; }
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
	: full_hse_body_item_list { $$ = $1; }
	;

full_hse_body_item_list
	: full_hse_body_item_list ';' full_hse_body_item
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
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
	: hse_loop { $$ = $1; }
	| hse_do_until { $$ = $1; }
	| hse_wait { $$ = $1; }
	| hse_selection { $$ = $1; }
	| hse_assignment { $$ = $1; }
	| SKIP { $$ = new HSE::skip($1); }
	;

hse_loop
	: BEGINLOOP hse_body ']'
		{ WRAP_LIST($1, $2, $3); $$ = new HSE::loop($2); }
	;

hse_do_until
	/* keep entering loop until all guards false */
	: BEGINLOOP hse_matched_det_guarded_command_list ']'
		{ WRAP_LIST($1, $2, $3); $$ = new HSE::do_until($2); }
	;

hse_wait
	: '[' expr ']'
		{ $$ = new HSE::wait($2);
		  DELETE_TOKEN($1); DELETE_TOKEN($3); }
	;

hse_selection
	: '[' hse_matched_det_guarded_command_list ']'
		{ $$ = $2; WRAP_LIST($1, $2, $3); }
	| '[' hse_nondet_guarded_command_list ']'
		{ $$ = $2; WRAP_LIST($1, $2, $3); }
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
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| hse_guarded_command ':' hse_guarded_command
		{ $$ = new HSE::nondet_selection($1);
			APPEND_LIST($$, $2, $3); }
	;

hse_matched_det_guarded_command_list
	: hse_unmatched_det_guarded_command_list THICKBAR hse_else_clause
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| hse_unmatched_det_guarded_command_list
	;

hse_unmatched_det_guarded_command_list
	: hse_unmatched_det_guarded_command_list THICKBAR hse_guarded_command
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
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
/* will there be a leak if dynamic cast fails? */

/*
//--- Language: PRS ---
// to do: add support for overriding default connection to Vdd, GND
// for power/ground isolation, and other tricks, pass gating... <-> <+> <=>
*/

prs_body
	: prs_body prs_body_item
		{ $$ = $1; $$->push_back($2); }
	| prs_body_item
		{ $$ = new PRS::rule_list($1); }
	;

prs_body_item
	: single_prs { $$ = $1; }
	| prs_loop { $$ = $1; }
	;

prs_loop
	: '(' ':' ID ':' range ':' prs_body ')'
		{ $$ = new PRS::loop($1, $3, $5, $7, $8);
		  DELETE_TOKEN($2); DELETE_TOKEN($4); DELETE_TOKEN($6); }
	;

single_prs
	/* note: type-check prs_expr as boolean return type */
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


/* want prs expr to be only ~, & and | expressions */
prs_expr
	: prs_or { $$ = $1; }
/*	used to be just "expr"		 */
	;

prs_paren_expr
	: '(' prs_expr ')'
		{ $$ = $2; DELETE_TOKEN($1); DELETE_TOKEN($3); }
	;

prs_unary_expr
	: member_index_expr { $$ = $1; }
	| prs_paren_expr { $$ = $1; }
	| prs_and_loop { $$ = $1; }
	| prs_or_loop { $$ = $1; }
	;

prs_not
	: '~' prs_unary_expr { $$ = new prefix_expr($1, $2); }
	| prs_unary_expr { $$ = $1; }
	;

prs_and
	: prs_and '&' prs_not
		{ $$ = new logical_expr($1, $2, $3); }
	| prs_not { $$ = $1; }
	;

prs_or
	: prs_or '|' prs_and
		{ $$ = new logical_expr($1, $2, $3); }
	| prs_and { $$ = $1; }
	;

/* non-short-circuit AND */
prs_and_loop
	: '(' '&' ':' ID ':' range ':' prs_expr ')'
		{ $$ = new PRS::op_loop($1, $2, $3, $4, $5, $6, $7, $8, $9); }
	;

/* non-short-circuit OR */
prs_or_loop
	: '(' '|' ':' ID ':' range ':' prs_expr ')'
		{ $$ = new PRS::op_loop($1, $2, $3, $4, $5, $6, $7, $8, $9); }
	;

/* end of PRS language */

/******************************************************************************
// Expressions, expressions, expressions
// mostly ripped from ANSI C++ grammar
******************************************************************************/
paren_expr
	: '(' expr ')'
		{ $$ = $2; DELETE_TOKEN($1); DELETE_TOKEN($3); }
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
		{ $$ = new id_expr($1); }
		/* for specifying unambiguous type from global scope */
	| absolute_id
		{ $$ = new id_expr($1); }
	;

absolute_id
	: SCOPE relative_id
		{ $$ = $2->force_absolute($1); }
	;

relative_id
	: qualified_id
	| ID
		/* wrap in qualified_id */
		{ $$ = new qualified_id($1); }
	;

qualified_id
	: qualified_id SCOPE ID
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| ID SCOPE ID
		{ $$ = new qualified_id($1);
		  APPEND_LIST($$, $2, $3); }
	;

/** was mandatory, but is now optional, blank items are allowed! */
member_index_expr_list
	: member_index_expr_list ',' optional_member_index_expr
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| optional_member_index_expr { $$ = new expr_list($1); }
	;

optional_member_index_expr
	: member_index_expr
	| { $$ = NULL; }
	;

/* this is what we want for expression arguments, without operators */
member_index_expr
/*	: primary_expr */
	: id_expr { $$ = $1; }
	/* array index: should forbid C-style id[N][M]? current allows... */
	| index_expr { $$ = $1; }
	| member_expr { $$ = $1; }
	/*			or id_expr? */
	/* no function calls in expressions... yet */
	;

/** This removes S/R conflict between (a[i])[j] and (a[i][j]) */
index_expr
	: member_expr sparse_range_list
		{ $$ = new index_expr($1, $2); }
	| id_expr sparse_range_list
		{ $$ = new index_expr($1, $2); }
	;

member_expr
	: index_expr '.' ID
		{ $$ = new member_expr($1, $2, $3); }
	| member_expr '.' ID
		{ $$ = new member_expr($1, $2, $3); }
	| id_expr '.' ID
		{ $$ = new member_expr($1, $2, $3); }
	;

/* single term */
simple_expr
	: member_index_expr { $$ = $1; }
	| literal { $$ = $1; }
	;

unary_expr
	: simple_expr { $$ = $1; }
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

/* short-circuit AND ? */
logical_and_expr
	: inclusive_or_expr
	| logical_and_expr LOGICAL_AND inclusive_or_expr
		{ $$ = new logical_expr($1, $2, $3); }
	;

/* short-circuit OR ? */
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

/**
assignment_stmt
	: binary_assignment { $$ = $1; }
	| unary_assignment { $$ = $1; }
	;
**/

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

optional_template_arguments_in_angles
/*	: member_index_expr_list_in_angles	*/
/*	replaced with shift_expr_optional_list to eliminate S/R on '>' */
	: shift_expr_optional_list_in_angles
		{ $$ = $1; }
/*		{ $$ = new template_argument_list($1); }	*/
	| { $$ = NULL; }
	;

shift_expr_optional_list_in_angles
	: '<' shift_expr_optional_list '>'
		{ $$ = $2; WRAP_ANGLE_LIST($1, $2, $3); }
	;

shift_expr_optional_list
	: shift_expr_optional_list ',' optional_shift_expr 
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| optional_shift_expr { $$ = new expr_list($1); }
	;

optional_shift_expr
	: shift_expr { $$ = $1; }
	| { $$ = NULL; }
	;

member_index_expr_list_in_parens
	: '(' member_index_expr_list ')'
		{ $$ = $2; WRAP_LIST($1, $2, $3); }
	;

expr_list_in_parens
	: '(' expr_list ')'
		{ $$ = $2; WRAP_LIST($1, $2, $3); }
	;

expr_list
	: expr_list ',' expr 
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| expr { $$ = new expr_list($1); }
	;

/* --- array declaration syntax ------------------------------------------- */
/** giving up CAST-style for C-style arrays */

range
	: expr RANGE expr 
		{ $$ = new range($1, $2, $3); }
	| expr { $$ = new range($1); }
	;

optional_dense_range_list
	: dense_range_list
		{ $$ = $1; }
	|	{ $$ = NULL; }
	;

/** not needed
optional_sparse_range_list
	: sparse_range_list
		{ $$ = $1; }
	|	{ $$ = NULL; }
	;
**/

dense_range_list
	: dense_range_list bracketed_dense_range
		{ $$ = $1; $$->push_back($2); }
	| bracketed_dense_range
		{ $$ = new dense_range_list($1); }
	;

sparse_range_list
	: sparse_range_list bracketed_sparse_range
		{ $$ = $1; $$->push_back($2); }
	| bracketed_sparse_range
		{ $$ = new range_list($1); }
	;

/** array declarations in template and port formals can only be dense */
bracketed_dense_range
	: '[' expr ']'
		{ DELETE_TOKEN($1); $$ = $2; DELETE_TOKEN($3); }
	;

/** array instantiations and references elsewhere may be sparse */
bracketed_sparse_range
	: '[' range ']'
		{ DELETE_TOKEN($1); $$ = $2; DELETE_TOKEN($3); }
	;

/* ----end array ---------------------------------------------------------- */
/* ---- complex expressions ----------------------------------------------- */

complex_aggregate_reference
	: array_concatenation { $$ = $1; }
	;

/* pasting arrays together */
array_concatenation
	: array_concatenation '#' complex_expr_term
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| complex_expr_term
		{ $$ = new array_concatenation($1); }
	;

complex_expr_term
	: array_construction { $$ = $1; }
	| loop_concatenation { $$ = $1; }
	| simple_expr { $$ = $1; }
	;

/* building up to higher-dimensions */
array_construction
	: '{' array_concatenation '}'
		{ $$ = new array_construction($1, $2, $3); }
	;

/**
	Later, introduces loop concatenations?
	Don't always know number of terms statically...
	For now, add to grammar, but don't implement for a while.  
**/
loop_concatenation
	: '(' '#' ':' ID ':' range ':' complex_expr_term ')'
		{ $$ = new loop_concatenation(
			$1, $2, $3, $4, $5, $6, $7, $8, $9); }
	;

optional_complex_aggregate_reference
	: complex_aggregate_reference { $$ = $1; }
	| { $$ = NULL; }
	;

/** items are optional! */
complex_aggregate_reference_list
	: complex_aggregate_reference_list ',' optional_complex_aggregate_reference
		{ $$ = $1; APPEND_LIST($1, $2, $3); }
	| optional_complex_aggregate_reference
		{ $$ = new expr_list($1); }
	;

/* ---- end complex expressions ------------------------------------------- */
%%
/**
	Upon error or exception, must clean up stacks!
	Now clean-up the symbol stack by calling destructors.
	Technically, this is not needed, as bulk memory is 
	reclaimed upon exit().  (This is a good exercise anyhow.)
	We are currently assuming that no other handler will
	take care of deleting the pointers on the stack.  
	Because the union-pointer resolution can only return
	one type, the base type, the mother destructor, 
	ART::parser::node::~node(), must be virtual.  
 */
static
void
yyfreestacks(const short* yyss, const short* yyssp, 
		const YYSTYPE* yyvs, const YYSTYPE* yyvsp, 
		const YYSTYPE yylval) {
	const short* s;
	const YYSTYPE* v;
	node* resolved_node = NULL;
	s=yyss+1;
	v=yyvs+1;
	for ( ; s <= yyssp && v <= yyvsp; s++, v++) {
		if (v) {
			resolved_node = yy_union_resolve(*v, *(s-1), *s);
			if (resolved_node)
				delete resolved_node;
		}
	}
	if (!at_eof()) {
		// free the last token (if not EOF)
		resolved_node = yy_union_lookup(yylval, yychar);
		if (resolved_node)
			delete resolved_node;
	}
}

/*---------------------------------------------------------------------------*/
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

	for yacc:
	short* yyss;		// state stack base
	short* yyssp;		// state stack pointer
	YYSTYPE* yyvs;		// value stack base
	YYSTYPE* yyvsp;		// value stack pointer
	YYSTYPE yylval;		// the last token received

	for bison:
	union yyalloc { short yyss; YYSTYPE yyvs; };
	short* yyss;		// is LOCAL to yyparse! (inaccecssible)
	YYSTYPE* yyvs;		// is LOCAL to yyparse! (inaccecssible)
	// all useful variables localized...

	// HACK TIME: hack the prototype for yyerror, 
	// and pass relevant pointers as arguments.  
*/

#if YYBISON
// shit... bison-isms: names were changed to protect the innocent
#define	yyname		yytname
#define	yysindex	yypact
#define	yyrindex	yydefact
/**
	#define	YYMAXTOKEN	YYMAXUTOK
	use MAXIMUM_BOGOSITY instead for bounding yychar
**/
#define	YYTABLESIZE	YYLAST
#endif

// following prototype MUST appear as is (after "static") for awk hack...
static
void yyerror(const char* msg) { 	// ancient compiler rejects
	const short* s;
	const YYSTYPE* v;
	node* resolved_node = NULL;
	// msg is going to be "syntax error" from y.tab.cc
	//	very useless in general
	cerr << "parse error: " << msg << endl;

/*	Define the following (-D) to disable sophisticated error reporting, 
 *	useful for bogus compilations.
 */
#if	!defined(NO_FANCY_YYERROR)
	// we've kept track of the position of every token
	cerr << "parser stacks:" << endl << "state\tvalue" << endl;

	/* bug fix: bad memory address with first *(s-1) if v is not
	 * guaranteed to be NULL, which it isn't!
	 * Thus we start with yyss+1, yyvs+1 for first valid stack entry.
	 */
	assert(!*yyss);		/* should be zero */
	cerr << *yyss;
	s=yyss+1;
	v=yyvs+1;
	for ( ; s <= yyssp && v <= yyvsp; s++, v++) {
		// how do we know which union member?
		// need to look at the state stack, and the transition
		// from the previous state
		/* assert(v); can have NULL on stack? yes. */
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
//	assert(resolved_node);
//	NULL check not necessarily valid if last token normally returned NULL
	if (at_eof()) {
		cerr << "\t" << yyname[0];	// "end-of-file"
	} else {
//		The last token from the lexer, yychar, tells us the last
//		token type returned.  
//		can't use: yy_union_resolve(yylval, *(s-1), *s);

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
	int accept_count = 0;
	{
		int yychar;
		int yyn;
		for (yychar = 0; yychar <= MAXIMUM_BOGOSITY; yychar++) {
			// try all terminal tokens
			if ((yyn = yysindex[*yyssp]) && 
					(yyn += yychar) >= 0 && 
					yyn <= YYTABLESIZE && 
					yycheck[yyn] == yychar) {
				cerr << '\t' << yyname[yychar]
					<< " (shift)" << endl;
				accept_count++;
			} else if ((yyn = yyrindex[*yyssp]) && 
					(yyn += yychar) >= 0 && 
					yyn <= YYTABLESIZE && 
					yycheck[yyn] == yychar) {
				cerr << '\t' << yyname[yychar]
					<< " (reduce)" << endl;
				accept_count++;
			}
		}
	}

	yyfreestacks(yyss, yyssp, yyvs, yyvsp, yylval);

#endif	// NO_FAKE_PREFIX

/***
	Dr. Fancy-Pants says:
	If there can be only one possible token, such as a terminating
	semicolon, then automatically fill it in or push NULL onto the 
	symbol stack, and proceed as if nothing happened.  
	Just remember that there was an error in the first place.  

	if (accept_count == 1) {
		...
	}
	
	The Oracle says:
	Multiple possible tokens? examine context and read the 
	programmer's mind to guess what was intended.  Duh!

	else if (accept_count < too_many) {
		...
	}
***/
	
	// or throw exception
	THROW_EXIT;
}

