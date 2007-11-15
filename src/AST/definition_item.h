/**
	\file "AST/definition_item.h"
	Base set of classes for the HAC parser.  
	$Id: definition_item.h,v 1.5.54.1 2007/11/15 23:48:35 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_definition_item.h,v 1.7.48.1 2005/12/11 00:45:04 fang Exp
 */

#ifndef __HAC_AST_DEFINITION_ITEM_H__
#define __HAC_AST_DEFINITION_ITEM_H__

#include "AST/root.h"
#include "util/what_fwd.h"

namespace HAC {
namespace parser {
//=============================================================================
/**
	Abstract base class for items that may be found in a definition body.  
	Now, all definition body items are root_item.
 */
class def_body_item : public root_item {
public:
	def_body_item() : root_item() { }

virtual	~def_body_item() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;

virtual	ROOT_CHECK_PROTO = 0;
};	// end class def_body_item

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

namespace util {
SPECIALIZE_UTIL_WHAT_DECLARATION(HAC::parser::def_body_item)
}

#endif	// __HAC_AST_DEFINITION_ITEM_H__

