/**
	\file "AST/instance_base.h"
	Instance-related parser base classes for HAC.  
	$Id: instance_base.h,v 1.2 2007/11/26 08:27:27 fang Exp $
 */

#ifndef __HAC_AST_INSTANCE_BASE_H__
#define __HAC_AST_INSTANCE_BASE_H__

// #include "AST/common.h"
#include "AST/definition_item.h"

namespace HAC {
namespace parser {
//=============================================================================
/**
	Base class for instance-related items, including declarations, 
	arrays, connections and aliases, conditionals, loops.  
 */
class instance_management : public def_body_item {
public:
	instance_management() : def_body_item() { }

virtual	~instance_management() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;

virtual	ROOT_CHECK_PROTO = 0;
};	// end class instance_management

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_INSTANCE_BASE_H__

