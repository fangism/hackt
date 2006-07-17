/**
	\file "AST/definition_item.h"
	Base set of classes for the HAC parser.  
	$Id: definition_item.h,v 1.3 2006/07/17 02:53:32 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_definition_item.h,v 1.7.48.1 2005/12/11 00:45:04 fang Exp
 */

#ifndef __HAC_AST_DEFINITION_ITEM_H__
#define __HAC_AST_DEFINITION_ITEM_H__

#include "AST/common.h"
#include "AST/node_list.h"

namespace HAC {
namespace parser {
//=============================================================================
/**
	Abstract base class for items that may be found in a definition body.  
	All definition body items are root_item.
	Except language_body...
 */
class def_body_item {
public:
	def_body_item() { }

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
/**
	Abstract base class for language bodies.  
	language_body is the only subclass of def_body_item that is 
	not also a subclass of root_item.  
	Language bodies cannot syntactically or semantically appear
	outside of a definition.  
 */
class language_body : public def_body_item {
protected:
	excl_ptr<const generic_keyword_type>	tag;	///< what language
public:
	explicit
	language_body(const generic_keyword_type* t);

virtual	~language_body();

virtual language_body*
	attach_tag(generic_keyword_type* t);

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const;

virtual	line_position
	rightmost(void) const = 0;

virtual	ROOT_CHECK_PROTO = 0;
};	// end class language_body

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_DEFINITION_ITEM_H__

