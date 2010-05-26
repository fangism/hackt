/**
	\file "Object/inst/connection_policy.tcc"
	$Id: connection_policy.tcc,v 1.12 2010/05/26 00:46:51 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CONNECTION_POLICY_TCC__
#define	__HAC_OBJECT_INST_CONNECTION_POLICY_TCC__

#include <iostream>
#include <sstream>
#include "Object/def/footprint.h"
#include "Object/inst/connection_policy.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/type/channel_direction_enum.h"	// for direction enum
#include "Object/unroll/unroll_context.h"
#include "Object/type/canonical_type.h"
#include "Object/common/dump_flags.h"		// debug only
#include "common/ICE.h"

#include "util/stacktrace.h"
#include "util/macros.h"
#include "util/memory/excl_ptr.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class bool_connect_policy method definitions

template <class AliasType>
void
bool_connect_policy::initialize_direction(const AliasType&, 
		const unroll_context&) {
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
	Static connectivity property checking, regarding drive/use.  
 */
template <class AliasType>
error_count
bool_connect_policy::__check_connection(const AliasType& a) {
	// TODO: check must/must-not directions!
	error_count ret;
#if BOOL_PRS_CONNECTIVITY_CHECKING
	STACKTRACE_VERBOSE;
// (!a.is_port_alias())	// wrong: misses aliases to direct ports
if (!a.is_aliased_to_port())
{
	// only check local non-port aliases
	const bool any_fanout_dn = a.attributes & BOOL_ANY_FANOUT_PULL_DN;
	const bool any_fanout_up = a.attributes & BOOL_ANY_FANOUT_PULL_UP;
	const bool any_fanin_dn = a.attributes & BOOL_ANY_FANIN_PULL_DN;
	const bool any_fanin_up = a.attributes & BOOL_ANY_FANIN_PULL_UP;
	const bool any_fanout = any_fanout_dn || any_fanout_up;
	const bool any_fanin = any_fanin_dn || any_fanin_up;
//	const bool dead = !any_fanout && !any_fanin;
	const bool floating = any_fanout && !any_fanin;
	const bool asym_fanin = any_fanout && (any_fanin_dn ^ any_fanin_up);
//	const bool asym_fanout = any_fanout_dn ^ any_fanout_up;
//	const bool warn = dead || floating || asym_fanin;
	const bool warn = floating || asym_fanin;
if (warn) {
	// don't evaluate name unless diagnostic is printed
	std::ostringstream oss;
	a.dump_hierarchical_name(oss);
	const string& n(oss.str());
	// TODO: configurable warnings
#if 0
	if (!any_fanout_dn) {
		cerr << "Warning: node " << n <<
			" does not fan-out to any pull-down rules." << endl;
		++ret.warnings;
	}
	if (!any_fanout_up) {
		cerr << "Warning: node " << n <<
			" does not fan-out to any pull-up rules." << endl;
		++ret.warnings;
	}
	if (!any_fanin_dn) {
		cerr << "Warning: node " << n <<
			" has no pull-up fan-ins." << endl;
		++ret.warnings;
	}
	if (!any_fanin_up) {
		cerr << "Warning: node " << n <<
			" has no pull-down fan-ins." << endl;
		++ret.warnings;
	}
#else
#if 0
	if (dead) {
		cerr << "Warning: unused node " << n <<
			" has neither PRS fanin nor fanout." << endl;
		++ret.warnings;
	}
#endif
	if (floating) {
		cerr << "Warning: node " << n <<
			" has no pull-up nor pull-dn PRS fanin." << endl;
		++ret.warnings;
	}
	if (asym_fanin) {
		if (any_fanin_dn) {
			cerr << "Warning: node " << n <<
				" has no PRS pull-up fanin." << endl;
			++ret.warnings;
		} else {
			cerr << "Warning: node " << n <<
				" has no PRS pull-dn fanin." << endl;
			++ret.warnings;
		}
	}
#endif
	// never error out for now
}
}
#endif
	return ret;
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
	TODO: what if channel is a member of a port/struct, and direction
		is only annotated locally, inside the definition body, e.g.
		foo.mychan?;
		Should the language allow this b/c it is inconveinent to
		specify the direction of a port member?
 */
template <class AliasType>
void
channel_connect_policy::initialize_direction(const AliasType& a, 
		const unroll_context&) {
	typedef	typename AliasType::container_type	ContainerType;
	typedef	ContainerType		collection_interface_type;
	typedef	typename collection_interface_type::traits_type
					traits_type;
	typedef	typename traits_type::tag_type		tag_type;
	typedef	instance_collection<tag_type>	instance_collection_type;
	STACKTRACE_VERBOSE;
	const ContainerType& p(*a.container);
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
error_count
channel_connect_policy::__check_connection(const AliasType& a) {
	typedef	typename AliasType::traits_type		traits_type;
	error_count ret;
	const connection_flags_type f = a.direction_flags;
	if (!(f & CONNECTED_TO_ANY_PRODUCER)) {
		a.dump_hierarchical_name(
			cerr << "WARNING: " << traits_type::tag_name << " ")
			<< " lacks connection to a producer." << endl;
		++ret.warnings;
	}
	if (!(f & CONNECTED_TO_ANY_CONSUMER)) {
		a.dump_hierarchical_name(
			cerr << "WARNING: " << traits_type::tag_name << " ")
			<< " lacks connection to a consumer." << endl;
		++ret.warnings;
	}
	return ret;
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
// class process_connect_policy method definitions

/**
	Take footprint of this type, from context and apply to subinstances.  
	This should be called upon instantiation, and also upon
	type completion, the case of relaxed actuals.  
	This could potentially be used to replay internal aliases, 
	which is currently managed by port_alias_tracker.  
	\param AliasType must be a substructured type, like process.
	TODO: calling type-completion for every alias that is 
		instantiated seems wasteful, especially if part of
		a strictly typed array... rewrite me.
 */
template <class AliasType>
void
process_connect_policy::initialize_direction(AliasType& a, 
		const unroll_context& c) {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	a.dump_hierarchical_name(cerr << "alias: ") << endl;
	a.dump_ports(cerr, dump_flags::default_value) << endl;
//	c.dump(cerr << "context: ") << endl;
#endif
	// check to make sure type is complete (no longer relaxed)
	// lookup resolved type of the alias or its container
	// a.complete_type_actuals(*a.container); from "alias_actuals.tcc"

	typedef	typename AliasType::container_type	container_type;
	typedef	typename container_type::instance_collection_parameter_type
						complete_type_type;
	typedef	typename complete_type_type::canonical_definition_type
						canonical_definition_type;
	// ordering issue: cannot complete type actuals if collection
	// was declared relaxed, so this work will have to be punted
 if (!a.container->get_canonical_collection().__get_raw_type().is_relaxed()) {
	const complete_type_type
		_type(a.complete_type_actuals(*a.container));
	INVARIANT(_type);
	// acquire the (already created) footprint
	const footprint&
		f(_type.get_base_def()->get_footprint(
			_type.get_raw_template_params()));
	// walk ports and apply flags from the local unique instance pool
	const port_alias_tracker& pt(f.get_scope_alias_tracker());
	// b/c port_alias_tracker only lists ports with *aliases*
	// copy-initialize: alias actuals and attributes and flags
#if ENABLE_STACKTRACE
	f.dump_type(STACKTRACE_INDENT_PRINT("instantiating type: ")) << endl;
	pt.dump(STACKTRACE_INDENT_PRINT("scope_alias_tracker: ") << endl);
#endif
	pt.export_alias_properties(a);
	// TODO: use footprint.port_alias_tracker::replay_internal_aliases
	// TODO: should connections occur before or after copying properties?
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("after inheriting alias properties: " << endl);
	a.dump_ports(cerr, dump_flags::default_value) << endl;
#endif
}
	// else type is relaxed, skip this until type is complete
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_CONNECTION_POLICY_TCC__

