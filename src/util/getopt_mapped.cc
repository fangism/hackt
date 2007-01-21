/**
	\file "util/getopt_mapped.cc"
	$Id: getopt_mapped.cc,v 1.2 2007/01/21 06:01:11 fang Exp $
 */

#include "util/getopt_mapped.h"
#include <iostream>

namespace util {
using std::ostream;
using std::endl;
//=============================================================================
/**     
	Standard bad option error message.  
 */     
void
unknown_option(ostream& os, const int o) {
       if (isprint(o))
		os << "Unknown option `-" << char(o) << "'." << endl;
	else os << "Unknown option character `" <<
		reinterpret_cast<void*>(o) << "'." << endl;
}

//=============================================================================
}	// end namespace util

