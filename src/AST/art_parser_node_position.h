/**
	"art_parser_node_position.h"
	Node whose sole purpose is to record a position of a token.  
	$Id: art_parser_node_position.h,v 1.2 2005/02/27 22:11:58 fang Exp $
 */

#ifndef	__ART_PARSER_NODE_POSITION_H__
#define	__ART_PARSER_NODE_POSITION_H__

#include "art_parser_base.h"
#include "memory/list_vector_pool_fwd.h"

namespace ART {
namespace parser {

class node_position : public node {
public:
	const line_position		position;

public:
	explicit
	node_position(const line_position& p) : node(), position(p) { }

	~node_position();

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

};	// end class node_position

}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_NODE_POSITION_H__

