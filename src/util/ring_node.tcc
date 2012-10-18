/**
	\file "util/ring_node.tcc"
	Implementation of ring_node class.
	$Id: ring_node.tcc,v 1.5 2006/04/27 05:51:52 fang Exp $
 */

#ifndef	__UTIL_RING_NODE_TCC__
#define	__UTIL_RING_NODE_TCC__

#include "util/ring_node.hh"

#ifndef	EXTERN_TEMPLATE_UTIL_RING_NODE

namespace util {
//=============================================================================
// class ring_node_derived method definitions

RING_NODE_DERIVED_TEMPLATE_SIGNATURE
ring_node_derived<T>::~ring_node_derived() {
	if (next != this) {
		next_type walk = next;
		while (walk->next != this) {
			walk = walk->next;
		}
		// found the node that points to this, update it
		walk->next = next;
	}
	// else this is the last node, just drops itself
	next = NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RING_NODE_DERIVED_TEMPLATE_SIGNATURE
bool
ring_node_derived<T>::contains(const this_type& r) const {
	const_next_type walk1 = this;
	const_next_type walk2 = &r;
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

//=============================================================================
}	// end namespace util

#endif	// EXTERN_TEMPLATE_UTIL_RING_NODE
#endif	// __UTIL_RING_NODE_TCC__

