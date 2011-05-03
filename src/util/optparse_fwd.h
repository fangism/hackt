/**
	\file "util/optparse_fwd.h"
	key-values option structure
	$Id: optparse_fwd.h,v 1.2 2011/05/03 19:21:07 fang Exp $
 */

#ifndef	__UTIL_OPTPARSE_FWD_H__
#define	__UTIL_OPTPARSE_FWD_H__

#include <string>
#include <list>

namespace util {

//=============================================================================
/**
	Storage structure for options.
 */
struct option_value {
	std::string			key;
	std::list<std::string>		values;

	bool
	empty(void) const { return key.empty(); }
};	// end struct option_value

typedef	std::list<option_value>		option_value_list;

//=============================================================================
}	// end namespace util

#endif	// __UTIL_OPTPARSE_FWD_H__

