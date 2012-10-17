/**
	\file "util/stream_ptr.cc"
	Explicit template instantiations for stream pointer classes.
	$Id: stream_ptr.cc,v 1.2 2007/08/28 04:54:31 fang Exp $
 */

#include "util/stream_ptr.hh"
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

