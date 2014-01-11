/**
	\file "util/graph/bare_digraph_fwd.hh"
	$Id: $
 */
#ifndef	__UTIL_GRAPH_BARE_DIGRAPH_FWD_HH__
#define	__UTIL_GRAPH_BARE_DIGRAPH_FWD_HH__

#include <set>
#include <vector>

namespace util {
namespace graph {

typedef std::vector<std::set<size_t> >			SCC_type;
class bare_digraph;

}	// end namespace graph
}	// end namespace util

#endif	// __UTIL_GRAPH_BARE_DIGRAPH_FWD_HH__
