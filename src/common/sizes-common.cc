/**
	\file "common/sizes-common.cc"
	$Id: sizes-common.cc,v 1.1 2007/02/27 02:27:57 fang Exp $
 */

#include <iostream>
#include <string>
#include <vector>
#include <valarray>
#include <map>
#include <list>
#include <set>
#include <cstdio>

#include "config.h"
#include "util/size_t.h"
#include "util/what.tcc"
#include "common/sizes-common.hh"

namespace HAC {
using std::ostream;
using std::endl;

//=============================================================================
ostream&
dump_class_sizes(ostream& o) {
	o << "class sizes:" << endl;
	__dump_class_size<void*>(o);
	__dump_class_size<short>(o);
	__dump_class_size<int>(o);
	__dump_class_size<size_t>(o);
	__dump_class_size<long>(o);
	__dump_class_size<float>(o);
	__dump_class_size<double>(o);
#if 0 && SIZEOF_LONG_DOUBLE
	__dump_class_size<long double>(o);
#endif
	__dump_class_size<FILE>(o);


	o << "STL containers:" << endl;
	__dump_class_size<std::string>(o);
	__dump_class_size<std::vector<size_t> >(o);
	__dump_class_size<std::valarray<size_t> >(o);
	__dump_class_size<std::list<size_t> >(o);
	__dump_class_size<std::map<std::string, void*> >(o);
	__dump_class_size<std::set<std::string> >(o);
	return o;
}

//=============================================================================
}	// end namespace HAC


