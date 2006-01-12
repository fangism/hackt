/**
	\file "util/tokenize.h"
	String tokenization facilities. 
	A poor-man's command-line lexer.  
	TODO: add support for quotation protection, escape sequences within...
	TODO: consider passing stateful predicate functors...
	$Id: tokenize.h,v 1.1.2.2 2006/01/12 07:10:00 fang Exp $
 */

#ifndef	__UTIL_TOKENIZE_H__
#define	__UTIL_TOKENIZE_H__

#include "util/tokenize_fwd.h"

namespace util {

// separate a string by whitespace (quick and dirty)
extern void
tokenize(const std::string&, string_list&);

}	// end namespace util

#endif	// __UTIL_TOKENIZE_H__

