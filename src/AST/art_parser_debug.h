/**
	\file "art_parser_debug.h"
	Parser-specific debug switches.
	Only include this in compilation modules (.cc) that need debugging,
	and not in any header files if possible.
	$Id: art_parser_debug.h,v 1.3 2004/11/02 07:51:36 fang Exp $
 */

#ifndef	__ART_PARSER_DEBUG_H__
#define	__ART_PARSER_DEBUG_H__

#include "art_debug.h"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

// use this to turn on/off
#define DEBUG_CHECK_BUILD	1		// {0,1,2}, ok to change
	// 0: off, 1: depends on MASTER_DEBUG_LEVEL, 2: force on
	// force on overrides all other conditions

#if	(DEBUG_CHECK_BUILD == 1)
  #define	TRACE_CHECK_BUILD_LEVEL	5	// ok to change, default 5
  #define	TRACE_CHECK_BUILD(stmt)	DEBUG(TRACE_CHECK_BUILD_LEVEL,stmt)
#elif	(DEBUG_CHECK_BUILD == 2)
// force on
  #define	TRACE_CHECK_BUILD(stmt)	stmt
#elif	(DEBUG_CHECK_BUILD == 0)
// turn off
  #define	TRACE_CHECK_BUILD_LEVEL	MASTER_DEBUG_LEVEL
  #define	TRACE_CHECK_BUILD(stmt)
#else
  #error	"invalid DEBUG_CHECK_BUILD value."
#endif


//=============================================================================

#endif	// __ART_PARSER_DEBUG_H__

