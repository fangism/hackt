/**
	\file "parser/type-parse-options.h"
	Preprocessor definitions to give to lexer file to suppress
	undefined macro evaluation warnings.  
	This is only needed because I turned on -Wundef for all 
	translation units.  Can you say "anal-retentive?"
	$Id: type-parse-options.h,v 1.4 2008/03/21 00:20:32 fang Exp $
 */

#ifndef	__PARSER_TYPE_PARSE_OPTIONS_H__
#define	__PARSER_TYPE_PARSE_OPTIONS_H__

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

// needed for bison-2.1
#ifndef	YYENABLE_NLS
#define	YYENABLE_NLS			0
#endif
#ifndef	ENABLE_NLS
#define	ENABLE_NLS			0
#endif

// in the original source, a local variable yylval shares the same name
// as the parameter we wish to pass by reference.  
#if defined(USING_BISON) && !defined(LIBBOGUS)
#define	YYPARSE_PARAM		null, YYSTYPE& type_lval, flex::lexer_state& _lexer_state
#endif

#if defined(YYBYACC)
#if defined(USING_YACC)
// we need more drastic measure to hack the prototype...
#endif
#if defined(USING_BYACC)
// ?
#endif
// some versons of byacc use YYPARSE_PARAM_TYPE
#define	YYPARSE_PARAM
#define	YYPARSE_PARAM_TYPE	void*, YYSTYPE& type_lval, flex::lexer_state& _lexer_state
#endif	// YYBYACC

#endif	// __PARSER_TYPE_PARSE_OPTIONS_H__

