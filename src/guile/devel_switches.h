/**
	\file "guile/devel_switches.h"
	Use of this file (ideally) should be confined to the guile directory.  

	$Id: devel_switches.h,v 1.1.2.2 2007/04/06 03:52:35 fang Exp $
 */

#ifndef	__HAC_GUILE_DEVEL_SWITCHES_H__
#define	__HAC_GUILE_DEVEL_SWITCHES_H__

// if we want project-wide switches
#if 0
#include "common/devel_switches.h"
#endif

// leave this error enabled for released code
#if 0
#error	Production code should NOT include this header file.  \
	However, if you are developing, use this file as you see fit.  
#endif
//=============================================================================

//=============================================================================

#endif	// __HAC_GUILE_DEVEL_SWITCHES_H__

