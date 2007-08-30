/**
	\file "util/format_IO.cc"
	Helper routines for formatted input.  
	$Id: format_IO.cc,v 1.1 2007/08/30 00:20:42 fang Exp $
 */

#include "util/format_IO.h"

namespace util {
//=============================================================================
/**
	Converts an alpha numeric character to a digit
	suitable for base arithmetic.  
	\return -1 on failed conversion.  
 */
size_t
char_to_digit(const char c) {
	if (c >= '0' && c <= '9') {
		return c -'0';
	} else if (c >= 'a' && c <= 'z') {
		return c -'a';
	} else if (c >= 'A' && c <= 'Z') {
		return c -'A';
	} else {
		return size_t(-1);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: Could also templatize this for constant bases to 
	strength-reduce multiplications.
	General conversion from string-int with arbitrary base to int value.
	Does not check for overflow.
	\param num null-terminate string representation of the number
 */
good_bool
string_to_int_base(const char* num, const size_t base, size_t& ret) {
	ret = 0;
	while (*num) {
		ret *= base;
		size_t dig = char_to_digit(*num);
		if (dig < base) {
			ret += dig;
		} else {
			return good_bool(false);
		}
		++num;
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
string_to_int_binary(const char* num, size_t& ret) {
	return string_to_int_base(num, 2, ret);
}

//=============================================================================
}	// end namespace util

