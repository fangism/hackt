/**
	\file "util/string_fwd.hh"
	Forward declarations of string class.  
	$Id: string_fwd.hh,v 1.4 2006/04/27 00:17:13 fang Exp $
 */

#ifndef	__UTIL_STRING_FWD_H__
#define	__UTIL_STRING_FWD_H__

#include "config.h"

/**
	For shame! including an internal header!
	Better let configure check for it!
	Gotcha: Intel's compiler doesn't have it, but configure may still
	pick it up in search paths, so we qualify with one more condition.  
 */
#if defined(HAVE_BITS_STRINGFWD_H) && HAVE_BITS_STRINGFWD_H && !defined(INTEL_COMPILER)
#include <bits/stringfwd.h>
#else
// otherwise it is safe to just include the whole <string> header
#include <string>
#endif	// HAVE_BITS_STRINGFWD_H

#endif	// __UTIL_STRING_FWD_H__

