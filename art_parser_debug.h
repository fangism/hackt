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
#define DEBUG_CHECK_BUILD	1		// bool, ok to change

#if	DEBUG_CHECK_BUILD
#define TRACE_CHECK_BUILD	5		// ok to change
#else
// turn off
#define TRACE_CHECK_BUILD	MASTER_DEBUG_LEVEL
#endif

//=============================================================================

#endif	// __ART_PARSER_DEBUG_H__

