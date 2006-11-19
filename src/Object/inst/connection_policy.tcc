/**
	\file "Object/inst/connection_policy.tcc"
	$Id: connection_policy.tcc,v 1.1.2.5 2006/11/19 02:20:03 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CONNECTION_POLICY_TCC__
#define	__HAC_OBJECT_INST_CONNECTION_POLICY_TCC__

#include <iostream>
#include "Object/inst/connection_policy.h"
#include "Object/inst/instance_collection.h"
#include "common/ICE.h"

#include "util/stacktrace.h"
#include "util/macros.h"
#include "util/memory/excl_ptr.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
/**
	Forces a flag synchronization.  
	No error.  
 */
template <class AliasType>
void
directional_connect_policy<true>::__update_flags(AliasType& a) {
	this->direction_flags = a.find()->direction_flags;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Issue an error/diagnostic if:
	both canonical node are already connected to a producer
 */
template <class AliasType>
good_bool
directional_connect_policy<true>::synchronize_flags(
		AliasType& l, AliasType& r) {
	typedef	typename AliasType::traits_type		traits_type;
	STACKTRACE_VERBOSE;
	if (&l == &r) {
		// permit self-aliases, of course
		return good_bool(true);
	}
	this_type& ll(l);	// static_cast
	this_type& rr(r);	// static_cast
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("ll.flags = " << size_t(ll.direction_flags) << endl);
	STACKTRACE_INDENT_PRINT("rr.flags = " << size_t(rr.direction_flags) << endl);
	l.dump_hierarchical_name(STACKTRACE_INDENT << "l: ") << endl;
	r.dump_hierarchical_name(STACKTRACE_INDENT << "r: ") << endl;
#endif
	const unsigned char _and = ll.direction_flags & rr.direction_flags;
	const unsigned char _or = ll.direction_flags | rr.direction_flags;
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
		// multiple consumers
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
		STACKTRACE_INDENT_PRINT("new flags = " << size_t(_or) << endl);
		ll.direction_flags = _or;
		rr.direction_flags = _or;
	}
	return good_bool(good);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Currently does NOT handle shared channels.  
	TODO: propagate local connection information to external, 
		bottom-up, formal to actual.  
	Cannot error, as this only initializes direction flags.  
 */
template <class ContainerPtrType>
void
directional_connect_policy<true>::initialize_direction(
		const ContainerPtrType p) {
	typedef	typename ContainerPtrType::element_type
					collection_interface_type;
	typedef	typename collection_interface_type::traits_type
					traits_type;
	typedef	typename traits_type::tag_type		tag_type;
	typedef	instance_collection<tag_type>	instance_collection_type;
	STACKTRACE_VERBOSE;
	NEVER_NULL(p);
	const bool f = p->is_formal();
	const instance_collection_type& c(p->get_canonical_collection());
	const char d = c.__get_raw_type().get_direction();
	switch (d) {
	case '\0': break;		// leave as initial value
	case '?':
		if (f) {
			direction_flags |= CONNECTED_TO_PRODUCER;
		} else {
			direction_flags |= CONNECTED_TO_CONSUMER;
		}
		break;
	case '!':
		if (f) {
			direction_flags |= CONNECTED_TO_CONSUMER;
		} else {
			direction_flags |= CONNECTED_TO_PRODUCER;
		}
		break;
	default:
		ICE(cerr, cerr << "Invalid direction.";)
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Issues diagnostic for dangling channel connections.  
	Could just make this a non-template function by passing
		the tag_name as a string.  
 */
template <class AliasType>
good_bool
directional_connect_policy<true>::__check_connection(const AliasType& a) {
	typedef	typename AliasType::traits_type		traits_type;
	const char f = a.direction_flags;
	if (!(f & (CONNECTED_TO_PRODUCER | CONNECTED_CHP_PRODUCER))) {
		a.dump_hierarchical_name(
			cerr << "WARNING: " << traits_type::tag_name << " ")
			<< " lacks connection to a producer." << endl;
	}
	if (!(f & (CONNECTED_TO_CONSUMER | CONNECTED_CHP_CONSUMER))) {
		a.dump_hierarchical_name(
			cerr << "WARNING: " << traits_type::tag_name << " ")
			<< " lacks connection to a consumer." << endl;
	}
	return good_bool(true);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_CONNECTION_POLICY_TCC__

