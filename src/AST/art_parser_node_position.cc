/**
	\file "art_parser_node_position.cc"
	$Id: art_parser_node_position.cc,v 1.1.2.1 2005/02/25 07:23:56 fang Exp $
 */

#include "art_parser_node_position.h"
#include "memory/list_vector_pool.tcc"

namespace ART {
namespace parser {
//=============================================================================
// class node_position method definitions

node_position::~node_position() { }

line_position
node_position::leftmost(void) const { return position; }

line_position
node_position::rightmost(void) const { return position; }

}	// end namespace parser
}	// end namespace ART

