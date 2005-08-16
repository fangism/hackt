/**
	\file "util/string_fwd.h"
	Forward declarations of string class.  
	$Id: string_fwd.h,v 1.2.28.1 2005/08/16 03:50:23 fang Exp $
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

