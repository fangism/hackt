/**
	\file "art_parser_definition_item.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_definition_item.h,v 1.2 2005/02/24 06:17:05 fang Exp $
 */

#ifndef __ART_PARSER_DEFINITION_ITEM_H__
#define __ART_PARSER_DEFINITION_ITEM_H__

#include "art_parser_base.h"

namespace ART {
namespace parser {
//=============================================================================
/**
	Abstract base class for items that may be found in a definition body.  
	All definition body items are root_item.
	Except language_body...
 */
class def_body_item : virtual public node {
public:
	def_body_item() : node() { }

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
/// definition body is just a list of definition items
typedef	node_list<const def_body_item>	def_body_item_list;

class definition_body : public def_body_item_list {
protected:
	typedef	def_body_item_list		parent;
public:
	definition_body();

	explicit
	definition_body(const def_body_item* d);

	~definition_body();

};	// end class definition_body

#define definition_body_wrap(b,l,e)					\
	IS_A(definition_body*, l->wrap(b,e))
#define definition_body_append(l,d,n)					\
	IS_A(definition_body*, l->append(d,n))

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
	excl_ptr<const token_keyword>	tag;	///< what language
public:
	explicit
	language_body(const token_keyword* t);

virtual	~language_body();

virtual language_body*
	attach_tag(token_keyword* t);

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

