/**
	\file "lexer/instref-lex-options.h"
	Preprocessor definitions to give to lexer file to suppress
	undefined macro evaluation warnings.  
	This is only needed because I turned on -Wundef for all 
	translation units.  Can you say "anal-retentive?"
	$Id: instref-lex-options.h,v 1.1.2.1 2006/01/14 06:52:02 fang Exp $
 */

#ifndef	__LEXER_INSTREF_LEX_OPTIONS_H__
#define	__LEXER_INSTREF_LEX_OPTIONS_H__

#include "config.h"

// this needs to be consistent with the %option never-interactive
#ifndef	YY_ALWAYS_INTERACTIVE
#define	YY_ALWAYS_INTERACTIVE		0
#endif

#if 0
// this is covered by %option nostack
#ifndef	YY_STACK_USED
#define	YY_STACK_USED			0
#endif
#endif

#if 0
// this is covered by %option nomain
#ifndef	YY_MAIN
#define	YY_MAIN				0
#endif
#endif

/**
	Define this to pass in additional parameters.  
	Coordinate with yylex's YY_DECL in "lex/instref-lex-options.h"

	We now provide the lexer's state to the lex routine, 
	in pursuit of more pure/reentrant scanning.  
	flex::lexer_state is defined in "lexer/flex_lexer_state.h".
	The name "foo" must match that used in the
	"lexer/purify_flex.awk" script and the name used in
	the "lexer/instref-lex.ll" lexer specification.  
 */
#define	YYLEX_PARAM	flex::lexer_state& foo

#ifndef	YY_DECL
#ifdef	YYLEX_PARAM
#define	YY_DECL		int __instref_lex (YYSTYPE* instref_lval, YYLEX_PARAM)
#else
#define	YY_DECL		int __instref_lex (YYSTYPE* instref_lval)
#endif
#endif

/**
	yacc hardcodes the call to yylex() without arguments
	so we have to hack it here.  
	Coordinate this name with YY_DECL in "lex/instref-lex-options.h"
 */
#ifdef	yylex
#undef	yylex
#endif

#if USING_YACC || USING_BYACC
#define	yylex()			__instref_lex(&yylval, _lexer_state)
#define	instref_lex()		__instref_lex(&instref_lval, _lexer_state)
#endif	// USING_YACC || USING_BYACC

#if USING_BISON
#ifdef	YYLEX_PARAM
// YYLEX_PARAM is a declarator, so we have to manually pass
// in the correct argument name.
#define	yylex(x,y)		__instref_lex(x, _lexer_state)
#define	instref_lex(x,y)		__instref_lex(x, _lexer_state)
#else
#define	yylex			__instref_lex
#define	instref_lex		__instref_lex
#endif
#endif	// USING_BISON


#endif	// __LEXER_INSTREF_LEX_OPTIONS_H__

