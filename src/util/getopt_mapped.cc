/**
	\file "util/getopt_mapped.cc"
	$Id: getopt_mapped.cc,v 1.1 2006/07/30 05:50:14 fang Exp $
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
       if (isprint(optopt))
		os << "Unknown option `-" << char(o) << "'." << endl;
	else os << "Unknown option character `" <<
		reinterpret_cast<void*>(o) << "'." << endl;
}

//=============================================================================
}	// end namespace util

