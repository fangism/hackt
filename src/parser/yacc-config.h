/**
	\file "parser/yacc-config.h"
	Configuration macros for yacc-style parser generator, 
	common across all parser files.  
	$Id: yacc-config.h,v 1.1 2010/04/27 18:33:22 fang Exp $
 */

#ifndef	__HAC_PARSER_YACC_CONFIG_H__
#define	__HAC_PARSER_YACC_CONFIG_H__

#include "config.h"

/** work-around for bison-1.875 and gcc-3.x, until bison is fixed **/
#if defined (__GNUC__) && (3 <= __GNUC__)
#define __attribute__(arglist)          /* empty */
#endif

/**
        When using yacc, this would otherwise be undefined.  
 */
#ifndef YYBISON
#define YYBISON         0
#endif

#if YYBISON
#if defined(USING_BISON) && !USING_BISON
#error  Inconsistency in configuration: YYBISON && !USING_BISON
#endif

#elif defined(YYBYACC) && YYBYACC

#if defined(USING_BYACC) && !USING_BYACC && !USING_YACC
// on many systems, yacc == byacc
#error  Inconsistency in configuration: YYBYACC && !USING_BYACC && !USING_YACC
#endif
#else   // !YYBISON && !YYBYACC

#if defined(USING_YACC) && !USING_YACC
#error  Inconsistency in configuration: !YYBISON && !YYBYACC && !USING_YACC
#endif
#endif	// YYBISON

#endif	// __HAC_PARSER_YACC_CONFIG_H__

