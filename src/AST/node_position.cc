/**
	\file "AST/node_position.cc"
	$Id: node_position.cc,v 1.2.80.1 2007/01/16 04:57:28 fang Exp $
	This file used to be the following before it was renamed:
	$Id: node_position.cc,v 1.2.80.1 2007/01/16 04:57:28 fang Exp $
 */

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include "AST/node_position.h"
#include "util/memory/chunk_map_pool.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/what.h"

namespace util {
	SPECIALIZE_UTIL_WHAT(HAC::parser::node_position, "node_position")
	SPECIALIZE_UTIL_WHAT(HAC::parser::keyword_position, "keyword_position")
namespace memory {
// explicit template instantiation needed for -O3
	template class count_ptr<const HAC::parser::node_position>;
}
}

namespace HAC {
namespace parser {
//=============================================================================
// class node_position method definitions

#if 0
node_position::~node_position() { }
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
node_position::what(ostream& o) const {
	return o << text;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
node_position::leftmost(void) const { return position; }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
node_position::rightmost(void) const { return position; }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(node_position)

//=============================================================================
// class keyword_position method definitions

#if 0
keyword_position::~keyword_position() { }
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
keyword_position::what(ostream& o) const {
	return o << "keyword: " << text;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
keyword_position::leftmost(void) const { return position; }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
keyword_position::rightmost(void) const {
	line_position ret = position;
	ret.col += strlen(text) -1;
	// even empty string has size 1, from NUL termination
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(keyword_position)

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

