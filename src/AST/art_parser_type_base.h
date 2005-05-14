/**
	\file "AST/art_parser_type.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_type_base.h,v 1.3.4.1 2005/05/14 22:38:37 fang Exp $
 */

#ifndef __AST_ART_PARSER_TYPE_BASE_H__
#define __AST_ART_PARSER_TYPE_BASE_H__

#include "AST/art_parser_base.h"

namespace ART {
namespace entity {
	class definition_base;
}
namespace parser {
using entity::definition_base;
//=============================================================================
/**
	Abstract base class for types in general (parameters, data, channel, 
	process...)
	Does not include any template parameters.  
 */
class type_base {
#define	USE_NEW_TYPE_BASE_CHECK				1

public:
#if USE_NEW_TYPE_BASE_CHECK
	typedef	never_ptr<const definition_base>	return_type;
#else
	typedef	never_ptr<const object>			return_type;
#endif

public:
	type_base() { }

virtual	~type_base() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;

#if USE_NEW_TYPE_BASE_CHECK
#define	TYPE_BASE_CHECK_PROTO						\
	type_base::return_type						\
	check_definition(context&) const
#else
#define	TYPE_BASE_CHECK_PROTO						\
	type_base::return_type						\
	check_build(context&) const
#endif

	/**
		Should return valid pointer to a fundamental type definition, 
		parameter, data, channel, or process.  
	 */
virtual	TYPE_BASE_CHECK_PROTO = 0;
};	// end class type_base

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __AST_ART_PARSER_TYPE_BASE_H__

