/**
	\file "util/tokenize.hh"
	String tokenization facilities. 
	A poor-man's command-line lexer.  
	TODO: add support for quotation protection, escape sequences within...
	TODO: consider passing stateful predicate functors...
	TODO: regex!
	$Id: tokenize.hh,v 1.6 2011/05/07 03:43:46 fang Exp $
 */

#ifndef	__UTIL_TOKENIZE_H__
#define	__UTIL_TOKENIZE_H__

#include "util/tokenize_fwd.hh"

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

/**
	Separate using a single (possibly multichar) string.
 */
extern
void
tokenize_string(const std::string&, const std::string&, string_list&);

/**
	Empty tokens are replaced with two instances of the
	separator to preserve the text.
	e.g. ".." is preserved, but ".xyz." is split,
		"//" is preserved, but "/" is split.
	This is really just a hack.
	Should ues a real parser instead.
 */
extern
void
tokenize_single_char_only(const std::string&, string_list&, const char);

}	// end namespace util

#endif	// __UTIL_TOKENIZE_H__

