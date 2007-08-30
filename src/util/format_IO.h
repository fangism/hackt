/**
	\file "util/format_IO.h"
	Helper routines for formatted input.  
	$Id: format_IO.h,v 1.1 2007/08/30 00:20:43 fang Exp $
 */

#ifndef	__UTIL_FORMAT_IO_H__
#define	__UTIL_FORMAT_IO_H__

#include "util/size_t.h"
#include "util/boolean_types.h"

namespace util {

extern
size_t
char_to_digit(const char);

extern
good_bool
string_to_int_base(const char*, const size_t, size_t&);

extern
good_bool
string_to_int_binary(const char*, size_t&);

}	// end namespace util

#endif	// __UTIL_FORMAT_IO_H__

