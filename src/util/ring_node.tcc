/**
	\file "ring_node.tcc"
	Implementation of ring_node class.
	$Id: ring_node.tcc,v 1.1.2.1 2005/02/04 05:40:24 fang Exp $
 */

#ifndef	__UTIL_RING_NODE_TCC__
#define	__UTIL_RING_NODE_TCC__

#include "ring_node.h"

namespace util {
//=============================================================================

#if 0
inline
ring_node_base::~ring_node_base() {
	if (next != this) {
		ring_node_base* walk = next;
		while (walk->next != this)
			walk = walk->next;
		// found the node that points to this, update it
		walk->next = next;
	}
	// else this is the last node, just drops itself
}
#endif

}	// end namespace util

#endif	// __UTIL_RING_NODE_TCC__

