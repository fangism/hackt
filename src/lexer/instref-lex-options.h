/**
	\file "lexer/instref-lex-options.h"
	Preprocessor definitions to give to lexer file to suppress
	undefined macro evaluation warnings.  
	This is only needed because I turned on -Wundef for all 
	translation units.  Can you say "anal-retentive?"
	$Id: instref-lex-options.h,v 1.5 2008/03/21 00:20:18 fang Exp $
 */

#ifndef	__LEXER_INSTREF_LEX_OPTIONS_H__
#define	__LEXER_INSTREF_LEX_OPTIONS_H__

#include "config.h"
#include "parser/instref-prefix.h"

// this needs to be consistent with the %option never-interactive
#ifndef	YY_ALWAYS_INTERACTIVE
#define	YY_ALWAYS_INTERACTIVE		0
#endif

/**
	NOTE: the instref lexer need-not be pure/re-entrant

	Define this to pass in additional parameters.  
	Coordinate with yylex's YY_DECL in "lex/instref-lex-options.h"

	We now provide the lexer's state to the lex routine, 
	in pursuit of more pure/reentrant scanning.  
	flex::lexer_state is defined in "lexer/flex_lexer_state.h".
	The name "foo" must match that used in the
	"lexer/purify_flex.awk" script and the name used in
	the "lexer/instref-lex.ll" lexer specification.  
 */
#ifndef	LIBBOGUS
#define	YYLEX_PARAM	flex::lexer_state& foo
#endif

/**
	*GAG*
 */
#ifdef	YYLEX_PARAM
#define	__YYLEX_PARAM		, YYLEX_PARAM
#define	__YYLEX_PARAM_VOID	YYLEX_PARAM
#define	__YYLEX_ARG		, foo
#define	__YYLEX_ARG_VOID	foo
#define	AT_EOF_PARAM		const YYLEX_PARAM
#else
#define	__YYLEX_PARAM
#define	__YYLEX_PARAM_VOID	void
#define	__YYLEX_ARG
#define	__YYLEX_ARG_VOID
#define	AT_EOF_PARAM		void
#endif

#ifndef	YY_DECL
#ifdef	YYLEX_PARAM
#define	YY_DECL		int __instref_lex (YYSTYPE* yylval, YYLEX_PARAM)
#else
#define	YY_DECL		int __instref_lex (YYSTYPE* yylval)
#endif
#endif

#if 0
/**
	yacc hardcodes the call to yylex() without arguments
	so we have to hack it here.  
	Coordinate this name with YY_DECL in "lex/instref-lex-options.h"
 */
#ifdef	yylex
#undef	yylex
#endif
#endif

#ifndef	LIBBOGUS
// else don't bother

#if defined(USING_YACC) || defined(USING_BYACC)
#define	instref_lex()		__instref_lex(&instref_lval, _lexer_state)
#endif	// USING_YACC || USING_BYACC

#if defined(USING_BISON)
#ifdef	YYLEX_PARAM
// YYLEX_PARAM is a declarator, so we have to manually pass
// in the correct argument name.
// only bison-2.1 and up define YYBISON_VERSION (as a "string", incidentally)
// bison-2.1 defines YYLEX differently than 2.0, *&^%$#@!
#ifdef	YYBISON_VERSION
#define	instref_lex(x, y)	__instref_lex(x, _lexer_state)
#else	// YYBISON_VERSION
#define	instref_lex(x, y)	__instref_lex(x, _lexer_state)
#endif	// YYBISON_VERSION
#else	// ifdef YYLEX_PARAM
// #define	yylex			__instref_lex
#define	instref_lex		__instref_lex
#endif	// YYLEX_PARAM
#endif	// USING_BISON

#else	// defined(LIBBOGUS)
#if USING_BISON
#ifdef	YYBISON_VERSION
#define	instref_lex(x)		__instref_lex(&yylval)
#else	// YYBISON_VERSION
#define	instref_lex		__instref_lex
#endif	// YYBISON_VERSION
#else	// USING_BISON
#define	instref_lex()		__instref_lex(&yylval)
#endif	// USING BISON
#endif	// LIBBOGUS

#endif	// __LEXER_INSTREF_LEX_OPTIONS_H__

