/**
	"art_parser_node_position.h"
	Node whose sole purpose is to record a position of a token.  
	$Id: art_parser_node_position.h,v 1.2.18.2 2005/04/09 23:09:53 fang Exp $
 */

#ifndef	__ART_PARSER_NODE_POSITION_H__
#define	__ART_PARSER_NODE_POSITION_H__

#include <cstring>
#include "art_parser_base.h"
#include "memory/chunk_map_pool_fwd.h"

namespace ART {
namespace parser {
using util::memory::chunk_map_pool;
using lexer::token_position;

//=============================================================================
/**
	Generic class containing information about a token's position.  
	sizeof this object == 16, includes text field.
 */
class node_position : public node {
	typedef	node_position		this_type;
public:
	const line_position		position;
	char				text[4];
public:
	node_position(const char* s, const line_position& p) :
		node(), position(p) {
		strncpy(text, s, 4);
	}

	node_position(const char* s, const token_position& p) :
		node(), position(p.line, p.col) {
		strncpy(text, s, 4);
	}

	~node_position() { }

	ostream&
	what(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

#if 0
	line_range
	where(void) const;
#endif

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)

};	// end class node_position

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_NODE_POSITION_H__

