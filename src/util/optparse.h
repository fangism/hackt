/**
	\file "util/optparse.h"
	Simple option parsing library.
	Simply tokenizes into string sets, which is then handed off
	elsewhere for processing.
	Options can come from file or command-line argv.
	$Id: optparse.h,v 1.1.2.1 2009/08/27 20:38:50 fang Exp $
 */

#ifndef	__UTIL_OPTPARSE_H__
#define	__UTIL_OPTPARSE_H__

#include <iosfwd>
#include <string>
#include <list>

namespace util {
using std::string;
using std::list;
using std::ostream;
using std::istream;

//=============================================================================
/**
	Storage structure for options.
 */
struct option_value {
	string		key;
	list<string>	values;

	bool
	empty(void) const { return key.empty(); }
};	// end struct option_value

typedef	list<option_value>		option_value_list;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
operator << (ostream&, const option_value&);

ostream&
operator << (ostream&, const option_value_list&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Parses a single option of the form:
	key=value,value,...
 */
extern
option_value
optparse(const string&, const char e, const char s);

extern
option_value
optparse(const string&, const char s);

extern
option_value
optparse(const string&);

/**
	This variant does not attempt to tokenize values into a list.
 */
extern
option_value
optparse_no_sep(const string&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Parse a space-separated sequence of option values.  
	e.g. key=value key=value key=value
 */
#if 0
extern
option_value_list
optparse_list(const string&, const char s);
#endif

extern
option_value_list
optparse_list(const string&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern
option_value_list
optparse_file(istream&);

//=============================================================================
}	// end namespace util

#endif	// __UTIL_OPTPARSE_H__

