/**
	\file "Object/inst/connection_policy.tcc"
	$Id: connection_policy.tcc,v 1.1.2.1 2006/11/16 20:28:45 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CONNECTION_POLICY_TCC__
#define	__HAC_OBJECT_INST_CONNECTION_POLICY_TCC__

#include <iostream>
#include "Object/inst/connection_policy.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
/**
	Issue an error/diagnostic if:
	both canonical node are already connected to a producer
 */
template <class AliasType>
good_bool
directional_connect_policy<true>::synchronize_flags(AliasType& l, AliasType& r) {
	typedef	typename AliasType::traits_type		traits_type;
	this_type& ll(l), rr(r);	// static_cast
	const unsigned char _and = ll.direction_flags & rr.direction_flags;
	bool good = true;
	if (_and & CONNECTED_TO_PRODUCER) {
		// multiple producers
		if (!(_and & CONNECTED_SHARED_PRODUCER)) {
			// at least one of them not sharing
			cerr << "Error: cannot alias two "
				"non-sharing producers of type " <<
				traits_type::tag_name << "." << endl;
			good = false;
		}
	}
	if (_and & CONNECTED_TO_CONSUMER) {
		// multiple producers
		if (!(_and & CONNECTED_SHARED_CONSUMER)) {
			// at least one of them not sharing
			cerr << "Error: cannot alias two "
				"non-sharing consumers of type " <<
				traits_type::tag_name << "." << endl;
			good = false;
		}
	}
	if (!good) {
		l.dump_hierarchical_name(cerr << "\tgot: ") << endl;
		r.dump_hierarchical_name(cerr << "\tand: ") << endl;
	} else {
		const unsigned char _or = 
			ll.direction_flags | rr.direction_flags;
		ll.direction_flags = _or;
		rr.direction_flags = _or;
	}
	return good_bool(good);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_CONNECTION_POLICY_TCC__

