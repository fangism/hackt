// "art_parser_debug.h"
// parser-specific debug switches
// only include this in compilation modules (.cc),
// not other header files if possible

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

