/**
	\file "ring_node.tcc"
	Implementation of ring_node class.
	$Id: ring_node.tcc,v 1.1.2.2 2005/02/05 02:24:04 fang Exp $
 */

#ifndef	__UTIL_RING_NODE_TCC__
#define	__UTIL_RING_NODE_TCC__

#include "ring_node.h"

namespace util {
//=============================================================================

#if !FORCE_INLINE_RING_NODE
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#if !FORCE_INLINE_RING_NODE
inline
bool
ring_node_base::contains(const ring_node_base& r) const {
	const ring_node_base* walk1 = this;
	const ring_node_base* walk2 = &r;
	do {
		if (walk1 == &r || walk2 == this)
			return true;
		else {
			walk1 = walk1->next;
			walk2 = walk2->next;
		}
	} while (walk1 != this && walk2 != &r);
	return false;
}
#endif


//=============================================================================
}	// end namespace util

#endif	// __UTIL_RING_NODE_TCC__

