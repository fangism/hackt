/**
	\file "util/tokenize.h"
	String tokenization facilities. 
	A poor-man's command-line lexer.  
	TODO: add support for quotation protection, escape sequences within...
	TODO: consider passing stateful predicate functors...
	$Id: tokenize.h,v 1.2.146.1 2009/08/27 20:38:52 fang Exp $
 */

#ifndef	__UTIL_TOKENIZE_H__
#define	__UTIL_TOKENIZE_H__

#include "util/tokenize_fwd.h"

namespace util {

// separate a string by whitespace (quick and dirty)
extern void
tokenize(const std::string&, string_list&);

/**
	Pass in a different set of separation characters.
 */
extern void
tokenize(const std::string&, string_list&, const char*);

}	// end namespace util

#endif	// __UTIL_TOKENIZE_H__

