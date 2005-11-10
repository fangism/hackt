/**
	\file "parser/hackt-parse-options.h"
	Preprocessor definitions to give to lexer file to suppress
	undefined macro evaluation warnings.  
	This is only needed because I turned on -Wundef for all 
	translation units.  Can you say "anal-retentive?"
	$Id: hackt-parse-options.h,v 1.2.2.1 2005/11/10 23:11:53 fang Exp $
	This file was formerly known as:
	Id: art++-parse-options.h,v 1.2 2005/06/19 01:58:50 fang Exp
 */

#ifndef	__PARSER_HACKT_PARSE_OPTIONS_H__
#define	__PARSER_HACKT_PARSE_OPTIONS_H__

#include "config.h"

// this could be configure-dependent!  
// since alloca is rather efficient, but sometimes not portable
#ifndef	YYSTACK_USE_ALLOCA
#define	YYSTACK_USE_ALLOCA		0
#endif

// some yacc/bison implementations will choose some reasonable
// default value if you predefine to 0
/***
	NB: bison-1.875 default differs from bison-2.0.
	With bison-1.875 if undefined macros were assumed to be 0, 
		then the code geneated would set it so some default
		if the value was 0.
	With bison-2.0, it chooses a default value if macro is
		*undefined*, not 0.
	To make both work we choose a reasonable value ourselves.  
***/
#ifndef	YYMAXDEPTH
#define	YYMAXDEPTH			1024
#endif

// introduced and used by bison-2.0
#ifndef	YYLTYPE_IS_TRIVIAL
#define	YYLTYPE_IS_TRIVIAL		0
#endif

// needed by bison-1.35
// to determine whether or not yylloc is used in the call to yylex
#ifndef	YYLSP_NEEDED
#define	YYLSP_NEEDED			0
#endif

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

#endif	// __PARSER_HACKT_PARSE_OPTIONS_H__

