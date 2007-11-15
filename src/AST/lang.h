/**
	\file "AST/lang.h"
	Base set of classes for the HAC sub-languages.  
	$Id: lang.h,v 1.1.2.1 2007/11/15 23:48:39 fang Exp $
 */

#ifndef __HAC_AST_LANG_H__
#define __HAC_AST_LANG_H__

/**
	Define to 1 to define language body as subclass of 
	instance_management_base, otherwise derive from def_body_item.
	Goal: ? (1 is experimental)
 */
#define	INSTANCE_MANAGEMENT_LANG		1

#if	INSTANCE_MANAGEMENT_LANG
#include "AST/instance_base.h"
#else
#include "AST/definition_item.h"
#endif

namespace HAC {
namespace parser {
//=============================================================================
/**
	Abstract base class for language bodies.  
	language_body is the only subclass of def_body_item that is 
	not also a subclass of root_item.  
	Language bodies cannot syntactically or semantically appear
	outside of a definition.  
 */
class language_body :
#if	INSTANCE_MANAGEMENT_LANG
		public instance_management
#else
		public def_body_item
#endif
{
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

#endif	// __HAC_AST_LANG_H__

