/**
	\file "Object/inst/bool_port_collector.h"
	$Id: bool_port_collector.h,v 1.1 2009/02/28 01:20:42 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_BOOL_PORT_COLLECTOR_H__
#define	__HAC_OBJECT_INST_BOOL_PORT_COLLECTOR_H__

#include "Object/inst/port_visitor.h"
#include <set>

namespace HAC {
namespace entity {
using std::set;

struct bool_port_collector : public port_visitor {
	set<size_t>			bool_indices;

	~bool_port_collector();

	VISIT_INSTANCE_ALIAS_INFO_PROTO(bool_tag);

	using port_visitor::visit;

};	// end struct bool_port_collector

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_BOOL_PORT_COLLECTOR_H__

