/**
	\file "art_parser_node_position.cc"
	$Id: art_parser_node_position.cc,v 1.2.18.1 2005/03/12 03:43:07 fang Exp $
 */

#include "art_parser_node_position.h"
#include "memory/chunk_map_pool.tcc"

namespace ART {
namespace parser {
//=============================================================================
// class node_position method definitions

#if 0
node_position::~node_position() { }
#endif

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

