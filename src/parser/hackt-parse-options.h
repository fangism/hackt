/**
	\file "parser/hackt-parse-options.h"
	Preprocessor definitions to give to lexer file to suppress
	undefined macro evaluation warnings.  
	This is only needed because I turned on -Wundef for all 
	translation units.  Can you say "anal-retentive?"
	$Id: hackt-parse-options.h,v 1.1.2.1 2005/11/04 22:23:30 fang Exp $
	This file was formerly known as:
	Id: art++-parse-options.h,v 1.2 2005/06/19 01:58:50 fang Exp
 */

#ifndef	__PARSER_HACKT_PARSE_OPTIONS_H__
#define	__PARSER_HACKT_PARSE_OPTIONS_H__

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

#endif	// __PARSER_HACKT_PARSE_OPTIONS_H__

