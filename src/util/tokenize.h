/**
	\file "util/tokenize.h"
	String tokenization facilities. 
	A poor-man's command-line lexer.  
	TODO: add support for quotation protection, escape sequences within...
	TODO: consider passing stateful predicate functors...
	$Id: tokenize.h,v 1.5 2011/02/14 08:03:25 fang Exp $
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

/**
	Separate using a single character.
 */
extern
void
tokenize_char(const std::string&, string_list&, const char);

}	// end namespace util

#endif	// __UTIL_TOKENIZE_H__

