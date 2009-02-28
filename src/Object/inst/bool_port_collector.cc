/**
	\file "Object/inst/bool_port_collector.cc"
	$Id: bool_port_collector.cc,v 1.1 2009/02/28 01:20:41 fang Exp $
 */

#include "Object/inst/bool_port_collector.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/traits/bool_traits.h"

namespace HAC {
namespace entity {

bool_port_collector::~bool_port_collector() { }

void
bool_port_collector::visit(const instance_alias_info<bool_tag>& a) {
	bool_indices.insert(a.instance_index);
}

}	// end namespace entity
}	// end namespace HAC

