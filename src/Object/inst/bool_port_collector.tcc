/**
	\file "Object/inst/bool_port_collector.cc"
	$Id: bool_port_collector.tcc,v 1.2 2009/08/28 20:44:54 fang Exp $
 */

#include "Object/inst/bool_port_collector.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/traits/bool_traits.h"

namespace HAC {
namespace entity {
//=============================================================================
// class bool_port_collector method definitions

template <class Container>
bool_port_collector<Container>::~bool_port_collector() { }

template <class Container>
void
bool_port_collector<Container>::visit(
		const instance_alias_info<bool_tag>& a) {
	this->bool_indices.insert(a.instance_index);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

