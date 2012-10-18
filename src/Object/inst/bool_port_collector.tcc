/**
	\file "Object/inst/bool_port_collector.tcc"
	$Id: bool_port_collector.tcc,v 1.3 2011/03/30 04:19:01 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_BOOL_PORT_COLLECTOR_TCC__
#define	__HAC_OBJECT_INST_BOOL_PORT_COLLECTOR_TCC__

#include "Object/inst/bool_port_collector.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/inst/alias_empty.hh"
#include "Object/traits/bool_traits.hh"

namespace HAC {
namespace entity {
//=============================================================================
// class meta_type_port_collector method definitions

template <class Tag, class Container>
meta_type_port_collector<Tag,Container>::~meta_type_port_collector() { }

template <class Tag, class Container>
void
meta_type_port_collector<Tag,Container>::visit(
		const instance_alias_info<Tag>& a) {
	this->indices.insert(a.instance_index);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_BOOL_PORT_COLLECTOR_TCC__

