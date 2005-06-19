/**
	\file "lexer/art++-lex-options.h"
	Preprocessor definitions to give to lexer file to suppress
	undefined macro evaluation warnings.  
	This is only needed because I turned on -Wundef for all 
	translation units.  Can you say "anal-retentive?"
	$Id: art++-lex-options.h,v 1.2 2005/06/19 01:58:50 fang Exp $
 */

#ifndef	__ARTXX_LEX_OPTIONS_H__
#define	__ARTXX_LEX_OPTIONS_H__

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

#endif	// __ARTXX_LEX_OPTIONS_H__

