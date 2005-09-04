/**
	\file "util/string_fwd.h"
	Forward declarations of string class.  
	$Id: string_fwd.h,v 1.3 2005/09/04 21:15:08 fang Exp $
 */

#ifndef	__UTIL_STRING_FWD_H__
#define	__UTIL_STRING_FWD_H__

#include "config.h"

// For shame! including an internal header!
// Better let configure check for it!
#if defined(HAVE_BITS_STRINGFWD_H) && HAVE_BITS_STRINGFWD_H
#include <bits/stringfwd.h>
#else
// otherwise it is safe to just include the whole <string> header
#include <string>
#endif	// HAVE_BITS_STRINGFWD_H

#endif	// __UTIL_STRING_FWD_H__

