/**
	\file "common/TODO.cc"
	$Id: TODO.cc,v 1.2 2005/09/04 21:15:05 fang Exp $
 */

#include <iostream>
#include "common/TODO.hh"

void
__finish_me_where(const char* who, const char* fun,
		const char* file, const size_t line) {
	std::cerr << who << ", finish writing " << fun << " at " <<
		file << ':' << line << std::endl;
}

