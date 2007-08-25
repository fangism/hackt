/**
	\file "util/stream_ptr.cc"
	Explicit template instantiations for stream pointer classes.
	$Id: stream_ptr.cc,v 1.1.2.1 2007/08/25 19:55:55 fang Exp $
 */

#include "util/stream_ptr.h"
#include <fstream>
#include "util/memory/count_ptr.tcc"

namespace util {
namespace memory {
using std::ifstream;
using std::ofstream;

template class count_ptr<ifstream>;
template class count_ptr<ofstream>;
// add more as they are needed

}	// end namespace memory
}	// end namespace util

