/**
	\file "art_parser_node_position.cc"
	$Id: art_parser_node_position.cc,v 1.4 2005/05/10 04:51:08 fang Exp $
 */

#include <iostream>
#include "AST/art_parser_node_position.h"
#include "util/memory/chunk_map_pool.tcc"
#include "util/what.h"

namespace util {
	SPECIALIZE_UTIL_WHAT(ART::parser::node_position, "node_position")
	SPECIALIZE_UTIL_WHAT(ART::parser::keyword_position, "keyword_position")
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
}	// end namespace ART

