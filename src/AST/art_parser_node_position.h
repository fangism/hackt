/**
	"art_parser_node_position.h"
	Node whose sole purpose is to record a position of a token.  
	$Id: art_parser_node_position.h,v 1.2.18.1 2005/03/12 03:43:07 fang Exp $
 */

#ifndef	__ART_PARSER_NODE_POSITION_H__
#define	__ART_PARSER_NODE_POSITION_H__

#include "art_parser_base.h"
#include "memory/chunk_map_pool_fwd.h"

namespace ART {
namespace parser {
using util::memory::chunk_map_pool;

//=============================================================================
/**
	Generic class containing information about a token's position.  
 */
class node_position : public node {
	typedef	node_position		this_type;
public:
	const line_position		position;

public:
	explicit
	node_position(const line_position& p) : node(), position(p) { }

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

