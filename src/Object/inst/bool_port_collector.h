/**
	\file "Object/inst/bool_port_collector.h"
	$Id: bool_port_collector.h,v 1.1.8.1 2009/08/12 00:29:33 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_BOOL_PORT_COLLECTOR_H__
#define	__HAC_OBJECT_INST_BOOL_PORT_COLLECTOR_H__

#include "Object/inst/port_visitor.h"

namespace HAC {
namespace entity {

/**
	Compatible container types: set::set, util::unique_list
	anything that uses keyed value insertion.
 */
template <class Container>
struct bool_port_collector : public port_visitor {
	typedef	Container		container_type;
	/**
		Value type must be integral (bool indices)
	 */
	typedef	typename container_type::value_type	value_type;
	container_type			bool_indices;

	~bool_port_collector();

	VISIT_INSTANCE_ALIAS_INFO_PROTO(bool_tag);

	using port_visitor::visit;

};	// end struct bool_port_collector

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_BOOL_PORT_COLLECTOR_H__

