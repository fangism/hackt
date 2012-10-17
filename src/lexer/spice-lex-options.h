/**
	\file "lexer/spice-lex-options.h"
	Preprocessor definitions to give to lexer file to suppress
	undefined macro evaluation warnings.  
	This is only needed because I turned on -Wundef for all 
	translation units.  Can you say "anal-retentive?"
	$Id: spice-lex-options.h,v 1.9 2008/03/21 00:20:17 fang Exp $
	This file was renamed from the following in prehistory:
	Id: art++-lex-options.h,v 1.2 2005/06/19 01:58:50 fang Exp
 */

#ifndef	__LEXER_SPICE_LEX_OPTIONS_H__
#define	__LEXER_SPICE_LEX_OPTIONS_H__

#include "config.h"

/* this needs to be consistent with the %option never-interactive */
#ifndef	YY_ALWAYS_INTERACTIVE
#define	YY_ALWAYS_INTERACTIVE		0
#endif

#if 0
/* this is covered by %option nostack */
#ifndef	YY_STACK_USED
#define	YY_STACK_USED			0
#endif
#endif

#if 0
/* this is covered by %option nomain */
#ifndef	YY_MAIN
#define	YY_MAIN				0
#endif
#endif

/**
	Define this to pass in additional parameters.  
	Coordinate with yylex's YY_DECL in "lex/spice-lex-options.h"

	We now provide the lexer's state to the lex routine, 
	in pursuit of more pure/reentrant scanning.  
	flex::lexer_state is defined in "lexer/flex_lexer_state.h".
	The name "foo" must match that used in the
	"lexer/purify_flex.awk" script and the name used in
	the "lexer/spice-lex.ll" lexer specification.  
 */
#define	YYLEX_PARAM	flex::lexer_state& foo

#ifndef	YY_DECL
#ifdef	YYLEX_PARAM
#define	YY_DECL		int	__spice_lex (YYSTYPE* spice_lval, YYLEX_PARAM)
#else
#define	YY_DECL		int	__spice_lex (YYSTYPE* spice_lval)
#endif
#endif

/**
	yacc hardcodes the call to yylex() without arguments
	so we have to hack it here.  
	Coordinate this name with YY_DECL in "lex/spice-lex-options.h"
 */
#ifdef	yylex
#undef	yylex
#endif

#if defined(USING_YACC) || defined(USING_BYACC)
#define	yylex()			__spice_lex(&yylval, _lexer_state)
#define	spice_lex()		__spice_lex(&spice_lval, _lexer_state)
#endif	/* USING_YACC || USING_BYACC */

#if defined(USING_BISON)
#ifdef	YYLEX_PARAM
/**
YYLEX_PARAM is a declarator, so we have to manually pass
in the correct argument name.
**/
#ifdef	YYBISON_VERSION
/**
only bison-2.1 and up define YYBISON_VERSION (as a "string", incidentally)
bison-2.1 defines YYLEX differently than 2.0, *&^%$#@!
**/
#define	yylex(x,y)		__spice_lex(x, _lexer_state)
#define	spice_lex(x,y)		__spice_lex(x, _lexer_state)
#else	/* YYBISON_VERSION */
#define	yylex(x,y)		__spice_lex(x, _lexer_state)
#define	spice_lex(x,y)		__spice_lex(x, _lexer_state)
#endif	/* YYBISON_VERSION */
#else	/* ifdef YYLEX_PARAM */
#define	yylex			__spice_lex
#define	spice_lex		__spice_lex
#endif	/* YYLEX_PARAM */
#endif	/* USING_BISON */


#endif	/* __LEXER_SPICE_LEX_OPTIONS_H__ */

