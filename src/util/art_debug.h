/**
	\file "art_debug.h"
	Master debug flags.  
	All other module-specific debugging flags should derive
	from this.  
 */

#ifndef	__ART_DEBUG_H__
#define	__ART_DEBUG_H__

/***
	This file is used as the master debug-level control.  
	To override values defined in these files, just compile
	with -D<variable>=<value>.
	This file also defines globally used (and hopefully useful)
	debug macros.  
***/

// Enable this mode when performing regression tests.
// Setting this to true (!= 0) will (should) suppress debug messages.  
#ifndef REGRESSION_TEST_MODE
#define REGRESSION_TEST_MODE	0		// default is off
#endif

// The higher the level of debugging, the more detailed reporting to stderr.  
// Other local debug flags should be derived based on this.  
#if	REGRESSION_TEST_MODE
#undef	MASTER_DEBUG_LEVEL			// override whatever is passed
#define	MASTER_DEBUG_LEVEL	0		// DO NOT CHANGE
#elif	!defined(MASTER_DEBUG_LEVEL)
#define	MASTER_DEBUG_LEVEL	2		// default, ok to change this
#endif

#if	REGRESSION_TEST_MODE
#define	DEBUG(lvl, stmt)	;
#else
#define	DEBUG(lvl, stmt)						\
	if (MASTER_DEBUG_LEVEL > lvl) { stmt; }
#endif
// note: no semicolon needed after DEBUG, but it doesn't hurt either

//////////////////// some useful reference levels /////////////////////////////

#define DEBUG_BASIC		1


#endif	// __ART_DEBUG_H__

