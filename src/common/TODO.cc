/**
	\file "common/TODO.cc"
	$Id: TODO.cc,v 1.1.2.1 2005/08/14 03:38:21 fang Exp $
 */

#include <iostream>
#include "common/TODO.h"

void
__finish_me_where(const char* who, const char* fun,
		const char* file, const size_t line) {
	std::cerr << who << ", finish writing " << fun << " at " <<
		file << ':' << line << std::endl;
}

