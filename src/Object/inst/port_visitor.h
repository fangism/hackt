/**
	\file "Object/inst/port_visitor.h"
	$Id: port_visitor.h,v 1.1 2009/02/28 01:20:43 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PORT_VISITOR_H__
#define	__HAC_OBJECT_INST_PORT_VISITOR_H__

#include "Object/inst/alias_visitor.h"

namespace HAC {
namespace entity {

/**
	General visitor that recursively visits public port subinstances.
 */
struct port_visitor : public alias_visitor {

// default ctor / dtor suffice
//	port_visitor() { }
virtual	~port_visitor() { }

VISIT_INSTANCE_ALIAS_INFO_PROTOS(virtual)

};	// end struct port_visitor

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PORT_VISITOR_H__

