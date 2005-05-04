/**
	\file "art_parser_definition_item.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_definition_item.h,v 1.4.4.1 2005/05/04 05:06:29 fang Exp $
 */

#ifndef __ART_PARSER_DEFINITION_ITEM_H__
#define __ART_PARSER_DEFINITION_ITEM_H__

#include "art_parser_base.h"
#include "art_parser_node_list.h"

namespace ART {
namespace parser {
//=============================================================================
/**
	Abstract base class for items that may be found in a definition body.  
	All definition body items are root_item.
	Except language_body...
 */
class def_body_item
#if USE_MOTHER_NODE
	: virtual public node
#endif
{
public:
#if USE_MOTHER_NODE
	def_body_item() : node() { }
#else
	def_body_item() { }
#endif

virtual	~def_body_item() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;

virtual	never_ptr<const object>
	check_build(context& ) const = 0;
};	// end class def_body_item

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<const def_body_item>	def_body_item_list;

/// definition body is just a list of definition items
class definition_body : public def_body_item_list {
protected:
	typedef	def_body_item_list		parent;
public:
	definition_body();

	explicit
	definition_body(const def_body_item* d);

	~definition_body();

};	// end class definition_body

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
};	// end class language_body

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_DEFINITION_ITEM_H__

