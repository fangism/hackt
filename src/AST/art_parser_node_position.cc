/**
	\file "art_parser_node_position.cc"
	$Id: art_parser_node_position.cc,v 1.2.18.2 2005/04/09 23:09:53 fang Exp $
 */

#include <iostream>
#include "art_parser_node_position.h"
#include "memory/chunk_map_pool.tcc"
#include "what.h"

namespace util {
	SPECIALIZE_UTIL_WHAT(ART::parser::node_position, "node_position")
}

namespace ART {
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
}	// end namespace parser
}	// end namespace ART

