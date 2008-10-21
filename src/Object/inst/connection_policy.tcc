/**
	\file "Object/inst/connection_policy.tcc"
	$Id: connection_policy.tcc,v 1.7 2008/10/21 00:24:30 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CONNECTION_POLICY_TCC__
#define	__HAC_OBJECT_INST_CONNECTION_POLICY_TCC__

#include <iostream>
#include "Object/inst/connection_policy.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/type/channel_direction_enum.h"	// for direction enum
#include "common/ICE.h"

#include "util/stacktrace.h"
#include "util/macros.h"
#include "util/memory/excl_ptr.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class bool_connect_policy method definitions

template <class ContainerType>
void
bool_connect_policy::initialize_direction(const ContainerType&) {
	// TODO: use direction annotations like channels
	// this->attributes = BOOL_DEFAULT_ATTRIBUTES;
	// unnecessary, b/c default ctor
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Forces a flag synchronization.  
	No checking.  No error.  
 */
template <class AliasType>
void
bool_connect_policy::__update_flags(AliasType& a) {
	this->attributes = a.find()->attributes;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static property checking, regarding drive/use.  
 */
template <class AliasType>
good_bool
bool_connect_policy::__check_connection(const AliasType& a) {
	// TODO: check must/must-not directions!
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class AliasType>
void
bool_connect_policy::connection_flag_setter::operator () (AliasType& a) {
	// important that this is done with the *canoncical* node
	if (!a.find()->set_connection_flags(update).good) {
		status.good = false;
	}
}

//=============================================================================
// class channel_connect_policy method definitions

/**
	Forces a flag synchronization.  
	No checking.  No error.  
 */
template <class AliasType>
void
channel_connect_policy::__update_flags(AliasType& a) {
	this->direction_flags = a.find()->direction_flags;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Issue an error/diagnostic if:
	both canonical node are already connected to a producer
	TODO: precise direction connection checking, w.r.t CHP,
		meta-references and nonmeta-references.
 */
template <class AliasType>
good_bool
channel_connect_policy::synchronize_flags(
		AliasType& l, AliasType& r) {
	typedef	typename AliasType::traits_type		traits_type;
	STACKTRACE_VERBOSE;
	if (&l == &r) {
		// permit self-aliases, of course
		return good_bool(true);
	}
	this_type& ll(l);	// static_cast
	this_type& rr(r);	// static_cast
	const connection_flags_type& lld(ll.direction_flags);
	const connection_flags_type& rrd(rr.direction_flags);
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("ll.flags = 0x" <<
		std::hex << size_t(lld) << endl);
	STACKTRACE_INDENT_PRINT("rr.flags = 0x" <<
		std::hex << size_t(rrd) << endl);
	l.dump_hierarchical_name(STACKTRACE_INDENT << "l: ") << endl;
	r.dump_hierarchical_name(STACKTRACE_INDENT << "r: ") << endl;
#endif
	// convenience aliases
	const connection_flags_type _and = lld & rrd;
	const connection_flags_type _or = lld | rrd;
	bool good = true;
	if ((lld & CONNECTED_TO_ANY_PRODUCER) &&
			(rrd & CONNECTED_TO_ANY_PRODUCER))
	{
		// multiple producers
		// TODO: strengthen condition?
		// shared, but connection must also be consistent?
		if (!(_and & CONNECTED_PRODUCER_IS_SHARED)) {
			// at least one of them not sharing
			cerr << "Error: cannot alias two "
				"non-sharing producers of type " <<
				traits_type::tag_name << "." << endl;
			good = false;
		}
	}
	if ((lld & CONNECTED_TO_ANY_CONSUMER) &&
			(rrd & CONNECTED_TO_ANY_CONSUMER))
	{
		// multiple consumers
		if (!(_and & CONNECTED_CONSUMER_IS_SHARED)) {
			// at least one of them not sharing
			cerr << "Error: cannot alias two "
				"non-sharing consumers of type " <<
				traits_type::tag_name << "." << endl;
			good = false;
		}
	}
	if (!check_meta_nonmeta_usage(_or, traits_type::tag_name).good) {
		// already have error message
		good = false;
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
	TODO: propagate local connection information to external, 
		bottom-up, formal to actual.  (done?)
	Cannot error, as this only initializes direction flags.  
	Policy: we assume port_formals are connected to their environment
		to suppress unconnected diagnostics.  
		However, port actual initializations are left up to
		an implementation switch: 
		PROPAGATE_CHANNEL_CONNECTIONS_HIERARCHICALLY
		if enabled, we don't initialize actuals with their directions, 
		we infer them from their port summaries.  
	Ports marked as shared not need their aliases flagged as shared
		because they should 
	NOTE: called by instance_alias_info::instantiate().
	optimization: refactor code to avoid repeated calls to same container.
 */
template <class ContainerType>
void
channel_connect_policy::initialize_direction(
		const ContainerType& p) {
	typedef	ContainerType		collection_interface_type;
	typedef	typename collection_interface_type::traits_type
					traits_type;
	typedef	typename traits_type::tag_type		tag_type;
	typedef	instance_collection<tag_type>	instance_collection_type;
	STACKTRACE_VERBOSE;
	const bool f = p.is_formal();
	const instance_collection_type& c(p.get_canonical_collection());
	const direction_type d = c.__get_raw_type().get_direction();
	switch (d) {
	case CHANNEL_TYPE_BIDIRECTIONAL: break;
		// leave as initial value
#if ENABLE_SHARED_CHANNELS
	case CHANNEL_TYPE_RECEIVE_SHARED:
		if (f) {
			direction_flags |= CONNECTED_PRODUCER_IS_SHARED;
			direction_flags |= CONNECTED_PORT_FORMAL_PRODUCER;
		} else {
			direction_flags |= CONNECTED_CONSUMER_IS_SHARED;
		}
		break;
#endif
	case CHANNEL_TYPE_RECEIVE:
		if (f) {
			direction_flags |= CONNECTED_PORT_FORMAL_PRODUCER;
		}
		break;
#if ENABLE_SHARED_CHANNELS
	case CHANNEL_TYPE_SEND_SHARED:
		if (f) {
			direction_flags |= CONNECTED_CONSUMER_IS_SHARED;
			direction_flags |= CONNECTED_PORT_FORMAL_CONSUMER;
		} else {
			direction_flags |= CONNECTED_PRODUCER_IS_SHARED;
		}
		break;
#endif
	case CHANNEL_TYPE_SEND:
		if (f) {
			direction_flags |= CONNECTED_PORT_FORMAL_CONSUMER;
		}
		break;
	default:
		ICE(cerr, cerr << "Invalid direction: " << d << endl;)
	}
#if ENABLE_STACKTRACE
	c.dump_hierarchical_name(STACKTRACE_INDENT << "collection: ") << endl;
	STACKTRACE_INDENT_PRINT("direction_flags = 0x" <<
		std::hex << size_t(direction_flags) << endl);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This variation initializes actuals' aliases (from subinstances)
	based on how they were connected in their corresponding
	formal definition collections.  
	This unmasks the bits that were set in the formal collection
		to prevent connection in the wrong direction.  
		Unmasking bits lets us check hierarchically whether or not
		channels WERE connected properly.  
	TODO: refactor to avoid repeated redundant calls.  
	\param a the corresponding alias in the formal collection.  
 */
template <class AliasType>
void
channel_connect_policy::initialize_actual_direction(
		const AliasType& a) {
	typedef	typename AliasType::container_type	collection_interface_type;
	typedef	typename collection_interface_type::traits_type
					traits_type;
	typedef	typename traits_type::tag_type		tag_type;
	typedef	instance_collection<tag_type>	instance_collection_type;
	STACKTRACE_VERBOSE;
	// const bool f = p.is_formal();
	// const instance_collection_type& c(p.get_canonical_collection());
	const instance_collection_type&
		c(a.container->get_canonical_collection());
	const char d = c.__get_raw_type().get_direction();
	// with bit fields, could just twiddle the consumer/producer halves...
	switch (d) {
	case CHANNEL_TYPE_BIDIRECTIONAL:
		direction_flags = a.direction_flags;
		break;
	case CHANNEL_TYPE_RECEIVE:
		// note: this clears out the META flag as well
		direction_flags =
			(a.direction_flags & ~CONNECTED_PORT_FORMAL_PRODUCER);
		if (a.direction_flags & CONNECTED_TO_ANY_CONSUMER) {
			direction_flags |= CONNECTED_TO_SUBSTRUCT_CONSUMER;
		}
		break;
	case CHANNEL_TYPE_SEND:
		// note: this clears out the META flag as well
		direction_flags =
			(a.direction_flags & ~CONNECTED_PORT_FORMAL_CONSUMER);
		if (a.direction_flags & CONNECTED_TO_ANY_PRODUCER) {
			direction_flags |= CONNECTED_TO_SUBSTRUCT_PRODUCER;
		}
		break;
#if ENABLE_SHARED_CHANNELS
	case CHANNEL_TYPE_RECEIVE_SHARED:
		// note: this clears out the META flag as well
		direction_flags =
			(a.direction_flags &
				~(CONNECTED_PORT_FORMAL_PRODUCER | CONNECTED_PRODUCER_IS_SHARED));
		if (a.direction_flags & CONNECTED_TO_ANY_CONSUMER) {
			direction_flags |= CONNECTED_TO_SUBSTRUCT_CONSUMER;
			direction_flags |= CONNECTED_CONSUMER_IS_SHARED;
		}
		break;
	case CHANNEL_TYPE_SEND_SHARED:
		// note: this clears out the META flag as well
		direction_flags =
			(a.direction_flags &
				~(CONNECTED_PORT_FORMAL_CONSUMER | CONNECTED_CONSUMER_IS_SHARED));
		if (a.direction_flags & CONNECTED_TO_ANY_PRODUCER) {
			direction_flags |= CONNECTED_TO_SUBSTRUCT_PRODUCER;
			direction_flags |= CONNECTED_PRODUCER_IS_SHARED;
		}
		break;
#endif
	default:
		ICE(cerr, cerr << "Invalid direction: " << d << endl;)
	}
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("a.direction_flags = 0x" <<
		std::hex << size_t(a.direction_flags) << endl);
	c.dump_hierarchical_name(STACKTRACE_INDENT << "collection: ") << endl;
	STACKTRACE_INDENT_PRINT("direction_flags = 0x" <<
		std::hex << size_t(direction_flags) << endl);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Issues diagnostic for dangling channel connections.  
	Could just make this a non-template function by passing
		the tag_name as a string.  
	TODO: finer diagnostic control to determine whether or not
		to promote warning to error.  
 */
template <class AliasType>
good_bool
channel_connect_policy::__check_connection(const AliasType& a) {
	typedef	typename AliasType::traits_type		traits_type;
	const connection_flags_type f = a.direction_flags;
	if (!(f & CONNECTED_TO_ANY_PRODUCER)) {
		a.dump_hierarchical_name(
			cerr << "WARNING: " << traits_type::tag_name << " ")
			<< " lacks connection to a producer." << endl;
	}
	if (!(f & CONNECTED_TO_ANY_CONSUMER)) {
		a.dump_hierarchical_name(
			cerr << "WARNING: " << traits_type::tag_name << " ")
			<< " lacks connection to a consumer." << endl;
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor's forwarding operator.  
	Sets flags of each element's canonical alias, 
	while checking for errors.
 */
template <class AliasType>
void
channel_connect_policy::connection_flag_setter::operator () (
		AliasType& a) {
	// important that this is done with the *canoncical* node
	if (!a.find()->set_connection_flags(update).good) {
		status.good = false;
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_CONNECTION_POLICY_TCC__

