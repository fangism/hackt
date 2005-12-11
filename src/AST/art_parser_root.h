/**
	\file "AST/art_parser_root.h"
	Base set of classes for the HAC parser.  
	$Id: art_parser_root.h,v 1.8.48.1 2005/12/11 00:45:10 fang Exp $
 */

#ifndef __AST_HAC_PARSER_ROOT_H__
#define __AST_HAC_PARSER_ROOT_H__

#include "AST/art_parser_base.h"
#include "AST/art_parser_node_list.h"

namespace HAC {
namespace parser {
//=============================================================================
/**
	Abstract base class for root-level items.  
	Root-level items include statements that can be found
	in namespaces.  
	Assertion: all root items are nonterminals.  
 */
class root_item {
public:
	root_item() { }

virtual	~root_item() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;

virtual	ROOT_CHECK_PROTO = 0;

};	// end class root_item

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef node_list<const root_item>	root_item_list;

/** list of root_items */
class root_body : public root_item_list {
protected:
	typedef	root_item_list			parent;
public:
	root_body();

	explicit
	root_body(const root_item* r);

	~root_body();
};	// end class root_body

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __AST_HAC_PARSER_ROOT_H__

