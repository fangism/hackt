/**
	\file "art_parser_type.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_type_base.h,v 1.1.22.2 2005/05/04 17:23:20 fang Exp $
 */

#ifndef __ART_PARSER_TYPE_BASE_H__
#define __ART_PARSER_TYPE_BASE_H__

#include "art_parser_base.h"

namespace ART {
namespace parser {
//=============================================================================
/**
	Abstract base class for types in general (parameters, data, channel, 
	process...)
	Does not include any template parameters.  
 */
class type_base {
public:
	type_base() { }

virtual	~type_base() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;

	/**
		Should return valid pointer to a fundamental type definition, 
		parameter, data, channel, or process.  
	 */
virtual	never_ptr<const object>
	check_build(context& c) const = 0;
};	// end class type_base

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_TYPE_BASE_H__

