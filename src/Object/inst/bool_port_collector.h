/**
	\file "Object/inst/bool_port_collector.h"
	$Id: bool_port_collector.h,v 1.3 2011/03/30 04:19:00 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_BOOL_PORT_COLLECTOR_H__
#define	__HAC_OBJECT_INST_BOOL_PORT_COLLECTOR_H__

#include "Object/inst/port_visitor.h"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Intended for traversing collections of port formals
	in declaration order.
	Compatible container types: set::set, util::unique_list
	anything that uses keyed value insertion.
	TODO: possible to template this?
 */
template <class Tag, class Container>
struct meta_type_port_collector : public port_visitor {
	typedef	Container		container_type;
	/**
		Value type must be integral (indices)
	 */
	typedef	typename container_type::value_type	value_type;
	container_type			indices;

	~meta_type_port_collector();

	VISIT_INSTANCE_ALIAS_INFO_PROTO(Tag);

	using port_visitor::visit;

};	// end struct meta_type_port_collector

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// typedef template
template <class Container>
struct bool_port_collector {
	typedef	meta_type_port_collector<bool_tag, Container>	type;
};

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_BOOL_PORT_COLLECTOR_H__

