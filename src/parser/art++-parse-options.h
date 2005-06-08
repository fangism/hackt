/**
	\file "parser/art++-parse-options.h"
	Preprocessor definitions to give to lexer file to suppress
	undefined macro evaluation warnings.  
	This is only needed because I turned on -Wundef for all 
	translation units.  Can you say "anal-retentive?"
	$Id: art++-parse-options.h,v 1.1.4.2 2005/06/08 19:13:33 fang Exp $
 */

#ifndef	__ARTXX_PARSE_OPTIONS_H__
#define	__ARTXX_PARSE_OPTIONS_H__

// this could be configure-dependent!  
// since alloca is rather efficient, but sometimes not portable
#ifndef	YYSTACK_USE_ALLOCA
#define	YYSTACK_USE_ALLOCA		0
#endif

// some yacc/bison implementations will choose some reasonable
// default value if you predefine to 0
#ifndef	YYMAXDEPTH
#define	YYMAXDEPTH			0
#endif

#endif	// __ARTXX_PARSE_OPTIONS_H__

