/**
	\file "lexer/hackt-lex-options.h"
	Preprocessor definitions to give to lexer file to suppress
	undefined macro evaluation warnings.  
	This is only needed because I turned on -Wundef for all 
	translation units.  Can you say "anal-retentive?"
	$Id: hackt-lex-options.h,v 1.2.2.2 2005/11/11 02:43:27 fang Exp $
	This file was renamed from the following in prehistory:
	Id: art++-lex-options.h,v 1.2 2005/06/19 01:58:50 fang Exp
 */

#ifndef	__LEXER_HACKT_LEX_OPTIONS_H__
#define	__LEXER_HACKT_LEX_OPTIONS_H__

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

#ifndef	YY_DECL
#ifdef	YYLEX_PARAM
#define	YY_DECL		int	__hackt_lex (YYSTYPE* hackt_lval, YYLEX_PARAM)
#else
#define	YY_DECL		int	__hackt_lex (YYSTYPE* hackt_lval)
#endif
#endif

// #undef	yylex

/**
	Define this to pass in additional parameters.  
	Coordinate with yylex's YY_DECL in "lex/hackt-lex-options.h"
 */
#undef	YYLEX_PARAM

/**
	yacc hardcodes the call to yylex() without arguments
	so we have to hack it here.  
	Coordinate this name with YY_DECL in "lex/hackt-lex-options.h"
 */
#if USING_YACC
#ifdef	yylex
#undef	yylex
#endif
#define	yylex()			__hackt_lex(&yylval)
#define	hackt_lex()		__hackt_lex(&hackt_lval)
#endif

#if USING_BISON
#define	yylex			__hackt_lex
#define	hackt_lex		__hackt_lex
#endif


#endif	// __LEXER_HACKT_LEX_OPTIONS_H__

